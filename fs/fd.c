/**MOD+************************************************************************/
/* Module:  fd.c                                                              */
/*                                                                            */
/* Purpose: file descriptor implementation                                    */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"
#include "mm/mem_pool.h"

STATIC MEM_POOL file_pool;
STATIC POOL_MAP(file_pool_map, FILE_MAX_NR);
STATIC POOL_MEM(file_pool_mem, FILE_MAX_NR, sizeof(VFS_FILE));

VFS_FD_TBL fd_table;

STATIC VOID mod_init_fd_table(VOID)
{
    MEM_POOL_INIT(file_pool, file_pool_map, file_pool_mem, WQ_TYPE_FIFO);
    init_mutex(&fd_table.file_lock, MUTEX_PROTOCOL_INHERIT, WQ_TYPE_FIFO);
}

MOD_INIT_CALL(mod_init_fd_table, MOD_FDTBL_LVL);

VFS_FILE *get_empty_filp(VOID)
{
    VFS_FILE *filp = mem_pool_alloc(&file_pool);

    if (NULL != filp) {
        memset(filp, 0, sizeof(*filp));
        filp->f_count = 1;
    }

    return filp;
}

VFS_FILE *__fget(INT fd)
{
    VFS_FILE *filp;

    BUG_ON((fd < 0) || (fd >= FD_MAX_NR));

    filp = fd_table.file_descs[fd];

    if (FILE_PTR_VALID(filp))
        ++filp->f_count;
    else
        filp = NULL;

    return filp;
}

VFS_FILE *fget(INT fd)
{
    VFS_FILE *filp;

    mutex_lock(&fd_table.file_lock);
    filp = __fget(fd);
    mutex_unlock(&fd_table.file_lock);

    return filp;
}

VOID __fput(VFS_FILE *filp)
{
    if ((--filp->f_count) <= 0) {
        if (filp->f_ops && filp->f_ops->release)
            filp->f_ops->release(filp);
        mem_pool_free(&file_pool, filp);
    }
}

VOID fput(VFS_FILE *filp)
{
    mutex_lock(&fd_table.file_lock);
    __fput(filp);
    mutex_unlock(&fd_table.file_lock);
}

INT __get_unused_fd(VOID)
{
    INT fd;

    for (fd = 0; fd < FD_MAX_NR; fd++) {
        if (NULL == fd_table.file_descs[fd]) {
            fd_table.file_descs[fd] = FD_ALLOCATED;
            break;
        }
    }

    if (FD_MAX_NR == fd)
        fd = -1;

    return fd;
}

INT get_unused_fd(VOID)
{
    INT fd;

    mutex_lock(&fd_table.file_lock);
    fd = __get_unused_fd();
    mutex_unlock(&fd_table.file_lock);

    return fd;
}

VOID __put_unused_fd(INT fd)
{
    BUG_ON((fd < 0) || (fd >= FD_MAX_NR));
    fd_table.file_descs[fd] = NULL;
}

VOID put_unused_fd(INT fd)
{
    mutex_lock(&fd_table.file_lock);
    __put_unused_fd(fd);
    mutex_unlock(&fd_table.file_lock);
}

VOID fd_assign(INT fd, VFS_FILE *filp)
{
    BUG_ON((fd < 0) || (fd >= FD_MAX_NR));
    BUG_ON(FD_ALLOCATED != fd_table.file_descs[fd]);

    mutex_lock(&fd_table.file_lock);
    fd_table.file_descs[fd] = filp;
    mutex_unlock(&fd_table.file_lock);
}

/******************************************************************************/
