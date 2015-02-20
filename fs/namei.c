/**MOD+************************************************************************/
/* Module:  namei.c                                                           */
/*                                                                            */
/* Purpose: path lookup & directory functions of MiniOS VFS                   */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"

VFS_MOUNT *curr_mnt;
VFS_DIR curr_dir;

INT match_len(CONST CHAR *s1, CONST CHAR *s2)
{
    INT len = 0;

    while (s1[len] && s2[len] && (s1[len] == s2[len]))
        len++;

    /*
     * return length only if s2 is an initial substring of s1,
     * and it terminates in s1 at end of string or a '/'.
     */

    /* special case for s2 == "/" */
    if ((1 == len) && ('/' == s2[0]) && (0 == s2[1]))
        return len;

    if ((0 == s2[len]) && ((0 == s1[len]) || ('/' == s1[len])))
        return len;
    else
        return 0;
}

INT vfs_path_lookup(CONST CHAR **name, VFS_MOUNT **mnt, VFS_DIR *dir)
{
    VFS_MOUNT *best;
    VFS_MOUNT *m;
    INT best_len = 0;
    INT i;
    INT len;
    INT mnt_len;

    if ('/' == **name) {
        /* search from root mount */
        for (i = 0; i < MNT_MAX_NR; i++) {
            m = &mnt_table[i];
            if (NULL == m->mnt_name)
                continue;

            len = match_len(*name, m->mnt_name);
            if (len > best_len)
                best = m, best_len = len;
        }

        if (0 == best_len) return -EINVAL;

        *dir = best->mnt_root;

    } else {
        /* search from current mount */
        *mnt = curr_mnt;
        *dir = curr_dir;

        best = *mnt;
        mnt_len = strlen((*mnt)->mnt_name);

        for (i = 0; i < MNT_MAX_NR; i++) {
            m = &mnt_table[i];
            if (NULL == m->mnt_name)
                continue;

            len = match_len(m->mnt_name, (*mnt)->mnt_name);
            if (len == mnt_len) {
                if ('/' == m->mnt_name[len])
                    len++;

                len = match_len(*name, &m->mnt_name[len]);
                if (len > best_len)
                    best = m, best_len = len;
            }
        }

        if (best != *mnt)
            *dir = best->mnt_root;
    }

    *name += best_len;
    if ('/' == **name)
        (*name)++;

    *mnt = best;

    return ENOERR;
}

/* make a directory */
INT mkdir(CONST CHAR *path, UINT32 mode)
{
    INT ret;
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE_SYSTEM *fs;
    VFS_PATH_INFO pathinfo;

    ret = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != ret)
        return -ENOENT;

    BUG_ON(NULL == mnt->mnt_fs);
    fs = mnt->mnt_fs;

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->mkdir))
        return -EPERM;

    pathinfo.name = path;
    pathinfo.dir = dir;

    ret = fs->fs_ops->mkdir(mnt, &pathinfo);

    return ret;
}

/* remove a directory */
INT rmdir(CONST CHAR *path)
{
    INT ret;
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE_SYSTEM *fs;
    VFS_PATH_INFO pathinfo;

    ret = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != ret)
        return -ENOENT;

    BUG_ON(NULL == mnt->mnt_fs);
    fs = mnt->mnt_fs;

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->rmdir))
        return -EPERM;

    pathinfo.name = path;
    pathinfo.dir = dir;

    ret = fs->fs_ops->rmdir(mnt, &pathinfo);

    return ret;
}

/* Change current directory */
INT chdir(CONST CHAR *path)
{
    INT ret;
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE_SYSTEM *fs;
    VFS_PATH_INFO pathinfo;
    VFS_DIR newdir;

    ret = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != ret)
        return -ENOENT;

    BUG_ON(NULL == mnt->mnt_fs);
    fs = mnt->mnt_fs;

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->chdir))
        return -EPERM;

    pathinfo.name = path;
    pathinfo.dir = dir;

    ret = fs->fs_ops->chdir(mnt, &pathinfo, &newdir);
    if (ENOERR != ret)
        return ret;

    if ((NULL != curr_mnt) && (NULL != curr_dir)) {
        /* Now detach from current curr_dir. We call the current directory's
           chdir routine with a NULL dir_out pointer.
         */
        fs = curr_mnt->mnt_fs;
        BUG_ON(NULL == fs);

        if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->chdir))
            return -EPERM;

        pathinfo.name = NULL;
        pathinfo.dir = curr_dir;

        ret = fs->fs_ops->chdir(curr_mnt, &pathinfo, NULL);
        if (ENOERR != ret)
            return ret;
    }

    curr_mnt = mnt;
    curr_dir = newdir;

    return ENOERR;
}

/* rename a file */
INT rename(CONST CHAR *path1, CONST CHAR *path2)
{
    INT ret = ENOERR;
    VFS_MOUNT *mnt1, *mnt2;
    VFS_DIR dir1, dir2;
    VFS_PATH_INFO pathinfo1, pathinfo2;
    VFS_FILE_SYSTEM *fs;

    ret = vfs_path_lookup(&path1, &mnt1, &dir1);
    if (ENOERR != ret)
        return -ENOENT;

    ret = vfs_path_lookup(&path2, &mnt2, &dir2);
    if (ENOERR != ret)
        return -ENOENT;

    /* cannot rename between different filesystems */
    if (mnt1 != mnt2)
        return -EXDEV;

    fs = mnt1->mnt_fs;
    BUG_ON(NULL == fs);

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->rename))
        return -ENOTSUP;

    pathinfo1.name = path1;
    pathinfo1.dir  = dir1;
    pathinfo2.name = path2;
    pathinfo2.dir  = dir2;

    ret = fs->fs_ops->rename(mnt1, &pathinfo1, &pathinfo2);

    return ret;
}

/* create a link from an existing file to a new one */
INT link(CONST CHAR *path1, CONST CHAR *path2)
{
    INT ret = ENOERR;
    VFS_MOUNT *mnt1, *mnt2;
    VFS_DIR dir1, dir2;
    VFS_PATH_INFO pathinfo1, pathinfo2;
    VFS_FILE_SYSTEM *fs;

    ret = vfs_path_lookup(&path1, &mnt1, &dir1);
    if (ENOERR != ret)
        return ret;

    ret = vfs_path_lookup(&path2, &mnt2, &dir2);
    if (ENOERR != ret)
        return ret;

    /* cannot hard-link between different filesystems */
    if (mnt1 != mnt2)
        return -EXDEV;

    fs = mnt1->mnt_fs;
    BUG_ON(NULL == fs);

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->link))
        return -ENOTSUP;

    pathinfo1.name = path1;
    pathinfo1.dir  = dir1;
    pathinfo2.name = path2;
    pathinfo2.dir  = dir2;

    ret = fs->fs_ops->link(mnt1, &pathinfo1, &pathinfo2);

    return ret;
}

/* unlink/remove a file */
INT unlink(CONST CHAR *path)
{
    INT ret;
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE_SYSTEM *fs;
    VFS_PATH_INFO pathinfo;

    ret = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != ret)
        return -ENOENT;

    BUG_ON(NULL == mnt->mnt_fs);
    fs = mnt->mnt_fs;

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->unlink))
        return -EPERM;

    pathinfo.name = path;
    pathinfo.dir = dir;

    ret = fs->fs_ops->unlink(mnt, &pathinfo);

    return ret;
}

/******************************************************************************/
