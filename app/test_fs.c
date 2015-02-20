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
#include "common/stdio.h"
#include "common/error.h"

void test_fs(void)
{
    init_fd_table();
	init_rootfs();

	int fd = open("/a.txt", O_RDWR);
	if (fd < 0) {
		printf("open a.txt failed\r\n");
		fd = open("/a.txt", O_WRONLY|O_CREAT);
		if (fd < 0)
			printf("open a.txt failed with O_CREAT\r\n");
		else
			printf("open a.txt successfully\r\n");
	}
	
	char *hello = "hello, minios";
	int len = write(fd, hello, strlen(hello));
	printf("fd:%d, write %d\r\n", fd, len);
	close(fd);

	fd = open("a.txt", O_RDONLY);
	char buf[100] = {0};
	len = read(fd, buf, 100);
	printf("fd:%d, read:%d, %s\r\n", fd, len, buf);
	close(fd);

	int res = mkdir("/abc/def", 0);
	printf("mkdir res: %s\r\n", strerror(res));

	res = mkdir("/home/", 0);
	printf("mkdir res: %s\r\n", strerror(res));
}



/******************************************************************************/
