//************************************************************/
//
//	PDF Error Size Implementation
//
//	Implements the SPXPDFErrorSize class, which describes the PDF
//	error size. The options are 'one_sigma' or '90_percent' 
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		29.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXPDFErrorSize.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXPDFErrorSize::";

//Must define the static debug variable in the implementation
bool SPXPDFErrorSize::debug;

//Constructs a PDFErrorSize object with a given string (effectively
//	calls the Parse() method on the input string)
SPXPDFErrorSize::SPXPDFErrorSize(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//	one_sigma XOR 90_percent
//
// Sets the type based on the input string
void SPXPDFErrorSize::Parse(std::string s) {
	std::string mn = "Parse: ";
	
	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;
	
	//Clear the type each time it is parsed
	this->Clear();
	
	if(!s.compare("one_sigma")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"one_sigma\"" << std::endl;
		type = ES_ONE_SIGMA;
		if(debug) std::cout << cn << mn << "PDF Error Size successfully set to \"one_sigma\"" << std::endl;
	}
	else if(!s.compare("90_percent")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"90_percent\"" << std::endl;
		type = ES_90_PERCENT;
		if(debug) std::cout << cn << mn << "PDF Error Size successfully set to \"90_percent\"" << std::endl;
	}
	else {
		type = ES_INVALID;
		
		std::ostringstream oss;
		oss << "Incorrect PDF Error Size: Unrecognized type option: \"" << s << "\" is invalid";
		throw SPXINIParseException("PDF", "pdf_error_size", oss.str());
	}
}

//Print displays the output of ToString to the console
void SPXPDFErrorSize::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's type data
std::string SPXPDFErrorSize::ToString(void) {
	std::string mn = "ToString: ";
	
	//Empty style
	if(this->IsEmpty()) {
		return "";
	}
	
	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_PDF_ERROR_SIZE";
	}
	
	if(type == ES_ONE_SIGMA) {
		return "OneSigma";
	}
	if(type == ES_90_PERCENT) {
		return "90Percent";
	}
	
	return "INVALID_PDF_ERROR_SIZE";
}

//Determines whether the error size is empty or not
bool SPXPDFErrorSize::IsEmpty(void) {
	std::string mn = "IsEmpty: ";
	
	return !(bool)type;
}

//Determines the validity of the error type
bool SPXPDFErrorSize::IsValid(void) {
	std::string mn = "IsValid: ";
	
	//Empty style: valid, but empty
	if(this->IsEmpty()){
		if(debug) std::cout << cn << mn << "PDF Error Size is empty" << std::endl;
		return true;
	}
	
	if(type == ES_INVALID) {
		if(debug) std::cout << cn << mn << "PDF Error Size is invalid: Set to ES_INVALID (-1)" << std::endl;
		return false;
	}
	
	if(type >= (ES_ONE_SIGMA | ES_90_PERCENT)) {
		if(debug) std::cout << cn << mn << "PDF Error Size is invalid: Type is set to unknown value: " << type << std::endl;
		return false;
	}
	
	return true;
}

