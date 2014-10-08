//************************************************************/
//
//	Frame Options Implementation
//
//	Implements the SPXFrameOptions class, which describes the options
//	for each frame to plot. The options are:
//
//		data_steering_files -- Comma separated list of all data steering files
//		grid_steering_files -- Comma separated list of all grid steering files
//		marker_style -- Comma separated list of marker style options
//		marker_color -- Comma separated list of marker color options
//		
//		Optional options:
//
//			ref_line_style -- Reference line style list
//			ref_line_color -- Reference line color list
//			desc -- Description of the entire plot (not a list)
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		29.09.2014
//
//************************************************************/
#include <stdlib.h> //atoi

#include "SPXFrameOptions.h"
#include "SPXUtilities.h"

//Class names for debug statements
const std::string cn = "SPXFrameOptions::";

//Must define the static debug variable in the implementation
bool SPXFrameOptions::debug;
bool SPXFrameOptionsInstance::debug;

//Constructs an SPXFrameOptions object with a vector of string vectors, where indices are:
//	options[0] --> Vector of data steering files
//	options[1] --> Vector of grid steering files
//	options[2] --> Vector of marker styles
//	options[3] --> Vector of marker colors
// 	options[4] --> Vector of reference line styles (optional)
//	options[5] --> Vector of reference line colors (optional)
//
//Description is the (optional) frame description
//And numberOfOptionsInstances is the size of the vector of string vectors
SPXFrameOptions::SPXFrameOptions(const std::vector<std::vector<std::string> > & options, const std::string description, unsigned int numberOfOptionsInstances) {
	this->SetDefaults();
	this->description = description;
	this->numberOfOptionsInstances = numberOfOptionsInstances;
	
	//Attempt to parse the options instances
	try {
		this->Parse(options);
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		
		throw SPXParseException("Unable to parse SPXFrameOptions instances");
	}	
}

