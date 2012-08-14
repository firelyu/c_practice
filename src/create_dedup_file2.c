#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>

#define		BLK_SIZE			8192	// The block size is 8KB
#define		BASE_BLK_SIZE		7168	// The 7K are the same
#define		TOTAL_BLK_COUNT		100
#define		DEDUP_RATE			30
#define		SAME_BLK_COUNT		(TOTAL_BLK_COUNT * DEDUP_RATE / 100 + 1)
#define		DIFF_BLK_COUNT		(TOTAL_BLK_COUNT - SAME_BLK_COUNT)
#define		UNIQUE_BLK_COUNT	(DIFF_BLK_COUNT + 1)

struct Block_Content {
	unsigned int	base_blk_size;
	unsigned int	delta_blk_size;
	char			*p_base_blk_cont;		// The head content are the samae.
	char			*p_delta_blk_cont;		// The tail content are different.
};

typedef struct Block_Content Block_Content;

void usage();
int write_same_cont(int fd, Block_Content *blk);
int write_rand_cont(int fd, Block_Content *blk);

void usage() {
	printf("Usage\n");
	printf("\tcreate_dedup_file <file_name>\n");
	printf("<file_name>\t\t-\tThe new file name.\n");
	printf("\n");
}

/*
 * Write the block content to the file. Don't modify
 * the content of blk->p_delta_blk_cont.
 */
int write_same_cont(int fd, Block_Content *blk) {
	int		wnumber;
	
	wnumber = write(fd, blk->p_base_blk_cont, blk->base_blk_size);
	if (wnumber != blk->base_blk_size) {
		perror("write_same_cont() write base content.");
		return 1;
	}
	
	wnumber = write(fd, blk->p_delta_blk_cont, blk->delta_blk_size);
	if (wnumber != blk->delta_blk_size) {
		perror("write_same_cont() write delta content.");
		return 1;
	}

	return 0;
}

/*
 * Write the block content to the file. Randomly modify the
 * content of blk->p_delta_blk_cont before write.
 */
int write_rand_cont(int fd, Block_Content *blk) {
	int		wnumber;
	
	wnumber = write(fd, blk->p_base_blk_cont, blk->base_blk_size);
	if (wnumber != blk->base_blk_size) {
		perror("write_rand_cont() write base content.");
		return 1;
	}
	
	if (setstate(blk->p_delta_blk_cont) == NULL) {
		perror("setstate()");
		return 2;
	}

	wnumber = write(fd, blk->p_delta_blk_cont, blk->delta_blk_size);
	if (wnumber != blk->delta_blk_size) {
		perror("write_rand_cont() write delta content.");
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	char			base_blk_cont[BASE_BLK_SIZE], delta_blk_cont[BLK_SIZE - BASE_BLK_SIZE];
	char			*file_name;
	int				fd, i;
	Block_Content	*p_same_blk, *p_delta_blk;

	if (argc < 2) {
		usage();
		exit(-1);
	};

	file_name = argv[1];
	fd = open(file_name, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		perror("Open file");
		exit(-1);
	}

	for (i =0; i < BASE_BLK_SIZE; i++) base_blk_cont[i] = i + 1;
	if (initstate(0, delta_blk_cont, sizeof(delta_blk_cont)) == NULL) {
		perror("initstate()");
		exit(-1);
	}
	
	p_same_blk->base_blk_size = BASE_BLK_SIZE;
	p_same_blk->delta_blk_size = BLK_SIZE - BASE_BLK_SIZE;
	p_same_blk->p_base_blk_cont = base_blk_cont;
	p_same_blk->p_delta_blk_cont = delta_blk_cont;

	for (i = 0; i < SAME_BLK_COUNT; i++)
		if (write_same_cont(fd, p_same_blk) != 0) exit(-1);

	for (i = 0; i < DIFF_BLK_COUNT; i++)
		if (write_rand_cont(fd, p_same_blk) != 0) exit(-1);

	if (close(fd) != 0) {
		perror("Close file");
		exit(-1);
	}

	return 0;
}
