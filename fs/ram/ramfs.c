/**MOD+************************************************************************/
/* Module:  ramfs.c                                                           */
/*                                                                            */
/* Purpose: RAM file system                                                   */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"
#include "fs/stat.h"
#include "mm/mem_pool.h"

#define RAMFS_BLOCK_SIZE      (4*1024)
#define RAMFS_BLOCKS_MAX_NR   8
#define RAMFS_NAME_MAX_LEN    32

#define RAMFS_NODE_TOTAL_NUM  64
#define RAMFS_BLOCK_TOTAL_NUM (RAMFS_NODE_TOTAL_NUM * RAMFS_BLOCKS_MAX_NR)

typedef struct {
    UINT8 data[RAMFS_BLOCK_SIZE];
} RAMFS_BLOCK;

/* File and directory node */
typedef struct {
    UINT32 mode;        /* node type */
    INT16  refcnt;      /* open file/current dir references */
    INT16  nlink;       /* number of links to this node */
    UINT32 size;        /* size of file in bytes */
    TIME   atime;       /* last access time */
    TIME   mtime;       /* last modified time */
    TIME   ctime;       /* last changed status time */

    RAMFS_BLOCK *blocks[RAMFS_BLOCKS_MAX_NR];   /* node's data */

} RAMFS_NODE;

/* Directory entry */
typedef struct {
    RAMFS_NODE *node;                 /* pointer to node */
    CHAR name[RAMFS_NAME_MAX_LEN];    /* file name */
} RAMFS_DIRENT;

/* ramfs directory search data */
typedef struct {
    RAMFS_NODE *dir;    /* directory to search */
    CONST CHAR *path;   /* path to follow */

    RAMFS_NODE *node;   /* node found */
    BOOL last;          /* last name in path ? */

    CONST CHAR *name;   /* last name fragment used */
    UINT32 namelen;     /* name fragment length */
} RAMFS_DIRSEARCH;

STATIC MEM_POOL ramfs_block_pool;
STATIC POOL_MAP(ramfs_block_pool_map, RAMFS_BLOCK_TOTAL_NUM);
STATIC POOL_MEM(ramfs_block_pool_mem, RAMFS_BLOCK_TOTAL_NUM,
                sizeof(RAMFS_BLOCK));

STATIC MEM_POOL ramfs_node_pool;
STATIC POOL_MAP(ramfs_node_pool_map, RAMFS_NODE_TOTAL_NUM);
STATIC POOL_MEM(ramfs_node_pool_mem, RAMFS_NODE_TOTAL_NUM, sizeof(RAMFS_NODE));

STATIC INT add_dirent(RAMFS_NODE *dir, CONST CHAR *name, UINT32 namelen,
                      RAMFS_NODE *node);
STATIC INT delete_dirent(RAMFS_NODE *dir, CONST CHAR *name, UINT32 namelen);
STATIC INLINE VOID free_node(RAMFS_NODE *n);
STATIC INT free_buffer(RAMFS_NODE *node);

STATIC INLINE RAMFS_BLOCK *alloc_block(VOID)
{
    RAMFS_BLOCK *b = mem_pool_alloc(&ramfs_block_pool);
    if (NULL != b)
        memset(b, 0, sizeof(RAMFS_BLOCK));
    return b;
}

STATIC INLINE VOID free_block(RAMFS_BLOCK *b)
{
    mem_pool_free(&ramfs_block_pool, b);
}

STATIC INLINE RAMFS_NODE *alloc_node(UINT32 mode)
{
    RAMFS_NODE *node = mem_pool_alloc(&ramfs_node_pool);
    if (NULL == node)
        return NULL;

    memset(node, 0, sizeof(RAMFS_NODE));
    node->mode = mode;
    node->atime = node->mtime = node->ctime = 0;

    if (S_ISDIR(mode)) {
        INT ret = add_dirent(node, ".", 1, node);
        if (ENOERR == ret)
            ret = add_dirent(node, "..", 2, node);

        if (ENOERR != ret) {
            free_buffer(node);
            free_node(node);
            return NULL;
        }
    }

    return node;
}

