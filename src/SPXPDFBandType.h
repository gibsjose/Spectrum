//************************************************************/
//
//	PDF Band Type Header
//
//	Outlines the SPXPDFBandType class, which describes the PDF
//	band type. The options are 'use_alpha_s' or 'use_error_band' 
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		29.09.2014
//
//************************************************************/

#ifndef SPXPDFBANDTYPE_H
#define SPXPDFBANDTYPE_H

//Bitfields for determining band type
const int BT_INVALID =		-1;
const int BT_ALPHA_S = 		(1 << 0);	//0b00000001
const int BT_ERROR_BAND = 	(1 << 1);	//0b00000010	

class SPXPDFBandType {

public:
	SPXPDFBandType () : type(0) {}
	SPXPDFBandType(unsigned int type) {
		this->type = type;
	}
	explicit SPXPDFBandType(std::string s);
	
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
	
	bool IsUseAlphaS(void) {
		if((type == BT_ALPHA_S) && this->IsValid()) {
			return true;
		}
		
		return false;
	}
	
	bool IsUseErrorBand(void) {
		if((type == BT_ERROR_BAND) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

private:
	static bool debug;
	unsigned int type;
};

#endif
