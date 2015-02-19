#include <stdio.h>
#include "fs/vfs.h"

extern VFS_MOUNT mnt_table[];


extern VFS_MOUNT *root_mnt;
extern VFS_DIR root_dir;
extern VFS_MOUNT *curr_mnt;
extern VFS_DIR curr_dir;

int main()
{
	char *path;
	VFS_MOUNT *mnt;
	VFS_DIR dir;

	vfs_initialize();
	root_mnt = &mnt_table[0];
	root_dir = "root_dir";
	curr_mnt = &mnt_table[1];
	curr_dir = "home_dir";

	mnt_table[0].mnt_name = "/";
	mnt_table[1].mnt_name = "/home";	
	mnt_table[2].mnt_name = "/home/abc";	
	mnt_table[3].mnt_name = "/home/abc/def";	

	path = "abc/de";
	vfs_path_lookup(&path, &mnt, &dir);
	printf("%s-%s-%s\n", path, dir, mnt->mnt_name);

	path = "/home/a";
	vfs_path_lookup(&path, &mnt, &dir);
	printf("%s-%s-%s\n", path, dir, mnt->mnt_name);

    path = "/home/abc/d";
	vfs_path_lookup(&path, &mnt, &dir);
	printf("%s-%s-%s\n", path, dir, mnt->mnt_name);
	return 0;
}
