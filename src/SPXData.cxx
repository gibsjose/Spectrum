//************************************************************/
//
//	Data Implementation
//
//	Implements the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		08.10.2014
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXData.h"

//Class name for debug statements
const std::string cn = "SPXData::";

//Must define the static debug variable in the implementation
bool SPXData::debug;

//Public helper method to parse the data file based on the type
void SPXData::Parse(void) {
	std::string mn = "Parse: ";
	
	if(debug) std::cout << cn << mn << "Parsing data file: " << dataSteeringFile.GetDataFile() << std::endl;

	try {
		OpenDataFile();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXFileIOException(cn + mn + "Unable to open data file");
	}

	SPXDataFormat df = dataSteeringFile.GetDataFormat();

	if(df.IsSpectrumT1S()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;
		ParseSpectrumT1S();
	} else if(df.IsSpectrumT1A()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;
		ParseSpectrumT1A();
	} else if(df.IsSpectrumT2S()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;
		ParseSpectrumT2S();
	} else if(df.IsSpectrumT2A()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;
		ParseSpectrumT2A();
	} else if(df.IsHERAFitter()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;
		ParseHERAFitter();
	} else {
		throw SPXParseException("DataSteeringFile " + dataSteeringFile.GetFilename() + " has invalid data format");
	}
}

void SPXData::ParseSpectrumT1S(void) {
	std::string mn = "ParseSpectrumT1S: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << dataSteeringFile.GetFilename() << std::endl;
}

void SPXData::ParseSpectrumT1A(void) {
	std::string mn = "ParseSpectrumT1A: ";
}

void SPXData::ParseSpectrumT2S(void) {
	std::string mn = "ParseSpectrumT2S: ";
}

void SPXData::ParseSpectrumT2A(void) {
	std::string mn = "ParseSpectrumT2A: ";
}

void SPXData::ParseHERAFitter(void) {
	std::string mn = "ParseHERAFitter: ";
}

//Print displays the output of ToString to the console
/*
void SPXData::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}
*/

//ToString does the opposite of Parse: it assembles a string based on
// the object's type data
/*
std::string SPXData::ToString(void) {
	std::string mn = "ToString: ";
}
*/

