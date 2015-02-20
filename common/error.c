/**MOD+************************************************************************/
/* Module:  error.c                                                           */
/*                                                                            */
/* Purpose: To provide the strerror() implementation                          */
/*          This implements strerror() as described in ANSI chap 7.11.6.2     */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/error.h"

struct error_s {
    int errnum;
    char *errstr;
} errors[] = {
    {ENOERR,     "No error"},
    {-EPERM,     "Not permitted"},
    {-ENOENT,    "No such entity"},
    {-ESRCH,     "No such process"},
    {-EBADF,     "Bad file handle"},
    {-ENOMEM,    "Out of memory"},
    {-EEXIST,    "File exists"},
    {-ENOTDIR,   "Not a directory"},
    {-EISDIR,    "Is a directory"},
    {-EINVAL,    "Invalid argument"},
    {-ENOSPC,    "No space left on device"},
};

char *strerror(int errnum)
{
    for (int i = 0; i < sizeof(errors)/sizeof(errors[0]); i++) {
        if (errors[i].errnum == errnum)
            return errors[i].errstr;
    }

    return "Unknown error";
}

/******************************************************************************/
