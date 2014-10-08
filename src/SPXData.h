//************************************************************/
//
//	Data Header
//
//	Outlines the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		08.10.2014
//
//************************************************************/

#ifndef SPXDATA_H
#define SPXDATA_H

#include <map>
#include <string>
#include <vector>

#include "SPXDataFormat.h"

class SPXData {

public:
	explicit SPXData (const SPXDataFormat & format) {
		this->format = format;
	}
	
	void Parse(void);
	void Print(void);
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}
	
	const SPXDataFormat & GetFormat(void) {
		return format;
	}

private:
	static bool debug;
	SPXDataFormat format;
	std::map<std::string, std::vector<double> > data;

	void ParseSpectrumT1S(void);
	void ParseSpectrumT1A(void);
	void ParseSpectrumT2S(void);
	void ParseSpectrumT2A(void);
	void ParseHERAPDF(void);
};

#endif
