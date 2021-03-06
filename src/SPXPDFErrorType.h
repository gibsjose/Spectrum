//************************************************************/
//
//	PDF Error Type Header
//
//	Outlines the SPXPDFErrorType class, which describes the PDF
//	error type. The options are 'alpha_s' or 'pdf_band' or 'scale'
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		29.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPDFERRORTYPE_H
#define SPXPDFERRORTYPE_H

#include <string>

#include "SPXUtilities.h"

//Bitfields for determining error type
const int ET_INVALID =		-1;
const int ET_ALPHA_S = 		(1 << 0);	//0b00000001
const int ET_PDF_BAND = 	(1 << 1);	//0b00000010
const int ET_SCALE_BAND = 	(1 << 2);	//0b00000100

class SPXPDFErrorType {

public:
	SPXPDFErrorType () : type(0) {}
	SPXPDFErrorType(unsigned int type) {
		this->type = type;
	}
	explicit SPXPDFErrorType(std::string s);

	void Parse(std::string s);
	void Print(void);
	std::string ToString(void);
	bool IsEmpty(void);
	bool IsValid(void);

	void Clear(void) {
		type = 0;
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	int GetType(void) {
		return (int)type;
	}

	bool IsAlphaS(void) {
		if((type == ET_ALPHA_S) && this->IsValid()) {
			return true;
		}

		return false;
	}

	bool IsPDFBand(void) {
		if((type == ET_PDF_BAND) && this->IsValid()) {
			return true;
		}

		return false;
	}

	bool IsScaleBand(void) {
		if((type == ET_SCALE_BAND) && this->IsValid()) {
			return true;
		}

		return false;
	}

private:
	static bool debug;
	unsigned int type;
};

#endif
