i/* NAME: Caleb Chau, Andrew Ding

 * EMAIL: caleb.h.chau@gmail.com, andrewxding@ucla.edu

 * ID: 204805602, 504748356

 */



#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <time.h>

#include <string.h>

#include <errno.h>



#include "ext2_fs.h"



void scan_directory(struct ext2_inode* inode, int inode_no);

void readIndirect(int owner_inode_no, uint32_t block_no, int level, uint32_t curr_offset);



int fileImage;

unsigned int numGroups;

struct ext2_super_block superblock;

struct ext2_group_desc* groupDescriptors;

int* ibitmap;



/* Defined i_mode values */

#define EXT2_S_IRWXU	0x01C0	    // user access rights mask

#define EXT2_S_IRWXG	0x0038	    // group access rights mask

#define EXT2_S_IRWXO	0x0007	    // others access rights mask



#define PREAD "pread"

#define MALLOC "malloc"



unsigned int blocksize;



void handle_error(char* operation) {

    int err_no = errno;

    char* err_message = strerror(err_no);



    fprintf(stderr, "%s failed: %s\r\n", operation, err_message);

}



ssize_t Pread(int fd, void* buf, size_t count, off_t offset) {

	int ret = pread(fd, buf, count, offset);



	if (ret == -1) {

		handle_error(PREAD);

		exit(2);

	}



	return ret;

}



void* Malloc(size_t size) {

	void* ptr = malloc(size);



	if (ptr == NULL) {

		handle_error(MALLOC);

		exit(2);

	}



	return ptr;

}



void formatTime(uint32_t timestamp, char* buf) {

	time_t epoch = timestamp;

	struct tm ts = *gmtime(&epoch);

	strftime(buf, 80, "%m/%d/%y %H:%M:%S", &ts);

}



void superblockSummary() {

	Pread(fileImage, &superblock, sizeof(struct ext2_super_block), 1024);



	if (superblock.s_magic != EXT2_SUPER_MAGIC) {

		fprintf(stderr, "Error: not an ext2 file system");

		exit(1);

	}



	blocksize = 1024 << superblock.s_log_block_size;



	fprintf(stdout, "SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n",

		superblock.s_blocks_count, superblock.s_inodes_count, blocksize, superblock.s_inode_size, superblock.s_blocks_per_group,

		superblock.s_inodes_per_group, superblock.s_first_ino

	);

}



void groupBlockInodeSummary() {//GROUP AND INODE AND BITMAP BLOCK READ

	numGroups = 1 + (superblock.s_blocks_count - 1) / superblock.s_blocks_per_group;



	groupDescriptors = Malloc(numGroups * sizeof(struct ext2_group_desc));

	Pread(fileImage, groupDescriptors, numGroups * sizeof(struct ext2_group_desc), 2048);



	unsigned int i, j, k;

	uint32_t blocksleft = superblock.s_blocks_count;

	uint32_t blocksper = superblock.s_blocks_per_group;

	uint32_t inodesleft = superblock.s_inodes_count;

	uint32_t inodesper= superblock.s_inodes_per_group;



	ibitmap = Malloc(numGroups * blocksize);



	for(i = 0; i < numGroups; i++) {

		//block fragmentation

		if (blocksleft < superblock.s_blocks_per_group) {

			blocksper = blocksleft;

		}



		//inode fragmentation

		if (inodesleft < superblock.s_inodes_per_group) {

			inodesper = inodesleft;

		}



		fprintf(stdout, "GROUP,%u,%u,%u,%u,%u,%u,%u,%u\n",

			i, blocksper, inodesper, groupDescriptors[i].bg_free_blocks_count, groupDescriptors[i].bg_free_inodes_count,

			groupDescriptors[i].bg_block_bitmap, groupDescriptors[i].bg_inode_bitmap, groupDescriptors[i].bg_inode_table);



		blocksleft -= blocksper;

		inodesleft -= inodesper;



		for(j = 0; j < blocksize; j++) {

			uint8_t byteblock, byteinode;



			Pread(fileImage, &byteblock, 1, (blocksize * groupDescriptors[i].bg_block_bitmap) + j);

			Pread(fileImage, &byteinode, 1, (blocksize * groupDescriptors[i].bg_inode_bitmap) + j);



			ibitmap[i * blocksize + j] = byteinode;

			int mask = 1;



			//look at the byte bit by bit, zero indicating a free block.

			for(k = 0; k < 8; k++) {

				if((byteblock & mask) == 0) {

					fprintf(stdout, "BFREE,%u\n", i * superblock.s_blocks_per_group + j * 8 + k + 1);

				}



				if((byteinode & mask) == 0) {

					fprintf(stdout, "IFREE,%u\n", i * superblock.s_inodes_per_group + j * 8 + k + 1);

				}



				mask <<= 1;

			}

		}

	}

}