STATIC INLINE VOID free_node(RAMFS_NODE *n)
{
    mem_pool_free(&ramfs_node_pool, n);
}

/* The returned size is one block available bytes at offset pos.
   User maybe continue to call it to get more spaces until it's ENOSPC.
 */
STATIC INT find_buffer(RAMFS_NODE *node, INT32 pos, UINT8 **buf, UINT32 *size,
                       BOOL alloc)
{
    INT bidx = pos / RAMFS_BLOCK_SIZE;
    INT bpos = pos % RAMFS_BLOCK_SIZE;
    RAMFS_BLOCK *block;

    *buf = NULL;
    *size = RAMFS_BLOCK_SIZE - bpos;

    if (bidx >= RAMFS_BLOCKS_MAX_NR)
        return ENOERR;

    block = node->blocks[bidx];

    if (NULL == block) {
        if (TRUE == alloc) {
            /* allocate one block */
            block = alloc_block();
            if (NULL == block)
                return -ENOSPC;
            node->blocks[bidx] = block;
        } else {
            return ENOERR;
        }
    }

    *buf = (UINT8 *)block + bpos;

    return ENOERR;
}

STATIC INT free_buffer(RAMFS_NODE *node)
{
    INT i;
    RAMFS_BLOCK *b;

    for (i = 0; i < RAMFS_BLOCKS_MAX_NR; i++) {
        b = node->blocks[i];
        if (NULL != b) free_block(b);
    }

    return ENOERR;
}

/* decrease a node's reference count.
   If this makes the ref count zero, and the number of links is either one
   for a file or two for a directory, then this node is detached from the
   directory tree and can be freed.
 */
STATIC INT dec_refcnt(RAMFS_NODE *node)
{
    INT ret = ENOERR;

    BUG_ON(NULL == node);

    node->refcnt--;

    BUG_ON(node->refcnt < 0);
    BUG_ON(node->nlink < 0);

    if ((0 == node->refcnt) &&
        ((S_ISREG(node->mode) && (0 == node->nlink)) ||
         (S_ISDIR(node->mode) && (1 == node->nlink))))
    {

        if (S_ISDIR(node->mode)) {
            delete_dirent(node, ".", 1);
            delete_dirent(node, "..", 2);
        }

        free_buffer(node);
        free_node(node);
    }

    return ret;
}

/* add an (file or directory) entry to a directory */
STATIC INT add_dirent(RAMFS_NODE *dir, CONST CHAR *name, UINT32 namelen,
                      RAMFS_NODE *node)
{
    INT pos = 0;
    UINT8 *buf;
    UINT32 size;
    INT ret;
    RAMFS_DIRENT *d;

    while (1) {
        ret = find_buffer(dir, pos, &buf, &size, TRUE);
        if (ENOERR != ret) return ret;

        d = (RAMFS_DIRENT *)buf;
        if ((size < sizeof(RAMFS_DIRENT)) || (NULL != d->node)) {
            pos += sizeof(RAMFS_DIRENT);
        } else {
            break;
        }
    }

    pos += sizeof(RAMFS_DIRENT);

    /* d points to a free dirent */
    d->node = node;
    BUG_ON(namelen >= RAMFS_NAME_MAX_LEN);
    memcpy(d->name, (VOID *)name, namelen);

    /* update directory times */
    dir->mtime = dir->ctime = 0;

    /* extend dir size if necessary */
    if (pos > dir->size)
        dir->size = pos;

    /* count the new link */
    node->nlink++;

    return ENOERR;
}

/* find an (file or directory) entry */
STATIC RAMFS_DIRENT *find_dirent(RAMFS_NODE *dir, CONST CHAR *name,
                                 UINT32 namelen)
{
    INT pos = 0;
    UINT8 *buf;
    UINT32 size;
    INT ret;
    RAMFS_DIRENT *d;

    while (pos < dir->size) {
        ret = find_buffer(dir, pos, &buf, &size, FALSE);
        if (ENOERR != ret) return NULL;

        d = (RAMFS_DIRENT *)buf;
        if ((size < sizeof(RAMFS_DIRENT)) || (NULL == d->node)) {
            pos += sizeof(RAMFS_DIRENT);
        } else {
            BUG_ON(namelen >= RAMFS_NAME_MAX_LEN);
            if (0 == memcmp(d->name, name, namelen))
                return d;
            else
                pos += sizeof(RAMFS_DIRENT);
        }
    }

    return NULL;
}

