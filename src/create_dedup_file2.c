#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<openssl/md5.h>

// Macro defination
#define		BLOCK_SIZE					8192	// The block size is 8KB
#define		TOTAL_BLOCK_COUNT			10
#define		DEDUP_RATE					30
#define		SAME_BLOCK_COUNT			(TOTAL_BLOCK_COUNT * DEDUP_RATE / 100 + 1)
#define		DIFF_BLOCK_COUNT			(TOTAL_BLOCK_COUNT - SAME_BLOCK_COUNT)
#define		UNIQUE_BLOCK_COUNT			(DIFF_BLOCK_COUNT + 1)
#define		FIND_POSITION_RETRYS		10
#define		MD5_STRING_LEN				16

// Global vars defination
unsigned int		bitmap[TOTAL_BLOCK_COUNT];
unsigned char		block_buf[BLOCK_SIZE];

// Struct defination
struct Block_Content {
	unsigned int	block_size;
	unsigned char	*block_cont;
};
typedef struct Block_Content Block_Content;

// Function declaration
void usage();
int write_block_cont(int fd, Block_Content *blk);
int fill_block_cont(Block_Content *blk);
int calculate_md5(Block_Content *blk, unsigned char *md5val);
int print_md5(unsigned char *md5val);
int generate_simple(int fd);
int generate_random(int fd);
int read_file(int fd);
int print_bitmap(unsigned int *bitmap, size_t size);
int print_block(Block_Content *blk, char *comment);

// Function defination
void usage() {
	printf("Usage\n");
	printf("  create_dedup_file <file_name>\n");
	printf("<file_name> - The new file name.\n");
	printf("\n");
}

/*
 * Write the block content to the file.
 */
int write_block_cont(int fd, Block_Content *blk) {
	int		wnumber;
	//unsigned char	md5val[MD5_STRING_LEN];
	
	//print_block(blk, "write_block_cont");
	wnumber = write(fd, blk->block_cont, blk->block_size);
	if (wnumber != blk->block_size) {
		perror("write_same_cont() write base content.");
		return 1;
	}
	//calculate_md5(blk, md5val);
	//print_md5(md5val);
	
	return 0;
}

/*
 * Fill the Block_Content with random data.
 */
int fill_block_cont(Block_Content *blk) {
	static unsigned int		call_count = 0;
	unsigned int			seed;
	int						i;
	
	// First call, initialize the Block_Content.
	if (call_count == 0) {
		for (i = 0; i < blk->block_size; i++) blk->block_cont[i] = i + 1;
	};
	call_count++;

	blk->block_cont[call_count / (0x0001 << sizeof(unsigned char))] += 1;
	//print_block(blk, "fill_block_cont");
	
	return 0;
}

/*
 * Calculate the md5 sum of the Block_Content.
 */
int calculate_md5(Block_Content *blk, unsigned char *md5val) {
	MD5_CTX			md5_content;
	
	MD5_Init(&md5_content);
	MD5_Update(&md5_content, blk->block_cont, blk->block_size);
	MD5_Final(md5val, &md5_content);

	return 0;
}

/*
 * Print the md5 sum.
 */
int print_md5(unsigned char *md5val) {
	int		i;

	for (i = 0; i < MD5_STRING_LEN; i++) printf("%02x", (unsigned int) md5val[i]);
	printf("\n");

	return 0;
}

/*
 * Generate the simple dedup file.
 * The heading blocks' content are the same. The later blocks' content are random.
 */
int generate_simple(int fd) {
	Block_Content	*p_blk_cont;
	int				i;

	p_blk_cont = (Block_Content *) malloc(sizeof(Block_Content));
	p_blk_cont->block_size = BLOCK_SIZE;
	p_blk_cont->block_cont = block_buf;
	
	fill_block_cont(p_blk_cont);
	for (i = 0; i < SAME_BLOCK_COUNT; i++)
		if (write_block_cont(fd, p_blk_cont) != 0) exit(-1);
	
	// Change the 2nd block
	for (i = 1024; i < 1024 * 2; i++) p_blk_cont->block_cont[i]++;
	for (i = 0; i < DIFF_BLOCK_COUNT; i++) {
		fill_block_cont(p_blk_cont);
		if (write_block_cont(fd, p_blk_cont) != 0) exit(-1);
	}
	
	free(p_blk_cont);

	return 0;
}

