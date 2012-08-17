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
#define		MD5_BLEN			16

struct Block_Content {
	unsigned int	base_blk_size;
	unsigned int	delta_blk_size;
	char			*p_base_blk_cont;		// The head content are the samae.
	char			*p_delta_blk_cont;		// The tail content are different.
};

typedef struct Block_Content Block_Content;

unsigned int	bitmap[TOTAL_BLK_COUNT];
char			block_buf[BLK_SIZE];

void usage();
int write_same_cont(int fd, Block_Content *blk);
int write_rand_cont(int fd, Block_Content *blk);
int calculate_md5(Block_Content *blk, unsigned char *md5val);
int print_md5(unsigned char *md5val);
int generate_simple(int fd);
int generate_random(int fd);
int read_file(int fd);
int print_bitmap(unsigned int *bitmap, size_t size);

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
	//unsigned char	md5val[MD5_BLEN];

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

	//calculate_md5(blk, md5val);
	//print_md5(md5val);
	
	return 0;
}

/*
 * Write the block content to the file. Randomly modify the
 * content of blk->p_delta_blk_cont before write.
 */
int write_rand_cont(int fd, Block_Content *blk) {
	int		wnumber;
	//unsigned char	md5val[MD5_BLEN];

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

	//calculate_md5(blk, md5val);
	//print_md5(md5val);
	
	return 0;
}

/*
 * calculate_md5 calculate the md5 sum of the Block_Content.
 */
int calculate_md5(Block_Content *blk, unsigned char *md5val) {
	MD5_CTX			md5_content;
	int				i;

	MD5_Init(&md5_content);
	MD5_Update(&md5_content, blk->p_base_blk_cont, blk->base_blk_size);
	MD5_Update(&md5_content, blk->p_delta_blk_cont, blk->delta_blk_size);
	MD5_Final(md5val, &md5_content);

	return 0;
}

int print_md5(unsigned char *md5val) {
	int		i;

	for (i = 0; i < MD5_BLEN; i++) printf("%02x", (unsigned int) md5val[i]);
	printf("\n");

	return 0;
}

int generate_simple(int fd) {
	char			base_blk_cont[BASE_BLK_SIZE], delta_blk_cont[BLK_SIZE - BASE_BLK_SIZE];
	Block_Content	one_blk_cont, *p_blk_cont;
	int				i;
	
	for (i = 0; i < BASE_BLK_SIZE; i++) base_blk_cont[i] = i + 1;
	if (initstate(0, delta_blk_cont, sizeof(delta_blk_cont)) == NULL) {
		perror("initstate()");
		exit(-1);
	}

	p_blk_cont = &one_blk_cont;
	p_blk_cont->base_blk_size = BASE_BLK_SIZE;
	p_blk_cont->delta_blk_size = BLK_SIZE - BASE_BLK_SIZE;
	p_blk_cont->p_base_blk_cont = base_blk_cont;
	p_blk_cont->p_delta_blk_cont = delta_blk_cont;
	
	for (i = 0; i < SAME_BLK_COUNT; i++)
		if (write_same_cont(fd, p_blk_cont) != 0) exit(-1);
	for (i = 0; i < 1024; i++) p_blk_cont->p_base_blk_cont[i]++;

	for (i = 0; i < DIFF_BLK_COUNT; i++)
		if (write_rand_cont(fd, p_blk_cont) != 0) exit(-1);
	
	return 0;
}

/*
 * generate_random() use a bitmap to trace writing block data.
 * Randomly select the location of the same block.
 */
int generate_random(int fd) {
	char			base_blk_cont[BASE_BLK_SIZE], delta_blk_cont[BLK_SIZE - BASE_BLK_SIZE];
	Block_Content	one_blk_cont, *p_blk_cont;
	unsigned int	i, pos, retry;

	for (i = 0; i < TOTAL_BLK_COUNT; i++) bitmap[i] = 0; 
	// char is 1B, unsigned int is 4B. Here is a cast from unsigned int
	// to char.
	for (i = 0; i < BASE_BLK_SIZE; i++) base_blk_cont[i] = i + 1;
	if (initstate(0, delta_blk_cont, sizeof(delta_blk_cont)) == NULL) {
		perror("initstate()");
		exit(-1);
	}

	p_blk_cont = &one_blk_cont;
	p_blk_cont->base_blk_size = BASE_BLK_SIZE;
	p_blk_cont->delta_blk_size = BLK_SIZE - BASE_BLK_SIZE;
	p_blk_cont->p_base_blk_cont = base_blk_cont;
	p_blk_cont->p_delta_blk_cont = delta_blk_cont;

	// Write the same block content.
	//printf("write_same_cont()\n");
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
	
	// Write the diff block content.
	//printf("write_rand_cont()\n");
	for (i = 0; i < TOTAL_BLK_COUNT; i++) {
		if (bitmap[i] == 0) {
			if (lseek(fd, i * BLK_SIZE, SEEK_SET) == -1) {
				perror("lseek()");
				return 1;
			}
			if (write_rand_cont(fd, p_blk_cont) != 0) return 1;
			bitmap[i] = 2;
		}
	}
	
	return 0;
}

int read_file(int fd) {
	int				rnumber, bnumber, i;
	unsigned char	md5val[MD5_BLEN];
	
	bnumber = 0;
	while ((rnumber = read(fd, block_buf, BLK_SIZE)) > 0) {
		/*
		if (rnumber != BLK_SIZE) {
		}
		else {
		}
		*/
		MD5(block_buf, rnumber, md5val);
		printf("Block[%4d] : ", bnumber);
		print_md5(md5val);
		bnumber++;
	}

	if (rnumber == -1) {
		perror("Read file");
		exit(-1);
	}

	return 0;
}

int print_bitmap(unsigned int *bitmap, size_t size) {
	unsigned int	i;

	printf("The bitmap of the random block:\n");
	for (i = 0; i < size; i++) printf("bitmap[%4u] : %u\n", i, bitmap[i]);
}

int main(int argc, char *argv[]) {
	char		*file_name;
	int			fd;

	if (argc < 2) {
		usage();
		exit(-1);
	};

	file_name = argv[1];
	fd = open(file_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		perror("Open file");
		exit(-1);
	}
	
	//generate_simple(fd);
	generate_random(fd);
	print_bitmap(bitmap, TOTAL_BLK_COUNT);
	lseek(fd, 0, SEEK_SET);
	read_file(fd);

	if (close(fd) != 0) {
		perror("Close file");
		exit(-1);
	}
	
	return 0;
}