/* dlete a named (file or directory) entry */
STATIC INT delete_dirent(RAMFS_NODE *dir, CONST CHAR *name, UINT32 namelen)
{
    RAMFS_DIRENT *d = find_dirent(dir, name, namelen);

    if (NULL == d)
        return -ENOENT;

    /* transfer nlink to refcount, why ? */
    d->node->refcnt++;
    d->node->nlink--;

    dec_refcnt(d->node);

    d->node = NULL;

    return ENOERR;
}

/******************************************************************************/

/* initialize a dirsearch object to start a search */
STATIC VOID init_dirsearch(RAMFS_DIRSEARCH *ds, RAMFS_NODE *dir,
                           CONST CHAR *name)
{
    ds->dir = dir;
    ds->path = name;

    ds->node = dir;
    ds->name = name;
    ds->namelen = 0;
    ds->last = FALSE;
}

/* search a single directory */
STATIC INT find_entry(RAMFS_DIRSEARCH *ds)
{
    CONST CHAR *n;
    UINT32 len = 0;
    RAMFS_DIRENT *d;

    /* check that there really is a directory */
    if (!S_ISDIR(ds->dir->mode))
        return -ENOTDIR;

    /* get name fragment from path */
    n = ds->path;

    /* isolate the next element of the path name */
    while (('\0' != *n) && ('/' != *n))
        n++, len++;

    if (len >= RAMFS_NAME_MAX_LEN)
        return -ENAMETOOLONG;

    /* check if this is the last path element */
    while ('/' == *n)
        n++;
    if ('\0' == *n)
        ds->last = TRUE;

    /* update name in dirsearch object */
    ds->name = ds->path;
    ds->namelen = len;

    /* here we have the name and its length.
       serach the directory for a matching entry.
     */
    d = find_dirent(ds->dir, ds->name, ds->namelen);
    if (NULL == d)
        return -ENOENT;

    /* pass back the node we have found */
    ds->node = d->node;

    return ENOERR;
}

/* main interface to directory search */
STATIC INT ramfs_find(RAMFS_DIRSEARCH *ds)
{
    INT ret;

    /* short circuit empty paths */
    if ('\0' == *(ds->path))
        return ENOERR;

    while (1) {

        /* skip separators */
        while ('/' == *(ds->path))
            ds->path++;

        ret = find_entry(ds);

        if (ENOERR != ret)
            return ret;

        if (TRUE == ds->last)
            return ENOERR;

        /* update dirsearch object for next search */
        ds->dir = ds->node;
        ds->path += ds->namelen;
    }
}

/******************************************************************************/
STATIC INT ramfs_mount(VFS_MOUNT *mnt, VFS_FILE_SYSTEM *fs)
{
    RAMFS_NODE *root;

    root = alloc_node(__stat_mode_DIR|S_IRWXU|S_IRWXG|S_IRWXO);
    if (NULL == root)
        return -ENOSPC;

    mnt->mnt_root = root;

    return ENOERR;
}

STATIC INT32 ramfs_read(VFS_FILE *filp, UINT8 *buf, UINT32 len)
{
    RAMFS_NODE *node = filp->f_data;
    UINT32 pos = filp->f_offset;
    UINT32 read_len = 0;
    INT ret;

    while ((len > 0) && (pos < node->size)) {
        UINT8 *tbuf;
        UINT32 tlen;
        UINT32 n = len;

        ret = find_buffer(node, pos, &tbuf, &tlen, FALSE);
        if (ENOERR != ret)
            return ret;

        /* adjust size to the end of file */
        if (n > (node->size - pos))
            n = node->size - pos;

        /* adjust size to the amount of contiguous data */
        if (n > tlen)
            n = tlen;

        if (NULL != tbuf)
            memcpy(buf, tbuf, n); /* copy data out */
        else
            memset(buf, 0, n);    /* hole, so return zeros here */

        /* update working vars */
        len -= n;
        buf += n;
        pos += n;
        read_len += n;
    }

    /* We successfully read some data, update access time and file offset */
    node->atime = 0;
    filp->f_offset = pos;

    return read_len;
}

