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
#include "SPXDrawUtilities.h"
#include "SPXFileUtilities.h"
#include "SPXMathUtilities.h"

namespace Test {
    extern bool TestFeatures;
}

namespace Options {
    extern bool Metadata;
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

class SPXUtilities {
public:

    static bool SortLargestRelativeError(TGraphAsymmErrors * g1, TGraphAsymmErrors * g2);
    static bool SortLargestBinNumber    (TGraphAsymmErrors * g1, TGraphAsymmErrors * g2);

    static std::map<int, TGraphAsymmErrors * > OrderBandMap(std::vector< TGraphAsymmErrors *> inputgraph);
    //static std::vector<TGraphAsymmErrors * > OrderGraphVector(std::vector< TGraphAsymmErrors *> inputgraph);
    static std::vector<TGraphAsymmErrors * > OrderLargestRelativeErrorGraphVector (std::vector< TGraphAsymmErrors *> inputgraph);


    static std::vector<TGraphAsymmErrors * > OrderLargestBinNumberGraphVector (std::vector< TGraphAsymmErrors *> inputgraph);

    static Color_t ICol(int i); // give color codes as function of 1...n
    static Color_t IColBlue(int i); // give color codes as function of 1...n
    static Color_t IColRed(int i); // give color codes as function of 1...n
    static Color_t IColGreen(int i); // give color codes as function of 1...n


    static void PrintMethodHeader(const std::string &className, const std::string &methodName) {
        std::cout << std::endl;
        std::cout << "================================================================" << std::endl;
        std::cout << className << methodName << std::endl;
        std::cout << "================================================================" << std::endl;
    }
};

#endif
