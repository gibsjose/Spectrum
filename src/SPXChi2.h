//************************************************************/
//
//  Calculations Chi2 from SPXPDF and SPXData-objects
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.1.2015
//	@Email:		tancredi.carli@cern.ch
//
//************************************************************/

#ifndef SPXCHI2_H
#define SPXCHI2_H

#include "SPXROOT.h"

//#include "SPXPDFSteeringFile.h"
//#include "SPXPlotConfiguration.h"
//#include "SPXGridCorrections.h"
//#include "SPXSteeringFile.h"
//#include "SPXUtilities.h"

#include "SPXData.h"
#include "SPXPDF.h"

static bool debug=true;

class SPXChi2 {

public:

 static double CalculateSimpleChi2(SPXPDF *pdf, SPXData *data);

private:
//	static bool debug;		       // Flag indicating debug mode

};

#endif
