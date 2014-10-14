//************************************************************/
//
//	PDF Header
//
//	Outlines the SPXPDF class, which manages the PDF data
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPDF_H
#define SPXPDF_H

#include "SPXROOT.h"

#include "SPXPDFSteeringFile.h"
#include "SPXException.h"

class SPXPDF {

public:
	SPXPDF(void) {}

	explicit SPXPDF(SPXPDFSteeringFile *psf) {
		this->psf = psf;
	}
	
	void Parse(void);
	void Print(void);
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}

private:
	static bool debug;					//Flag indicating debug mode
	SPXPDFSteeringFile *psf;			//PDF Steering file
};

#endif
