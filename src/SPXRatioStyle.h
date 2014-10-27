//************************************************************/
//
//	Ratio Style Header
//
//	Outlines the SPXRatioStyle class, which describes the type of
//	ratio to be plotted in the ratio section. Ratio styles can
//	include: [data AND/OR reference AND/OR convolute] in the
//	numerator, and [data XOR reference XOR convolute] in the
//	demominator.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXRATIOSTYLE_H
#define SPXRATIOSTYLE_H

//Bitfields for determining numerator/denominator contents
const int RS_INVALID =		-1;
const int RS_DATA = 		(1 << 0);	//0b00000001
const int RS_REFERENCE = 	(1 << 1);	//0b00000010
const int RS_CONVOLUTE = 	(1 << 2);	//0b00000100

class SPXRatioStyle {

public:
	SPXRatioStyle () : numerator(0), denominator(0) {}
	SPXRatioStyle(unsigned int numerator, unsigned int denominator) {
		this->numerator = numerator;
		this->denominator = denominator;
	}

	//Constructor with style string, plot number string, and ratio style number string
	SPXRatioStyle(std::string &s, unsigned int pn, unsigned int rsn);

	void Parse(std::string &s);
	void Print(void);
	std::string ToString(void);
	bool IsEmpty(void);
	bool IsValid(void);

	void Clear(void) {
		numerator = 0;
		denominator = 0;
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	//Return cast as (int) so it can be represented
	// as '-1' during debug and error output
	int GetNumerator(void) {
		return (int)numerator;
	}

	int GetDenominator(void) {
		return (int)denominator;
	}

	bool IsConvoluteOverData(void) {
		if(NumeratorIsConvolute() && DenominatorIsData()) {
			return true;
		} else {
			return false;
		}
	}

	bool IsDataOverConvolute(void) {
		if(NumeratorIsData() && DenominatorIsConvolute()) {
			return true;
		} else {
			return false;
		}
	}

	bool IsConvoluteOverReference(void) {
		if(NumeratorIsConvolute() && DenominatorIsReference()) {
			return true;
		} else {
			return false;
		}
	}

	bool IsDataOverData(void) {
		if(NumeratorIsData() && DenominatorIsData()) {
			return true;
		} else {
			return false;
		}
	}

	//////////////////////////////////////////////////////////////////////

	bool NumeratorIsConvolute(void) {
		return NumeratorIs(RS_CONVOLUTE);
	}

	bool NumeratorIsData(void) {
		return NumeratorIs(RS_DATA);
	}

	bool DenominatorIsConvolute(void) {
		return DenominatorIs(RS_CONVOLUTE);
	}

	bool DenominatorIsReference(void) {
		return DenominatorIs(RS_REFERENCE);
	}

	bool DenominatorIsData(void) {
		return DenominatorIs(RS_DATA);
	}

	bool NumeratorIs(unsigned char mask) {
		if(numerator == mask) {
			return true;
		}

		return false;
	}

	bool DenominatorIs(unsigned char mask) {
		if(denominator == mask) {
			return true;
		}

		return false;
	}

private:
	static bool debug;
	unsigned int numerator;
	unsigned int denominator;

	//Plot number and ratio style number for exception handling
	std::string plotNumber;
	std::string ratioStyleNumber;
};

#endif
