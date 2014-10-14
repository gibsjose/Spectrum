//************************************************************/
//
//	Frame Options Implementation
//
//	Implements the SPXPlotConfiguration class, which describes the options
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
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		29.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/
#include <stdlib.h> //atoi

#include "SPXPlotConfiguration.h"
#include "SPXUtilities.h"

//Class names for debug statements
const std::string cn = "SPXPlotConfiguration::";

//Must define the static debug variable in the implementation
bool SPXPlotConfiguration::debug;
bool SPXPlotConfigurationInstance::debug;

//Constructs an SPXPlotConfiguration object with a vector of string vectors, where indices are:
//	options[0] --> Vector of data steering files
//	options[1] --> Vector of grid steering files
//	options[2] --> Vector of marker styles
//	options[3] --> Vector of marker colors
// 	options[4] --> Vector of reference line styles (optional)
//	options[5] --> Vector of reference line colors (optional)
//
//Description is the (optional) frame description
//And numberOfConfigurationInstances is the size of the vector of string vectors
SPXPlotConfiguration::SPXPlotConfiguration(const std::vector<std::vector<std::string> > & options, const std::string description, unsigned int numberOfConfigurationInstances) {
	this->SetDefaults();
	this->description = description;
	this->numberOfConfigurationInstances = numberOfConfigurationInstances;
	
	//Attempt to parse the options instances
	try {
		this->Parse(options);
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		
		throw SPXParseException("Unable to parse SPXPlotConfiguration instances");
	}	
}

//Parse the vector of vector of strings into a vector of SPXPlotConfigurationInstances, convert
//	strings to integers where needed, and check each instance for errors (and IsEmpty) before
//	adding it to the vector (call AddOptionsInstance(), which checks IsEmpty and IsValid)
void SPXPlotConfiguration::Parse(const std::vector<std::vector<std::string> > & options) {
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
		std::cout << "\tdata_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		tmpVector = options[1];
		std::cout << "\tgrid_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		tmpVector = options[2];
		std::cout << "\tmarker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		tmpVector = options[3];
		std::cout << "\tmarker_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		
		if(options.size() >= 5) {
			tmpVector = options[4];
			std::cout << "\tref_line_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No reference line style option specified" << std::endl;
		}
		
		if(options.size() == 6) {
			tmpVector = options[5];
			std::cout << "\tref_line_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No reference line color option specified" << std::endl;
		}
	}

	if(debug) std::cout << cn << mn << "numberOfConfigurationInstances = " << numberOfConfigurationInstances << std::endl;
	
	//Check options vector sizes against numberOfConfigurationInstances (should ALL be equal)
	if(numberOfConfigurationInstances != options[0].size()) {
		std::ostringstream oss;
		oss << "Size of Data Steering File vector (" << options[0].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfConfigurationInstances != options[1].size()) {
		std::ostringstream oss;
		oss << "Size of Grid Steering File vector (" << options[1].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfConfigurationInstances != options[2].size()) {
		std::ostringstream oss;
		oss << "Size of Marker Style vector (" << options[2].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfConfigurationInstances != options[3].size()) {
		std::ostringstream oss;
		oss << "Size of Marker Color vector (" << options[3].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	
	if(options.size() >= 5) {
		if(numberOfConfigurationInstances != options[4].size()) {
			std::ostringstream oss;
			oss << "Size of Reference Line Style vector (" << options[4].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
			throw SPXParseException(oss.str());
		}
	}
	
	if(options.size() == 6) {
		if(numberOfConfigurationInstances != options[5].size()) {
			std::ostringstream oss;
			oss << "Size of Reference Line Color vector (" << options[5].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
			throw SPXParseException(oss.str());
		}
	}
	
	//For each options instance, create an SPXPlotConfigurationInstance object and add it to the vector
	for(int i = 0; i < numberOfConfigurationInstances; i++) {	
		SPXPlotConfigurationInstance pci;
		pci.SetDefaults();
		
		//Create objects and set the filename for the Data/Grid Steering Files
		pci.dataSteeringFile = SPXDataSteeringFile(options[0][i]);
		pci.gridSteeringFile = SPXGridSteeringFile(options[1][i]);
		
		pci.markerStyle = atoi(options[2][i].c_str());
		pci.markerColor = atoi(options[3][i].c_str());
		
		if(options.size() >= 5) {
			pci.refLineStyle = atoi(options[4][i].c_str());
		}
		
		if(options.size() == 6) {
			pci.refLineColor = atoi(options[5][i].c_str());
		}
		
		//Attempt to add the configuration instance
		try {
			AddConfigurationInstance(pci);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			throw SPXParseException("ERROR: Could not add options configuration to configuration instance vector: Instance was empty or invalid");
		}
	}
}

//Print displays the output of ToString to the console
/*
void SPXPlotConfiguration::Print(void) {
	std::string mn = "Print: ";
	//std::cout << this->ToString() << std::endl;
}
*/

//ToString does the opposite of Parse
/*
const std::string & SPXPlotConfiguration::ToString(void) const {
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
bool SPXPlotConfiguration::IsEmpty(void) {
	std::string mn = "IsEmpty: ";

	//Return true if all options vector is empty and numberOfConfigurationInstances is 0
	if((numberOfConfigurationInstances == 0) && (configurationInstances.size() == 0)) {
		return true;
	}
	
	return false;
}

//Determines the validity of the frame options
bool SPXPlotConfiguration::IsValid(void) {
	std::string mn = "IsValid: ";
	
	//Valid, but empty
	if(IsEmpty()) {
		return true;
	}
	
	//Return false if the numberOfConfigurationInstances does not match the size of the configurationInstances vector
	if(numberOfConfigurationInstances != configurationInstances.size()) {
		if(debug) std::cout << cn << mn << "Size of Options Instances vector (" << configurationInstances.size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")" << std::endl;
		return false;
	}
	
	//Return false if ANY of the frame options are invalid
	for(int i = 0; i < numberOfConfigurationInstances; i++) {
		if(!configurationInstances[i].IsValid()) {
			if(debug) std::cout << cn << mn << "An invalid options instance was found at index " << i << std::endl;
			return false;
		}
	}
	
	return true;
}