void blockInodeRead() {

	unsigned int inode_no, i, j;

	int mask, k;

	struct ext2_inode inode;

	char fileType = '?';



	for (i = 0; i < numGroups; i++) {

		// First read root directory (inode_no = 2)

		// Afterwards, set to first non-reserved inode

		for (inode_no = 2; inode_no < superblock.s_inodes_count; inode_no++) {

			int valid_inode = 1;



			// Check if inode is valid

			for (j = 0 ; j < blocksize; j++) {

				uint8_t byte = ibitmap[i * blocksize + j];

				mask = 1;



				for (int k = 0; k < 8; k++) {

					unsigned long curr_inode_no = i * superblock.s_inodes_per_group + j * 8 + k + 1;



					if (curr_inode_no == inode_no) {

						if ((byte & mask) == 0) {

							valid_inode = 0;

						}



						break;

					}



					mask <<= 1;

				}

			}



			if (valid_inode) {

				off_t offset = 1024 + (groupDescriptors[i].bg_inode_table - 1) * blocksize + (inode_no - 1) * sizeof(struct ext2_inode);



				Pread(fileImage, &inode, sizeof(struct ext2_inode), offset);



				// Creation time

				char ctime[80];

				char mtime[80];

				char atime[80];



				formatTime(inode.i_ctime, ctime);

				formatTime(inode.i_mtime, mtime);

				formatTime(inode.i_atime, atime);



				uint16_t mode = inode.i_mode & (EXT2_S_IRWXU | EXT2_S_IRWXG | EXT2_S_IRWXO);



				// Get inode file format

				if (S_ISDIR(inode.i_mode)) {

					fileType = 'd';

				} else if (S_ISREG(inode.i_mode)) {

					fileType = 'f';

				} else if (S_ISLNK(inode.i_mode)) {

					fileType = 's';

				}



				fprintf(stdout, "INODE,%d,%c,%o,%u,%u,%u,%s,%s,%s,%u,%u",

						inode_no,

						fileType,

						mode,

						inode.i_uid,

						inode.i_gid,

						inode.i_links_count,

						ctime,

						mtime,

						atime,

						inode.i_size,

						inode.i_blocks);



				// Print block addresses

				for (k = 0; k < EXT2_N_BLOCKS; k++) {

					fprintf(stdout, ",%u", inode.i_block[k]);

				}

				fprintf(stdout, "\n");



				if(fileType == 'd'){

					scan_directory(&inode, inode_no);

				}



				// Check indirect blocks

				if (inode.i_block[EXT2_IND_BLOCK] != 0) {

					readIndirect(inode_no, inode.i_block[EXT2_IND_BLOCK], 1, 12);

				}



				if (inode.i_block[EXT2_DIND_BLOCK] != 0) {

					readIndirect(inode_no, inode.i_block[EXT2_DIND_BLOCK], 2, 268);

				}



				if (inode.i_block[EXT2_TIND_BLOCK] != 0) {

					readIndirect(inode_no, inode.i_block[EXT2_TIND_BLOCK], 3, 65804);

				}



				// If currently reading root directory, go to first

				// non-reserved inode for next loop

				if (inode_no == 2) {

					inode_no = superblock.s_first_ino - 1;

				}

			}

		}

	}

}



// scan directory

void scan_directory(struct ext2_inode* inode, int inode_no) {

	unsigned char block[blocksize];

	struct ext2_dir_entry* entry;

	unsigned int size = 0;



	// direct blocks

	for (int i = 0; i < EXT2_NDIR_BLOCKS; i++) {

		off_t offset = 1024 + (inode->i_block[i] - 1) * blocksize;



		Pread(fileImage, block, blocksize, offset);



		entry = (struct ext2_dir_entry*) block;



		while((size < inode->i_size) && entry->file_type) {

			char file_name[EXT2_NAME_LEN + 1];

			memcpy(file_name, entry->name, entry->name_len);

			file_name[entry->name_len] = 0;



			if (entry->inode != 0) {

				fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",

						inode_no,

						size,

						entry->inode,

						entry->rec_len,

						entry->name_len,

						file_name);

			}



			size += entry->rec_len;

			entry = (void*) entry + entry->rec_len;

		}

	}

}



// indirect block references

void readIndirect(int owner_inode_no, uint32_t block_no, int level, uint32_t curr_offset) {

	uint32_t i;

	uint32_t numEntries = blocksize / sizeof(uint32_t);

	uint32_t entries[numEntries];

	memset(entries, 0, sizeof(entries));



	Pread(fileImage, entries, blocksize, 1024 + (block_no - 1) * blocksize);



	for (i = 0; i < numEntries; i++) {

		if (entries[i] != 0) {

			fprintf(stdout, "INDIRECT,%u,%u,%u,%u,%u\n",

					owner_inode_no,

					level,

					curr_offset,

					block_no,

					entries[i]);



            if (level == 2 || level == 3) {

			    readIndirect(owner_inode_no, entries[i], level - 1, curr_offset);

            }

		}



        if (level == 1) {

            curr_offset++;

        } else if (level == 2 || level == 3) {

            curr_offset += level == 2 ? 256 : 65536;

        }

	}

}



int main(int argc, char **argv) {

	// read the file system image from the provided file name

	 if (argc != 2) {

        fprintf(stderr, "Error: wrong number of arguments. Usage: %s diskname\n", argv[0]);

        exit(1);

    }



	if ((fileImage = open(argv[1], O_RDONLY)) < 0) {

		fprintf(stderr, "Error: opening disk file\n");

		exit(1);

	}



	superblockSummary();

	groupBlockInodeSummary();

	blockInodeRead();

}
