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
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
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
SPXRatioStyle::SPXRatioStyle(std::string &s, unsigned int pn, unsigned int rsn) {
	this->Clear();

	std::ostringstream pss;
	std::ostringstream rss;
	pss << "PLOT_" << pn;
	rss << "ratio_style_" << rsn;

	this->plotNumber = pss.str();
	this->ratioStyleNumber = rss.str();
	this->Parse(s);
}

//Takes a string argument in ONLY the following four forms:
//
//		data / convolute
//		convolute / data
//		convolute / reference
//		data / data
//
// Sets the numerator and denominator bitfields based on the input string
void SPXRatioStyle::Parse(std::string &s) {
	std::string mn = "Parse: ";

	//Clear the numerator/denominator and dataStat/Tot flags each time it is parsed
	this->Clear();

	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;

	//Look for special ratio styles:
	//	data_stat	<-- Plot data/data statistical errors in ratio section
	//	data_tot	<-- Plot data/data total errors in ratio section

	if(!s.compare("data_stat")) {
		dataStat = true;
		dataTot = false;
		if(debug) std::cout << cn << mn << "Ratio Style set to \"data_stat\"" << std::endl;
		return;
	}

	if(!s.compare("data_tot")) {
		dataStat = false;
		dataTot = true;
		if(debug) std::cout << cn << mn << "Ratio Style set to \"data_tot\"" << std::endl;
		return;
	}

	dataStat = false;
	dataTot = false;

	std::string den;
	std::string num;

	//Parse the string into numerator and denominator (delimit with '/')
	std::vector<std::string> v = SPXStringUtilities::ParseString(s, '/');

	if(v.size() != 2) {
		throw SPXINIParseException(plotNumber, ratioStyleNumber, "Incorrect ratio style: Must be in one of the forms: data_stat OR data_tot OR numerator / denominator");
	}

	//Parse the numerator by commas
	num = v.at(0);

	//Obtain the denominator
	den = v.at(1);

	if(debug) std::cout << cn << mn << "Numerator set to: " << num << std::endl;
	if(debug) std::cout << cn << mn << "Denominator set to: " << den << std::endl;

	//Check the numerator and denominator for size errors
	if(num.empty()) {
		numerator = RS_INVALID;
		denominator = RS_INVALID;

		throw SPXINIParseException(plotNumber, ratioStyleNumber, "Incorrect ratio style: Numerator must be EXACTLY one of: \"data\" or \"convolute\"");
	}
	if(den.empty()) {
		numerator = RS_INVALID;
		denominator = RS_INVALID;

		throw SPXINIParseException(plotNumber, ratioStyleNumber, "Incorrect ratio style: Denominator must be EXACTLY one of: \"data\", \"reference\", or \"convolute\"");
	}

	//Validate the numerator
	if(!num.compare("data")) {
		if(debug) std::cout << cn << mn << "Found numerator string = \"data\"" << std::endl;
		numerator |= RS_DATA;
	}
	else if(!num.compare("reference")) {
		throw SPXINIParseException(plotNumber, ratioStyleNumber, "Incorrect ratio style: Numerator cannot be \"reference\"");
		numerator = RS_INVALID;
	}
	else if(!num.compare("convolute")) {
		if(debug) std::cout << cn << mn << "Found numerator string = \"convolute\"" << std::endl;
		numerator |= RS_CONVOLUTE;
	}
	else {
		numerator = RS_INVALID;
		denominator = RS_INVALID;

		std::ostringstream oss;
		oss << "Incorrect ratio style: Unrecognized ratio numerator option: \"" << num << "\" is invalid";
		throw SPXINIParseException(plotNumber, ratioStyleNumber, oss.str());
	}

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
		throw SPXINIParseException(plotNumber, ratioStyleNumber, oss.str());
	}

	//Check for invalid combinations
	if(numerator == RS_DATA) {
		if(denominator == RS_REFERENCE) {
			numerator = RS_INVALID;
			denominator = RS_INVALID;
			throw SPXINIParseException(plotNumber, ratioStyleNumber, "Invalid ratio combination: data / reference is not supported");
		}
	}

	if(numerator == RS_CONVOLUTE) {
		if(denominator == RS_CONVOLUTE) {
			numerator = RS_INVALID;
			denominator = RS_INVALID;
			throw SPXINIParseException(plotNumber, ratioStyleNumber, "Invalid ratio combination: convolute / convolute is not supported");
		}
	}

	if(debug) std::cout << cn << mn << plotNumber << " " << ratioStyleNumber << ": Denominator = " << den << "(" << denominator << ")" << std::endl;
	if(debug) std::cout << cn << mn << plotNumber << " " << ratioStyleNumber << ": Numerator = " << num << "(" << numerator << ")" << std::endl;
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

	if(dataStat) {
		return "data_stat";
	}

	if(dataTot) {
		return "data_tot";
	}

	//Empty style
	if(this->IsEmpty()) {
		return "";
	}

	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_RATIO_STYLE";
	}

	//Build numerator and denominator
	std::string num;
	std::string den;

	if(numerator == RS_DATA) {
		num = "data";
	}
	else if(numerator == RS_CONVOLUTE) {
		num = "convolute";
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

	return num + " / " + den;
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

	if((numerator != RS_DATA) && (numerator != RS_CONVOLUTE)) {
		if(debug) std::cout << cn << mn << "Numerator is invalid: numerator = " << numerator << std::endl;
		return false;
	}

	if((denominator != RS_DATA) && (denominator != RS_REFERENCE) && (denominator != RS_CONVOLUTE)) {
		if(debug) std::cout << cn << mn << "Denominator is invalid: denominator = " << denominator << std::endl;
		return false;
	}

	return true;
}
