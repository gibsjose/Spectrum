#!/bin/bash

# Spectrum Setup Script for Joe's SLC6 Virtual Machine

BASEDIR=/usr/local

### Add program paths to standard paths PATH & LD_LIBRARY_PATH

### ROOT ###
# Uncomment to use ROOT v5.34.05
export ROOTSYS=${BASEDIR}/root

### LHAPDF ###
export LHAPDFPATH=${BASEDIR}/lhapdf/5.9.1

### FASTJET ###
#export FASTJET=/afs/cern.ch/sw/lcg/external/fastjet/3.0.3/${platform}

### HOPPET ###
export HOPPET=${BASEDIR}/hoppet/1.1.5

### APPLGRID ###
export APPLGRID=${BASEDIR}/applgrid/1.4.56

### SHERPA ###
#export SHERPA=${BASEDIR}/sherpa

#
### explicitly update paths
export PATH=${FASTJET}/bin:${HOPPET}/bin:${APPLGRID}/bin:${SHERPA}/bin:${ROOTSYS}/bin:${LHAPDFPATH}/bin:${PATH}
export LD_LIBRARY_PATH=${FASTJET}/lib:${HOPPET}/lib:${APPLGRID}/lib:${SHERPA}/lib:${ROOTSYS}/lib:${LHAPDFPATH}/lib:${LD_LIBRARY_PATH}

export ARCH="-m64"
export CXXFLAGS=$ARCH
export F90FLAGS=$ARCH
export CFLAGS=$ARCH
export FFLAGS=$ARCH
export LDFLAGS=$ARCH
