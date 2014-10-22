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

#include "SPXCrossSection.h"
#include "SPXData.h"

#include "SPXException.h"

class SPXPlot {

public:
	explicit SPXPlot(SPXSteeringFile *steeringFile, unsigned int plotNumber) {
		this->steeringFile = steeringFile;
		this->id = plotNumber;
	}

	void Initialize(void);
	void Plot(void);

	//Helper methods //@TODO Public or private?
	void SPXPlot::CreateCanvas(void);
	void SPXPlot::DetermineOverlayFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax);
	void SPXPlot::DetermineRatioFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax);
	void SPXPlot::DivideCanvasIntoPads(void);
	void SPXPlot::ConfigurePads(void);
	void SPXPlot::DrawOverlayPadFrame(void);
	void SPXPlot::DrawRatioPadFrame(void);
	void SPXPlot::DrawOverlay(void);
	void SPXPlot::DrawRatio(void);
	void SPXPlot::UpdateCanvas(void);
	void SPXPlot::CanvasToPNG(void);
	std::string SPXPlot::GetPNGFilename(std::string desc)

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

	//ROOT Components
	TCanvas *canvas;
	TPad *overlayPad;
	TPad *ratioPad;

	void InitializeData(void);
	void InitializeCrossSections(void);
	void NormalizeCrossSections(void);
};

#endif
