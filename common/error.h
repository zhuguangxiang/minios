/**INC+************************************************************************/
/* Header:  error.h                                                           */
/*                                                                            */
/* Purpose: To provide a common set of error codes                            */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_ERROR_H_
#define _MINIOS_ERROR_H_

#define ENOERR           0     /* No error */
#define EPERM            1     /* Not permitted */
#define ENOENT           2     /* No such entity */
#define ESRCH            3     /* No such process */
#define EINTR            4     /* Operation interrupted */
#define EIO              5     /* I/O error */
#define EBADF            9     /* Bad file handle */
#define EAGAIN           11    /* Try again later */
#define ENOMEM           12    /* Out of memory */

#define EBUSY            16    /* Resource busy */
#define EEXIST           17    /* File exists */
#define EXDEV            18    /* Cross-device link */
#define ENODEV           19    /* No such device */
#define ENOTDIR          20    /* Not a directory */
#define EISDIR           21    /* Is a directory */
#define EINVAL           22    /* Invalid argument */
#define ENFILE           23    /* Too many open files in system */
#define EMFILE           24    /* Too many open files */
#define EFBIG            27    /* File too large */
#define ENOSPC           28    /* No space left on device */
#define ESPIPE           29    /* Illegal seek */
#define EROFS            30    /* Read-only file system */
#define EDOM             33    /* Argument to math function outside valid */
#define ERANGE           34    /* Math result cannot be represented */
#define EDEADLK          35    /* Resource deadlock would occur */

#define ENOSYS           38    /* Function not implemented */
#define ENAMETOOLONG     60    /* File name too long */
#define ENOTEMPTY        66    /* Directory not empty */
#define ENOTSUP          95    /* Not supported error */
#define EEOF             200   /* End of file reached */
#define ENOSUPP          201   /* Operation not supported */
#define EDEVNOSUPP       202   /* Device does not support this operation */

#define ETIMEOUT         1000  /* Waiting resource timeout */
#define ENOAVAIL         1001  /* Resource not available */

char *strerror(int errnum);

#endif /* _MINIOS_ERROR_H_ */

/******************************************************************************/
