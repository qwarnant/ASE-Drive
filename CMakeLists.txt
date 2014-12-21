cmake_minimum_required(VERSION 2.8.4)
project(ASE_TP07)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    bloc.c
    bloc.h
    config.h
    drive.c
    drive.h
    hardware.h
    hardware.ini
    if_cfile.c
    if_dfile.c
    if_nfile.c
    if_pfile.c
    ifile.c
    ifile.h
    inode.c
    inode.h
    Makefile
    mbr.c
    mbr.h
    mount.c
    mount.h
    checkdisk.c
    mvol.c
    README
    tools.c
    tools.h
    vdiskA.bin
    vdiskB.bin)

add_executable(ASE_TP07 ${SOURCE_FILES})