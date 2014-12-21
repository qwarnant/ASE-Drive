#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "drive.h"
#include "mbr.h"
#include "bloc.h"

static char *cmdname;
static char *processtype;

static void usage() {
	fprintf(stderr, "usage: %s process [options]... [args]...\n", cmdname);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	/* Variable declaration */
	unsigned char* buffer;
	int opterr, optind, index, c, vSize = HDA_SECTORSIZE - 1, vCylinder = -1,
			vSector = -1, free_size = 0, count = 0, i, j;
	char* optopt;
	struct vol_s newVol;

	/* Disk and partition initialization */
	init_master();
	load_mbr();
	init_volume();

	/* Variable initialization */
	srand(time(NULL));
	buffer = (unsigned char*) malloc(HDA_SECTORSIZE * sizeof(unsigned char));
	opterr = 0;

	/* Check the usage of the main program */
	cmdname = argv[0];
	processtype = argv[1];
	if (argc == 1) {
		usage();
		return EXIT_FAILURE;
	}

	/* Get the status of the disk (free space) */
	if (strcmp(processtype, "status") == 0) {
		if (!load_super(CURRENT_VOL)) {
			fprintf(stderr, "No file system on the chosen partition\n");
			return EXIT_FAILURE;
		}
		printf("Space status of the volume : %s \n", current_super.super_name);
		free_size = mbr.mbr_vol[CURRENT_VOL].vol_n_sector - 1;
		double percent = (current_super.super_nb_free / (double) free_size)
				* 100;
		printf("Free space : %f %\n", percent);
		printf("Free blocs : %d\n", current_super.super_nb_free);
		return EXIT_SUCCESS;
	}

	/* Fill the partition */
	if (strcmp(processtype, "debug_fill") == 0) {
		printf("Filling the current partition ...\n");
		if (!load_super(CURRENT_VOL)) {
			fprintf(stderr, "No file system on the chosen partition\n");
			return EXIT_FAILURE;
		}
		int b;
		do {
			b = new_bloc();
			if(b != BLOC_NULL) {
				count++;
				printf(".");
				fflush(stdout);
			}
		} while (b != 0);
		printf("Number of allocated blocs : %d\n", count);
		return EXIT_SUCCESS;
	}

	/* Random free of the partition */
	if (strcmp(processtype, "debug_free") == 0) {
		if (!load_super(CURRENT_VOL)) {
			fprintf(stderr, "No file system on the chosen partition\n");
		}

		/* Random free of the partition blocs */
		int it = 255 - current_super.super_nb_free;
		int n;

		count = 0;

		for (n = 0; n < it / 10; n++) {
			int random = rand() % it;
			free_bloc(random);
			printf("%d\n", random);

			count++;
		}
		printf("Number of desallocated blocs : %d\n", count);

		return EXIT_SUCCESS;
	}

	/* Make a new filesystem */
	if (strcmp(processtype, "mknfs") == 0) {
		init_super(CURRENT_VOL);
		return EXIT_SUCCESS;
	}

	if (strcmp(processtype, "dfs") == 0) {
		for (index = 0; index < mbr.mbr_n_vol; index++) {
			struct super_s cur_super;
			read_bloc_n(index, SUPER, sizeof(cur_super),
					(unsigned char*) &cur_super);
			printf("%s\t", cur_super.super_name);
			printf("%d\n", cur_super.super_nb_free);
		}
		return EXIT_SUCCESS;
	}

	/* Process to format the entire disk	 */
	if (strcmp(processtype, "frmt") == 0) {
		for (i = 0; i < HDA_MAXCYLINDER; i++) {
			for (j = 0; j < HDA_MAXSECTOR; j++) {
				format_sector(i, j, HDA_SECTORSIZE, 0);
			}
		}
		printf("The entire disk has been successfully formated.\n");
		return EXIT_SUCCESS;
	}

	/* Process to dump the content of the first sector */
	if (strcmp(processtype, "dmps") == 0) {
		read_sector_n(0, 0, HDA_SECTORSIZE, buffer);
		dump(buffer, HDA_SECTORSIZE, 0, 1);
		return EXIT_SUCCESS;
	}

	/* Process type unknown */
	usage();

	return EXIT_FAILURE;

}