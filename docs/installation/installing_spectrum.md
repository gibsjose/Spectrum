![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#Installing Spectrum

##Installing Dependencies
ROOT can be installed in any order, but the rest of the dependencies **must** be installed in the following order:

* LHAPDF
* HOPPET
* APPLGRID

The installation of LHAPDF and HOPPET is very straightforward. Simply follow the instructions posted on their page. The basic installation procedure is as follows, assuming that we have a dependency called `library` which is contained in the tarball `library.tgz`:

```bash
$ tar -xzf <library.tgz>
$ cd <library>
$ ./configure --prefix="path/to/your/desired/installation/"
$ make
$ make install
```

To install APPLgrid, the only extra step is to ensure that the LHAPDF and HOPPET libraries are installed and that their installation directories exist in the `PATH` and `LD_LIBRARY_PATH` variables. The Following script can be used, assuming you are on a CERN machine with AFS access and SLC6. Otherwise, this script can be easily adapted to suit your needs.

```bash
#!/bin/bash

user=$USER
U=${user:0:1}

export WORK=/afs/cern.ch/work/${U}/${USER}

export platform=x86_64-slc6-gcc46-opt

export external=/afs/cern.ch/sw/lcg/external
export releases=/afs/cern.ch/sw/lcg/app/releases
export contrib=/afs/cern.ch/sw/lcg/contrib

source ${contrib}/gcc/4.6/${platform}/setup.sh

export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/${platform}/root
export HOPPET=${WORK}/public/usr-slc6/hoppet/1.1.5

export PATH=${HOPPET}/bin:${ROOTSYS}/bin:${PATH}
export LD_LIBRARY_PATH=${HOPPET}/lib:${ROOTSYS}/lib:${LD_LIBRARY_PATH}
```

Next, repeat the same steps as above to install APPLgrid.

Installation instructions for ROOT can be found [here](http://root.cern.ch/drupal/content/downloading-root), although typically you simply download a ROOT binary for your distribution.

To check that you have properly installed the dependencies, the following commands should be able to be executed without issue:

```bash
$ root-config --version
$ applgrid-config --version
$ hoppet-config --prefix
$ lhapdf-config --version
```

##Building Spectrum

Once all the dependencies are installed properly, you should be able to proceed with installing Spectrum.

For the makefile to work properly, you must ensure that your `PATH` contains the correct path to the required libraries, and that your `LD_LIBRARY_PATH` does as well. If you installed the dependencies correctly, this should not be an issue.

To this end, there is a `scripts/` directory that contains a plentitude of scripts that we have tested on different systems. Some may be more specific than others, but all are adaptable with minimal effort.

Now you must **source** the setup script. You cannot simply run the setup script: `./setup_script.sh` of course, since this spawns a new process and the variables will not be set when it returns to the parent.

```bash
$ source setup_script.sh
```

Now you can proceed with using the Spectrum makefile:

```bash
$ make
```

That's it. If there are any errors issued by the makefile, especially if there are **lots** of errors, check to make sure that your `PATH` and `LD_LIBRARY_PATH` variables are properly set to contain the `.../bin` and `.../lib` directories of the dependencies.
