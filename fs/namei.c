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
    INT res;
    VFS_MOUNT *mnt;
    VFS_DIR dir;
    VFS_FILE_SYSTEM *fs;
    VFS_PATH_INFO pathinfo;

    res = vfs_path_lookup(&path, &mnt, &dir);
    if (ENOERR != res)
        return -ENOENT;

    BUG_ON(NULL == mnt->mnt_fs);
    fs = mnt->mnt_fs;

    if ((NULL == fs->fs_ops) || (NULL == fs->fs_ops->mkdir))
        return -EPERM;

    pathinfo.name = path;
    pathinfo.dir = dir;

    res = fs->fs_ops->mkdir(mnt, &pathinfo);

    return res;
}

/******************************************************************************/
