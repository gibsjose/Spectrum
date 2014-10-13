//************************************************************/
//
//	Ratio Style Implementation
//
//	Implements the SPXRatioStyle class, which describes the type of
//	ratio to be plotted in the ratio section. Ratio styles can
//	include: [data AND/OR reference AND/OR convolute] in the
//	numerator, and [data XOR reference XOR convolute] in the
//	demominator.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		25.09.2014
//
//************************************************************/
#include <iostream>
#include <sstream>

#include "SPXRatioStyle.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXRatioStyle::";

//Must define the static debug variable in the implementation
bool SPXRatioStyle::debug;

//Constructs an SPXRatioStyle object with a given string (effectively
//	calls the Parse() method on the input string)
SPXRatioStyle::SPXRatioStyle(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//	[data{,} | reference{,} | convolute]/[data XOR reference XOR convolute]
//
//	Such as:
//		"data,reference/convolute"
//	or
//		"convolute/data"
// but NOT
//		"convolute/data,reference" <--- ERROR!
//
// Sets the numerator and denominator bitfields based on the input string
void SPXRatioStyle::Parse(std::string s) {
	std::string mn = "Parse: ";
	
	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;
	
	std::string den;
	std::vector<std::string> num;
	
	//Clear the numerator/denominator each time it is parsed
	this->Clear();
	
	//Parse the string into numerator and denominator (delimit with '/')
	std::vector<std::string> v = SPXStringUtilities::ParseString(s, '/');
	
	//Parse the numerator by commas
	num = SPXStringUtilities::CommaSeparatedListToVector(v[0]);
	
	//Obtain the denominator
	den = v[1]; 
	
	if(debug) std::cout << cn << mn << "Denominator set to: " << den << std::endl;
	
	//Check the numerator and denominator for size errors
	if(num.size() > 3) {
		numerator = RS_INVALID;
		denominator = RS_INVALID;
		
		throw SPXINIParseException("GRAPH", "ratio_style", "Incorrect ratio style: Numerator can only be a combination of: \"data\", \"reference\", and \"convolute\"");
	}
	if(num.size() < 1) {
		numerator = RS_INVALID;
		denominator = RS_INVALID;
		
		throw SPXINIParseException("GRAPH", "ratio_style", "ERROR: Incorrect ratio style: Numerator must be at least ONE of: \"data\", \"reference\", or \"convolute\"");
	}
	if(den.empty()) {
		numerator = RS_INVALID;
		denominator = RS_INVALID;
		
		throw SPXINIParseException("GRAPH", "ratio_style", "ERROR: Incorrect ratio style: Denominator must be EXACTLY one of: \"data\", \"reference\", or \"convolute\"");
	}
	
	//Validate the numerator
	for(int i = 0; i < num.size(); i++) {
		if(debug) std::cout << cn << mn << "num[" << i << "] = " << num[i] << std::endl;
		
		if(!num[i].compare("data")) {
			if(debug) std::cout << cn << mn << "Found numerator string = \"data\"" << std::endl;
			numerator |= RS_DATA;
		}
		else if(!num[i].compare("reference")) {
			if(debug) std::cout << cn << mn << "Found numerator string = \"reference\"" << std::endl;
			numerator |= RS_REFERENCE;
		}
		else if(!num[i].compare("convolute")) {
			if(debug) std::cout << cn << mn << "Found numerator string = \"convolute\"" << std::endl;
			numerator |= RS_CONVOLUTE;
		}
		else {
			numerator = RS_INVALID;
			denominator = RS_INVALID;
			
			std::ostringstream oss;
			oss << "Incorrect ratio style: Unrecognized ratio numerator option: \"" << num[i] << "\" is invalid";
			throw SPXINIParseException("GRAPH", "ratio_style", oss.str());
		}
	}
	
	if(debug) std::cout << cn << mn << "Numerator = " << numerator << std::endl;
	
	//Validate the denominator
	if(!den.compare("data")) {
		if(debug) std::cout << cn << mn << "Found denominator string = \"data\"" << std::endl;
		denominator = RS_DATA;
	}
	else if(!den.compare("reference")) {
		if(debug) std::cout << cn << mn << "Found denominator string = \"reference\"" << std::endl;
		denominator = RS_REFERENCE;
	}
	else if(!den.compare("convolute")) {
		if(debug) std::cout << cn << mn << "Found denominator string = \"convolute\"" << std::endl;
		denominator = RS_CONVOLUTE;
	}
	else {
		numerator = RS_INVALID;
		denominator = RS_INVALID;
		
		std::ostringstream oss;
		oss << "Incorrect ratio style: Unrecognized ratio denominator option: \"" << den << "\" is invalid";
		throw SPXINIParseException("GRAPH", "ratio_style", oss.str());
	}
	
	if(debug) std::cout << cn << mn << "Denominator = " << denominator << std::endl;
}

//Print displays the output of ToString to the console
void SPXRatioStyle::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the numerator and denominator bitfields
std::string SPXRatioStyle::ToString(void) {
	std::string mn = "ToString: ";
	
	//Empty style
	if(this->IsEmpty()) {
		return "";
	}
	
	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_RATIO_STYLE";
	}
	
	//Build numerator and denominator
	std::vector<std::string> num;
	std::string den;	
	
	num.clear();
	
	if(numerator & RS_DATA) {
		num.push_back("data");
	}
	if(numerator & RS_REFERENCE) {
		num.push_back("reference");
	}
	if(numerator & RS_CONVOLUTE) {
		num.push_back("convolute");
	}
	
	if(denominator == RS_DATA) {
		den = "data";
	}
	else if(denominator == RS_REFERENCE) {
		den = "reference";
	}
	else if(denominator == RS_CONVOLUTE) {
		den = "convolute";
	}
	
	return SPXStringUtilities::VectorToCommaSeparatedList(num) + " / " + den;
}

//Determines whether the ratio style is empty or not
bool SPXRatioStyle::IsEmpty(void) {
	std::string mn = "IsEmpty: ";
	
	if(!(numerator | denominator)) {
		return true;
	}
	
	return false;
}

//Determines the validity of the ratio style
bool SPXRatioStyle::IsValid(void) {
	std::string mn = "IsValid: ";
	
	//Empty numerator and denominator: valid, but empty
	if(this->IsEmpty()){
		if(debug) std::cout << cn << mn << "Empty numerator and denominator" << std::endl;
		return true;
	}
	
	//Only one of numerator OR denominator empty
	if((!!numerator) ^ (!!denominator)) {
		if(debug) std::cout << cn << mn << "Mismatch: numerator = " << numerator << " denominator = " << denominator << std::endl;
		return false;
	}
	
	if(numerator > (RS_DATA | RS_REFERENCE | RS_CONVOLUTE)) {
		if(debug) std::cout << cn << mn << "Numerator is out of bounds: numerator = " << numerator << std::endl;
		return false;
	}
	
	if((denominator != RS_DATA) && (denominator != RS_REFERENCE) && (denominator != RS_CONVOLUTE)) {
		if(debug) std::cout << cn << mn << "Denominator is invalid: denominator = " << denominator << std::endl;
		return false;
	}
	
	return true;
}

