//************************************************************/
//
//	Ratio Style Header
//
//	Outlines the SPXRatioStyle class, which describes the type of
//	ratio to be plotted in the ratio section.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXRATIOSTYLE_H
#define SPXRATIOSTYLE_H

#include <string>

//Bitfields for determining numerator/denominator contents
const int RS_INVALID =		-1;
const int RS_DATA = 		(1 << 0);	//0b00000001
const int RS_REFERENCE = 	(1 << 1);	//0b00000010
const int RS_CONVOLUTE = 	(1 << 2);	//0b00000100
const int RS_NOMINAL = 		(1 << 3);	//0b00001000

class SPXRatioStyle {

public:
	SPXRatioStyle () : numerator(0), denominator(0), dataStat(false), dataTot(false) {}
	SPXRatioStyle(unsigned int numerator, unsigned int denominator) {
		this->numerator = numerator;
		this->denominator = denominator;

		dataStat = false;
		dataTot = false;
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

		dataStat = false;
		dataTot = false;

		zeroNumeratorErrors = false;
		zeroDenominatorErrors = false;
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

	bool IsDataStat(void) {
		return dataStat;
	}

	bool IsDataTot(void) {
		return dataTot;
	}

	bool GetZeroNumeratorErrors(void) {
		return zeroNumeratorErrors;
	}

	bool GetZeroDenominatorErrors(void) {
		return zeroDenominatorErrors;
	}

	bool IsConvoluteOverData(void) {
		if(NumeratorIsConvolute() && DenominatorIsData()) {
			return true;
		} else {
			return false;
		}
	}

	bool IsConvoluteOverConvolute(void) {
		if(NumeratorIsConvolute() && DenominatorIsConvolute()) {
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

	bool IsConvoluteOverNominal(void) {
		if(NumeratorIsConvolute() && DenominatorIsNominal()) {
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

	bool DenominatorIsNominal(void) {
		return DenominatorIs(RS_NOMINAL);
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

	bool dataStat;	//Special ratio: Plot data/data with statistical errors
	bool dataTot;	//Specrial ratio: Plot data/data with total errors

	bool zeroNumeratorErrors;		//Flags for whether or not to zero num/den errors. Set with '!' in steering file
	bool zeroDenominatorErrors;

	//Plot number and ratio style number for exception handling
	std::string plotNumber;
	std::string ratioStyleNumber;
};

#endif
