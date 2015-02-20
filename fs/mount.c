/**MOD+************************************************************************/
/* Module:  mount.c                                                           */
/*                                                                            */
/* Purpose: mount function of MiniOS VFS                                      */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"

VFS_MOUNT mnt_table[MNT_MAX_NR];

extern VFS_FILE_SYSTEM __FS_TBL_START__, __FS_TBL_END__;

INT mount(CONST CHAR *dir, CONST CHAR *fs_name, CONST CHAR *dev_name)
{
    VFS_MOUNT *m;
    VFS_FILE_SYSTEM *fs;
    INT res;
    INT i;

    /* Search the mount table for an empty entry */
    for (i = 0; i < MNT_MAX_NR; i++) {
        m = &mnt_table[i];
        if (NULL == m->mnt_name)
            break;
    }

    if (i == MNT_MAX_NR)
        return -ENOMEM;

    /* Search the fs table for the filesystem implementaion */
    for (fs = &__FS_TBL_START__; fs != &__FS_TBL_END__; fs++) {
        if (!strcmp(fs_name, fs->fs_name))
            break;
    }

    if (fs == &__FS_TBL_END__)
        return -ENODEV;

    /* Match one */
    m->mnt_fsname = fs_name;
    m->mnt_devname = dev_name;
    m->mnt_name = dir;

    res = fs->fs_ops->mount(m, fs);
    if (ENOERR == res)
        m->mnt_fs = fs;
    else
        m->mnt_name = NULL;

    return res;
}

INT umount(CONST CHAR *name)
{
    INT i;
    INT res;
    VFS_MOUNT *m;

    for (i = 0; i < MNT_MAX_NR; i++) {
        m = &mnt_table[i];

        /* ignore empty */
        if (NULL == m->mnt_name)
            continue;

        /* match names */
        if (0 == strcmp(name, m->mnt_name))
            break;
    }

    if (i == MNT_MAX_NR)
        return -EINVAL;

    res = m->mnt_fs->fs_ops->umount(m);
    if (ENOERR == res)
        m->mnt_name = NULL;

    return res;
}

VFS_MOUNT *find_mnt(CONST CHAR *name)
{
    VFS_MOUNT *m;
    INT i;

    for (i = 0; i < MNT_MAX_NR; i++) {
        m = &mnt_table[i];
        if (NULL == m->mnt_name)
            continue;

        if (0 == strcmp(m->mnt_name, name))
            break;
    }

    return (i == MNT_MAX_NR) ? NULL : m;
}

/******************************************************************************/
