//************************************************************/
//
//	Overlay Style Implementation
//
//	Implements the SPXOverlayStyle class, which describes which datasets
//	will be plotted on the overlay graph. Options include: data,
//	reference, and convolute.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		26.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXOverlayStyle.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXOverlayStyle::";

//Must define the static debug variable in the implementation
bool SPXOverlayStyle::debug;

//Constructs an SPXOverlayStyle object with a given string (effectively
//	calls the Parse() method on the input string)
SPXOverlayStyle::SPXOverlayStyle(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//	{data}, {reference}, {convolute}
//
// Thus, the string can be any combination of the
// three options, but it MUST be at least ONE of
// the options.
//
// Sets the style based on the input string
void SPXOverlayStyle::Parse(std::string s) {
	std::string mn = "Parse: ";

	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;

	//Clear the style each time it is parsed
	this->Clear();

	//Parse the string into style options
	std::vector<std::string> v = SPXStringUtilities::CommaSeparatedListToVector(s);

	if(debug) {
		std::cout << cn << mn << "Configuration string: " << s << " was parsed into:" << std::endl;
		for(int i = 0; i < v.size(); i++) {
			std::cout << "\t" << v[i] << std::endl;
		}
		std::cout << std::endl;
	}

	//Check the parsed style options vector for size errors
	if(v.size() > 3) {
		style = OS_INVALID;
		throw SPXINIParseException("PLOT_x", "overlay_style", "Incorrect overlay style: Configuration string can only be a combination of: \"data\", \"reference\", and \"convolute\"");

	}
	if(v.size() < 1) {
		style = OS_INVALID;
		throw SPXINIParseException("PLOT_x", "overlay_style", "Incorrect overlay style: Configuration string must be at least ONE of: \"data\", \"reference\", or \"convolute\"");
	}

	//Create the style based on the configuration string(s)
	for(int i = 0; i < v.size(); i++) {
		if(debug) std::cout << cn << mn << "Checking style string: " << v[i] << std::endl;
		if(!v[i].compare("data")) {
			if(debug) std::cout << cn << mn << "Successfully matched style string: \"data\"" << std::endl;
			style |= OS_DATA;
			if(debug) std::cout << cn << mn << "Overlay style successfully set to contain \"data\"" <<std::endl;
		}
		else if(!v[i].compare("reference")) {
			if(debug) std::cout << cn << mn << "Successfully matched style string: \"reference\"" << std::endl;
			style |= OS_REFERENCE;
			if(debug) std::cout << cn << mn << "Overlay style successfully set to contain \"reference\"" <<std::endl;
		}
		else if(!v[i].compare("convolute")) {
			if(debug) std::cout << cn << mn << "Successfully matched style string: \"convolute\"" << std::endl;
			style |= OS_CONVOLUTE;
			if(debug) std::cout << cn << mn << "Overlay style successfully set to contain \"convolute\"" <<std::endl;
		}
		else {
			style = OS_INVALID;

			std::ostringstream oss;
			oss << "Incorrect overlay style: Unrecognized style option: \"" << v[i] << "\" is invalid";
			throw SPXINIParseException("PLOT_x", "overlay_style", oss.str());
		}
	}
}

//Print displays the output of ToString to the console
void SPXOverlayStyle::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's style data
std::string SPXOverlayStyle::ToString(void) {
	std::string mn = "ToString: ";

	//Empty style
	if(this->IsEmpty()) {
		return "";
	}

	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_OVERLAY_STYLE";
	}

	//Build syle option vector
	std::vector<std::string> v;

	if(style & OS_DATA) {
		v.push_back("data");
	}
	if(style & OS_REFERENCE) {
		v.push_back("reference");
	}
	if(style & OS_CONVOLUTE) {
		v.push_back("convolute");
	}

	return SPXStringUtilities::VectorToCommaSeparatedList(v);
}

//Determines whether the overlay style is empty or not
bool SPXOverlayStyle::IsEmpty(void) {
	std::string mn = "IsEmpty: ";

	return !(bool)style;
}

//Determines the validity of the overlay style
bool SPXOverlayStyle::IsValid(void) {
	std::string mn = "IsValid: ";

	//Empty style: valid, but empty
	if(this->IsEmpty()){
		if(debug) std::cout << cn << mn << "Overlay style is empty" << std::endl;
		return true;
	}

	if(style == OS_INVALID) {
		if(debug) std::cout << cn << mn << "Overlay style is invalid: Set to OS_INVALID (-1)" << std::endl;
		return false;
	}

	if(style > (OS_DATA | OS_REFERENCE | OS_CONVOLUTE)) {
		if(debug) std::cout << cn << mn << "Overlay style is invalid: Style is set to unknown value: " << style << std::endl;
		return false;
	}

	return true;
}
