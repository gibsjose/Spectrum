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
	explicit SPXRatioStyle(std::string s);
	
	void Parse(std::string s);
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
	
	bool NumeratorContains(unsigned char mask) {
		if((numerator & mask) && this->IsValid()) {
			return true;
		}
		
		return false;
	}
	
	bool DenominatorContains(unsigned char mask) {
		if((denominator & mask) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

private:
	static bool debug;
	unsigned int numerator;
	unsigned int denominator;
};

#endif
