#ifndef MBR_H_
#define MBR_H_

/* Define the magic number of the MBR structure */
#define MBR_MAGIC 0xCAFEBABE

/* Define the maximum number of volume on the disk */
#define MAX_VOL 8

/* Define the current default volume number */
#define DFT_CURRENT_VOLUME 1

/* Define the MBR of the current disk */
struct mbr_s mbr;

/* Define the several types of a disk volume */
enum vol_type_e {
	VOLT_PR, /* Primary */
	VOLT_SND, /* Secondary */
	VOLT_OTHER, /* Others */
	VOLT_FREE /* Free volume */
};

/* Define the structure of a disk volume */
struct vol_s {
	unsigned int vol_first_cylinder;
	unsigned int vol_first_sector;
	unsigned int vol_n_sector;
	enum vol_type_e vol_type;
};

/* Define the structure of the master boot record of a disk */
struct mbr_s {
	struct vol_s mbr_vol[MAX_VOL];
	unsigned int mbr_n_vol;
	unsigned int mbr_magic;
};

/*
 * load_mbr()
 * This method loads the master boot record (MBR) of the current disk in the memory
 */
void load_mbr();

/*
 * save_mbr()
 * This method saves the current master boot record (MBR) from the memory to the disk
 */
void save_mbr();

/*
 * read_bloc()
 * This method reads a bloc from the disk and put it in the memory
 */
void read_bloc(unsigned int, unsigned int, unsigned char*);

/*
 * write_bloc()
 * This method writes a bloc from the memory to the disk
 */
void write_bloc(unsigned int, unsigned int, unsigned char*);

/*
 * read_bloc_n()
 * This method reads a specified number of blocs from the disk and put these blocks in the memory
 */
void read_bloc_n(unsigned int, unsigned int, unsigned int, unsigned char*);

/*
 * write_bloc_n()
 * This method writes a specified number of blocs from the memory to the disk
 */
void write_bloc_n(unsigned int, unsigned int, unsigned int, unsigned char*);

/*
 * format_vol()
 * This method erases the content of an entire disk
 */
void format_vol(unsigned int);

/*
 * get_vol_type()
 * Returns the string of the volume type's name
 */
char* get_vol_type(enum vol_type_e);
#endif
