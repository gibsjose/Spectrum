//************************************************************/
//
//	PDF Error Type Implementation
//
//	Implements the SPXPDFErrorType class, which describes the PDF
//	error type. The options are 'alpha_s' or 'pdf_band'
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		29.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXPDFErrorType.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXPDFErrorType::";

//Must define the static debug variable in the implementation
bool SPXPDFErrorType::debug=false;

//Constructs an SPXPDFErrorType object with a given string (effectively
//	calls the Parse() method on the input string)
SPXPDFErrorType::SPXPDFErrorType(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//	alpha_s XOR pdf_band
//
// Sets the type based on the input string
void SPXPDFErrorType::Parse(std::string s) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;

	//Clear the type each time it is parsed
	this->Clear();

	if(!s.compare("alphas_band")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"alpha_s\"" << std::endl;
		type = ET_ALPHA_S;
		if(debug) std::cout << cn << mn << "PDF Error Type successfully set to \"alpha_s\"" << std::endl;
	}
	else if(!s.compare("pdf_band")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"pdf_band\"" << std::endl;
		type = ET_PDF_BAND;
		if(debug) std::cout << cn << mn << "PDF Error Type successfully set to \"pdf_band\"" << std::endl;
	} else if(!s.compare("scale_band")) {
		if(debug) std::cout << cn << mn << "Successfully matched type string: \"scale_band\"" << std::endl;
		type = ET_SCALE_BAND;
		if(debug) std::cout << cn << mn << "Scale Uncertainty successfully set to \"scale_band\"" << std::endl;

	} else {
		type = ET_INVALID;

		std::ostringstream oss;
		oss << "Incorrect PDF Error Type: Unrecognized type option: \"" << s << "\" is invalid";
		throw SPXINIParseException("PDF", "pdf_error_type", oss.str());
	}
}

//Print displays the output of ToString to the console
void SPXPDFErrorType::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's type data
std::string SPXPDFErrorType::ToString(void) {
	std::string mn = "ToString: ";

	//Empty style
	if(this->IsEmpty()) {
		return "";
	}

	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_PDF_ERROR_TYPE";
	}

	if(type == ET_ALPHA_S) {
		return "AlphaS";
	}
	if(type == ET_PDF_BAND) {
		return "PDFBand";
	}

	if(type == ET_SCALE_BAND) {
		return "SCALEBand";
	}

	return "INVALID_PDF_ERROR_TYPE";
}

//Determines whether the error type is empty or not
bool SPXPDFErrorType::IsEmpty(void) {
	std::string mn = "IsEmpty: ";

	return !(bool)type;
}

//Determines the validity of the error type
bool SPXPDFErrorType::IsValid(void) {
	std::string mn = "IsValid: ";

	//Empty style: valid, but empty
	if(this->IsEmpty()){
	 std::cout << cn << mn << "WARNING: PDF Error Type is empty" << std::endl;
	 return true;
	}

	if(type == ET_INVALID) {
	 std::cout << cn << mn << "ERROR PDF Error Type is invalid: Set to ET_INVALID (-1)" << std::endl;
	 return false;
	}

	if(type >= (ET_ALPHA_S | ET_PDF_BAND | ET_SCALE_BAND)) {
	 std::cout << cn << mn << "ERROR PDF Error Type is invalid: Type is set to unknown value: " << type << std::endl;
	 return false;
	}

	return true;
}
