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

//typedef std::pair<std::string, std::string> StringPair_T;

class SPXRatio {

public:

    SPXRatio(SPXPlotConfiguration &pc, SPXRatioStyle &rs) {
        plotConfiguration = pc;
    	ratioStyle = rs;
        MatchBinning=true;
        AddTotalSyst=0;  

        denominatorGraph=0;
	denominatorGraphstatonly=0;

    }

    void Parse(std::string &s);
    void AddDataFileGraphMap(StringGraphMap_T &);
    void AddDataFileLabelMap(StringLabelMap_T &);

    void AddReferenceFileGraphMap(StringPairGraphMap_T &);
    void AddNominalFileGraphMap(StringPairGraphMap_T &);
    void AddConvoluteFileGraphMap(StringPairGraphMap_T &);
    void AddConvoluteFilePDFMap(StringPairPDFMap_T &);
    void GetGraphs(void);
    void Print(void);
    void SetMatchBinning(bool matchbin) { MatchBinning=matchbin; return; }

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

    bool IsDataOverData(void) {
        return ratioStyle.IsDataOverData();
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

    void PrintDataFileGraphMapKeys(std::ostream &out = std::cout);
    void PrintDataFileLabelMapKeys(std::ostream &out = std::cout);

    void PrintReferenceFileGraphMapKeys(std::ostream &out = std::cout);
    void PrintNominalFileGraphMapKeys(std::ostream &out = std::cout);
    void PrintConvoluteFilePDFMapKeys(std::ostream &out = std::cout);
    void PrintConvoluteFileGraphMapKeys(std::ostream &out = std::cout);
 
    bool DataFileGraphMapHasSystematics();
   
    void Divide(void);

    std::vector<TGraphAsymmErrors *> GetRatioGraph(void) {
     if(ratioGraph.size()==0) {
      throw SPXGraphException("SPXRatio::GetRatioGraph: Ratio graph is empty"); 
     }
     return ratioGraph;
    }

    std::vector<TGraphAsymmErrors *> GetRatioGraphStat(void) {
     //if(ratioGraphstatonly.size()==0) {
       //throw SPXGraphException("SPXRatio::GetRatioGraph: Ratio graphstatonly is empty");
     //}
     return ratioGraphstatonly;
    }

    TGraphAsymmErrors *GetDenominatorGraph(void) {
      if(!denominatorGraph) {
    		throw SPXGraphException("SPXRatio::GetDenominatorGraph: graph is empty");
    	}

      return denominatorGraph;
    }

    //TGraphAsymmErrors *GetNumeratorGraph(void) {
    //  if(!numeratorGraph.back()) {
    //		throw SPXGraphException("SPXRatio::GetNumeratorGraph: graph is empty");
    // 	}
    //
    //  return numeratorGraph.back();
    //}

    void SetNumberatorDataLabel(const std::string &label) {
     numeratorDataLabel=label;		    
    }

    void SetDenominatorDataLabel(const std::string &label) {
     denominatorDataLabel=label;		    
    }
   
    std::string GetNumeratorDataLabel(void) {
     return numeratorDataLabel;		    
    }

    std::string GetDenominatorDataLabel(void) {
     return  denominatorDataLabel;		    
    }

    std::string GetDenominatorDataFile(void) {
     return denominatorDataFile;		   
    }

    std::string GetNumeratorDataFile(void) {
     return numeratorDataFile;
    }

    std::string & ToString(void) {
        return ratioString;
    }

    void SetDataDirectory(const std::string &dir) {
        dataDirectory = dir;
    }

    std::string GetDataDirectory() {
     return dataDirectory;
    }

    void SetGridDirectory(const std::string &dir) {
        gridDirectory = dir;
    }

    void SetPDFDirectory(const std::string &dir) {
        pdfDirectory = dir;
    }

    void Draw(std::string option, int istat, int itot, bool plotmarker, double xbox=0.5, double ybox=0.3);

    void SetAddTotalSystematics(int icol){
     AddTotalSyst=icol;  
     return; 
    }

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

    std::string numeratorConvoluteGridFile;	    // Grid file if numerator contains convolute
    std::string numeratorConvolutePDFFile;	    // PDF file if numerator contains convolute
    std::string numeratorDataFile;		    // Data file if numerator contains data
    std::string numeratorDataLabel;		    // Data Label file 

    std::string denominatorConvoluteGridFile;	    // Grid file if denominator contains convolute
    std::string denominatorConvolutePDFFile;	    // PDF file if denomintator contains convolute
    std::string denominatorReferenceGridFile;	    // Grid file if denominator contains reference
    std::string denominatorNominalGridFile;         // Grid file if denominator contains nominal

    std::string denominatorDataFile;		    //Data file if denominator contains data
    std::string denominatorDataLabel;		    // Data Label file 

    std::vector<TGraphAsymmErrors *> numeratorGraph;
    std::vector<TGraphAsymmErrors *> numeratorGraphstatonly;
    std::vector<TGraphAsymmErrors *> ratioGraph;
    std::vector<TGraphAsymmErrors *> ratioGraphstatonly;

    int AddTotalSyst;  // Flag to add total systematics in ratio 0: off >0 color
    bool MatchBinning;  //Flag to indocate if matchbinning should be called

    TGraphAsymmErrors *denominatorGraph;            //TGraph of the denominator
    TGraphAsymmErrors *denominatorGraphstatonly;    //TGraph of the denominator for statistical uncertainty only

    StringGraphMap_T * dataFileGraphMap;            //Map of data files to their data TGraph

    StringLabelMap_T * dataFileLabelMap;        //Map of data file to their data labels
    StringPairGraphMap_T * referenceFileGraphMap;   //Map of grid/graph file pairs to their grid reference TGraph
    StringPairGraphMap_T * nominalFileGraphMap;     //Map of grid/graph file pairs to their pdf nominal TGraph
    StringPairGraphMap_T * convoluteFileGraphMap;   //Map of grid/graph file pairs to their convolute pdf objects
    StringPairPDFMap_T * convoluteFilePDFMap;       //Map of grid/pdf file pairs to their convolute pdf objects

    bool MatchesConvoluteString(std::string &s);

};

#endif