STATIC INT32 ramfs_write(VFS_FILE *filp, UINT8 *buf, UINT32 len)
{
    RAMFS_NODE *node = filp->f_data;
    UINT32 pos = filp->f_offset;
    UINT32 write_len = 0;
    INT ret;

    if (filp->f_flags & O_APPEND)
        pos = filp->f_offset = node->size;

    while (len > 0) {
        UINT8 *tbuf;
        UINT32 tlen;
        UINT32 n = len;

        ret = find_buffer(node, pos, &tbuf, &tlen, TRUE);

        if (-ENOSPC == ret)
            break;

        if (ENOERR != ret)
            return ret;

        /* adjust size to this block */
        if (n > tlen)
            n = tlen;

        /* copy data in */
        memcpy(tbuf, buf, n);

        /* update working vars */
        len -= n;
        buf += n;
        pos += n;
        write_len += n;
    }

    /* We successfully write some data, update access time, file size
       and file offset
     */
    node->mtime = node->ctime = 0;

    if (pos > node->size)
        node->size = pos;

    filp->f_offset = pos;

    return write_len;
}

/* seek a new file's position */
STATIC INT ramfs_lseek(VFS_FILE *filp, INT32 *offset, INT whence)
{
    RAMFS_NODE *node = filp->f_data;
    INT32 pos = *offset;

    switch (whence) {
        case SEEK_SET:
            // Pos is already where we want to be.
            break;

        case SEEK_CUR:
            // Add pos to current offset.
            pos += filp->f_offset;
            break;

        case SEEK_END:
            // Add pos to file size.
            pos += node->size;
            break;

        default:
            return -EINVAL;
    }

    // All OK, set filp offset and return new position.
    *offset = filp->f_offset = pos;

    return ENOERR;
}

/* close(release) a file */
STATIC INT ramfs_release(VFS_FILE *filp)
{
    RAMFS_NODE *node = filp->f_data;

    dec_refcnt(node);

    filp->f_data = NULL;

    return ENOERR;
}

STATIC VFS_FILE_OPERATIONS ramfs_fileops = {
    .read  = ramfs_read,
    .write = ramfs_write,
    .lseek = ramfs_lseek,
    .release = ramfs_release,
};

STATIC INT ramfs_open(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo, UINT32 mode,
                      VFS_FILE *filp)
{
    RAMFS_DIRSEARCH ds;
    RAMFS_NODE *node;
    INT ret;

    init_dirsearch(&ds, pathinfo->dir, pathinfo->name);

    ret = ramfs_find(&ds);

    if (-ENOENT == ret) {
        if ((TRUE == ds.last) && (O_CREAT & mode)) {
            node = alloc_node(__stat_mode_REG|S_IRWXU|S_IRWXG|S_IRWXO);
            if (NULL == node)
                return -ENOSPC;

            ret = add_dirent(ds.dir, ds.name, ds.namelen, node);
            if (ENOERR != ret) {
                free_buffer(node);
                free_node(node);
                return ret;
            }

            ret = ENOERR;
        }
    } else if (ENOERR == ret) {
        if (((O_CREAT|O_EXCL) & mode) == (O_CREAT|O_EXCL))
            ret = -EEXIST;
        else
            node = ds.node;
    }

    if ((ENOERR == ret) && (O_TRUNC & mode)) {
        /* clear node buffer */
        ret = free_buffer(node);
        node->size = 0;

        /* update file times */
        node->ctime = node->mtime = 0;
    }

    if (ENOERR != ret)
        return ret;

    /* check that there actually is a file */
    if (S_ISDIR(node->mode))
        return -EISDIR;

    /* count successful open */
    node->refcnt++;

    /* initialize the file object */
    filp->f_flags |= (mode & FILE_MODE_MASK);
    filp->f_ops    = &ramfs_fileops;
    filp->f_offset = (O_APPEND & mode) ? node->size : 0;
    filp->f_data   = node;

    return ENOERR;
}

