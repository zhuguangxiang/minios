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

STATIC VFS_MOUNT mnt_table[MNT_MAX_NR];
VFS_MOUNT *root_mnt;
VFS_DIR root_dir;
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
    INT best_len;
    INT mnt_len;
    INT len;
    INT i;

    if ('/' == **name) {
        /* search from root mount */
        *mnt = root_mnt;
        *dir = root_dir;
    } else {
        /* search from current mount */
        *mnt = curr_mnt;
        *dir = curr_dir;
    }

    best = *mnt;
    best_len = 0;
    mnt_len = strlen((*mnt)->mnt_name);

    for (i = 0; i < MNT_MAX_NR; i++) {

        m = &mnt_table[i];
        if (NULL == m->mnt_name)
            continue;

        len = match_len(m->mnt_name, (*mnt)->mnt_name);
        if (len == mnt_len) {
            if ('/' == **name) {
                len = 0;
            } else {
                if ('/' == m->mnt_name[len])
                    len++;
            }
            len = match_len(*name, &m->mnt_name[len]);
            if (len > best_len) {
                best = m;
                best_len = len;
            }
        }
    }

    if (best != *mnt)
        *dir = best->mnt_root;

    *name += best_len;
    if ('/' == **name)
        (*name)++;

    *mnt = best;

    return ENOERR;
}

extern VFS_FILE_SYSTEM __FS_TBL_START__, __FS_TBL_END__;

INT mount(CONST CHAR *dev_name, CONST CHAR *dir, CONST CHAR *fs_name)
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
    if (ENOERR != res) {
        m->mnt_fs = fs;
    } else {
        m->mnt_name = NULL;
    }

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

    if (i != MNT_MAX_NR)
        return -EINVAL;

    res = m->mnt_fs->fs_ops->umount(m);
    if (ENOERR == res)
        m->mnt_name = NULL;

    return res;
}

/******************************************************************************/
