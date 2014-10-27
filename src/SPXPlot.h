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
			SPXCrossSection::SetDebug(true);
			SPXRatio::SetDebug(true);
		}
	}

	void Initialize(void);
	void Plot(void);

	//Helper methods //@TODO Public or private?
	void SetAxisLabels(void);
	void ScaleAxes(void);
	void CreateCanvas(void);
	void DetermineOverlayFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax);
	void DetermineRatioFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax);
	void DivideCanvasIntoPads(void);
	void ConfigurePads(void);
	void DrawOverlayPadFrame(void);
	void DrawRatioPadFrame(void);
	void DrawOverlay(void);
	void DrawRatio(void);
	void UpdateCanvas(void);
	void CanvasToPNG(void);
	std::string GetPNGFilename(std::string desc);

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
	static bool debug;								//Flag indicating debug mode
	SPXSteeringFile *steeringFile;					//Fully parsed steering file
	unsigned int id;								//Plot number ID (0-based)
	std::vector<SPXData> data;						//Vector of data
	std::vector<SPXCrossSection> crossSections;		//Vector of cross sections
	std::vector<SPXRatio> ratios;					//Vector of ratios

	//@TODO: Upon initialization of each Data/Cross Section object, use the steering file as a key
	// 		and get the corresponding TGraph from it. What to do for CrossSections? Convolutions?  
	std::map<std::string, TGraphAsymmErrors *> fileToGraphMap;		//Steering File to TGraph Map

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
};

#endif
