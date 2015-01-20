//************************************************************/
//
//	Plot Header
//
//	Outlines the SPXPlot class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT objects
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		13.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPLOT_H
#define SPXPLOT_H

#include <vector>
#include <set>
#include <map>

#include "SPXSteeringFile.h"

#include "SPXRatio.h"
#include "SPXCrossSection.h"
#include "SPXData.h"

#include "SPXException.h"

class SPXPlot {

public:
	explicit SPXPlot(SPXSteeringFile *steeringFile, unsigned int plotNumber) {
		this->steeringFile = steeringFile;
		this->id = plotNumber;

		if(debug) {
		 SPXData::SetDebug(true);
		 SPXDataFormat::SetDebug(true);
		 SPXCrossSection::SetDebug(true);
		 SPXGridCorrections::SetDebug(true);
		 SPXRatio::SetDebug(true);
		}
	}

	void Initialize(void);
	void Plot(void);

	//Helper methods //@TODO Public or private?
	void SetAxisLabels(void);
	void ScaleAxes(void);
	void CreateCanvas(void);
	void MatchOverlayBinning(void);
	//void ApplyGridCorrections(void);
	void DetermineOverlayFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax);
	void DetermineRatioFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax);
	void DivideCanvasIntoPads(void);
	void ConfigurePads(void);
	void DrawOverlayPadFrame(void);
	void DrawRatioPadFrame(void);
	void DrawOverlay(void);
	void DrawRatio(void);
	void DrawLegend();
	void UpdateCanvas(void);
	void CanvasToPNG(void);
	std::string GetPNGFilename(std::string desc);

	void StaggerConvoluteOverlay(void);
	void StaggerConvoluteRatio(void);
	void DrawDataStatErrors(void);

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;

		if(debug) {
			SPXData::SetDebug(true);
			SPXCrossSection::SetDebug(true);
		}
	}

private:
	static bool debug;					// Flag indicating debug mode
	SPXSteeringFile *steeringFile;				// Fully parsed steering file
	unsigned int id;					// Plot number ID (0-based)
	std::vector<SPXData> data;				// Vector of data
	std::vector<SPXCrossSection> crossSections;		// Vector of cross sections
	std::vector<SPXRatio> ratios;				// Vector of ratios

	//File-to-Graph Maps: Used to facilitate ratio drawing via the SPXRatio class
	//Each ratio object will maintain a reference to these three maps, allowing it to obtain
	//the TGraphAsymmErrors * associated with a Data Steering File (data), Grid Steering File (reference),
	//or Grid/PDF Steering File Pair (convolute)
	//
	//NOTE: typedefs are in SPXGraphUtilities.h
	//
	StringGraphMap_T dataFileGraphMap;
	StringPairGraphMap_T referenceFileGraphMap;
	StringPairGraphMap_T nominalFileGraphMap;
	StringPairGraphMap_T convoluteFileGraphMap;

	//Sets used for eliminating duplicate data/cross section plots, since
	// different plot configuration instances could share a data/cross section, and we
	// don't need to plot the same one multiple times
	std::set<std::string> dataSet;
	std::set<StringPair_T> crossSectionSet;

	//ROOT Components
	TCanvas *canvas;
	TPad *overlayPad;
	TPad *ratioPad;

	TH1F *overlayFrameHisto;
	TH1F *ratioFrameHisto;

	TAxis *xAxisOverlay;
	TAxis *yAxisOverlay;
	TAxis *xAxisRatio;
	TAxis *yAxisRatio;

	//Frame Bounds
	double xMinOverlay;
	double xMaxOverlay;
	double yMinOverlay;
	double yMaxOverlay;
	double xMinRatio;
	double xMaxRatio;
	double yMinRatio;
	double yMaxRatio;

	void InitializeData(void);
	void InitializeCrossSections(void);
	void NormalizeCrossSections(void);
	void InitializeRatios(void);
};

#endif
