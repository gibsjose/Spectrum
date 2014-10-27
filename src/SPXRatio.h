//************************************************************/
//
//	Ratio Header
//
//	Outlines the SPXRatio class, which validates the ratio
//  type string, and then grabs the required TGraphs and
//  performs the necessary divisions, returning a fully
//  configured and ready-to-draw TGraph.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXRATIO_H
#define SPXRATIO_H

#include <map>

#include "SPXROOT.h"

#include "SPXRatioStyle.h"
#include "SPXUtilities.h"
#include "SPXException.h"

class SPXRatio {

public:

    SPXRatio(SPXRatioStyle &rs, std::string &s, std::map<std::string, TGraphAsymmErrors *> &fileToGraphMap) {
    	ratioStyle = rs;
        this->fileToGraphMap = fileToGraphMap;
    	Parse(s);
    }

    void Parse(std::string &s);
    void Print(void);

    static bool GetDebug(void) {
        return debug;
    }

    static void SetDebug(bool b) {
        debug = b;
    }

    std::string & GetNumeratorBlob(void) {
        return numeratorBlob;
    }

    std::string & GetDenominatorBlob(void) {
        return denominatorBlob;
    }

    TGraphAsymmErrors *GetRatioGraph(void) {

    	try {
    		ratioGraph = SPXGraphUtilities::Divide(numeratorGraph, denominatorGraph, AddErrors);
    		return ratioGraph;
    	} catch(const SPXException &e) {
    		std::cerr << e.what() << std::endl;

    		throw SPXGraphException("SPXRatio::GetRatioGraph: Unable to divide numerator and denominator to calculate ratio");
    	}
    }

    TGraphAsymmErrors *GetNumeratorGraph(void) {
    	if(!numeratorGraph) {
    		throw SPXGraphException("SPXRatio::GetNumeratorGraph: Numerator graph is empty");
    	}

    	return numeratorGraph;
    }

    TGraphAsymmErrors *GetDenominatorGraph(void) {
    	if(!denominatorGraph) {
    		throw SPXGraphException("SPXRatio::GetDenominatorGraph: Denominator graph is empty");
    	}

    	return denominatorGraph;
    }

    std::string & ToString(void) {
        return ratioString;
    }

private:
    static bool debug;

    SPXRatioStyle ratioStyle;

    std::string numeratorBlob;
    std::string denominatorBlob;

    std::string ratioString;

    std::string numeratorConvoluteGridFile;			//Grid file if numerator contains convolute
    std::string numeratorConvolutePDFFile;			//PDF file if numerator contains convolute
    std::string numeratorDataFile;					//Data file if numerator contains data
    std::string denominatorConvoluteGridFile;		//Grid file if denominator contains convolute
    std::string denominatorConvolutePDFFile;		//PDF file if denomintator contains convolute
    std::string denominatorReferenceGridFile;		//Grid file if denominator contains reference
    std::string denominatorDataFile;				//Data file if denominator contains data

    TGraphAsymmErrors *numeratorGraph;
    TGraphAsymmErrors *denominatorGraph;

    TGraphAsymmErrors *ratioGraph;

    std::map<std::string, TGraphAsymmErrors *> fileToGraphMap;

    bool MatchesConvoluteString(std::string &s);
    void GetGraphs(void);
};

#endif
