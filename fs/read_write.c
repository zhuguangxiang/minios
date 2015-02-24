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

#include "common/stdarg.h"
#include "fs/fs.h"

STATIC INT32 do_filp_read(VFS_FILE *filp, VOID *buf, UINT32 len)
{
    INT32 read_len;

    if (!(filp->f_flags & FREAD))
        return -EBADF;

    if (!filp->f_ops || !filp->f_ops->read)
        return -EINVAL;

    read_len = filp->f_ops->read(filp, buf, len);

    return read_len;
}

INT32 read(INT fd, VOID *buf, UINT32 len)
{
    INT32 read_len;
    VFS_FILE *filp = fget(fd);

    if (NULL == filp)
        return -EBADF;

    read_len = do_filp_read(filp, buf, len);

    fput(filp);

    return read_len;
}

STATIC INT32 do_filp_write(VFS_FILE *filp, VOID *buf, UINT32 len)
{
    INT32 write_len;

    if (!(filp->f_flags & FWRITE))
        return -EBADF;

    if (!filp->f_ops || !filp->f_ops->write)
        return -EINVAL;

    write_len = filp->f_ops->write(filp, buf, len);

    return write_len;
}

INT32 write(INT fd, VOID *buf, UINT32 len)
{
    INT32 write_len;
    VFS_FILE *filp = fget(fd);

    if (NULL == filp)
        return -EBADF;

    write_len = do_filp_write(filp, buf, len);

    fput(filp);

    return write_len;
}

INT32 lseek(INT fd, INT32 offset, INT whence)
{
    INT32 ret = -EBADF;
    VFS_FILE *filp = fget(fd);

    if (NULL == filp)
        return ret;

    ret = -EINVAL;

    if (whence <= SEEK_MAX) {
        if (filp->f_ops && filp->f_ops->lseek)
            ret = filp->f_ops->lseek(filp, &offset, whence);
    }

    fput(filp);

    if (ENOERR != ret)
        return ret;
    else
        return offset;
}

INT fstat(INT fd, VFS_STAT_INFO *stat)
{
    INT ret;
    VFS_FILE *filp;

    if (NULL == stat)
        return -EINVAL;

    filp = fget(fd);
    if (NULL == filp)
        return -EBADF;

    ret = -ENOTSUP;

    if (filp->f_ops && filp->f_ops->fstat)
        ret = filp->f_ops->fstat(filp, stat);

    fput(filp);
    return ret;
}

INT ioctl(INT fd, INT cmd, ...)
{
    INT ret;
    VFS_FILE *filp;

    filp = fget(fd);
    if (NULL == filp)
        return -EBADF;

    ret = -ENOTSUP;

    if (filp->f_ops && filp->f_ops->ioctl) {
        va_list args;
        VOID *data;
        va_start(args, cmd);
        data = va_arg(args, VOID *);
        va_end(args);
        ret = filp->f_ops->ioctl(filp, cmd, data);
    }

    fput(filp);
    return ret;
}

/******************************************************************************/
