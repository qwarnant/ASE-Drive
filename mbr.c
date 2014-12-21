#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "drive.h"
#include "mbr.h"

/*
 *	check_bloc_values()
 *	Verify if the volume and the bloc values are possible in the current drive
 */
static int check_bloc_values(int vol, int bloc) {
	if (bloc < 0 || vol < 0) {
		return 0;
	}
	if (vol > mbr.mbr_n_vol) {
		return 0;
	}
	return 1;
}

/*
 * sector_of_bloc()
 * Returns the sector of the current bloc on the target volume
 */
static unsigned int sector_of_bloc(int vol, int b) {
	assert(check_bloc_values(vol, b) == 1);
	return (mbr.mbr_vol[vol].vol_first_sector + b) % HDA_MAXSECTOR;
}

/*
 * cylinder_of_bloc()
 * Returns the cylinder of the current bloc on the target volume
 */
static unsigned int cylinder_of_bloc(int vol, int b) {
	assert(check_bloc_values(vol, b) == 1);
	return mbr.mbr_vol[vol].vol_first_cylinder
			+ (mbr.mbr_vol[vol].vol_first_sector + b) / HDA_MAXSECTOR;
}

void load_mbr() {
	if (sizeof(struct mbr_s) > HDA_SECTORSIZE) {
		fprintf(stderr, "Fatal error");
		exit(1);
	}

	read_sector_n(0, 0, sizeof(struct mbr_s), (unsigned char*) &mbr);
	if (mbr.mbr_magic != MBR_MAGIC) {
		mbr.mbr_n_vol = 0;
		mbr.mbr_magic = MBR_MAGIC;
		save_mbr();
		fprintf(stderr, "WARNING : The disk is new and empty.\n");
	}
}

void save_mbr() {
	write_sector_n(0, 0, sizeof(struct mbr_s), (unsigned char*) &mbr);
}
void read_bloc(unsigned int vol, unsigned int nbloc, unsigned char* buffer) {
	unsigned blocCylinder = cylinder_of_bloc(vol, nbloc);
	unsigned blocSector = sector_of_bloc(vol, nbloc);

	read_sector(blocCylinder, blocSector, buffer);
}
void write_bloc(unsigned int vol, unsigned int nbloc, unsigned char* buffer) {
	unsigned blocCylinder = cylinder_of_bloc(vol, nbloc);
	unsigned blocSector = sector_of_bloc(vol, nbloc);

	write_sector(blocCylinder, blocSector, buffer);
}

void read_bloc_n(unsigned int vol, unsigned int nbloc, unsigned int n,
		unsigned char* buffer) {
	unsigned blocCylinder = cylinder_of_bloc(vol, nbloc);
	unsigned blocSector = sector_of_bloc(vol, nbloc);

	read_sector_n(blocCylinder, blocSector, n, buffer);
}
void write_bloc_n(unsigned int vol, unsigned int nbloc, unsigned int n,
		unsigned char* buffer) {
	unsigned blocCylinder = cylinder_of_bloc(vol, nbloc);
	unsigned blocSector = sector_of_bloc(vol, nbloc);

	write_sector_n(blocCylinder, blocSector, n, buffer);
}

void format_vol(unsigned int vol) {
	format_sector(mbr.mbr_vol[vol].vol_first_cylinder,
			mbr.mbr_vol[vol].vol_first_sector, mbr.mbr_vol[vol].vol_n_sector,
			0);
}

char* get_vol_type(enum vol_type_e type) {

	if (type == VOLT_PR) {
		return "PRIMARY";
	}

	if (type == VOLT_SND) {
		return "SECONDARY";
	}

	return "OTHER";
}