//************************************************************/
//
//	Spectrum LHAPDF Header
//
//	Interfaces with the LHAPDF Fortran library
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		15.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

// include file (perhaps only valid in C++?) for interfacing to 
// LHAPDF. Note that an extra f77 routine initpdfsetC_ is needed
// in order to deal with difficulties of string passing...      
//
// NOTE: Currenty this is incomplete insofar as not all routines are
//       supported.

#ifndef __LHAPDF_h__
#define __LHAPDF_h__  1 

#include<string>
using namespace std;

//
// These are the direct definitions for the f77 part
// 
extern "C" void initpdfsetc_(const char *);
extern "C" void initpdf_(const int *);
extern "C" double alphaspdf_(const double *);
extern "C" void evolvepdf_(const double *, const double *, double *);

// These are slightly more C++ like ways of dealing with things...
// Do not create a class, since in any case we can only have one
// instance of the PDF object...

//inline void   initPDFset(const string & name) {  initpdfsetc_(name.c_str());}
inline void   initPDF(const int & mem) {initpdf_(&mem);}
inline double alphasPDF(const double & Q) {return alphaspdf_(&Q);}
inline double alphasUnity(const double & Q) { return 1.0000000;}

// returns x*f(x,Q^2) in a C-style array where f[0] is tbar, etc.
// f should of course be defined f[13]...
inline void   evolvePDF(const double & x, const double & Q, double * f) {
  evolvepdf_(&x , &Q, f);
}

#endif /* __LHAPDF_h__ */
