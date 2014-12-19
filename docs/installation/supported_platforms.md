![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#Supported Platforms

Any platform with a C++ compiler is capable of running Spectrum. However, there are more specific requirements for the libraries that Spectrum depends on, such as APPLgrid, LHAPDF, and Hoppet. Some of these libraries also require a fortran compiler.

In essence, any machine with the following tools is capable of building and running Spectrum:

* GCC C++ Compiler (or any C++ compiler)
* gFortran Compiler (or any fortran compiler)
* GNU Make (non-GNU Make will result in some makefile features not working properly (i.e. header file dependency listings))