//Parse the vector of vector of strings into a vector of SPXFrameOptionsInstances, convert
//	strings to integers where needed, and check each instance for errors (and IsEmpty) before
//	adding it to the vector (call AddOptionsInstance(), which checks IsEmpty and IsValid)
void SPXFrameOptions::Parse(const std::vector<std::vector<std::string> > & options) {
	std::string mn = "Parse: ";
	
	//Return false if size of options vector is less than 4
	if(options.size() < 4) {
		std::ostringstream oss;
		
		oss << "Too few vectors in options vector: Only " << options.size() << " options: Options vector MUST contain vectors for:" << std::endl;
		oss << "\t Data Steering Files" << std::endl;
		oss << "\t Grid Steering Files" << std::endl;
		oss << "\t Marker Styles" << std::endl;
		oss << "\t Marker Colors" << std::endl;
		
		oss << "\t\t\t\t\t And optionally for:" << std::endl;
		oss << "\t Reference Line Styles" << std::endl;
		oss << "\t Reference Line Colors";
		
		throw SPXParseException(oss.str());
	}
	
	if(debug) {
		std::cout << cn << mn << "Parsing frame options vector:" << std::endl;
		
		std::vector<std::string> tmpVector;
		
		tmpVector = options[0];
		std::cout << "\tdata_steering_files = " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		tmpVector = options[1];
		std::cout << "\tgrid_steering_files = " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		tmpVector = options[2];
		std::cout << "\tmarker_style = " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		tmpVector = options[3];
		std::cout << "\tmarker_color = " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		if(options.size() >= 5) {
			tmpVector = options[4];
			std::cout << "\tref_line_style = " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No reference line style option specified" << std::endl;
		}
		
		if(options.size() == 6) {
			tmpVector = options[5];
			std::cout << "\tref_line_color = " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No reference line color option specified" << std::endl;
		}
	}
	
	//Check options vector sizes against numberOfOptionsInstances (should ALL be equal)
	if(numberOfOptionsInstances != options[0].size()) {
		std::ostringstream oss;
		oss << "Size of Data Steering File vector (" << options[0].size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfOptionsInstances != options[1].size()) {
		std::ostringstream oss;
		oss << "Size of Grid Steering File vector (" << options[1].size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfOptionsInstances != options[2].size()) {
		std::ostringstream oss;
		oss << "Size of Marker Style vector (" << options[2].size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfOptionsInstances != options[3].size()) {
		std::ostringstream oss;
		oss << "Size of Marker Color vector (" << options[3].size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")";
		throw SPXParseException(oss.str());
	}
	
	if(options.size() >= 5) {
		if(numberOfOptionsInstances != options[4].size()) {
			std::ostringstream oss;
			oss << "Size of Reference Line Style vector (" << options[4].size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")";
			throw SPXParseException(oss.str());
		}
	}
	
	if(options.size() == 6) {
		if(numberOfOptionsInstances != options[5].size()) {
			std::ostringstream oss;
			oss << "Size of Reference Line Color vector (" << options[5].size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")";
			throw SPXParseException(oss.str());
		}
	}
	
	//For each options instance, create an SPXFrameOptionsInstance object and add it to the vector
	for(int i = 0; i < numberOfOptionsInstances; i++) {	
		SPXFrameOptionsInstance foi;
		foi.SetDefaults();
		
		//Create objects and set the filename for the Data/Grid Steering Files
		foi.dataSteeringFile = SPXDataSteeringFile(options[0][i]);
		foi.gridSteeringFile = SPXGridSteeringFile(options[1][i]);
		
		foi.markerStyle = atoi(options[2][i].c_str());
		foi.markerColor = atoi(options[3][i].c_str());
		
		if(options.size() >= 5) {
			foi.refLineStyle = atoi(options[4][i].c_str());
		}
		
		if(options.size() == 6) {
			foi.refLineColor = atoi(options[5][i].c_str());
		}
		
		//Attempt to add the options instance
		try {
			AddOptionsInstance(foi);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			throw SPXParseException("ERROR: Could not add options instance to options instance vector: Instance was empty or invalid");
		}
	}
}

//Print displays the output of ToString to the console
/*
void SPXFrameOptions::Print(void) {
	std::string mn = "Print: ";
	//std::cout << this->ToString() << std::endl;
}
*/

//ToString does the opposite of Parse
/*
const std::string & SPXFrameOptions::ToString(void) const {
	std::string mn = "ToString: ";
	
	//Empty frame options
	if(this->IsEmpty()) {
		return "";
	}
	
	//Check for validity
	if(!this->IsValid()) {
		return "INVALID_FRAME_OPTIONS";
	}
	
	
}
*/

//Determines whether the Frame Options object is empty or not
bool SPXFrameOptions::IsEmpty(void) {
	std::string mn = "IsEmpty: ";

	//Return true if all options vector is empty and numberOfOptionsInstances is 0
	if((numberOfOptionsInstances == 0) && (optionsInstances.size() == 0)) {
		return true;
	}
	
	return false;
}

//Determines the validity of the frame options
bool SPXFrameOptions::IsValid(void) {
	std::string mn = "IsValid: ";
	
	//Valid, but empty
	if(IsEmpty()) {
		return true;
	}
	
	//Return false if the numberOfOptionsInstances does not match the size of the optionsInstances vector
	if(numberOfOptionsInstances != optionsInstances.size()) {
		if(debug) std::cout << cn << mn << "Size of Options Instances vector (" << optionsInstances.size() << ") is NOT equal to the number of options instances (" << numberOfOptionsInstances << ")" << std::endl;
		return false;
	}
	
	//Return false if ANY of the frame options are invalid
	for(int i = 0; i < numberOfOptionsInstances; i++) {
		if(!optionsInstances[i].IsValid()) {
			if(debug) std::cout << cn << mn << "An invalid options instance was found at index " << i << std::endl;
			return false;
		}
	}
	
	return true;
}

