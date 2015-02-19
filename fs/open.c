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

STATIC VFS_FILE *do_filp_open(CONST CHAR *path, INT oflag)
{
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE *filp;
    INT res;
    VFS_PATH_INFO path_info;

    res = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != res)
        return NULL;

    filp = get_empty_filp();
    if (NULL == filp)
        return NULL;

    path_info.name = path;
    path_info.dir = dir;

    res = mnt->mnt_fs->fs_ops->open(mnt, &path_info, oflag, filp);
    if (ENOERR != res) {
        __fput(filp);
        return NULL;
    }

    filp->f_mnt = mnt;

    return filp;
}

INT open(CONST CHAR *path, INT oflag, ...)
{
    INT fd;

    if (0 == (oflag & O_RDWR))
        return -EINVAL;

    fd = get_unused_fd();

    if (fd >= 0) {
        VFS_FILE *filp = do_filp_open(path, oflag);
        if (NULL == filp) {
            put_unused_fd(fd);
            fd = -1;
        } else {
            fd_assign(fd, filp);
        }
    }

    return fd;
}

STATIC INT do_filp_close(VFS_FILE *filp)
{
    INT res = ENOERR;

    BUG_ON(0 == filp->f_count);

    if (filp->f_ops && filp->f_ops->flush)
        res = filp->f_ops->flush(filp);

    fput(filp);

    return res;
}

INT close(INT fd)
{
    INT res;
    VFS_FILE *filp;

    filp = fget(fd);
    if (NULL == filp)
        return -EBADF;

    put_unused_fd(fd);

    res = do_filp_close(filp);

    return res;
}

/******************************************************************************/
