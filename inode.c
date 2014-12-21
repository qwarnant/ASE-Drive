/*
 * inode.c
 *
 *  Created on: 25 nov. 2014
 *      Author: warnant
 */

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "tools.h"
#include "inode.h"
#include "bloc.h"
#include "mbr.h"

void read_inode(unsigned int inumber, struct inode_s* inode) {
	read_bloc_n(CURRENT_VOL, inumber, sizeof(struct inode_s),
			(unsigned char*) inode);
}
void write_inode(unsigned int inumber, struct inode_s* inode) {
#ifdef DEBUG
	printf("current_vol : %d\n", CURRENT_VOL);
	printf("inumber : %d\n", inumber);
#endif
	write_bloc_n(CURRENT_VOL, inumber, sizeof(struct inode_s),
			(unsigned char*) inode);
}

unsigned int create_inode(enum inode_type_e type) {
	struct inode_s inode;

	/* Set all the structure to 0 */
	memset(&inode, 0, sizeof(struct inode_s));
	inode.in_type = type;

	/* Get a new bloc number for the new inode */
	unsigned inumber = new_bloc();
	assert(inumber);
	write_inode(inumber, &inode);

	return inumber;
}
int delete_inode(unsigned int inumber) {
	struct inode_s inode;
	int i;
	int blocs[NNBPB];
	int blocs2[NNBPB];

	/* Loading of the inode */
	read_inode(inumber, &inode);
	/* Delete the direct blocs of the inode */
	free_blocs(inode.in_direct, N_DIRECT);

	/* Loading of the indirect blocs of the inode */
	if (inode.in_indirect) {
		read_bloc_n(CURRENT_VOL, inode.in_indirect, sizeof(int), &blocs);
		/* Delete the indirect blocs of the inode */
		free_blocs(blocs, NNBPB);
		free_bloc(inode.in_indirect);
	}

	/* Loading the double indirect blocs of the inode */
	if (inode.in_d_indirect) {
		read_bloc_n(CURRENT_VOL, inode.in_d_indirect, sizeof(int), &blocs);
		for (i = 0; i < NNBPB; i++) {

			/* For each double indirection, load the bloc table and delete each bloc and the double indirection itself */
			if (blocs[i]) {
				read_bloc_n(CURRENT_VOL, blocs[i], sizeof(int), &blocs2);
				free_blocs(blocs2, NNBPB);
				free_bloc(blocs[i]);
			}
		}
	}

	free_bloc(inumber);

	return 0;
}

unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
		bool_t do_allocate) {
	struct inode_s inode;
	int blocs[NNBPB];
	int blocs2[NNBPB];

	read_inode(inumber, &inode);

	if (fbloc < N_DIRECT) {
		if (do_allocate && inode.in_direct[fbloc] == BLOC_NULL) {
			inode.in_direct[fbloc] = new_bloc();
			write_inode(inumber, &inode);
		}
		return inode.in_direct[fbloc];
	}

	fbloc -= N_DIRECT; /* Translation */

	if (fbloc < NNBPB) {
		if (inode.in_indirect == BLOC_NULL) {
			if (do_allocate) {
				inode.in_indirect = new_bloc();
				write_inode(inumber, &inode);
			} else {
				return BLOC_NULL;
			}
		}

		read_bloc_n(CURRENT_VOL, inode.in_indirect, sizeof(int), &blocs);

		return blocs[fbloc];
	}

	fbloc -= NNBPB;

	if (fbloc >= NNBPB) {

		if (inode.in_d_indirect == BLOC_NULL) {
			if (do_allocate) {
				inode.in_d_indirect = new_bloc();
				write_inode(inumber, &inode);
			} else {
				return BLOC_NULL;
			}
		}

		if (do_allocate) {
			blocs2[fbloc] = new_bloc();
		} else {
			return BLOC_NULL;
		}

		if (blocs2[fbloc / NNBPB] == BLOC_NULL) {
			if (do_allocate) {
				blocs[fbloc % NNBPB] = new_bloc();
			} else {
				return BLOC_NULL;
			}
			write_bloc(CURRENT_VOL, blocs2[fbloc / NNBPB],
					(unsigned char*) &blocs);
		}

		read_bloc_n(CURRENT_VOL, blocs2[fbloc / NNBPB], sizeof(int), &blocs);

		return blocs[fbloc % NNBPB];

	}

	return BLOC_NULL;
}
