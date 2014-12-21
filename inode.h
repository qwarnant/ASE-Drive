/*
 * inode.h
 *
 *  Created on: 14 oct. 2014
 *      Author: warnant
 */
#include "drive.h"
#include "tools.h"

#ifndef INODE_H_
#define INODE__H_

#define N_DIRECT 2
#define BLOC_NULL 0
#define NNBPB HDA_SECTORSIZE/sizeof(int)
#define DATA_BLOC_SIZE 1024

enum inode_type_e {
	IT_FILE, IT_DIR, IT_OTHER
};

struct inode_s {
	enum inode_type_e in_type;
	unsigned int in_size;
	unsigned int in_direct[N_DIRECT];
	unsigned int in_indirect;
	unsigned int in_d_indirect;
};


void read_inode(unsigned int, struct inode_s*);
void write_inode(unsigned int, struct inode_s*);

unsigned int create_inode(enum inode_type_e);
int delete_inode(unsigned int);

unsigned int vbloc_of_fbloc(unsigned int, unsigned int, bool_t);

#endif /* INODE__H_ */
