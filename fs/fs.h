/**INC+************************************************************************/
/* Header:  vfs.h                                                             */
/*                                                                            */
/* Purpose: Virtual file system of MiniOS                                     */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_FS_H_
#define _MINIOS_FS_H_

#include "common/types.h"
#include "common/error.h"
#include "common/string.h"
#include "common/bug.h"
#include "kernel/mutex.h"

#define FD_MAX_NR    32
#define FILE_MAX_NR  64
#define MNT_MAX_NR   8

typedef struct vfs_file_system VFS_FILE_SYSTEM;
typedef struct vfs_mount VFS_MOUNT;
typedef VOID *VFS_DIR;
typedef struct vfs_file_operations VFS_FILE_OPERATIONS;
typedef struct vfs_file VFS_FILE;

typedef struct {
    VFS_DIR dir;
    CONST CHAR *name;
} VFS_PATH_INFO;

typedef struct {
    INT (*mount)(VFS_MOUNT *, VFS_FILE_SYSTEM *);
    INT (*umount)(VFS_MOUNT *);
    INT (*open)(VFS_MOUNT *, VFS_PATH_INFO *, UINT32 mode, VFS_FILE *);
    INT (*mkdir)(VFS_MOUNT *, VFS_PATH_INFO *);
    INT (*rmdir)(VFS_MOUNT *, VFS_PATH_INFO *);
    INT (*chdir)(VFS_MOUNT *, VFS_PATH_INFO *, VFS_DIR *);
    INT (*rename)(VFS_MOUNT *, VFS_PATH_INFO *, VFS_PATH_INFO *);
    INT (*link)(VFS_MOUNT *, VFS_PATH_INFO *, VFS_PATH_INFO *);
    INT (*unlink)(VFS_MOUNT *, VFS_PATH_INFO *);
    INT (*symlink)(VFS_MOUNT *, VFS_PATH_INFO *, CONST CHAR *);
} VFS_FILE_SYSTEM_OPERATIONS;

struct vfs_file_system {
    CONST CHAR *fs_name;
    VOID *fs_data;
    VFS_FILE_SYSTEM_OPERATIONS *fs_ops;
};

#define FS_TBL_ENTRY(_l, _name, _data, _ops) \
    VFS_FILE_SYSTEM _l USER_SECTION(".fs_tbl") = {_name, _data, _ops}

struct vfs_mount {
    CONST CHAR *mnt_name;
    CONST CHAR *mnt_fsname;
    CONST CHAR *mnt_devname;

    VFS_FILE_SYSTEM *mnt_fs;
    VFS_DIR mnt_root;
    VOID *mnt_data;
};

struct vfs_file_operations {
    INT32 (*read)(VFS_FILE *, UINT8 *, UINT32);
    INT32 (*write)(VFS_FILE *, UINT8 *, UINT32);
    INT   (*lseek)(VFS_FILE *, INT32, INT);
    INT   (*flush)(VFS_FILE *);
    INT   (*release)(VFS_FILE *);
    INT   (*ioctl)(VFS_FILE *, UINT32, VOID *);
};

struct vfs_file {
    UINT32 f_flags;
    INT32  f_count;
    UINT32 f_offset;
    VOID *f_data;
    VFS_FILE_OPERATIONS *f_ops;
    VFS_MOUNT *f_mnt;
};

typedef struct {
    MUTEX file_lock;
    VFS_FILE *file_descs[FD_MAX_NR];
} VFS_FD_TBL;

/* File access modes used for open() and fnctl() */
#define O_RDONLY    (1<<0)   /* Open for reading only */
#define O_WRONLY    (1<<1)   /* Open for writing only */
#define O_RDWR      (O_RDONLY|O_WRONLY) /* Open for reading and writing */

/* File access mode mask */
#define O_ACCMODE   (O_RDONLY|O_RDWR|O_WRONLY)

/* open() mode flags */

#define O_CREAT     (1<<3)    /* Create file it it does not exist */
#define O_EXCL      (1<<4)    /* Exclusive use */
#define O_NOCTTY    (1<<5)    /* Do not assign a controlling terminal */
#define O_TRUNC     (1<<6)    /* Truncate */

/* File status flags used for open() and fcntl() */
#define O_APPEND    (1<<7)    /* Set append mode */
#define O_DSYNC     (1<<8)    /* Synchronized I/O data integrity writes */
#define O_NONBLOCK  (1<<9)    /* No delay */
#define O_RSYNC     (1<<10)   /* Synchronized read I/O */
#define O_SYNC      (1<<11)   /* Synchronized I/O file integrity writes */

#define FREAD       O_RDONLY
#define FWRITE      O_WRONLY
#define FNONBLOCK   O_NONBLOCK
#define FAPPEND     O_APPEND

// Mask for open mode bits stored in file object
#define FILE_MODE_MASK (FREAD|FWRITE|FNONBLOCK|FAPPEND)

#define SEEK_SET    0   /* seek relative to beginning of file */
#define SEEK_CUR    1   /* seek relative to current file position */
#define SEEK_END    2   /* seek relative to end of file */
#define SEEK_MAX    SEEK_END

INT vfs_path_lookup(CONST CHAR **name, VFS_MOUNT **mnt, VFS_DIR *dir);
INT mount(CONST CHAR *dir, CONST CHAR *fs_name, CONST CHAR *dev_name);
INT umount(CONST CHAR *name);

VFS_FILE *get_empty_filp(VOID);
VFS_FILE *__fget(INT fd);
VFS_FILE *fget(INT fd);
VOID __fput(VFS_FILE *filp);
VOID fput(VFS_FILE *filp);

INT __get_unused_fd(VOID);
INT get_unused_fd(VOID);
VOID __put_unused_fd(INT fd);
VOID put_unused_fd(INT fd);
VOID fd_assign(INT fd, VFS_FILE *filp);

#define FD_ALLOCATED  ((VOID *)1)
#define FILE_PTR_VALID(filp) ((NULL != filp) && (FD_ALLOCATED != filp))

INT open(CONST CHAR *path, INT oflag, ...);
INT close(INT fd);
INT read(INT fd, VOID *buf, UINT32 len);
INT write(INT fd, VOID *buf, UINT32 len);
INT lseek(INT fd, INT32 offset, INT whence);
INT mkdir(CONST CHAR *path, UINT32 mode);

extern VFS_FD_TBL fd_table;
extern VFS_MOUNT mnt_table[MNT_MAX_NR];
extern VFS_MOUNT *curr_mnt;
extern VFS_DIR curr_dir;

VFS_MOUNT *find_mnt(CONST CHAR *name);
VOID init_fd_table(VOID);
VOID init_rootfs(VOID);

#endif /* _MINIOS_FS_H_ */

/******************************************************************************/
