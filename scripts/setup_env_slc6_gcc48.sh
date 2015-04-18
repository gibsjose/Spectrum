#!/bin/bash
# 
# 
### generic setup to be run at the start of EVERY LXPLUS session
#  SLC6 system with gcc4.8
#
#Get the first letter of the user's name
user=$USER
U=${user:0:1}

### setup for SLC6
#export platform=x86_64-slc6-gcc46-opt
export platform=x86_64-slc6-gcc48-opt
# 
#Install directory for installation of APPLgrid Hoppet  etc.
#export INSTALLDIR=/afs/cern.ch/work/$U/$USER/public/usr-slc6/
#export INSTALLDIR=/afs/cern.ch/work/$U/$USER/public/${platform}
export INSTALLDIR=/afs/cern.ch/work/a/applgrid/public/${platform}
#export BASEDIR=/afs/cern.ch/user/$U/$USER/www/Spectrum 
export BASEDIR=`pwd`
#
#
export external=/afs/cern.ch/sw/lcg/external
export releases=/afs/cern.ch/sw/lcg/app/releases
export contrib=/afs/cern.ch/sw/lcg/contrib
#
#source ${contrib}/gcc/4.6/${platform}/setup.sh
source ${contrib}/gcc/4.8.1/${platform}/setup.sh
#
#
#
### Add program paths to standard paths PATH & LD_LIBRARY_PATH

### ROOT ###
# Uncomment to use ROOT v5.34.05
#export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/${platform}/root
export  ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/6.02.05/${platform}/root

### LHAPDF ###
# version 5
#export LHAPDFPATH=${BASEDIR}/lhapdf/5.9.1
#export LHAPDFPATH=/afs/cern.ch/sw/lcg/external/MCGenerators/lhapdf/5.8.9

# version 6

export LHAPDFPATH=/afs/cern.ch/sw/lcg/external/MCGenerators_lcgcmt67c/lhapdf/6.1.5
export LHAPDFBIN=$LHAPDFPATH/${platform}
#export LHAPATH=$LHAPDFPATH/share/lhapdf/PDFsets
export LHAPATH=/afs/cern.ch/sw/lcg/external/lhapdfsets/current
#
#export LHAPDFPATH=/afs/cern.ch/sw/lcg/external/MCGenerators_lcgcmt67b/lhapdf6/6.1.5/${platform}
#export LHAPDF_DATA_PATH=/afs/cern.ch/sw/lcg/external/lhapdfsets/current
#export LHAPDF_DATA_PATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current


### FASTJET ###
#export FASTJET=/afs/cern.ch/sw/lcg/external/fastjet/3.0.3/${platform}

### HOPPET ###
export HOPPET=$INSTALLDIR/hoppet/1.1.5

### APPLGRID ###
export APPLGRID=$INSTALLDIR/applgrid/1.4.56 

### SHERPA ###
#export SHERPA=${BASEDIR}/sherpa
#
### explicitly update paths
export PATH=${APPLGRID}/bin:${HOPPET}/bin:${LHAPDFBIN}/bin:${ROOTSYS}/bin:${PATH}
export LD_LIBRARY_PATH=${APPLGRID}/lib:${HOPPET}/lib:${ROOTSYS}/lib:${LHAPDFBIN}/lib:${LD_LIBRARY_PATH}
#
#
cd $BASEDIR
#rm -f PDFsets
#ln -s `lhapdf-config --pdfsets-path`  PDFsets

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
export CXX_STD="-std=c++0x"
export ARCH="-m64"
export CXXFLAGS=$ARCH
export F90FLAGS=$ARCH
export CFLAGS=$ARCH
export FFLAGS=$ARCH
export LDFLAGS=$ARCH
