#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbr.h"
#include "drive.h"

/* ------------------------------
 command list
 ------------------------------------------------------------*/
struct _cmd {
	char *name;
	void (*fun)(struct _cmd *c);
	char *comment;
};

static void list(struct _cmd *c);
static void new(struct _cmd *c);
static void del(struct _cmd *c);
static void help(struct _cmd *c);
static void save(struct _cmd *c);
static void quit(struct _cmd *c);
static void xit(struct _cmd *c);
static void none(struct _cmd *c);

static struct _cmd commands[] = {
		{ "list", list, "display the partition table" }, { "new", new,
				"create a new partition" },
		{ "del", del, "delete a partition" }, { "save", save, "save the MBR" },
		{ "quit", quit, "save the MBR and quit" }, { "exit", xit,
				"exit (without saving)" },
		{ "help", help, "display this help" }, { 0, none,
				"unknown command, try help" } };

/* ------------------------------
 dialog and execute
 ------------------------------------------------------------*/

static void execute(const char *name) {
	struct _cmd *c = commands;

	while (c->name && strcmp(name, c->name))
		c++;
	(*c->fun)(c);
}

static void loop(void) {
	char name[64];

	while (printf("> "), scanf("%62s", name) == 1)
		execute(name);
}

/* ------------------------------
 command execution
 ------------------------------------------------------------*/
static void list(struct _cmd *c) {
	unsigned int i;
	int first_sector;
	int first_cylinder;
	int last_sector;
	int last_cylinder;
	int nb_sec;
	int type;

	if (mbr.mbr_n_vol != 0) {
		printf("#\tStart (C, S)\tEnd (C, S)\tSize\tType\n");
		for (i = 0; i < mbr.mbr_n_vol; i++) {
			if (mbr.mbr_vol[i].vol_type != VOLT_FREE) {
				first_sector = mbr.mbr_vol[i].vol_first_sector;
				first_cylinder = mbr.mbr_vol[i].vol_first_cylinder;
				nb_sec = mbr.mbr_vol[i].vol_n_sector;
				type = mbr.mbr_vol[i].vol_type;
				last_sector = (first_sector + nb_sec - 1) % HDA_MAXSECTOR;
				last_cylinder = first_cylinder
						+ ((first_sector + nb_sec - 1) / HDA_MAXSECTOR);
				printf("%d\t%d, %d\t\t%d, %d\t\t%d\t", i, first_cylinder,
						first_sector, last_cylinder, last_sector, nb_sec);
				printf("%s\n", get_vol_type(type));

			}
		}
	} else {
		printf("There is no partition on the disk.\n");
	}
}

static void new(struct _cmd *c) {
	unsigned int i;
	unsigned int first_cylinder;
	unsigned int first_sector;
	unsigned int nsector;
	unsigned int type;
	unsigned int maxAlloc;

	/* Initiliaze the disk if it is empty */
	if(mbr.mbr_n_vol == 0) {
		for (i = 0; i < MAX_VOL; i++) {
			mbr.mbr_vol[i].vol_first_cylinder = 0;
			mbr.mbr_vol[i].vol_first_sector = 0;
			mbr.mbr_vol[i].vol_n_sector = 0;
			mbr.mbr_vol[i].vol_type = VOLT_FREE;
		}
	}

	if (mbr.mbr_n_vol < MAX_VOL) {
		do {
			printf("Cylindre de début (0 - %d) : ", HDA_MAXCYLINDER - 1);
			scanf("%d", &first_cylinder);
		} while (first_cylinder >= HDA_MAXCYLINDER);

		do {
			if (first_cylinder == 0) {
				printf("Secteur de début (1 - %d) : ", HDA_MAXSECTOR - 1);
			} else {
				printf("Secteur de début (0 - %d) : ", HDA_MAXSECTOR - 1);
			}
			scanf("%d", &first_sector);
		} while (first_sector >= HDA_MAXSECTOR
				|| (first_cylinder == 0 && first_sector == 0));

		maxAlloc = HDA_MAXSECTOR * HDA_MAXCYLINDER
				- first_cylinder * HDA_MAXSECTOR - first_sector;
		do {
			printf("Nombre de secteurs (1 - %d) : ", maxAlloc);
			scanf("%d", &nsector);
		} while (nsector == 0 || nsector > maxAlloc);

		do {
			printf("Type de disque (0 = PRIMARY, 1 = SECONDARY, 2 = OTHERS) : ");
			scanf("%d", &type);
		} while (type >= 3);

		for (i = 0; i < MAX_VOL; i++) {
			if (mbr.mbr_vol[i].vol_type == VOLT_FREE) {
				mbr.mbr_vol[i].vol_first_cylinder = first_cylinder;
				mbr.mbr_vol[i].vol_first_sector = first_sector;
				mbr.mbr_vol[i].vol_n_sector = nsector;
				switch (type) {
					case 0:
						mbr.mbr_vol[i].vol_type = VOLT_PR;
						break;
					case 1:
						mbr.mbr_vol[i].vol_type = VOLT_SND;
						break;
					case 2:
						mbr.mbr_vol[i].vol_type = VOLT_OTHER;
						break;
				}
				break;
			}
		}
		mbr.mbr_n_vol++;
	} else {
		printf(
				"You cannot add new partition to the disk, the maximum number of partition has been reached.\n");
	}
}

static void del(struct _cmd *c) {
	unsigned int vol;

	if (mbr.mbr_n_vol >= 0 && mbr.mbr_n_vol < MAX_VOL) {
		do {
			printf("Partition to delete : ");
			scanf("%d", &vol);
		} while (vol >= 8);

		mbr.mbr_vol[vol].vol_type = VOLT_FREE;
		mbr.mbr_n_vol--;
	} else {
		printf(
				"You cannot delete partition because there is no partition on the disk.\n");
	}
}

static void save(struct _cmd *c) {
	if (mbr.mbr_n_vol != 0) {
		save_mbr();
	} else {
		printf("There is nothing to save.\n");
	}
}

static void quit(struct _cmd *c) {
	if (mbr.mbr_n_vol != 0) {
		save_mbr();
		exit(EXIT_SUCCESS);
	} else {
		printf("There is nothing to save.\n");
	}
}

static void do_xit() {
	exit(EXIT_SUCCESS);
}

static void xit(struct _cmd *dummy) {
	do_xit();
}

static void help(struct _cmd *dummy) {
	struct _cmd *c = commands;

	for (; c->name; c++)
		printf("%s\t-- %s\n", c->name, c->comment);
}

static void none(struct _cmd *c) {
	printf("%s\n", c->comment);
}

int main(int argc, char **argv) {
	init_master();
	load_mbr();

	/* dialog with user */
	loop();

	do_xit();

	exit(EXIT_SUCCESS);
}








