//************************************************************/
//
//	Frame Options Header
//
//	Outlines the SPXFrameOptions class, which describes the options
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

#ifndef SPXFRAMEOPTIONS_H
#define SPXFRAMEOPTIONS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "SPXDataSteeringFile.h"
#include "SPXGridSteeringFile.h"
#include "SPXException.h"

const int FO_EMPTY_STYLE = -1;
const int FO_EMPTY_COLOR = -1;

const std::string focn = "SPXFrameOptions::";
const std::string foicn = "SPXFrameOptionsInstance::";

struct SPXFrameOptionsInstance {
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
			//if(debug) std::cout << "SPXFrameOptionsInstance::IsEmpty: " << "Not empty: Data steering file = " << dataSteeringFile.GetFilename() << std::endl; 
			return false;
		}
		
		if(!gridSteeringFile.GetFilename().empty()) {
			//std::cout << "SPXFrameOptionsInstance::IsEmpty: " << "Not empty: Grid steering file = " << gridSteeringFile.GetFilename() << std::endl; 
			return false;
		}
		
		if(markerStyle != FO_EMPTY_STYLE) {
			//std::cout << "SPXFrameOptionsInstance::IsEmpty: " << "Not empty: Marker style = " << markerStyle << std::endl; 
			return false;
		}
		
		if(markerColor != FO_EMPTY_STYLE) {
			//std::cout << "SPXFrameOptionsInstance::IsEmpty: " << "Not empty: Marker color = " << markerColor << std::endl; 
			return false;
		}
		
		if(debug) std::cout << foicn << mn << "SPXFrameOptionsInstance object is EMPTY" << std::endl;
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
	
		if(debug) std::cout << foicn << mn << "SPXFrameOptionsInstance object is VALID" << std::endl;
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

class SPXFrameOptions {

public:
	SPXFrameOptions () : numberOfOptionsInstances(0) {
		SetDefaults();
	}
	
	SPXFrameOptions(const std::vector<std::vector<std::string> > & options, const std::string description, unsigned int numberOfOptionsInstances);
	
	//Add an options instance to the optionsInstances vector
	void AddOptionsInstance(const SPXFrameOptionsInstance & instance) {
		std::string mn = "AddOptionsInstance: ";
		
		if(instance.IsEmpty()) {
			throw SPXParseException("SPXFrameOptions::AddOptionsInstance: Could not add options instance: Instance is empty");
		}
		
		if(!instance.IsValid()) {
			throw SPXParseException("SPXFrameOptions::AddOptionsInstance: Could not add options instance: Instance is invalid");
		}
		
		//Add the instance to the instance vector
		optionsInstances.push_back(instance);
		if(debug) std::cout << focn << mn << "Successfully added an options instance to the instance vector" << std::endl;	
	}

	void Parse(const std::vector<std::vector<std::string> > & options);
	//void Print(void);
	//const std::string & ToString(void) const;
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
		SPXFrameOptionsInstance::SetDebug(b);
	}
	
	std::string GetDescription(void) const {
		return description;
	}
	
	unsigned int GetNumberOfOptionsInstances(void) const {
		return optionsInstances.size();
	}
	
	const std::vector<SPXFrameOptionsInstance> & GetFrameOptionsInstances(void) const {
		return optionsInstances;
	}
	
	SPXFrameOptionsInstance & GetFrameOptionsInstance(unsigned int index) {
		std::string mn = "GetFrameOptionsInstance:";
		
		if((index + 1) > optionsInstances.size()) {
			int top = optionsInstances.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXFrameOptions::GetFrameOptionsInstance: Index out of range");
		}
		
		return optionsInstances.at(index);
	}

private:
	static bool debug;
	std::string description;
	unsigned int numberOfOptionsInstances;
	std::vector<SPXFrameOptionsInstance> optionsInstances;
	
	void SetDefaults(void) {
		std::string mn = "SetDefaults: ";
		
		description.clear();
		numberOfOptionsInstances = 0;
		optionsInstances.clear();	
	}
};

#endif
