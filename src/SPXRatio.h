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

#include "SPXPlotConfiguration.h"
#include "SPXRatioStyle.h"
#include "SPXUtilities.h"
#include "SPXException.h"

class SPXRatio {

public:

    SPXRatio(SPXPlotConfiguration &pc, SPXRatioStyle &rs) {
        plotConfiguration = pc;
    	ratioStyle = rs;
    }

    void Parse(std::string &s);
    void AddDataFileGraphMap(StringGraphMap_T &);
    void AddReferenceFileGraphMap(StringGraphMap_T &);
    void AddConvoluteFileGraphMap(StringPairGraphMap_T &);
    void GetGraphs(void);
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

    bool HasConvolute(void) {
        if(ratioStyle.IsDataOverData()) {
            return false;
        }

        return true;
    }

    bool HasData(void) {
        if(ratioStyle.IsConvoluteOverReference()) {
            return false;
        }

        return true;
    }

    bool HasReference(void) {
        if(ratioStyle.IsConvoluteOverReference()) {
            return true;
        }

        return false;
    }

    void Divide(void) {
        //Grab the plot configuration instance
        SPXPlotConfigurationInstance pci;

        if(ratioStyle.IsConvoluteOverData() || ratioStyle.IsConvoluteOverReference()) {
            pci = plotConfiguration.GetPlotConfigurationInstance(numeratorConvolutePDFFile);
            pci.Print();
        }

        else if(ratioStyle.IsDataOverConvolute()) {
            pci = plotConfiguration.GetPlotConfigurationInstance(denominatorConvolutePDFFile);
        }

        //@TODO What if it's Data/Data???
        else {
            if(debug) std::cout << "SPXRatio::Divide: Data/Data: Could not get pci" << std::endl;
        }

        try {
            //Divide graphs
            ratioGraph = SPXGraphUtilities::Divide(numeratorGraph, denominatorGraph, AddErrors);

            //Style ratio graph
            if(true) {
                if(debug) std::cout << "SPXRatio::Divide: Obtaining PDF Fill Options..." << std::endl;
                ratioGraph->SetFillStyle(pci.pdfFillStyle);
                ratioGraph->SetFillColor(pci.pdfFillColor);
                ratioGraph->SetMarkerStyle(pci.pdfMarkerStyle);
                ratioGraph->SetMarkerColor(pci.pdfFillColor);
                ratioGraph->SetLineColor(pci.pdfFillColor);
                ratioGraph->SetMarkerSize(1.0);

                if(debug) std::cout << "SPXRatio::Divide: Set PDF Fill Options:" << std::endl;
                if(debug) std::cout << "\t Fill Style = " << pci.pdfFillStyle << std::endl;
                if(debug) std::cout << "\t Fill Color = " << pci.pdfFillColor << std::endl;
                if(debug) std::cout << "\t Marker Style = " << pci.pdfMarkerStyle << std::endl;
            }
        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;
            throw SPXGraphException("SPXRatio::Divide: Unable to divide numerator and denominator to calculate ratio");
        }
    }

    TGraphAsymmErrors *GetRatioGraph(void) {
        if(!ratioGraph) {
            throw SPXGraphException("SPXRatio::GetRatioGraph: Ratio graph is empty");
        }

        return ratioGraph;
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

    void SetDataDirectory(const std::string &dir) {
        dataDirectory = dir;
    }

    void SetGridDirectory(const std::string &dir) {
        gridDirectory = dir;
    }

    void SetPDFDirectory(const std::string &dir) {
        pdfDirectory = dir;
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

    std::string numeratorConvoluteGridFile;			//Grid file if numerator contains convolute
    std::string numeratorConvolutePDFFile;			//PDF file if numerator contains convolute
    std::string numeratorDataFile;					//Data file if numerator contains data
    std::string denominatorConvoluteGridFile;		//Grid file if denominator contains convolute
    std::string denominatorConvolutePDFFile;		//PDF file if denomintator contains convolute
    std::string denominatorReferenceGridFile;		//Grid file if denominator contains reference
    std::string denominatorDataFile;				//Data file if denominator contains data

    TGraphAsymmErrors *numeratorGraph;              //TGraph of the numerator
    TGraphAsymmErrors *denominatorGraph;            //TGraph of the denominator
    TGraphAsymmErrors *ratioGraph;                  //TGraph: numeratorGraph / denominatorGraph

    StringGraphMap_T * dataFileGraphMap;            //Map of data files to their data TGraph
    StringGraphMap_T * referenceFileGraphMap;       //Map of grid files to their reference TGraph
    StringPairGraphMap_T * convoluteFileGraphMap;   //Map of grid/pdf file pairs to their convolute TGraph

    bool MatchesConvoluteString(std::string &s);
};

#endif
