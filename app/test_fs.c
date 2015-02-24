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
#include "fs/stat.h"
#include "common/stdio.h"
#include "common/error.h"

void test_fs_rename_file(void)
{
	char *hello;
	int ret;
	int fd = creat("/a.txt", 0);
	if (fd >= 0)
	{
		hello = "hello,hello,minios ramfs";
		ret = write(fd, hello, strlen(hello));
		if (ret > 0)
			printf("write count: %d\r\n", ret);
		else
			printf("write error: %s\r\n", strerror(ret));
	}
	else
	{
		printf("creat a.txt failed\r\n");
	}

	close(fd);

	ret = rename("/a.txt", "/b.txt");
	printf("rename:%d\r\n", ret);

////////////////////////////////////////////////////////////////////////	

	fd = creat("/a.txt", 0);
	if (fd >= 0)
	{
		hello = "hi,hi,minios ramfs";
		ret = write(fd, hello, strlen(hello));
		if (ret > 0)
			printf("write count: %d\r\n", ret);
		else
			printf("write error: %s\r\n", strerror(ret));
	}
	else
	{
		printf("creat a.txt failed\r\n");
	}

	ret = rename("/a.txt", "/b.txt");
	printf("rename:%d\r\n", ret);

	hello = "this is a test minios-vfs";
	ret = write(fd, hello, strlen(hello));
	if (ret > 0)
		printf("write count: %d\r\n", ret);
	else
		printf("write error: %s\r\n", strerror(ret));

	close(fd);

	//reopen must be failed
	fd = open("/a.txt", O_RDWR);
	BUG_ON(fd >= 0);

	fd = open("/b.txt", O_RDONLY);
	BUG_ON(fd < 0);	
}

void test_fs1(void)
{
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

/////////////////////////////////////////////////////////////////////
	
	char *hello = "hello, minios";
	int len = write(fd, hello, strlen(hello));
	printf("fd:%d, write %d\r\n", fd, len);
	close(fd);

/////////////////////////////////////////////////////////////////////

	fd = open("a.txt", O_RDONLY);
	char buf[100] = {0};
	len = read(fd, buf, 100);
	printf("fd:%d, read:%d, %s\r\n", fd, len, buf);
	close(fd);

/////////////////////////////////////////////////////////////////////

	int ret = mkdir("/abc/def", 0);
	printf("mkdir ret: %s\r\n", strerror(ret));

	ret = mkdir("/home/", 0);
	printf("mkdir ret: %s\r\n", strerror(ret));

/////////////////////////////////////////////////////////////////////
	fd = creat("/home/a.txt", 0);
	printf("creat ret: %s\r\n", strerror(fd));
	if (fd >= 0)
	{
		hello = "hello,hello,minios ramfs";
		ret = write(fd, hello, strlen(hello));
		if (ret > 0)
			printf("write count: %d\r\n", ret);
		else
			printf("write error: %s\r\n", strerror(ret));

		ret = read(fd, buf, sizeof(buf));
		if (ret > 0)
			printf("read:%s\r\n", buf);
		else
			printf("read error: %s\r\n", strerror(ret));
	}

	int fd1 = open("/home/a.txt", O_RDONLY);
	if (fd1 >= 0)
	{
		ret = read(fd1, buf, sizeof(buf));
		if (ret > 0)
			printf("read:%s\r\n", buf);
		else
			printf("read error: %s\r\n", strerror(ret));

		ret = lseek(fd1, 5, SEEK_SET);
		if (ret > 0)
		{
			printf("lseek:%d\r\n", ret);
		}
		else
		{
			printf("lseek:%s\r\n", strerror(ret));
		}

		ret = read(fd1, buf, sizeof(buf));
		if (ret > 0)
			printf("read:%s\r\n", buf);
		else
			printf("read error: %s\r\n", strerror(ret));	
	}

	close(fd);
	close(fd1);

	fd = creat("/home/abcdefghijklmnopqrstuvwxyz1234567890.txt", 0);
	if (fd < 0)
		printf("creat long name : %s\r\n", strerror(fd));
	else
		printf("creat long name successfully??\r\n");

/////////////////////////////////////////////////////////////////////
	ret = link("/home/a.txt", "/b.txt");
	if (0 != ret)
	{	
		printf("link :%s\r\n", strerror(ret));
	}	
	else
	{
		printf("link succesfully\r\n");
	}

	fd = open("/b.txt", O_RDONLY);
	if (fd >= 0)
	{
		ret = read(fd, buf, sizeof(buf));
		printf("link read:%d, %s\r\n", ret, buf);
	}
	else
	{
		printf("open link file failed:%s\r\n", strerror(fd));
	}
}

