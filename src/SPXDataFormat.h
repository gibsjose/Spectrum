//************************************************************/
//
//	Data Format Header
//
//	Outlines the SPXDataFormat class, which describes the
//	formats available for experimental data.
//	The options are:
//
//		spectrum		(DF_SPECTRUM)
//		herafitter		(DF_HERAFITTER)
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXDATAFORMAT_H
#define SPXDATAFORMAT_H

#include <string>

#include "SPXUtilities.h"

//Bitfields for determining band format
const int DF_INVALID =			-1;
const int DF_SPECTRUM =			(1 << 0);	//0b10000000
const int DF_HERAFITTER =		(1 << 1);	//0b01000000

class SPXDataFormat {

public:
	SPXDataFormat () : format(0) {}
	SPXDataFormat(unsigned int format) {
		this->format = format;
	}
	explicit SPXDataFormat(std::string s);

	void Parse(std::string s);
	void Print(void);
	std::string ToString(void) const;
	bool IsEmpty(void) const;
	bool IsValid(void) const;

	void Clear(void) {
		format = 0;
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	int GetFormat(void) const {
		return (int)format;
	}

	bool Is(unsigned int mask) const {
		if(format == mask) {
			return true;
		} else {
			return false;
		}
	}

	bool IsSpectrum(void) const {
		if((format == DF_SPECTRUM) && this->IsValid()) {
			return true;
		}

		return false;
	}

	//bool IsHERAFitter(void) const {
	//	if((format == DF_HERAFITTER) && this->IsValid()) {
	//		return true;
	//	}
	//
	//	return false;
	//}

private:
	static bool debug;
	unsigned int format;
};

#endif
