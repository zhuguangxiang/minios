/**INC+************************************************************************/
/* Header:  stat.h                                                            */
/*                                                                            */
/* Purpose: POSIX file characteristics                                        */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_FS_STAT_H_
#define _MINIOS_FS_STAT_H_

#define __stat_mode_DIR    (1<<0)
#define __stat_mode_CHR    (1<<1)
#define __stat_mode_BLK    (1<<2)
#define __stat_mode_REG    (1<<3)
#define __stat_mode_FIFO   (1<<4)
#define __stat_mode_MQ     (1<<5)
#define __stat_mode_SEM    (1<<6)
#define __stat_mode_SHM    (1<<7)
#define __stat_mode_LNK    (1<<8)
#define __stat_mode_SOCK   (1<<9)

#define S_ISDIR(__mode)    ((__mode) & __stat_mode_DIR )
#define S_ISCHR(__mode)    ((__mode) & __stat_mode_CHR )
#define S_ISBLK(__mode)    ((__mode) & __stat_mode_BLK )
#define S_ISREG(__mode)    ((__mode) & __stat_mode_REG )
#define S_ISFIFO(__mode)   ((__mode) & __stat_mode_FIFO )
#if !defined(_POSIX_C_SOURCE) || (_POSIX_C_SOURCE >= 200112L)
#define S_ISLNK(__mode)    ((__mode) & __stat_mode_LNK )
#define S_ISSOCK(__mode)   ((__mode) & __stat_mode_SOCK )
#endif

#define S_IRUSR  (1<<16)
#define S_IWUSR  (1<<17)
#define S_IXUSR  (1<<18)
#define S_IRWXU  (S_IRUSR|S_IWUSR|S_IXUSR)

#define S_IRGRP  (1<<19)
#define S_IWGRP  (1<<20)
#define S_IXGRP  (1<<21)
#define S_IRWXG  (S_IRGRP|S_IWGRP|S_IXGRP)

#define S_IROTH  (1<<22)
#define S_IWOTH  (1<<23)
#define S_IXOTH  (1<<24)
#define S_IRWXO  (S_IROTH|S_IWOTH|S_IXOTH)

#define S_ISUID  (1<<25)
#define S_ISGID  (1<<26)

#endif /* _MINIOS_STAT_H_ */

/******************************************************************************/
