#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<openssl/md5.h>

#define		BLK_SIZE			8192	// The block size is 8KB
#define		BASE_BLK_SIZE		7168	// The 7K are the same
#define		TOTAL_BLK_COUNT		10
#define		DEDUP_RATE			30
#define		SAME_BLK_COUNT		(TOTAL_BLK_COUNT * DEDUP_RATE / 100 + 1)
#define		DIFF_BLK_COUNT		(TOTAL_BLK_COUNT - SAME_BLK_COUNT)
#define		UNIQUE_BLK_COUNT	(DIFF_BLK_COUNT + 1)
#define		FIND_EMPTY_RETRY	10

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
int calculate_md5(Block_Content *blk);
int generate_1(int fd);

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

	calculate_md5(blk);
	
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
	
	srandom(random());
	if (setstate(blk->p_delta_blk_cont) == NULL) {
		perror("setstate()");
		return 2;
	}
	wnumber = write(fd, blk->p_delta_blk_cont, blk->delta_blk_size);
	if (wnumber != blk->delta_blk_size) {
		perror("write_rand_cont() write delta content.");
		return 1;
	}

	calculate_md5(blk);
	
	return 0;
}

/*
 * calculate_md5 calculate the md5 sum of the Block_Content.
 */
int calculate_md5(Block_Content *blk) {
	MD5_CTX			md5_content;
	unsigned char	md5val[16];
	int				i;

	MD5_Init(&md5_content);
	MD5_Update(&md5_content, blk->p_base_blk_cont, blk->base_blk_size);
	MD5_Update(&md5_content, blk->p_delta_blk_cont, blk->delta_blk_size);
	MD5_Final(md5val, &md5_content);

	printf("The md5 check sum is :\n");
	for (i = 0; i < 16; i++) printf("%02x", (unsigned int) md5val[i]);
	printf("\n");

	return 0;
}

int generate_1(int fd) {
	char			base_blk_cont[BASE_BLK_SIZE], delta_blk_cont[BLK_SIZE - BASE_BLK_SIZE];
	Block_Content	*p_blk_cont;
	int				i;
	
	for (i = 0; i < BASE_BLK_SIZE; i++) base_blk_cont[i] = i + 1;
	if (initstate(0, delta_blk_cont, sizeof(delta_blk_cont)) == NULL) {
		perror("initstate()");
		exit(-1);
	}

	p_blk_cont = (Block_Content *) malloc(sizeof(Block_Content));
	p_blk_cont->base_blk_size = BASE_BLK_SIZE;
	p_blk_cont->delta_blk_size = BLK_SIZE - BASE_BLK_SIZE;
	p_blk_cont->p_base_blk_cont = base_blk_cont;
	p_blk_cont->p_delta_blk_cont = delta_blk_cont;
	
	for (i = 0; i < SAME_BLK_COUNT; i++)
		if (write_same_cont(fd, p_blk_cont) != 0) exit(-1);
	for (i = 0; i < 1024; i++) p_blk_cont->p_base_blk_cont[i]++;

	for (i = 0; i < DIFF_BLK_COUNT; i++)
		if (write_rand_cont(fd, p_blk_cont) != 0) exit(-1);
	
	free(p_blk_cont);

	return 0;
}

/*
 * generate_2() use a bitmap to trace writing block data.
 * Randomly select the location of the same block.
 */
int generate_2(int fd) {
	unsigned int	bitmap[TOTAL_BLK_COUNT];
	char			base_blk_cont[BASE_BLK_SIZE], delta_blk_cont[BLK_SIZE - BASE_BLK_SIZE];
	Block_Content	*p_blk_cont;
	unsigned int	i, pos, retry;

	for (i = 0; i < TOTAL_BLK_COUNT; i++) bitmap[i] = 0; 
	// char is 1B, unsigned int is 4B. Here is a cast from unsigned int
	// to char.
	for (i = 0; i < BASE_BLK_SIZE; i++) base_blk_cont[i] = i + 1;
	if (initstate(0, delta_blk_cont, sizeof(delta_blk_cont)) == NULL) {
		perror("initstate()");
		exit(-1);
	}

	p_blk_cont = (Block_Content *) malloc(sizeof(Block_Content));
	p_blk_cont->base_blk_size = BASE_BLK_SIZE;
	p_blk_cont->delta_blk_size = BLK_SIZE - BASE_BLK_SIZE;
	p_blk_cont->p_base_blk_cont = base_blk_cont;
	p_blk_cont->p_delta_blk_cont = delta_blk_cont;

	// Write the same block content.
	for (i = 0; i < SAME_BLK_COUNT; i++) {
		retry = 0;
		do {
			srand(retry);
			pos = rand() % TOTAL_BLK_COUNT;
			retry++;
		} while (bitmap[pos] != 0 && retry < FIND_EMPTY_RETRY);

		if (retry == FIND_EMPTY_RETRY) {
			printf("Can't find empty block in bitmap after %u retry.\n", retry);
			return 1;
		}

		if (lseek(fd, pos * BLK_SIZE, SEEK_SET) == -1) {
			perror("lseek()");
			return 1;
		}
		if (write_same_cont(fd, p_blk_cont) != 0) return 1;
		bitmap[pos] = 1;
	}
	
	printf("The bitmap of the same block:\n");
	for (i = 0; i < TOTAL_BLK_COUNT; i++)
		if (bitmap[i] == 1) printf("[%u] : %u\n", i, bitmap[i]);
	
	// Write the diff block content.
	for (i = 0; i < TOTAL_BLK_COUNT; i++) {
		if (bitmap[i] == 0) {
			if (write_rand_cont(fd, p_blk_cont) != 0) return 1;
			bitmap[i] = 2;
		}
	}

	printf("The bitmap of the random block:\n");
	for (i = 0; i < TOTAL_BLK_COUNT; i++)
		if (bitmap[i] == 2) printf("[%u] : %u\n", i, bitmap[i]);
	
	return 0;
}

int main(int argc, char *argv[]) {
	char		*file_name;
	int			fd;

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
	
	//generate_1(fd);
	generate_2(fd);

	if (close(fd) != 0) {
		perror("Close file");
		exit(-1);
	}
	
	return 0;
}
