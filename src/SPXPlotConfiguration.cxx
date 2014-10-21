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

//Constructs an SPXPlotConfiguration object with a map of string vectors, where keys are
//	options["data_steering_files"] --> Vector of data steering files
//	options["grid_steering_files"] --> Vector of grid steering files
//	options["marker_style"] --> Vector of marker styles
//	options["marker_color"] --> Vector of marker colors
// 	options["ref_line_style"] --> Vector of reference line styles (optional)
//	options["ref_line_color"] --> Vector of reference line colors (optional)
//	options["x_scale"] --> Vector of X Scales (optional)
//	options["y_scale"] --> Vector of Y Scales (optional)
//
//xLog is whether or not to plot the X Axis Logarithmically
//yLog is whether or not to plot the Y Axis Logarithmically
//Description is the (optional) frame description
//And numberOfConfigurationInstances is the size of the vector of string vectors
SPXPlotConfiguration::SPXPlotConfiguration(std::map<std::string, std::vector<std::string> > & options, bool xLog, bool yLog, const std::string & description, unsigned int numberOfConfigurationInstances) {
	this->SetDefaults();
	this->xLog = xLog;
	this->yLog = yLog;
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

//Parse the map of vectors of strings into a vector of SPXPlotConfigurationInstances, convert
//	strings to integers where needed, and check each instance for errors (and IsEmpty) before
//	adding it to the vector (call AddOptionsInstance(), which checks IsEmpty and IsValid)
void SPXPlotConfiguration::Parse(std::map<std::string, std::vector<std::string> > &options) {
	std::string mn = "Parse: ";

	//Make sure all required vectors exist
	{
		if(options.count("data_steering_files") == 0) {
			throw SPXParseException("The options map MUST contain a vector for data_steering_files");
		}

		if(options.count("grid_steering_files") == 0) {
			throw SPXParseException("The options map MUST contain a vector for grid_steering_files");
		}

		if(options.count("marker_style") == 0) {
			throw SPXParseException("The options map MUST contain a vector for marker_style");
		}

		if(options.count("marker_color") == 0) {
			throw SPXParseException("The options map MUST contain a vector for marker_color");
		}
	}

	if(debug) {
		std::cout << cn << mn << "Parsing frame options vector:" << std::endl;

		std::vector<std::string> tmpVector;

		tmpVector = options["data_steering_files"];
		std::cout << "\tdata_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["grid_steering_files"];
		std::cout << "\tgrid_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["marker_style"];
		std::cout << "\tmarker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["marker_color"];
		std::cout << "\tmarker_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		if(options.count("ref_line_style")) {
			tmpVector = options["ref_line_style"];
			std::cout << "\tref_line_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No reference line style option specified" << std::endl;
		}

		if(options.count("ref_line_color")) {
			tmpVector = options["ref_line_color"];
			std::cout << "\tref_line_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No reference line color option specified" << std::endl;
		}

		if(options.count("x_scale")) {
			tmpVector = options["x_scale"];
			std::cout << "\tx_scale = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No X Scale option specified" << std::endl;
		}

		if(options.count("y_scale")) {
			tmpVector = options["y_scale"];
			std::cout << "\ty_scale = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Y Scale option specified" << std::endl;
		}
	}

	if(debug) std::cout << cn << mn << "numberOfConfigurationInstances = " << numberOfConfigurationInstances << std::endl;

	//Check options vector sizes against numberOfConfigurationInstances (should ALL be equal)
	if(numberOfConfigurationInstances != options["data_steering_files"].size()) {
		std::ostringstream oss;
		oss << "Size of Data Steering File vector (" << options["data_steering_files"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfConfigurationInstances != options["grid_steering_files"].size()) {
		std::ostringstream oss;
		oss << "Size of Grid Steering File vector (" << options["grid_steering_files"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfConfigurationInstances != options["marker_style"].size()) {
		std::ostringstream oss;
		oss << "Size of Marker Style vector (" << options["marker_style"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}
	if(numberOfConfigurationInstances != options["marker_color"].size()) {
		std::ostringstream oss;
		oss << "Size of Marker Color vector (" << options["marker_color"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
		throw SPXParseException(oss.str());
	}

	if(options.count("ref_line_style")) {
		if(numberOfConfigurationInstances != options["ref_line_style"].size()) {
			std::ostringstream oss;
			oss << "Size of Reference Line Style vector (" << options["ref_line_style"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
			throw SPXParseException(oss.str());
		}
	}

	if(options.count("ref_line_color")) {
		if(numberOfConfigurationInstances != options["ref_line_color"].size()) {
			std::ostringstream oss;
			oss << "Size of Reference Line Color vector (" << options["ref_line_color"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
			throw SPXParseException(oss.str());
		}
	}

	if(options.count("x_scale")) {
		if(numberOfConfigurationInstances != options["x_scale"].size()) {
			std::ostringstream oss;
			oss << "Size of X Scale vector (" << options["x_scale"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
			throw SPXParseException(oss.str());
		}
	}

	if(options.count("y_scale")) {
		if(numberOfConfigurationInstances != options["y_scale"].size()) {
			std::ostringstream oss;
			oss << "Size of Y Scale vector (" << options["y_scale"].size() << ") is NOT equal to the number of options instances (" << numberOfConfigurationInstances << ")";
			throw SPXParseException(oss.str());
		}
	}

	//For each options instance, create an SPXPlotConfigurationInstance object and add it to the vector
	for(int i = 0; i < numberOfConfigurationInstances; i++) {
		SPXPlotConfigurationInstance pci;
		pci.SetDefaults();

		//Create objects and set the filename for the Data/Grid Steering Files
		pci.dataSteeringFile = SPXDataSteeringFile(options["data_steering_files"][i]);
		pci.gridSteeringFile = SPXGridSteeringFile(options["grid_steering_files"][i]);

		pci.markerStyle = atoi(options["marker_style"][i].c_str());
		pci.markerColor = atoi(options["marker_color"][i].c_str());

		if(options.count("ref_line_style")) {
			pci.refLineStyle = atoi(options["ref_line_style"][i].c_str());
		}

		if(options.count("ref_line_color")) {
			pci.refLineColor = atoi(options["ref_line_color"][i].c_str());
		}

		if(options.count("x_scale")) {
			pci.xScale = atoi(options["x_scale"][i].c_str());
		} else {
			pci.xScale = 1.0;
		}

		if(options.count("y_scale")) {
			pci.yScale = atoi(options["y_scale"][i].c_str());
		} else {
			pci.yScale = 1.0;
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
