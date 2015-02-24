/**MOD+************************************************************************/
/* Module:  open.c                                                            */
/*                                                                            */
/* Purpose: open & close functions of MiniOS VFS                              */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"

STATIC VFS_FILE *do_filp_open(CONST CHAR *path, INT oflag, INT *error)
{
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE *filp = NULL;
    VFS_FILE_SYSTEM *fs;
    VFS_PATH_INFO pathinfo;
    INT ret;

    ret = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != ret) {
        *error = ret;
        goto error;
    }

    fs = mnt->mnt_fs;
    BUG_ON(NULL == fs);

    filp = get_empty_filp();
    if (NULL == filp) {
        *error = -EMFILE;
        goto error;
    }

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->lookup)) {
        *error = -ENOTSUP;
        goto error;
    }

    pathinfo.name = path;
    pathinfo.dir = dir;

    ret = fs->fs_ops->lookup(mnt, &pathinfo, oflag, filp);
    if (ENOERR != ret) {
        *error = ret;
        goto error;
    }

    if ((NULL != filp->f_ops) && (NULL != filp->f_ops->open))
        ret = filp->f_ops->open(filp);

    if (ENOERR != ret) {
        *error = ret;
        goto error;
    }

    filp->f_mnt = mnt;

    return filp;

error:

    if (NULL != filp)
        __fput(filp);

    return NULL;
}

INT open(CONST CHAR *path, INT oflag, ...)
{
    INT fd;
    INT error = ENOERR;

    if (0 == (oflag & O_RDWR))
        return -EINVAL;

    fd = get_unused_fd();

    if (fd >= 0) {
        VFS_FILE *filp = do_filp_open(path, oflag, &error);
        if (NULL == filp) {
            put_unused_fd(fd);
            return error;
        } else {
            fd_assign(fd, filp);
        }
    }

    return fd;
}

/* create a file, and return fd if successful */
INT creat(CONST CHAR *path, UINT32 mode)
{
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

STATIC INT do_filp_close(VFS_FILE *filp)
{
    INT ret = ENOERR;

    BUG_ON(0 == filp->f_count);

    if (filp->f_ops && filp->f_ops->flush)
        ret = filp->f_ops->flush(filp);

    fput(filp);

    return ret;
}

INT close(INT fd)
{
    INT ret;
    VFS_FILE *filp;

    mutex_lock(&fd_table.file_lock);

    BUG_ON((fd < 0) || (fd >= FD_MAX_NR));

    filp = fd_table.file_descs[fd];

    if (!FILE_PTR_VALID(filp)) {
        mutex_unlock(&fd_table.file_lock);
        return -EBADF;
    }

    __put_unused_fd(fd);

    mutex_unlock(&fd_table.file_lock);

    ret = do_filp_close(filp);

    return ret;
}

/******************************************************************************/
