/**MOD+************************************************************************/
/* Module:  test_fs.c                                                         */
/*                                                                            */
/* Purpose: fs test                                                           */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "fs/fs.h"


void test_fs(void)
{
    init_fd_table();
	init_rootfs();
}



/******************************************************************************/
