//************************************************************/
//
//	Data Format Implementation
//
//	Implements the SPXDataFormat class, which describes the
//	formats available for experimental data.
//	The options are:
//
//		spectrum		(DF_SPECTRUM)
//		herafitter		(DF_HERAFITTER)
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXDataFormat.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXDataFormat::";

//Must define the static debug variable in the implementation
bool SPXDataFormat::debug;

//Constructs an SPXDataFormat object with a given string (effectively
//	calls the Parse() method on the input string)
SPXDataFormat::SPXDataFormat(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//
//	spectrum		XOR
//	herafitter
//
// Sets the format based on the input string
void SPXDataFormat::Parse(std::string s) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;

	//Clear the format each time it is parsed
	this->Clear();

	if(!s.compare("spectrum")) {
		if(debug) std::cout << cn << mn << "Successfully matched format string: \"spectrum\"" << std::endl;
		format = DF_SPECTRUM;
		if(debug) std::cout << cn << mn << "Data Format successfully set to \"spectrum\"" << std::endl;
	}
	else if(!s.compare("herafitter")) {
		if(debug) std::cout << cn << mn << "Successfully matched format string: \"herafitter\"" << std::endl;
		format = DF_HERAFITTER;
		if(debug) std::cout << cn << mn << "Data Format successfully set to \"herafitter\"" << std::endl;
	}
	else {
		format = DF_INVALID;

		std::ostringstream oss;
		oss << "Incorrect Data Format: Unrecognized format option: \"" << s << "\" is invalid";
		throw SPXINIParseException("DATA", "data_format", oss.str());
	}
}

//Print displays the output of ToString to the console
void SPXDataFormat::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's format data
std::string SPXDataFormat::ToString(void) const {
	std::string mn = "ToString: ";

	//Empty style
	if(this->IsEmpty()) {
		return "";
	}

	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_DATA_FORMAT";
	}

	if(format == DF_SPECTRUM) {
		return "Spectrum";
	} else if(format == DF_HERAFITTER) {
		return "HERAFitter";
	}

	return "INVALID_DATA_FORMAT";
}

//Determines whether the band format is empty or not
bool SPXDataFormat::IsEmpty(void) const {
	std::string mn = "IsEmpty: ";

	return !(bool)format;
}

//Determines the validity of the band format
bool SPXDataFormat::IsValid(void) const {
	std::string mn = "IsValid: ";

	//Empty style: valid, but empty
	if(this->IsEmpty()){
		if(debug) std::cout << cn << mn << "Data Format is empty" << std::endl;
		return true;
	}

	if(format == DF_INVALID) {
		if(debug) std::cout << cn << mn << "Data Format is invalid: Set to DF_INVALID (-1)" << std::endl;
		return false;
	}

	if(format == DF_SPECTRUM) {
		return true;
	} else if(format == DF_HERAFITTER) {
		return true;
	} else {
		if(debug) std::cout << cn << mn << "Data format is invalid: Set to unknown value: " << format << std::endl;
		return false;
	}
}