STATIC INT ramfs_mkdir(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo)
{
    RAMFS_DIRSEARCH ds;
    RAMFS_NODE *node;
    INT ret;

    init_dirsearch(&ds, pathinfo->dir, pathinfo->name);

    ret = ramfs_find(&ds);

    if (-ENOENT == ret) {
        if (TRUE == ds.last) {
            /* create new node */
            node = alloc_node(__stat_mode_DIR|S_IRWXU|S_IRWXG|S_IRWXO);
            if (NULL == node)
                return -ENOSPC;

            /* add node's dir entry to its parent node */
            ret = add_dirent(ds.dir, ds.name, ds.namelen, node);
            if (ENOERR != ret) {
                free_buffer(node);
                free_node(node);
                return ret;
            }
        }
        /* if this was not the last element, then and intermediate
           directory does not exist.
        */
    } else if (ENOERR == ret) {
        /* if there is no error, something already exists with that name */
        ret = -EEXIST;
    }

    return ret;
}

/* Remove a directory. FIXME: Empty directory ??? */
STATIC INT ramfs_rmdir(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo)
{
    RAMFS_DIRSEARCH ds;
    INT ret;

    init_dirsearch(&ds, pathinfo->dir, pathinfo->name);

    ret = ramfs_find(&ds);

    if (ENOERR != ret)
        return ret;

    /* Check that this is actually a directory. */
    if (!S_ISDIR(ds.node->mode))
        return -EPERM;

    /* Delete the entry. This will adjust the link values
       accordingly and if the directory is now unreferenced,
       will cause it to be deleted.
     */

    ret = delete_dirent(ds.dir, ds.name, ds.namelen);

    return ret;
}

/* Change directory support. */
STATIC INT ramfs_chdir(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo, VFS_DIR *outdir)
{
    if (NULL != outdir) {
        /* This is a request to get a new directory pointer in *outdir */
        RAMFS_DIRSEARCH ds;
        INT ret;

        init_dirsearch(&ds, pathinfo->dir, pathinfo->name);

        ret = ramfs_find(&ds);

        if (ENOERR != ret)
            return ret;

        /* Check that this is a directory. */
        if (!S_ISDIR(ds.node->mode))
            return -ENOTDIR;

        /* Increment ref count to keep this directory in existent
            while it is the current curr_dir.
         */
        ds.node->refcnt++;

        /* pass it out */
        *outdir = ds.node;
    } else {
        /* If no output dir is required, this means that the mnt and
           dir arguments are the current curr_dir setting and we should
           forget this fact.
         */

        RAMFS_NODE *node = pathinfo->dir;

        /* Just decrement directory reference count. */
        dec_refcnt(node);
    }

    return ENOERR;
}

/* rename a file/dir. dir -->> dir maybe memory leak */
STATIC INT ramfs_rename(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo1,
                        VFS_PATH_INFO *pathinfo2)
{
    RAMFS_DIRSEARCH ds1, ds2;
    INT ret;

    init_dirsearch(&ds1, pathinfo1->dir, pathinfo1->name);

    ret = ramfs_find(&ds1);

    if (ENOERR != ret)
        return ret;

    init_dirsearch(&ds2, pathinfo2->dir, pathinfo2->name);

    ret = ramfs_find(&ds2);

    /* allow through renames to non-existing objects */
    if ((TRUE == ds2.last) && (-ENOENT == ret))
        ds2.node = NULL, ret = ENOERR;

    if (ENOERR != ret)
        return ret;

    /* null rename, just return */
    if (ds1.node == ds2.node)
        return ENOERR;

    /* deal with any entry that is at the destination */
    if (NULL != ds2.node) {
        /* Check that we are renaming like-for-like */
        if (!S_ISDIR(ds1.node->mode) && S_ISDIR(ds2.node->mode))
            return -EISDIR;
        if (S_ISDIR(ds1.node->mode) && !S_ISDIR(ds2.node->mode))
            return -ENOTDIR;

        /* Now delete the destination directory entry */
        ret = delete_dirent(ds2.dir, ds2.name, ds2.namelen);
        if (ENOERR != ret)
            return ret;
    }

    /* Now make a new direntry at the destination
       and delete the old entry at the source.
     */
    ret = add_dirent(ds2.dir, ds2.name, ds2.namelen, ds1.node);
    if (ENOERR == ret)
        ret = delete_dirent(ds1.dir, ds1.name, ds1.namelen);

    /* update times */
    if (ENOERR == ret) {
        ds1.dir->ctime = ds1.dir->mtime = 0;
        ds2.dir->ctime = ds2.dir->mtime = 0;
    }

    return ret;
}

