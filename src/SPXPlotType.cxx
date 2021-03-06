//************************************************************/
//
//	Plot Type Implementation
//
//	Implements the SPXPlotType class, which defines the four
//	supported plot types: see documention in SPXPlotType.h
//
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN 
//	@Date:		27.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXPlotType.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXPlotType::";

//Must define the static debug variable in the implementation
bool SPXPlotType::debug;

//Constructs a SPXPlotType object with a given string (effectively
//	calls the Parse() method on the input string)
SPXPlotType::SPXPlotType(std::string s) {
	this->Clear();
	this->Parse(s);
}

//Sets the type based on the input string
void SPXPlotType::Parse(std::string s) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
	if(debug) std::cout << cn << mn << "Parsing configuration string: " << s << std::endl;

	//Clear the type each time it is parsed
	this->Clear();

	//Parse the string into type options
	std::vector<std::string> v = SPXStringUtilities::CommaSeparatedListToVector(s);

	if(debug) {
		std::cout << cn << mn << "Configuration string: " << s << " was parsed into:" << std::endl;
		for(int i = 0; i < v.size(); i++) {
			std::cout << "\t" << v[i] << std::endl;
		}
		std::cout << std::endl;
	}

	//Check the parsed type options vector for size errors
	//if(v.size() != 3) {
	//	type = PT_INVALID;
	//	throw SPXParseException("Incorrect plot type: Configuration string MUST be a combination of ALL three: \"data\", grid, and \"pdf\"");
	//}

	if(v.size() == 0) {
	 type = PT_INVALID;
  	 throw SPXParseException("Incorrect plot type: Configuration string is empty Must be \"data\" or grid, and \"pdf\"");
	}       

	std::string data="";
	std::string grid="";
	std::string pdf="";

	if(v.size() == 1) {
	 data = v.at(0);
        }
 
	if(v.size() == 2) {
	 grid = v.at(0);        
	 pdf  = v.at(1);
        }

        if(v.size() == 3) {
  	 data = v.at(0);
	 grid = v.at(1);        
	 pdf  = v.at(2);
        }

        if(v.size() > 3) {
	 type = PT_INVALID;
	 throw SPXParseException("Incorrect plot type: Configuration string can not be bigger than three: \"data\", grid, and \"pdf\"");
	}

	//Parse data
	if(!data.compare("data")) {
		if(debug) std::cout << cn << mn << "Successfully matched \"data\"" << std::endl;
		type &= ~PT_DATA;
	}
	else if(!data.compare("data[]")) {
		if(debug) std::cout << cn << mn << "Successfully matched \"data[]\"" << std::endl;
		type |= PT_DATA;
	}

        if (debug) std::cout << cn << mn << "After data type= "<<type << std::endl;
	//else {
	//	throw SPXParseException("Incorrect plot type: Data configuration string: Unrecognized option: " + data + ": MUST be either \"data\" or \"data[]\"");
	//}

	//Parse grid
	if(!grid.compare("grid")) {
		if(debug) std::cout << cn << mn << "Successfully matched \"grid\"" << std::endl;
		type &= ~PT_GRID;
	}
	else if(!grid.compare("grid[]")) {
		if(debug) std::cout << cn << mn << "Successfully matched \"grid[]\"" << std::endl;
		type |= PT_GRID;
	}

        if (debug) std::cout << cn << mn << "After grid type= "<<type << std::endl;
	//else {
	//	throw SPXParseException("Incorrect plot type: Grid configuration string: Unrecognized option: " + grid + ": MUST be either \"grid\" or \"grid[]\"");
	//}

	//Parse pdf
	if(!pdf.compare("pdf")) {
		if(debug) std::cout << cn << mn << "Successfully matched \"pdf\"" << std::endl;
		type &= ~PT_PDF;
	}
	else if(!pdf.compare("pdf[]")) {
		if(debug) std::cout << cn << mn << "Successfully matched \"pdf[]\"" << std::endl;
		type |= PT_PDF;
	}

        if (debug) std::cout << cn << mn << "After pdf type= "<<type << std::endl;

	//else {
	//	throw SPXParseException("Incorrect plot type: PDF configuration string: Unrecognized option: " + pdf + ": MUST be either \"pdf\" or \"pdf[]\"");
	//}

	//Check for incorrect combinations
	if(!IsValid()) {
		throw SPXParseException("Incorrect plot type: The combination: " + this->ToString() + " is invalid");
	}

}

//Print displays the output of ToString to the console
void SPXPlotType::Print(void) {
	std::string mn = "Print: ";
	std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the object's type data
std::string SPXPlotType::ToString(void) {
	std::string mn = "ToString: ";

	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_PLOT_TYPE";
	}

	//Build syle option vector
	std::vector<std::string> v;

	if(type & PT_DATA) {
		v.push_back("data[]");
	} else {
		v.push_back("data");
	}

	if(type & PT_GRID) {
		v.push_back("grid[]");
	} else {
		v.push_back("grid");
	}

	if(type & PT_PDF) {
		v.push_back("pdf[]");
	} else {
		v.push_back("pdf");
	}

	return SPXStringUtilities::VectorToCommaSeparatedList(v);
}

//Determines the validity of the display type
bool SPXPlotType::IsValid(void) {
	std::string mn = "IsValid: ";

	if(type == PT_INVALID) {
		if(debug) std::cout << cn << mn << "Plot type is invalid: Set to PT_INVALID (-1)" << std::endl;
		return false;
	}

	//if(!IsType1() && !IsType2() && !IsType3() && !IsType4()) {
	if(!IsType1() && !IsType2() && !IsType3() && !IsType4() && !IsType5()) {
		if(debug) std::cout << cn << mn << "Plot type is invalid: Not a known combination: " << type << std::endl; 
		return false;
	}

	return true;
}
