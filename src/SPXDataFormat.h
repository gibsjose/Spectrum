//************************************************************/
//
//	Data Format Header
//
//	Outlines the SPXDataFormat class, which describes the 
//	formats available for experimental data. 
//	The options are:
//
//		spectrum_t1s 	(DF_SPECTRUM_T1S)
//		spectrum_t1a 	(DF_SPECTRUM_T1A)
//		spectrum_t2s 	(DF_SPECTRUM_T2S)
//		spectrum_t2a 	(DF_SPECTRUM_T2A)
//		herapdf			(DF_HERAPDF)
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		08.10.2014
//
//************************************************************/

#ifndef SPXDATAFORMAT_H
#define SPXDATAFORMAT_H

//Bitfields for determining band format
const int DF_INVALID =			-1;
const int DF_SPECTRUM_T1S = 	(1 << 0);	//0b00000001
const int DF_SPECTRUM_T1A = 	(1 << 1);	//0b00000010	
const int DF_SPECTRUM_T2S = 	(1 << 2);	//0b00000100
const int DF_SPECTRUM_T2A =		(1 << 3);	//0b00001000
const int DF_HERAPDF =			(1 << 4);	//0b00010000

class SPXDataFormat {

public:
	SPXDataFormat () : format(0) {}
	SPXDataFormat(unsigned int format) {
		this->format = format;
	}
	explicit SPXDataFormat(std::string s);
	
	void Parse(std::string s);
	void Print(void);
	std::string ToString(void);
	bool IsEmpty(void);
	bool IsValid(void);
	
	void Clear(void) {
		format = 0;
	}
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}
	
	int GetFormat(void) {
		return (int)format;
	}
	
	bool Is(unsigned int mask) {
		if(format == mask) {
			return true;
		} else {
			return false;
		}
	}

	bool IsSpectrumT1S(void) {
		if((format == DF_SPECTRUM_T1S) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsSpectrumT1A(void) {
		if((format == DF_SPECTRUM_T1A) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsSpectrumT2S(void) {
		if((format == DF_SPECTRUM_T2S) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsSpectrumT2A(void) {
		if((format == DF_SPECTRUM_T2A) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsHERAPDF(void) {
		if((format == DF_HERAPDF) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

private:
	static bool debug;
	unsigned int format;
};

#endif
