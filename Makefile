# $Id: Makefile,v 1.2 2009/11/17 10:02:37 marquet Exp $
##############################################################################

ROOTDIR=/home/qwarnant/workspace/ASE

CC	=  gcc 
CFLAGS	=  -m32 -Wall -Werror -ansi -pedantic
CFLAGS  += -std=iso9899:1999
CFLAGS  += -g -w
LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= if_pfile if_nfile if_cfile if_dfile mvol checkdisk
OBJECTS	= $(addsuffix .o,\
	  tools ifile mount inode bloc mbr drive)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Binaries
###------------------------------------------------------------
if_cfile : if_cfile.o $(OBJECTS)
if_nfile : if_nfile.o $(OBJECTS)
if_dfile : if_dfile.o $(OBJECTS)
if_pfile : if_pfile.o $(OBJECTS)
mvol : mvol.o $(OBJECTS)
checkdisk: checkdisk.o $(OBJECTS)

% : %.o
	$(CC) -m32 -o $@ $^ -L$(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
bloc.o: bloc.c bloc.h mbr.h tools.h
drive.o: drive.c drive.h config.h
if_cfile.o: if_cfile.c ifile.h inode.h drive.h tools.h mount.h
if_dfile.o: if_dfile.c ifile.h inode.h drive.h tools.h mount.h
if_nfile.o: if_nfile.c ifile.h inode.h drive.h tools.h mount.h bloc.h
if_pfile.o: if_pfile.c ifile.h inode.h drive.h tools.h mount.h
ifile.o: ifile.c inode.h drive.h tools.h ifile.h bloc.h mbr.h
inode.o: inode.c inode.h drive.h tools.h mbr.h bloc.h ifile.h tools.h
mbr.o: mbr.c mbr.h drive.h 
mount.o: mount.c config.h tools.h mbr.h bloc.h drive.h
tools.o: tools.c tools.h
mvol.o: mvol.c mbr.h drive.h
checkdisk.o: checkdisk.c drive.h hardware.h mbr.h
%.o: %.c
	$(CC) $(CFLAGS) -c $< 

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean depend
clean:
	$(RM) *.o $(BINARIES)
depend :
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 
