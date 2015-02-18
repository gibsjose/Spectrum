//************************************************************/
//
//	Graph Utilities Header
//
//	Some useful graph utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		13.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXGRAPHUTILITIES_H
#define SPXGRAPHUTILITIES_H

#include <iostream>
#include <sstream>
#include <vector>

#include "SPXStringUtilities.h"
#include "SPXROOT.h"
#include "SPXException.h"

//Adds extra space for the frame bounds (set to OFF to disable)
#define PERFORM_DELTA_MIN_MAX	1

#if PERFORM_DELTA_MIN_MAX
const double DELTA_MIN_MAX = 0.10;	//Extra space on the graph for min/max: 0.10 = 10%
#endif

typedef enum DivideErrorType_t {
	AddErrors = 0,					//Add Y Errors quadratically
	ZeroAllErrors = 1,				//Set all Y Errors to zero
	ZeroNumGraphErrors = 2,			//Set Numerator Graph Y Errors to zero
	ZeroDenGraphErrors = 3 			//Set Denominator Graph Y Errors to zero
} DivideErrorType_t;

//Typedefs for SPXPlot and SPXRatio Maps
typedef std::map<std::string, TGraphAsymmErrors *> StringGraphMap_T;
typedef std::map<std::pair<std::string, std::string>, TGraphAsymmErrors *> StringPairGraphMap_T;

typedef std::pair<std::string, TGraphAsymmErrors *> StringGraphPair_T;
typedef std::pair<std::string, std::string> StringPair_T;
typedef std::pair<StringPair_T, TGraphAsymmErrors *> StringPairGraphPair_T;

class SPXGraphUtilities {

public:

	static double GetXMin(std::vector<TGraphAsymmErrors *> graphs);
	static double GetXMax(std::vector<TGraphAsymmErrors *> graphs);
	static double GetYMin(std::vector<TGraphAsymmErrors *> graphs);
	static double GetYMax(std::vector<TGraphAsymmErrors *> graphs);

	static void MatchBinning(TGraphAsymmErrors *master, TGraphAsymmErrors *slave, bool dividedByBinWidth);

	static TGraphAsymmErrors * Divide(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, DivideErrorType_t dt);

	static void ClearXErrors(TGraphAsymmErrors * g);
	static void ClearYErrors(TGraphAsymmErrors * g);

	static void SetAllXErrors(TGraphAsymmErrors * g, double err);
	static void SetAllYErrors(TGraphAsymmErrors * g, double err);

	static void ScaleXErrors(TGraphAsymmErrors * g, double scale);
	static void ScaleYErrors(TGraphAsymmErrors * g, double scale);

        static void SPXCopyGraphProperties(TGraphErrors* g1, TGraphErrors* g2);

	static TH1D* GraphToHistogram(TGraphAsymmErrors * g);

	static TH1D* GetEdgeHistogram(TGraphAsymmErrors * g, bool low = false);

	static void HistogramToGraph(TGraphAsymmErrors * g, TH1 *h);
	static void ClearGraph(TGraphAsymmErrors * g);

	static double GetYBinWidthUnitsScale(std::string master, std::string slave);
	static double GetXUnitsScale(std::string master, std::string slave);
	static double GetYUnitsScale(std::string master, std::string slave);

	static void Scale(TGraphAsymmErrors *graph, double xScale, double yScale);

	//static void Normalize(TGraphAsymmErrors *graph, double yBinWidthScale, bool normalizeToTotalSigma, bool divideByBinWidth);

	static double GetTotalSigma(TGraphAsymmErrors * g, bool dividedByBinWidth);
	static void DivideByBinWidth(TGraphAsymmErrors * g);

        static TGraphAsymmErrors* TH1TOTGraphAsymm(TH1 *h1);

        static void Multiply(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, int noerr=0);

        static int CompareValues(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, bool bandsize=true);
};

#endif
