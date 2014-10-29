#!/bin/bash

#####################################################
#
# Setup Script for SLC5
#
#####################################################

#Get the first letter of the user's name
user=$USER
U=${user:0:1}

#
#Base directory for installation of APPLgrid, LHAPDF, Hoppet, FastJet, etc.
export BASEDIR=/afs/cern.ch/work/$U/$USER/public/usr-slc5/

#
#
#
### setup for SLC5
export platform=x86_64-slc5-gcc43-opt
export external=/afs/cern.ch/sw/lcg/external
export releases=/afs/cern.ch/sw/lcg/app/releases
export contrib=/afs/cern.ch/sw/lcg/contrib

export GCC43_INSTALL_DIR=${contrib}/gcc/4.3.2/${platform}
source ${contrib}/gcc/4.3/x86_64-slc5/setup.sh
export PATH=${GCC43_INSTALL_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC43_INSTALL_DIR}/lib64:${LD_LIBRARY_PATH}

#
#
#
### Add program paths to standard paths PATH & LD_LIBRARY_PATH

### ROOT ###
# Uncomment to use ROOT v5.34.05
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/${platform}/root

### LHAPDF ###
export LHAPDFPATH=${BASEDIR}/lhapdf/5.9.1

### FASTJET ###
export FASTJET=/afs/cern.ch/sw/lcg/external/fastjet/3.0.3/${platform}

### HOPPET ###
export HOPPET=${BASEDIR}/hoppet/1.1.5

### APPLGRID ###
export APPLGRID=${BASEDIR}/applgrid/1.4.56

### SHERPA ###
export SHERPA=${BASEDIR}/sherpa

#
### explicitly update paths
export PATH=${FASTJET}/bin:${HOPPET}/bin:${APPLGRID}/bin:${SHERPA}/bin:${ROOTSYS}/bin:${LHAPDFPATH}/bin:${PATH}
export LD_LIBRARY_PATH=${FASTJET}/lib:${HOPPET}/lib:${APPLGRID}/lib:${SHERPA}/lib:${ROOTSYS}/lib:${LHAPDFPATH}/lib:${LD_LIBRARY_PATH}

#
#
#
### Link to APPLgrids and PDFSets to local directory
#unlink PDFsets
#ln -s /afs/cern.ch/work/${U}/${USER}/public/PDFsets
#
#unlink MyGrids
#ln -s /afs/cern.ch/work/${U}/${USER}/public/MyGrids
#
#
#
export ARCH="-m64"
export CXXFLAGS=$ARCH
export F90FLAGS=$ARCH
export CFLAGS=$ARCH
export FFLAGS=$ARCH
export LDFLAGS=$ARCH
