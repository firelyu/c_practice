#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>

#define		BLK_SIZE			8 * 1024
#define		TOTAL_BLK_NUMBER	3

typedef	struct Block_Data Block_Data;

struct Block_Data {
	char	*base_blk;	//BLK_SIZE - 1
	char	*delta;		// The length is 1
};

void usage();
void fill_blk(char *blk, unsigned int size);
int write_to_file(int fd, Block_Data *blk);

void usage() {
	printf("Usage:\n");
	printf("\targv[0] <filename>\n");
	printf("<filename> - The new file name.\n");
	printf("\n");
	printf("\n");
}

void fill_blk(char *blk, unsigned int size) {
	int		i;
	for (i = 0; i < size; i++) blk[i] = i;
}

int write_to_file(int fd, Block_Data *blk) {
	int		wnumber;
	
	wnumber = write(fd, blk->base_blk, BLK_SIZE - 1);
	if (wnumber != BLK_SIZE - 1) {
		perror("Write base block data");
		return 1;
	}

	wnumber = write(fd, blk->delta, 1);
	if (wnumber != 1) {
		perror("Write delta block data");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	char	base_block_data[BLK_SIZE - 1], delta_list[TOTAL_BLK_NUMBER];;
	char	*filename;
	int		fd, i;
	Block_Data	*basebd;

	if (argc < 2) {
		usage();
		exit(-1);
	}
	
	// Create the base block data.
	fill_blk(base_block_data, BLK_SIZE - 1);
	for (i = 0; i < TOTAL_BLK_NUMBER + 1; i++) delta_list[i] = 'a' + 1;
	basebd = (Block_Data *)malloc(sizeof(Block_Data));
	basebd->base_blk = base_block_data;
	basebd->delta = &delta_list[0];

	filename = argv[1];
	// Always create new file.
	fd = open(filename, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 
	if (fd == -1) {
		perror("Open the file");
		return 1;
	}

	// Write the to the file.
	for (i = 0; i < TOTAL_BLK_NUMBER; i++) {
		if (write_to_file(fd, basebd) != 0) goto end;
	}

end:
	free(basebd);
	close(fd);
	
	return 0;
}