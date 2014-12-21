#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "drive.h"
#include "config.h"

static void check_hda() {
	int sectorSize, sectorMax, cylinderMax;
	_out(HDA_CMDREG, CMD_DSKINFO);

	sectorSize = _in(HDA_DATAREGS + 4);
	sectorSize = sectorSize << 8;
	sectorSize |= _in(HDA_DATAREGS + 5);
	assert(sectorSize == HDA_SECTORSIZE);

	sectorMax = _in(HDA_DATAREGS + 2);
	sectorMax = sectorMax << 8;
	sectorMax |= _in(HDA_DATAREGS + 3);
	assert(sectorMax == HDA_MAXSECTOR);

	cylinderMax = _in(HDA_DATAREGS);
	cylinderMax = cylinderMax << 8;
	cylinderMax |= _in(HDA_DATAREGS + 1);

	assert(cylinderMax == HDA_MAXCYLINDER);
}

static void goto_sector(unsigned int cylinder, unsigned int sector) {
	check_hda();
	check_hda_bounds(cylinder, sector);

	_out(HDA_DATAREGS, (cylinder >> 8) & 0xFF);
	_out(HDA_DATAREGS + 1, cylinder & 0xFF);
	_out(HDA_DATAREGS + 2, (sector >> 8) & 0xFF);
	_out(HDA_DATAREGS + 3, sector & 0xFF);
	_out(HDA_CMDREG, CMD_SEEK);
	_sleep(HDA_IRQ);
}

static void init_hw_lib() {
	/* HARDWARE LIB INITIALIZATION */
	HW_CONFIG = getenv("HW_CONFIG");
	if (HW_CONFIG == NULL) {
		HW_CONFIG = DFT_HW_CONFIG;
		fprintf(stderr,
				"Warning : The env variable HW_CONFIG is not set on your system. The default value %s has been chosen for the current execution.\n",
				HW_CONFIG);
	}

}

static void dummy() {
}


void check_hda_bounds(int cylinder, int sector) {
	if (cylinder
			< 0|| cylinder >= HDA_MAXCYLINDER || sector < 0 || sector >= HDA_MAXSECTOR) {
		fprintf(stderr, "Error : Number of cylinder %d or sector %d is out of range.\n", cylinder, sector);
		exit(EXIT_FAILURE);
	}
}

void init_master() {
	int i;

	init_hw_lib();

	if (init_hardware(HW_CONFIG) == 0) {
		fprintf(stderr, "Error for disk initialization\n");
		exit(1);
	}

	for (i = 0; i < IRQ_VECOTR_SIZE; i++) {
		IRQVECTOR[i] = dummy;
	}

}

void read_sector(unsigned int cylinder, unsigned int sector,
		unsigned char* buffer) {
	read_sector_n(cylinder, sector, HDA_SECTORSIZE, buffer);
}
void read_sector_n(unsigned int cylinder, unsigned int sector,
		unsigned int size, unsigned char* buffer) {

	goto_sector(cylinder, sector);

	_out(HDA_DATAREGS, 1);
	_out(HDA_DATAREGS + 1, 1);
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);

	memcpy(buffer, MASTERBUFFER, size);
}

void write_sector(unsigned int cylinder, unsigned int sector,
		unsigned char* buffer) {
	write_sector_n(cylinder, sector, HDA_SECTORSIZE, buffer);
}
void write_sector_n(unsigned int cylinder, unsigned int sector,
		unsigned int size, unsigned char* buffer) {

	memcpy(MASTERBUFFER, buffer, size);

	goto_sector(cylinder, sector);

	_out(HDA_DATAREGS, 1);
	_out(HDA_DATAREGS + 1, 1);

	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
}

void format_sector(unsigned int cylinder, unsigned int sector,
		unsigned int nsector, unsigned int value) {
	check_hda_bounds(cylinder, sector);

	goto_sector(cylinder, sector);

	_out(HDA_DATAREGS, (nsector >> 8) && 0xFF);
	_out(HDA_DATAREGS + 1, nsector & 0xFF);

	_out(HDA_DATAREGS + 2, (value >> 24) && 0xFF);
	_out(HDA_DATAREGS + 3, (value >> 16) && 0xFF);
	_out(HDA_DATAREGS + 4, (value >> 8) && 0xFF);
	_out(HDA_DATAREGS + 5, value & 0xFF);

	_out(HDA_CMDREG, CMD_FORMAT);

	_sleep(HDA_IRQ);
}

/* dump buffer to stdout,
 and octal dump if octal_dump; an ascii dump if ascii_dump! */
void dump(unsigned char *buffer, unsigned int buffer_size, int ascii_dump,
		int octal_dump) {
	int i, j;

	for (i = 0; i < buffer_size; i += 16) {
		/* offset */
		printf("%.8o", i);

		/* octal dump */
		if (octal_dump) {
			for (j = 0; j < 8; j++)
				printf(" %.2x", buffer[i + j]);
			printf(" - ");

			for (; j < 16; j++)
				printf(" %.2x", buffer[i + j]);

			printf("\n");
		}
		/* ascii dump */
		if (ascii_dump) {
			printf("%8c", ' ');

			for (j = 0; j < 8; j++)
				printf(" %1c ", isprint(buffer[i+j]) ? buffer[i + j] : ' ');
			printf(" - ");

			for (; j < 16; j++)
				printf(" %1c ", isprint(buffer[i+j]) ? buffer[i + j] : ' ');

			printf("\n");
		}

	}
}
