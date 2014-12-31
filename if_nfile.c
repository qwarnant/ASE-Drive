/* ------------------------------
   $Id: if_nfile.c,v 1.1 2009/11/16 05:38:07 marquet Exp $
   ------------------------------------------------------------

   Create a new file, read from stdin, return the inumber 
   Philippe Marquet, Nov 2009
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "ifile.h"
#include "mount.h"
#include "tools.h"
#include "bloc.h"

static void
nfile()
{
    file_desc_t fd;
    unsigned int inumber;
    int status;
    int c;
    
    inumber = create_ifile(IT_FILE);
    ffatal(inumber, "erreur creation fichier\n");
    printf("%u\n", inumber);

    status = open_ifile(&fd, inumber);
    ffatal(!status, "erreur ouverture fichier %d\n", inumber);
    
    while((c=getchar()) != EOF)
        if(writec_ifile(&fd, c) == RETURN_FAILURE) {
            ffatal(!status, "erreur ecriture caractere\n");
            assert(current_super.super_magic == SUPER_MAGIC);
        }

    close_ifile(&fd);
    assert(current_super.super_magic == SUPER_MAGIC);
}

static void
usage(const char *prgm)
{
    fprintf(stderr, "[%s] usage:\n\t"
            "%s\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
    if (argc != 1)
        usage(argv[0]);

    mount();
    assert(current_super.super_magic == SUPER_MAGIC);

    nfile();
    umount();
    assert(current_super.super_magic == SUPER_MAGIC);

    exit(EXIT_SUCCESS);         
}
