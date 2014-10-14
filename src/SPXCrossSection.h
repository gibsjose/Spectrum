//************************************************************/
//
//	CrossSection Header
//
//	Outlines the SPXCrossSection class, which performs the
//	convolution and all math required to calculate the cross-
//	section from a PDF and a grid file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXCROSSSECTION_H
#define SPXCROSSSECTION_H

#include "SPXROOT.h"

#include "SPXGrid.h"
#include "SPXPDF.h"
#include "SPXSteeringFile.h"
#include "SPXException.h"

class SPXCrossSection {

public:
	explicit SPXCrossSection(SPXSteeringFile *sf) {
		this->steeringFile = sf;
	}
	
	void Parse(void);
	void Print(void);
	void Draw(void);
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}

private:
	static bool debug;					//Flag indicating debug mode
	SPXSteeringFile *steeringFile;		//Fully parsed steering file
	SPXGrid grid;						//Grid
	SPXPDF pdf;							//PDF
};

#endif
