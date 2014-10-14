//************************************************************/
//
//	PDF Error Size Header
//
//	Outlines the SPXPDFErrorSize class, which describes the PDF
//	error size. The options are 'one_sigma' or '90_percent' 
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		29.09.2014
//
//************************************************************/

#ifndef SPXPDFERRORSIZE_H
#define SPXPDFERRORSIZE_H

//Bitfields for determining error size type
const int ES_INVALID =		-1;
const int ES_ONE_SIGMA = 	(1 << 0);	//0b00000001
const int ES_90_PERCENT = 	(1 << 1);	//0b00000010	

class SPXPDFErrorSize {

public:
	SPXPDFErrorSize () : type(0) {}
	SPXPDFErrorSize(unsigned int type) {
		this->type = type;
	}
	explicit SPXPDFErrorSize(std::string s);
	
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
	
	bool IsOneSigma(void) {
		if((type == ES_ONE_SIGMA) && this->IsValid()) {
			return true;
		}
		
		return false;
	}
	
	bool Is90Percent(void) {
		if((type == ES_90_PERCENT) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

private:
	static bool debug;
	unsigned int type;
};

#endif
