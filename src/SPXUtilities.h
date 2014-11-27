//************************************************************/
//
//	Spectrum Utilities Header
//
//	Simply includes all the other utility classes
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXUTILITIES_H
#define SPXUTILITIES_H

#include "SPXStringUtilities.h"
#include "SPXGraphUtilities.h"
#include "SPXFileUtilities.h"
#include "SPXMathUtilities.h"

namespace Test {
    extern bool TestFeatures;
}

//Typedefs for Maps/Pairs
typedef std::map<std::string, std::vector<double> >  StringDoubleVectorMap_T;
typedef std::pair<std::string, std::vector<double> >  StringDoubleVectorPair_T;
typedef StringDoubleVectorMap_T::iterator StringDoubleVectorMapIterator_T;

//Matrix Typedefs
typedef std::vector<std::vector<double> >  DoubleMatrix_T;

typedef std::map<std::string, DoubleMatrix_T >  StringDoubleMatrixMap_T;
typedef std::pair<std::string, DoubleMatrix_T >  StringDoubleMatrixPair_T;
typedef StringDoubleMatrixMap_T::iterator StringDoubleMatrixMapIterator_T;

#endif
