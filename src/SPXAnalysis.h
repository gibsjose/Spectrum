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

class SPXAnalysis {

public:
	SPXAnalysis (void) {}
	explicit SPXAnalysis(SPXSteeringFile &steeringFile) : this->steeringFile(steeringFile) {}

	void Initialize(void) {}
	void Plot(void) {}
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}

private:
	static bool debug;					//Flag indicating debug mode
	SPXSteeringFile &steeringFile;		//Reference to a fully parsed steering file	
	std::vector<SPXPlot> plots;			//Vector of plots
};

#endif
