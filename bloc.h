/*
 * sbloc.h
 *
 *  Created on: 4 nov. 2014
 *      Author: warnant
 */

#ifndef SBLOC_H_
#define SBLOC_H_

/* Define the number of the super bloc of a volume */
#define SUPER 0

/* Define the magic number of the super bloc structure */
#define SUPER_MAGIC 0xCAFEBABE

/* Define the number of a null bloc when an error occurred */
#define BLOC_NULL 0

/* Define the current volume used for the file operations */
unsigned CURRENT_VOL;

/* Define the current super bloc of the current volume */
struct super_s current_super;

/* Define the structure of the super bloc of a volume */
struct super_s {
	unsigned int super_magic;
	unsigned int super_first_free_bloc;
	unsigned int super_nb_free;
	char* super_serial_number;
	char* super_name;
};

/* Define the free bloc structure in the free bloc list of the volume */
struct free_bloc_s {
	unsigned int fb_n_free;
	unsigned int fb_next;
};

/*
 * init_super()
 * This method initializes the structure of a new super bloc for the current volume
 */
void init_super(unsigned int);

/*
 * load_super()
 * This method searches for the current super structure which is written on the disk and load it in the memory
 */
int load_super(unsigned int);

/*
 * save_super()
 * This method writes the current super bloc structure on the disk
 */
void save_super();

/*
 * init_volume()
 * This method initializes the CURRENT_VOL global var
 * If the environment variable is set, the CURRENT_VOL receives this value
 */
int init_volume();

/*
 * new_bloc()
 * This method searches for a free bloc in the free bloc list and returns the first free bloc
 * if the disk is not full
 */
unsigned int new_bloc();

/*
 * free_bloc()
 * This method dealloc the bloc and put it in the free bloc list
 */
void free_bloc(unsigned int bloc);

/*
 * free_blocs()
 * This method dealloc an array of blocs and put all in the free bloc list
 */
void free_blocs(unsigned int*, unsigned int);


#endif /* SBLOC_H_ */
