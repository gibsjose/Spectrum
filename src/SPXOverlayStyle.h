//************************************************************/
//
//	Overlay Style Header
//
//	Outlines the SPXOverlayStyle class, which describes which datasets
//	will be plotted on the overlay graph. Options include: data,
//	reference, and convolute.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		26.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXOVERLAYSTYLE_H
#define SPXOVERLAYSTYLE_H

#include <string>

//Bitfields for determining style type
const int OS_INVALID =		-1;
const int OS_DATA = 		(1 << 0);	//0b00000001
const int OS_REFERENCE = 	(1 << 1);	//0b00000010
const int OS_CONVOLUTE =	(1 << 2);	//0b00000100
const int OS_PDF =			(1 << 3);	//0b00001000

class SPXOverlayStyle {

public:
	SPXOverlayStyle () : style(0) {}
	SPXOverlayStyle(unsigned int style) {
		this->style = style;
	}
	explicit SPXOverlayStyle(std::string s);

	void Parse(std::string s);
	void Print(void);
	std::string ToString(void);
	bool IsEmpty(void);
	bool IsValid(void);

	void Clear(void) {
		style = 0;
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	int GetStyle(void) {
		return (int)style;
	}

	bool Contains(int mask) {
		//Invalid mask... Does not contain data, reference, or convolute
		if(!(mask & (OS_DATA | OS_REFERENCE | OS_CONVOLUTE | OS_PDF))) {
			return false;
		}

		if(((style & mask) == OS_DATA) && (this->IsValid())) {
			return true;
		} else if(((style & mask) == OS_REFERENCE) && (this->IsValid())) {
			return true;
		} else if(((style & mask) == OS_CONVOLUTE) && (this->IsValid())) {
			return true;
		} else if(((style & mask) == OS_PDF) && (this->IsValid())) {
			return true;
		} else {
			return false;
		}

		return false;
	}

	bool ContainsData(void) {
		if((style & OS_DATA) && this->IsValid()) {
			return true;
		}

		return false;
	}

	bool ContainsReference(void) {
		if((style & OS_REFERENCE) && this->IsValid()) {
			return true;
		}

		return false;
	}

	bool ContainsConvolute(void) {
		if((style & OS_CONVOLUTE) && this->IsValid()) {
			return true;
		}

		return false;
	}

	bool ContainsPDF(void) {
		if((style & OS_PDF) && this->IsValid()) {
			return true;
		}

		return false;
	}

private:
	static bool debug;
	unsigned int style;
};

#endif
