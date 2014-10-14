//************************************************************/
//
//	Display Style Header
//
//	Outlines the SPXDisplayStyle class, which describes exactly what
//	plots will be made on the TCanvas. The options are Overlay,
//	Ratio, or both.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		26.09.2014
//
//************************************************************/

#ifndef SPXDISPLAYSTYLE_H
#define SPXDISPLAYSTYLE_H

//Bitfields for determining style type
const int DS_INVALID =		-1;
const int DS_OVERLAY = 		(1 << 0);	//0b00000001
const int DS_RATIO = 		(1 << 1);	//0b00000010	

class SPXDisplayStyle {

public:
	SPXDisplayStyle () : style(0) {}
	SPXDisplayStyle(unsigned int style) {
		this->style = style;
	}
	explicit SPXDisplayStyle(std::string s);
	
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
		if(!(mask & (DS_OVERLAY | DS_RATIO))) {
			return false;
		}

		if(((style & mask) == DS_OVERLAY) && (this->IsValid())) {
			return true;
		} else if(((style & mask) == DS_RATIO) && (this->IsValid())) {
			return true;
		} else {
			return false;
		}

		return false;
	}

	bool ContainsOverlay(void) {
		if((style & DS_OVERLAY) && this->IsValid()) {
			return true;
		}
		
		return false;
	}
	
	bool ContainsRatio(void) {
		if((style & DS_RATIO) && this->IsValid()) {
			return true;
		}
		
		return false;
	}

private:
	static bool debug;
	unsigned int style;
};

#endif
