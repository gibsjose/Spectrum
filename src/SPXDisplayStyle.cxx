//************************************************************/
//
//	Display Style Implementation
//
//	Implements the SPXDisplayStyle class, which describes exactly what
//	plots will be made on the TCanvas. The options are Overlay,
//	Ratio, or both.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		26.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXDisplayStyle.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXDisplayStyle::";

//Must define the static debug variable in the implementation
bool SPXDisplayStyle::debug;

//Constructs a SPXDisplayStyle object with a given string (effectively
//	calls the Parse() method on the input string)
SPXDisplayStyle::SPXDisplayStyle(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Takes a string argument in the form:
//	{overlay}, {ratio}
//
// Thus, the string can be one of three options:
//	overlay
//	ratio
//	overlay, ratio
//
// Sets the style based on the input string
void SPXDisplayStyle::Parse(std::string s) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
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
	if(v.size() > 2) {
		style = DS_INVALID;
		throw SPXINIParseException("PLOT_x", "display_style", "Incorrect display style: Configuration string can only be a combination of: \"overlay\" and \"ratio\"");
	}
	if(v.size() < 1) {
		style = DS_INVALID;
		throw SPXINIParseException("PLOT_x", "display_style", "Incorrect display style: Configuration string must be at least ONE of: \"overlay\" or \"ratio\"");
	}

	//Create the style based on the configuration string(s)
	for(int i = 0; i < v.size(); i++) {
		if(debug) std::cout << cn << mn << "Checking style string: " << v[i] << std::endl;
		if(!v[i].compare("overlay")) {
			if(debug) std::cout << cn << mn << "Successfully matched style string: \"overlay\"" << std::endl;
			style |= DS_OVERLAY;
			if(debug) std::cout << cn << mn << "Display style successfully set to contain \"overlay\"" <<std::endl;
		}
		else if(!v[i].compare("ratio")) {
			if(debug) std::cout << cn << mn << "Successfully matched style string: \"ratio\"" << std::endl;
			style |= DS_RATIO;
			if(debug) std::cout << cn << mn << "Display style successfully set to contain \"ratio\"" <<std::endl;
		}
		else {
			style = DS_INVALID;

			std::ostringstream oss;
			oss << "Incorrect display style: Unrecognized style option: \"" << v[i] << "\" is invalid";
			throw SPXINIParseException("PLOT_x", "display_style", oss.str());
		}
	}
}

//Print displays the output of ToString to the console
void SPXDisplayStyle::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's style data
std::string SPXDisplayStyle::ToString(void) {
	std::string mn = "ToString: ";

	//Empty style
	if(this->IsEmpty()) {
		return "";
	}

	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_DISPLAY_STYLE";
	}

	//Build syle option vector
	std::vector<std::string> v;

	if(style & DS_OVERLAY) {
		v.push_back("overlay");
	}
	if(style & DS_RATIO) {
		v.push_back("ratio");
	}

	return SPXStringUtilities::VectorToCommaSeparatedList(v);
}

//Determines whether the display style is empty or not
bool SPXDisplayStyle::IsEmpty(void) {
	std::string mn = "IsEmpty: ";

	return !(bool)style;
}

//Determines the validity of the display style
bool SPXDisplayStyle::IsValid(void) {
	std::string mn = "IsValid: ";

	//Empty style: valid, but empty
	if(this->IsEmpty()){
		if(debug) std::cout << cn << mn << "Display style is empty" << std::endl;
		return true;
	}

	if(style == DS_INVALID) {
		if(debug) std::cout << cn << mn << "Display style is invalid: Set to DS_INVALID (-1)" << std::endl;
		return false;
	}

	if(style > (DS_OVERLAY | DS_RATIO)) {
		if(debug) std::cout << cn << mn << "Display style is invalid: Style is set to unknown value: " << style << std::endl;
		return false;
	}

	return true;
}
