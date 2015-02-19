/**MOD+************************************************************************/
/* Module:  read_write.c                                                      */
/*                                                                            */
/* Purpose: read, write & lseek functions of MiniOS VFS                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"

STATIC INT do_filp_read(VFS_FILE *filp, VOID *buf, UINT32 len, UINT32 *pos)
{
    INT res;

    if (!(filp->f_flags & O_RDONLY))
        return -EBADF;

    if (!filp->f_ops || !filp->f_ops->read)
        return -EINVAL;

    res = filp->f_ops->read(filp, buf, len, pos);

    return res;
}

INT read(INT fd, VOID *buf, UINT32 len)
{
    INT res = -EBADF;
    VFS_FILE *filp = fget(fd);

    if (NULL != filp) {
        UINT32 pos = filp->f_offset;
        res = do_filp_read(filp, buf, len, &pos);
        filp->f_offset = pos;
        fput(filp);
    }

    return res;
}

STATIC INT do_filp_write(VFS_FILE *filp, VOID *buf, UINT32 len, UINT32 *pos)
{
    INT res;

    if (!(filp->f_flags & O_WRONLY))
        return -EBADF;

    if (!filp->f_ops || !filp->f_ops->write)
        return -EINVAL;

    res = filp->f_ops->write(filp, buf, len, pos);

    return res;
}

INT write(INT fd, VOID *buf, UINT32 len)
{
    INT res = -EBADF;
    VFS_FILE *filp = fget(fd);

    if (NULL != filp) {
        UINT32 pos = filp->f_offset;
        res = do_filp_write(filp, buf, len, &pos);
        filp->f_offset = pos;
        fput(filp);
    }

    return res;
}

INT lseek(INT fd, INT32 offset, INT whence)
{
    INT res = -EBADF;
    VFS_FILE *filp = fget(fd);

    if (NULL == filp)
        return res;

    res = -EINVAL;

    if (whence <= SEEK_MAX) {
        if (filp->f_ops && filp->f_ops->lseek)
            res = filp->f_ops->lseek(filp, offset, whence);
    }

    fput(filp);

    return res;
}

/******************************************************************************/
