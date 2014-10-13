//************************************************************/
//
//	Analysis Header
//
//	Outlines the SPXAnalysis class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT configuration via
//	the SPXPlot class.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		08.10.2014
//
//************************************************************/

#ifndef SPXANALYSIS_H
#define SPXANALYSIS_H

//#include "SPXPlot.h"
#include "SPXData.h" //TEMPORARY!
#include "SPXSteeringFile.h"
#include "SPXException.h"

class SPXAnalysis {

public:
	explicit SPXAnalysis(SPXSteeringFile *sf) : steeringFile(*sf) {}

	void Initialize(void) {
		/*
		try {
			for(int i = 0; i < steeringFile.GetNumberOfPlots(); i++) {
				SPXPlot plot = SPXPlot();
				plot.Initilize();
				plots.push_back(plot);
			}
		} catch(const SPXException &e) {
			throw;
		}
		*/
	}

	void Plot(void) {
		//TEMPORARY!!!
		SPXData::SetDebug(true);
		SPXFrameOptionsInstance &foi = steeringFile.GetFrameOptionsInstance(0, 0);
		SPXData data = SPXData(foi);

		try {
			data.Parse();	//Parse the data
			data.Print();	//Print the data to the console
			data.Draw();	//Draw the data in a frame
		} catch(const SPXException &e) {
			throw;
		}

		/*
		try {
			for(int i = 0; i < plots.size(); i++) {
				plots[i].Plot();
			}
		} catch(const SPXException &e) {
			throw;
		}
		*/
	}
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}

private:
	static bool debug;					//Flag indicating debug mode
	SPXSteeringFile &steeringFile;		//Reference to a fully parsed steering file	
	//std::vector<SPXPlot> plots;			//Vector of plots
};

#endif