char *node_permission(UINT32 mode)
{
	char *s;
	switch (mode & S_IFMT)
	{
		case S_IFDIR:
			s = "drwxrwxrwx";
			break;
		case S_IFREG:
			s = "-rwxrwxrwx";
			break;
		default:
			s = "none";
			break;
	}

	return s;
}

void test_ls(char *path)
{
	int fd, ret;
	VFS_DIRENT dirinfo;
	VFS_STAT_INFO statinfo;
	char fullpath[128];

	printf("list %s:\r\n", path);

	stat(path, &statinfo);
	if (!(statinfo.st_mode & S_IFDIR))
	{
		char *name = strrchr(path, '/');
		if (!name) name = path;
		else name++;

		printf("%s  %d  %4d  %s\r\n", node_permission(statinfo.st_mode),
				statinfo.st_nlink, 
				(statinfo.st_mode & S_IFDIR) ? 4096 : statinfo.st_size,
				name);
		return;
	}

	fd = open(path, O_RDONLY);
	ret = read(fd, &dirinfo, sizeof(dirinfo));
	if (ret > 0) {
		snprintf(fullpath, 511, "%s/%s", path, dirinfo.name);
		stat(fullpath, &statinfo);
		printf("%s  %d  %4d  %s\r\n", node_permission(statinfo.st_mode),
				statinfo.st_nlink, 
				(statinfo.st_mode & S_IFDIR) ? 4096 : statinfo.st_size,
				dirinfo.name);
    }
	while (ret > 0) {
		ret = read(fd, &dirinfo, sizeof(dirinfo));
		if (ret > 0) {
			snprintf(fullpath, 511, "%s/%s", path, dirinfo.name);
			stat(fullpath, &statinfo);
			printf("%s  %d  %4d  %s\r\n", node_permission(statinfo.st_mode),
					statinfo.st_nlink, 
					(statinfo.st_mode & S_IFDIR) ? 4096 : statinfo.st_size,
					dirinfo.name);
		}
	}
}

void test_fs_dir(void)
{
	int ret;
	ret = mkdir("/home", 0);
	printf("mkdir ret:%d\r\n", ret);
	ret = mkdir("/dev", 0);
	printf("mkdir ret:%d\r\n", ret);
	ret = mkdir("/sys", 0);
	printf("mkdir ret:%d\r\n", ret);
	ret = mkdir("/mnt", 0);
	printf("mkdir ret:%d\r\n", ret);

	int fd = creat("/a.txt", 0);
	char *hi = "hello, minios vfs";
	write(fd, hi, strlen(hi));
	close(fd);
	
	test_ls("/.");

	rmdir("/sys");

	test_ls("/.");

	test_ls("/a.txt");

	fd = creat("/home/b.txt", 0);
	hi = "hello, home";
	write(fd, hi, strlen(hi));
	close(fd);

	test_ls("/home");
}

void test_fs(void)
{
	init_rootfs();

	test_fs_dir();
	//test_fs_rename_file();
	//test_fs_rename_dir();
	//test_fs_link();
	//test_fs_unlink();
	//test_fs_mkdir();
	//test_fs_rmdir();
}

/******************************************************************************/