/* make a new dir entry for a file */
STATIC INT ramfs_link(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo1,
                      VFS_PATH_INFO *pathinfo2)
{
    RAMFS_DIRSEARCH ds1, ds2;
    INT ret;

    init_dirsearch(&ds1, pathinfo1->dir, pathinfo1->name);

    ret = ramfs_find(&ds1);

    if (ENOERR != ret)
        return ret;

    init_dirsearch(&ds2, pathinfo2->dir, pathinfo2->name);

    ret = ramfs_find(&ds2);

    if (ENOERR == ret) {
        /* don't allow links to existing objects */
        return -EEXIST;
    }

    /* allow through links to non-existing terminal objects */
    if ((TRUE == ds2.last) && (-ENOENT == ret))
        ds2.node = NULL, ret = ENOERR;

    if (ENOERR != ret)
        return ret;

    /* now we know that there is no existing node at the destination,
       make a new dirent at the destination.
     */

    ret = add_dirent(ds2.dir, ds2.name, ds2.namelen, ds1.node);
    if (ENOERR == ret)
        ds1.node->ctime = ds2.dir->ctime = ds2.dir->mtime = 0;

    return ret;
}

/* Remove a file link from its directory. */
STATIC INT ramfs_unlink(VFS_MOUNT *mnt, VFS_PATH_INFO *pathinfo)
{
    RAMFS_DIRSEARCH ds;
    INT ret;

    init_dirsearch(&ds, pathinfo->dir, pathinfo->name);

    ret = ramfs_find(&ds);

    if (ENOERR != ret)
        return ret;

    /* cannot unlink directories, use rmdir() instead */
    if (S_ISDIR(ds.node->mode))
        return -EPERM;

    /* delete it from its directory */
    ret = delete_dirent(ds.dir, ds.name, ds.namelen);

    return ret;
}

VFS_FILE_SYSTEM_OPERATIONS ramfs_operations = {
    .mount   = ramfs_mount,
    .umount  = NULL,
    .open    = ramfs_open,
    .mkdir   = ramfs_mkdir,
    .rmdir   = ramfs_rmdir,
    .chdir   = ramfs_chdir,
    .rename  = ramfs_rename,
    .link    = ramfs_link,
    .unlink  = ramfs_unlink,
    .symlink = NULL,
};

FS_TBL_ENTRY(rootfs_entry, "rootfs", 0, &ramfs_operations);
FS_TBL_ENTRY(ramfs_entry,  "ramfs",  0, &ramfs_operations);

STATIC VOID mod_init_ramfs(VOID)
{
    MEM_POOL_INIT(ramfs_block_pool, ramfs_block_pool_map,
                  ramfs_block_pool_mem, WQ_TYPE_FIFO);
    MEM_POOL_INIT(ramfs_node_pool, ramfs_node_pool_map,
                  ramfs_node_pool_mem, WQ_TYPE_FIFO);
}

MOD_INIT_CALL(mod_init_ramfs, MOD_RAMFS_LVL);

VOID init_rootfs(VOID)
{
    INT ret;

    ret = mount("/", "rootfs", "rootfs");
    BUG_ON(ENOERR != ret);

    ret = chdir("/");
    BUG_ON(ENOERR != ret);
    BUG_ON(NULL == curr_mnt || NULL == curr_dir);

    //ret = mkdir("/home", 0);
    BUG_ON(ENOERR != ret);

    //ret = mkdir("/proc", 0);
    BUG_ON(ENOERR != ret);

    //ret = mkdir("/dev", 0);
    BUG_ON(ENOERR != ret);
}

/******************************************************************************/
