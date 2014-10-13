//************************************************************/
//
//	Plot Header
//
//	Outlines the SPXPlot class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT objects
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		13.10.2014
//
//************************************************************/

#ifndef SPXPLOT_H
#define SPXPLOT_H

#include <vector>

#include "SPXSteeringFile.h"

//#include "SPXCrossSection.h" //@TODO Create SPXCrossSection class
#include "SPXData.h"

#include "SPXUtilities.h"
#include "SPXException.h"

class SPXPlot {

public:
	explicit SPXPlot(SPXSteeringFile *steeringFile) {
		this->steeringFile = steeringFile;
	}

	void Initialize(void);
	void Plot(void);
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}

private:
	static bool debug;								//Flag indicating debug mode
	SPXSteeringFile *steeringFile;					//Fully parsed steering file

	std::vector<SPXData> data;						//Vector of data
	std::vector<SPXCrossSection> crossSections;		//Vector of cross sections
};

#endif
