#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	struct stat* stat_buffer = (struct stat*) calloc(1, sizeof(stat));
	char* filename = "file1";
	int ret = lstat(filename, stat_buffer);
	if(ret == -1)
	{
		printf("error executing lstat!\n");
		return 0;
	}
	printf("Inode: %ld\n", stat_buffer->st_ino);
	printf("Number of hard links: %ld\n", stat_buffer->st_nlink);
	printf("uid: %d\n",stat_buffer->st_uid);
	printf("gid: %d\n",stat_buffer->st_gid);
	printf("Size: %ld\n",stat_buffer->st_size);
	printf("Block size: %ld\n",stat_buffer->st_blksize);
	printf("Number of blocks: %ld\n",stat_buffer->st_blocks);
	
	printf("Last status change:       %s\n", ctime(&stat_buffer->st_ctime));
        printf("Last file access:         %s\n", ctime(&stat_buffer->st_atime));
        printf("Last file modification:   %s\n", ctime(&stat_buffer->st_mtime));

	return 0;
}
