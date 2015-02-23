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

#include "SPXPDF.h"

#include "SPXPlotConfiguration.h"
#include "SPXRatioStyle.h"
#include "SPXUtilities.h"
#include "SPXException.h"


typedef std::map<std::string, SPXPDF *> StringPDFMap_T;
typedef std::map<std::pair<std::string, std::string>, SPXPDF *> StringPairPDFMap_T;

typedef std::pair<std::string, SPXPDF *> StringPDFPair_T;
typedef std::pair<StringPair_T, SPXPDF *> StringPairPDFPair_T;


class SPXRatio {

public:

    SPXRatio(SPXPlotConfiguration &pc, SPXRatioStyle &rs) {
        plotConfiguration = pc;
    	ratioStyle = rs;
    }

    void Parse(std::string &s);
    void AddDataFileGraphMap(StringGraphMap_T &);
    void AddReferenceFileGraphMap(StringPairGraphMap_T &);
    void AddNominalFileGraphMap(StringPairGraphMap_T &);
    void AddConvoluteFileGraphMap(StringPairGraphMap_T &);
    void AddConvoluteFilePDFMap(StringPairPDFMap_T &);
    void GetGraphs(void);
    void Print(void);

    std::string CheckForAlias(std::string &, const std::string);

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

    SPXRatioStyle & GetRatioStyle(void) {
        return ratioStyle;
    }

    bool IsDataStat(void) {
        return ratioStyle.IsDataStat();
    }

    bool IsDataTot(void) {
        return ratioStyle.IsDataTot();
    }

    bool HasConvolute(void) {
        if(ratioStyle.IsDataOverConvolute() || ratioStyle.IsConvoluteOverData() || ratioStyle.IsConvoluteOverReference() || ratioStyle.IsConvoluteOverNominal()) {
            return true;
        }

        return false;
    }

    bool HasData(void) {
        if(ratioStyle.IsDataOverConvolute() || ratioStyle.IsConvoluteOverData() || ratioStyle.IsDataOverData()) {
            return true;
        }

        return false;
    }

    bool HasReference(void) {
        if(ratioStyle.IsConvoluteOverReference()) {
            return true;
        }

        return false;
    }

    bool HasNominal(void) {
        if(ratioStyle.IsConvoluteOverNominal()) {
            return true;
        }

        return false;
    }

    void PrintDataFileGraphMapKeys(std::ostream &out = std::cout) {
        out << "SPXRatio::PrintDataFileGraphMapKeys: " << std::endl;
        out << "\t Key Format: [Data]" << std::endl;
        out << "\t ============ KNOWN KEYS ============" << std::endl;
        for(StringGraphMap_T::iterator it = dataFileGraphMap->begin(); it != dataFileGraphMap->end(); ++it) {
            out << "\t [" << it->first << "]" << std::endl;
        }
        out << "\t ====================================" << std::endl << std::endl;
    }

    void PrintReferenceFileGraphMapKeys(std::ostream &out = std::cout) {
        out << "SPXRatio::PrintReferenceFileGraphMapKeys: " << std::endl;
        out << "\t Key Format: [Grid, PDF]" << std::endl;
        out << "\t ============ KNOWN KEYS ============" << std::endl;
        for(StringPairGraphMap_T::iterator it = convoluteFileGraphMap->begin(); it != convoluteFileGraphMap->end(); ++it) {
            out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
        }
        out << "\t ====================================" << std::endl << std::endl;
    }

    void PrintNominalFileGraphMapKeys(std::ostream &out = std::cout) {
        out << "SPXRatio::PrintNominalFileGraphMapKeys: " << std::endl;
        out << "\t Key Format: [Grid, PDF]" << std::endl;
        out << "\t ============ KNOWN KEYS ============" << std::endl;
        for(StringPairGraphMap_T::iterator it = convoluteFileGraphMap->begin(); it != convoluteFileGraphMap->end(); ++it) {
            out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
        }
        out << "\t ====================================" << std::endl << std::endl;
    }

    void PrintConvoluteFilePDFMapKeys(std::ostream &out = std::cout) {
        out << "SPXRatio::PrintConvoluteFilePDFMapKeys: " << std::endl;
        out << "\t Key Format: [Grid, PDF]" << std::endl;
        out << "\t ============ KNOWN KEYS ============" << std::endl;
        for(StringPairPDFMap_T::iterator it = convoluteFilePDFMap->begin(); it != convoluteFilePDFMap->end(); ++it) {
            out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
        }
        out << "\t ====================================" << std::endl << std::endl;
    }

    void PrintConvoluteFileGraphMapKeys(std::ostream &out = std::cout) {
        out << "SPXRatio::PrintConvoluteFileGraphMapKeys: " << std::endl;
        out << "\t Key Format: [Grid, PDF]" << std::endl;
        out << "\t ============ KNOWN KEYS ============" << std::endl;
        for(StringPairGraphMap_T::iterator it = convoluteFileGraphMap->begin(); it != convoluteFileGraphMap->end(); ++it) {
            out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
        }
        out << "\t ====================================" << std::endl << std::endl;
    }

    void Divide(void);

    std::vector<TGraphAsymmErrors *> GetRatioGraph(void) {
    if(ratioGraph.size()==0) {
     throw SPXGraphException("SPXRatio::GetRatioGraph: Ratio graph is empty");
    }
     return ratioGraph;
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

    void SetDataDirectory(const std::string &dir) {
        dataDirectory = dir;
    }

    void SetGridDirectory(const std::string &dir) {
        gridDirectory = dir;
    }

    void SetPDFDirectory(const std::string &dir) {
        pdfDirectory = dir;
    }

    void Draw(string option, int istat, int itot);

private:
    static bool debug;

    SPXPlotConfiguration plotConfiguration;
    SPXRatioStyle ratioStyle;

    std::string dataDirectory;
    std::string gridDirectory;
    std::string pdfDirectory;

    std::string numeratorBlob;
    std::string denominatorBlob;

    std::string ratioString;

    std::string numeratorConvoluteGridFile;			//Grid file if numerator contains convolute
    std::string numeratorConvolutePDFFile;			//PDF file if numerator contains convolute
    std::string numeratorDataFile;					//Data file if numerator contains data
    std::string denominatorConvoluteGridFile;		//Grid file if denominator contains convolute
    std::string denominatorConvolutePDFFile;		//PDF file if denomintator contains convolute
    std::string denominatorReferenceGridFile;		//Grid file if denominator contains reference
    std::string denominatorNominalGridFile;         //Grid file if denominator contains nominal
    std::string denominatorDataFile;				//Data file if denominator contains data

    std::vector<TGraphAsymmErrors *> numeratorGraph;
    std::vector<TGraphAsymmErrors *> ratioGraph;
    TGraphAsymmErrors *denominatorGraph;            //TGraph of the denominator

    StringGraphMap_T * dataFileGraphMap;            //Map of data files to their data TGraph
    StringPairGraphMap_T * referenceFileGraphMap;   //Map of grid/graph file pairs to their grid reference TGraph
    StringPairGraphMap_T * nominalFileGraphMap;     //Map of grid/graph file pairs to their pdf nominal TGraph
    StringPairGraphMap_T * convoluteFileGraphMap;   //Map of grid/graph file pairs to their convolute pdf objects
    StringPairPDFMap_T * convoluteFilePDFMap;     //Map of grid/pdf file pairs to their convolute pdf objects

    bool MatchesConvoluteString(std::string &s);

};

#endif