/*
 * Generate random dedup file.
 * Use a bitmap to trace writing block data.
 * Randomly select the positions of the same block.
 */
int generate_random(int fd) {
	Block_Content	*p_blk_cont;
	unsigned int	i, pos, retry;
	unsigned char	md5val[MD5_STRING_LEN];

	for (i = 0; i < TOTAL_BLOCK_COUNT; i++) bitmap[i] = 0; 

	p_blk_cont = (Block_Content *) malloc(sizeof(Block_Content));
	p_blk_cont->block_size = BLOCK_SIZE;
	p_blk_cont->block_cont = block_buf;

	// Write the same block content.
	//printf("write_same_cont()\n");
	fill_block_cont(p_blk_cont);
	for (i = 0; i < SAME_BLOCK_COUNT; i++) {
		retry = 0;
		do {
			srand(retry);
			pos = rand() % TOTAL_BLOCK_COUNT;
			retry++;
		} while (bitmap[pos] != 0 && retry < FIND_POSITION_RETRYS);

		if (retry == FIND_POSITION_RETRYS) {
			printf("Can't find empty block in bitmap after %u retry.\n", retry);
			return 1;
		}

		if (lseek(fd, pos * BLOCK_SIZE, SEEK_SET) == -1) {
			perror("lseek()");
			return 1;
		}
		//calculate_md5(p_blk_cont, md5val);
		//print_md5(md5val);
		//printf("Position : %u\n", pos);
		if (write_block_cont(fd, p_blk_cont) != 0) return 1;
		bitmap[pos] = 1;
	}
	
	// Write the diff block content.
	//printf("write_rand_cont()\n");
	for (i = 0; i < TOTAL_BLOCK_COUNT; i++) {
		if (bitmap[i] == 0) {
			fill_block_cont(p_blk_cont);
			if (lseek(fd, i * BLOCK_SIZE, SEEK_SET) == -1) {
				perror("lseek()");
				return 1;
			}
			if (write_block_cont(fd, p_blk_cont) != 0) return 1;
			bitmap[i] = 2;
		}
	}

	free(p_blk_cont);

	return 0;
}

/*
 * Read the file, and calculate the md5 sum for
 * each block.
 */
int read_file(int fd) {
	int				rnumber, bnumber, i;
	unsigned char	md5val[MD5_STRING_LEN];
	
	lseek(fd, 0, SEEK_SET);
	bnumber = 0;
	while ((rnumber = read(fd, block_buf, BLOCK_SIZE)) > 0) {
		/*
		// TODO : If the file size is not 8K * n.
		if (rnumber != BLOCK_SIZE) {
		}
		else {
		}
		*/
		MD5(block_buf, rnumber, md5val);
		printf("block[%4d] : ", bnumber);
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

	for (i = 0; i < size; i++) printf("bitmap[%4u] : %u\n", i, bitmap[i]);
}

int print_block(Block_Content *blk, char *comment) {
	unsigned int	i;
	unsigned char   md5val[MD5_STRING_LEN];
	
	if (comment != NULL) printf("%s :\n", comment);
	for (i = 0; i < blk->block_size; i++) printf("%02x", blk->block_cont[i]);
	printf("\n");

	calculate_md5(blk, md5val);
	printf("md5sum : ");
	print_md5(md5val);
	
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
	fd = open(file_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		perror("Open file");
		exit(-1);
	}
	
	//generate_simple(fd);
	generate_random(fd);
	print_bitmap(bitmap, TOTAL_BLOCK_COUNT);
	read_file(fd);

	if (close(fd) != 0) {
		perror("Close file");
		exit(-1);
	}
	
	return 0;
}
