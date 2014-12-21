#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tools.h"
#include "bloc.h"
#include "mbr.h"

void init_super(unsigned int vol) {
	struct free_bloc_s bloc;

	unsigned free_size = mbr.mbr_vol[vol].vol_n_sector - 1;

	current_super.super_magic = SUPER_MAGIC;
	current_super.super_nb_free = free_size;
	current_super.super_first_free_bloc = 1;

	current_super.super_name = "super_name";
	current_super.super_serial_number = "serial_number";

	bloc.fb_n_free = free_size;
	bloc.fb_next = BLOC_NULL;
	write_bloc_n(vol, SUPER + 1, sizeof(struct free_bloc_s), (unsigned char*) &bloc);

	CURRENT_VOL = vol;
	save_super();

}

int load_super(unsigned int vol) {
	CURRENT_VOL = vol;
	read_bloc_n(vol, SUPER, sizeof(struct super_s),
			(unsigned char*) &current_super);
#ifdef DEBUG
	printf("Super bloc name : %s\n", current_super.super_name);
#endif
	return current_super.super_magic == SUPER_MAGIC;
}

void save_super() {
	assert(current_super.super_magic == SUPER_MAGIC);
	write_bloc_n(CURRENT_VOL, SUPER, sizeof(struct super_s),
			(unsigned char*) &current_super);
}

int init_volume() {
	char* ENV_CURRENT_VOL;
	ENV_CURRENT_VOL = getenv("CURRENT_VOL");
	if (ENV_CURRENT_VOL == NULL) {
		CURRENT_VOL = DFT_CURRENT_VOLUME;
		fprintf(stderr,
				"Warning : The env variable CURRENT_VOL is not set on your system. The default value %d has been chosen for the current execution.\n",
				CURRENT_VOL);
	}
	return CURRENT_VOL;
}

unsigned int new_bloc() {
	unsigned new_bloc;
	struct free_bloc_s free_bloc;

#ifdef DEBUG
	printf("Free blocs on the current super bloc=%d\n", current_super.super_nb_free);
#endif

	if (current_super.super_nb_free == 0) {
#ifdef DEBUG
		printf("Volume full current_super.super_nb_free=%d\n", current_super.super_nb_free);
#endif
		return BLOC_NULL;
	}

	read_bloc_n(CURRENT_VOL, current_super.super_first_free_bloc,
			sizeof(struct free_bloc_s), (unsigned char*) &free_bloc);

	new_bloc = current_super.super_first_free_bloc;

#ifdef DEBUG
	printf("current_super.super_first_free_bloc=%d\n", current_super.super_first_free_bloc);
	printf("New allocated bloc=%d\n", new_bloc);
#endif

	if (free_bloc.fb_n_free == 1) {
		current_super.super_first_free_bloc = free_bloc.fb_next;
	} else {

		free_bloc.fb_n_free--;
		current_super.super_first_free_bloc++;

	}

#ifdef DEBUG
	printf("Next bloc=%d\n", current_super.super_first_free_bloc);
#endif

	write_bloc_n(CURRENT_VOL, current_super.super_first_free_bloc,
			sizeof(free_bloc), (unsigned char*) &free_bloc);

	current_super.super_nb_free--;
	save_super();

	return new_bloc;
}
void free_bloc(unsigned int bloc) {
	struct free_bloc_s new_free_bloc;
	struct free_bloc_s current_bloc;
	unsigned num_prev_bloc = 0;
	unsigned num_next_bloc = current_super.super_first_free_bloc;

#ifdef DEBUG
	printf("Bloc to free=%d\n", bloc);
#endif

	if (bloc == BLOC_NULL) {
		fprintf(stderr, "Wrong bloc\n");
		return;
	}

	if(num_next_bloc != BLOC_NULL) {
		while (num_prev_bloc < bloc) {

			num_prev_bloc = num_next_bloc;
			if(num_prev_bloc == BLOC_NULL) {
				break;
			}

			read_bloc_n(CURRENT_VOL, num_prev_bloc, sizeof(struct free_bloc_s),
					(unsigned char*) &current_bloc);

			num_next_bloc = current_bloc.fb_next;

#ifdef DEBUG
			printf("num_prev_bloc=%d, num_next_bloc=%d\n", num_prev_bloc, num_next_bloc);
#endif

		}
	}

	new_free_bloc.fb_n_free = 1;
	new_free_bloc.fb_next = num_next_bloc;

	read_bloc_n(CURRENT_VOL, num_prev_bloc, sizeof(struct free_bloc_s),
			(unsigned char*) &current_bloc);
	current_bloc.fb_next = num_prev_bloc;
	write_bloc_n(CURRENT_VOL, num_prev_bloc, sizeof(struct free_bloc_s),
			(unsigned char*) &current_bloc);

	write_bloc_n(CURRENT_VOL, bloc, sizeof(struct free_bloc_s), (unsigned char*) &new_free_bloc);

	if(num_next_bloc == BLOC_NULL) {
		current_super.super_first_free_bloc = bloc;
	}

	current_super.super_nb_free++;

#ifdef DEBUG
	printf("num_prev_bloc=%d\n", num_prev_bloc);
	printf("new_free_bloc.fb_n_free=%d, new_free_bloc.fb_next=%d\n", new_free_bloc.fb_n_free, new_free_bloc.fb_next);
	printf("current_bloc.fb_next=%d\n", current_bloc.fb_next);
	printf("current_super.super_nb_free=%d\n", current_super.super_nb_free);

#endif

	save_super();
}

void free_blocs(unsigned int* blocs, unsigned int size) {
	int i;
	for (i = 0; i < size; i++) {
#ifdef DEBUG
		printf("Bloc to deallocate=%d\n", blocs[i]);
#endif
		free_bloc(blocs[i]);
	}
}
