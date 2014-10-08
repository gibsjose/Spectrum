//************************************************************/
//
//	PDF Band Type Implementation
//
//	Implements the SPXPDFBandType class, which describes the PDF
//	band type. The options are 'use_alpha_s' or 'use_error_band' 
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		29.09.2014
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXPDFBandType.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXPDFBandType::";

//Must define the static debug variable in the implementation
bool SPXPDFBandType::debug;

//Constructs an SPXPDFBandType object with a given string (effectively
//	calls the Parse() method on the input string)
SPXPDFBandType::SPXPDFBandType(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//	use_alpha_s XOR use_error_band
//
// Sets the type based on the input string
void SPXPDFBandType::Parse(std::string s) {
	std::string mn = "Parse: ";
	
	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;
	
	//Clear the type each time it is parsed
	this->Clear();
	
	if(!s.compare("use_alpha_s")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"use_alpha_s\"" << std::endl;
		type = BT_ALPHA_S;
		if(debug) std::cout << cn << mn << "PDF Band Type successfully set to \"use_alpha_s\"" << std::endl;
	}
	else if(!s.compare("use_error_band")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"use_error_band\"" << std::endl;
		type = BT_ERROR_BAND;
		if(debug) std::cout << cn << mn << "PDF Band Type successfully set to \"use_error_band\"" << std::endl;
	}
	else {
		type = BT_INVALID;
		
		std::ostringstream oss;
		oss << "Incorrect PDF Band Type: Unrecognized type option: \"" << s << "\" is invalid";
		throw SPXINIParseException("PDF", "pdf_band_type", oss.str());
	}
}

//Print displays the output of ToString to the console
void SPXPDFBandType::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's type data
std::string SPXPDFBandType::ToString(void) {
	std::string mn = "ToString: ";
	
	//Empty style
	if(this->IsEmpty()) {
		return "";
	}
	
	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_PDF_BAND_TYPE";
	}
	
	if(type == BT_ALPHA_S) {
		return "use_alpha_s";
	}
	if(type == BT_ERROR_BAND) {
		return "use_error_band";
	}
	
	return "INVALID_PDF_BAND_TYPE";
}

//Determines whether the band type is empty or not
bool SPXPDFBandType::IsEmpty(void) {
	std::string mn = "IsEmpty: ";
	
	return !(bool)type;
}

//Determines the validity of the band type
bool SPXPDFBandType::IsValid(void) {
	std::string mn = "IsValid: ";
	
	//Empty style: valid, but empty
	if(this->IsEmpty()){
		if(debug) std::cout << cn << mn << "PDF Band Type is empty" << std::endl;
		return true;
	}
	
	if(type == BT_INVALID) {
		if(debug) std::cout << cn << mn << "PDF Band Type is invalid: Set to BT_INVALID (-1)" << std::endl;
		return false;
	}
	
	if(type >= (BT_ALPHA_S | BT_ERROR_BAND)) {
		if(debug) std::cout << cn << mn << "PDF Band Type is invalid: Type is set to unknown value: " << type << std::endl;
		return false;
	}
	
	return true;
}

