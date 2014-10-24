//************************************************************/
//
//	Analysis Header
//
//	Outlines the SPXAnalysis class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT configuration via
//	the SPXPlot class.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXANALYSIS_H
#define SPXANALYSIS_H

#include "SPXPlot.h"
#include "SPXSteeringFile.h"
#include "SPXException.h"

class SPXAnalysis {

public:
	explicit SPXAnalysis(SPXSteeringFile *steeringFile) {
		this->steeringFile = steeringFile;

		try {
			this->Initialize();
		} catch(const SPXException &e) {
			throw;
		}
	}

	void Run(void) {
		try {
			for(int i = 0; i < plots.size(); i++) {
				plots[i].Plot();
			}
		} catch(const SPXException &e) {
			throw;
		}
	}

private:
	SPXSteeringFile *steeringFile;		//Pointer to a fully parsed steering file
	std::vector<SPXPlot> plots;			//Vector of plots

	void Initialize(void) {

		//Turn on debug modes for Plot
		if(steeringFile->GetDebug()) {
			SPXPlot::SetDebug(true);
		}

		try {
			for(int i = 0; i < steeringFile->GetNumberOfPlotConfigurations(); i++) {
				SPXPlot plot = SPXPlot(steeringFile, i);
				plot.Initialize();
				plots.push_back(plot);
			}
		} catch(const SPXException &e) {
			throw;
		}
	}
};

#endif
