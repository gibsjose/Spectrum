//************************************************************/
//
//	Plot Configurations Header
//
//	Outlines the SPXPlotConfiguration class, which describes the options
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
//
//************************************************************/

#ifndef SPXPLOTCONFIGURATION_H
#define SPXPLOTCONFIGURATION_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "SPXDataSteeringFile.h"
#include "SPXGridSteeringFile.h"
#include "SPXException.h"

const int FO_EMPTY_STYLE = -1;
const int FO_EMPTY_COLOR = -1;

const std::string focn = "SPXPlotConfiguration::";
const std::string foicn = "SPXPlotConfigurationInstance::";

struct SPXPlotConfigurationInstance {
	SPXDataSteeringFile dataSteeringFile;
	SPXGridSteeringFile gridSteeringFile;
	int markerStyle;
	int markerColor;
	int refLineStyle;
	int refLineColor;
	static bool debug;
	
	static void SetDebug(bool b) {
		debug = b;
	}
	
	void SetDefaults(void) {
		markerStyle = FO_EMPTY_STYLE;
		markerColor = FO_EMPTY_COLOR;
		refLineStyle = FO_EMPTY_STYLE;
		refLineColor = FO_EMPTY_COLOR;
	}
	
	//Returns true if ALL required fields are empty
	bool IsEmpty(void) const {
		std::string mn = "IsEmpty: ";
		
		if(!dataSteeringFile.GetFilename().empty()) {
			//if(debug) std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Data steering file = " << dataSteeringFile.GetFilename() << std::endl; 
			return false;
		}
		
		if(!gridSteeringFile.GetFilename().empty()) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Grid steering file = " << gridSteeringFile.GetFilename() << std::endl; 
			return false;
		}
		
		if(markerStyle != FO_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Marker style = " << markerStyle << std::endl; 
			return false;
		}
		
		if(markerColor != FO_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Marker color = " << markerColor << std::endl; 
			return false;
		}
		
		if(debug) std::cout << foicn << mn << "SPXPlotConfigurationInstance object is EMPTY" << std::endl;
		return true;
	}
	
	//Returns true ONLY if all required fields are valid
	bool IsValid(void) const {
		std::string mn = "IsValid: ";
		
		//Empty, but valid
		if(this->IsEmpty()) {
			return true;
		}
		
		//Not empty, but fields are missing
		if(dataSteeringFile.GetFilename().empty()) {
			if(debug) std::cout << foicn << mn << "Data Steering Filename field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}
		
		if(gridSteeringFile.GetFilename().empty()) {
			if(debug) std::cout << foicn << mn << "Grid Steering Filename field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}
		
		if(markerStyle == FO_EMPTY_STYLE) {
			if(debug) std::cout << foicn << mn << "Marker Style field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}
		
		if(markerColor == FO_EMPTY_COLOR) {
			if(debug) std::cout << foicn << mn << "Marker Color field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}
		
		//@TODO Implement the following cases of invalidity:
		
		//Data Steering File does not exist
		
		//Grid Steering File does not exist
		
		//Marker Style is invalid
		
		//Marker Color is invalid
	
		if(debug) std::cout << foicn << mn << "SPXPlotConfigurationInstance object is VALID" << std::endl;
		return true;
	}
	
	const std::string ToString(void) const {
		std::ostringstream tmp;
		
		//tmp << "\t\t\tData steering file: " << dataSteeringFile << std::endl;
		//tmp << "\t\t\tGrid steering file: " << gridSteeringFile << std::endl;
		tmp << "\t\t\tMarker Style: " << markerStyle << std::endl;
		tmp << "\t\t\tMarker Color: " << markerColor << std::endl;
		tmp << "\t\t\tReference Line Style: " << refLineStyle << std::endl;
		tmp << "\t\t\tReference Line Color: " << refLineColor << std::endl;
		
		return tmp.str();
	}
};

class SPXPlotConfiguration {

public:
	SPXPlotConfiguration () : numberOfConfigurationInstances(0) {
		SetDefaults();
	}
	
	SPXPlotConfiguration(const std::vector<std::vector<std::string> > & options, const std::string description, unsigned int numberOfConfigurationInstances);
	
	//Add an configuration instance to the configurationInstances vector
	void AddConfigurationInstance(const SPXPlotConfigurationInstance & instance) {
		std::string mn = "AddConfigurationInstance: ";
		
		if(instance.IsEmpty()) {
			throw SPXParseException("SPXPlotConfiguration::AddConfigurationInstance: Could not add configuration instance: Instance is empty");
		}
		
		if(!instance.IsValid()) {
			throw SPXParseException("SPXPlotConfiguration::AddConfigurationInstance: Could not add configuration instance: Instance is invalid");
		}
		
		//Add the instance to the instance vector
		configurationInstances.push_back(instance);
		if(debug) std::cout << focn << mn << "Successfully added a configuration instance to the instance vector" << std::endl;	
	}

	void Parse(const std::vector<std::vector<std::string> > & options);
	bool IsEmpty(void);
	bool IsValid(void);
	
	void Clear(void) {
		SetDefaults();
	}
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
		SPXPlotConfigurationInstance::SetDebug(b);
	}
	
	std::string GetDescription(void) const {
		return description;
	}
	
	unsigned int GetNumberOfConfigurationInstances(void) const {
		return configurationInstances.size();
	}
	
	const std::vector<SPXPlotConfigurationInstance> & GetPlotConfigurationInstances(void) const {
		return configurationInstances;
	}
	
	SPXPlotConfigurationInstance & GetPlotConfigurationInstance(unsigned int index) {
		std::string mn = "GetFrameOptionsInstance:";
		
		if((index + 1) > configurationInstances.size()) {
			int top = configurationInstances.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXPlotConfiguration::GetFrameOptionsInstance: Index out of range");
		}
		
		return configurationInstances.at(index);
	}

private:
	static bool debug;
	std::string description;
	unsigned int numberOfConfigurationInstances;
	std::vector<SPXPlotConfigurationInstance> configurationInstances;
	
	void SetDefaults(void) {
		std::string mn = "SetDefaults: ";
		
		description.clear();
		numberOfConfigurationInstances = 0;
		configurationInstances.clear();	
	}
};

#endif
