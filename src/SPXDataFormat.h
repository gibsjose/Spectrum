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
//		herafitter		(DF_HERAFITTER)
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
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
const int DF_HERAFITTER =		(1 << 4);	//0b00010000

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

	bool IsSymmetric(void) const {
		if(this->IsValid()) {
			if(format == DF_SPECTRUM_T1S) {
				return true;
			}

			if(format == DF_SPECTRUM_T2S) {
				return true;
			}
		}

		return false;
	}

	bool IsAsymmetric(void) const {
		if(this->IsValid()) {
			if(format == DF_SPECTRUM_T1A) {
				return true;
			}

			if(format == DF_SPECTRUM_T2A) {
				return true;
			}
		}

		return false;
	}

	bool IsSpectrumT1S(void) const {
		if((format == DF_SPECTRUM_T1S) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsSpectrumT1A(void) const {
		if((format == DF_SPECTRUM_T1A) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsSpectrumT2S(void) const {
		if((format == DF_SPECTRUM_T2S) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsSpectrumT2A(void) const {
		if((format == DF_SPECTRUM_T2A) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

	bool IsHERAFitter(void) const {
		if((format == DF_HERAFITTER) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

private:
	static bool debug;
	unsigned int format;
};

#endif
