/* ------------------------------
   $Id: mount.c,v 1.2 2009/11/17 16:20:54 marquet Exp $
   ------------------------------------------------------------

   Initialization and finalization
   Philippe Marquet, Nov 2009
   
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "hardware.h"
#include "config.h"
#include "tools.h"

#ifdef SOL
#   include "mbr+sol.h"
#   include "super+sol.h"
#else
#	include "drive.h"
#   include "mbr.h"
#	include "bloc.h"
#endif

/* load super bloc of the $CURRENT_VOLUME
   set CURRENT_VOL accordingly */
static void
load_current_vol ()
{
    char* current_vol_str;
    int status;

    current_vol_str = getenv("CURRENT_VOL");
    ffatal(current_vol_str != NULL, "no definition of $CURRENT_VOL");

    errno = 0;
    CURRENT_VOL = strtol(current_vol_str, NULL, 10);
    ffatal(!errno, "bad value of $CURRENT_VOL %s", current_vol_str);
    
    status = load_super(CURRENT_VOL);
    ffatal(status, "unable to load super of vol %d", CURRENT_VOL);
}

/* ------------------------------
   Initialization and finalization fucntions
   ------------------------------------------------------------*/
void
mount()
{
    char *hw_config;
    int status, i; 

    /* Hardware initialization */
    init_master();
    hw_config = HW_CONFIG;
    
    /* Allows all IT */
    _mask(1);

    /* Load MBR and current volume */
    load_mbr();
    load_current_vol();
}

void
umount()
{
    /* save current volume super bloc */
    save_super();

    /* bye */
}
 

