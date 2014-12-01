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
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPLOTCONFIGURATION_H
#define SPXPLOTCONFIGURATION_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "SPXPlotType.h"
#include "SPXRatioStyle.h"
#include "SPXDisplayStyle.h"
#include "SPXOverlayStyle.h"

#include "SPXDataSteeringFile.h"
#include "SPXGridSteeringFile.h"
#include "SPXPDFSteeringFile.h"

#include "SPXException.h"

const int PC_EMPTY_STYLE = -1;
const int PC_EMPTY_COLOR = -1;

const std::string focn = "SPXPlotConfiguration::";
const std::string foicn = "SPXPlotConfigurationInstance::";

struct SPXPlotConfigurationInstance {
	std::string dataDirectory;
	std::string gridDirectory;
	std::string pdfDirectory;
	SPXDataSteeringFile dataSteeringFile;
	SPXGridSteeringFile gridSteeringFile;
	SPXPDFSteeringFile pdfSteeringFile;
	int dataMarkerStyle;
	int dataMarkerColor;
	int pdfFillColor;
	int pdfFillStyle;
	int pdfMarkerStyle;
	double xScale;
	double yScale;
	unsigned int id;

	static bool debug;

	static void SetDebug(bool b) {
		debug = b;
	}

	void SetDefaults(void) {
		dataDirectory = ".";
		gridDirectory = ".";
		pdfDirectory = ".";
		dataMarkerStyle = PC_EMPTY_STYLE;
		dataMarkerColor = PC_EMPTY_COLOR;
		pdfFillStyle = PC_EMPTY_STYLE;
		pdfFillColor = PC_EMPTY_COLOR;
		pdfMarkerStyle = PC_EMPTY_STYLE;
		xScale = 1.0;
		yScale = 1.0;
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

		if(!pdfSteeringFile.GetFilename().empty()) {
			return false;
		}

		if(dataMarkerStyle != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Marker style = " << dataMarkerStyle << std::endl;
			return false;
		}

		if(dataMarkerColor != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Marker color = " << dataMarkerColor << std::endl;
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

		if(pdfSteeringFile.GetFilename().empty()) {
			if(debug) std::cout << foicn << mn << "PDF Steering Filename field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}

		if(dataMarkerStyle == PC_EMPTY_STYLE) {
			if(debug) std::cout << foicn << mn << "Marker Style field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}

		if(dataMarkerColor == PC_EMPTY_COLOR) {
			if(debug) std::cout << foicn << mn << "Marker Color field missing from FrameOptionsInstance Object" << std::endl;
			return false;
		}

		if(debug) std::cout << foicn << mn << "SPXPlotConfigurationInstance object is VALID" << std::endl;
		return true;
	}

	const std::string ToString(void) const {
		std::ostringstream tmp;

		tmp << "\t Data Marker Style: " << dataMarkerStyle << std::endl;
		tmp << "\t Data Marker Color: " << dataMarkerColor << std::endl;
		tmp << "\t PDF Fill Style: " << pdfFillStyle << std::endl;
		tmp << "\t PDF Fill Color: " << pdfFillColor << std::endl;
		tmp << "\t PDF Marker Style: " << pdfMarkerStyle << std::endl;
		tmp << "\t X Scale: " << xScale << std::endl;
		tmp << "\t Y Scale: " << yScale << std::endl;
		return tmp.str();
	}

	void Print(void) const {
		std::cout << ToString() << std::endl;
	}
};

typedef std::map<std::string, SPXPlotConfigurationInstance> StringPCIMap_T;
typedef std::pair<std::string, SPXPlotConfigurationInstance> StringPCIPair_T;

class SPXPlotConfiguration {

public:
	SPXPlotConfiguration () {
		SetDefaults();
	}

	SPXPlotConfiguration(std::map<std::string, std::vector<std::string> > & options, unsigned int id);

	//Add an configuration instance to the configurationInstances vector
	void AddConfigurationInstance(SPXPlotConfigurationInstance & instance) {
		std::string mn = "AddConfigurationInstance: ";

		if(instance.IsEmpty()) {
			throw SPXParseException("SPXPlotConfiguration::AddConfigurationInstance: Could not add configuration instance: Instance is empty");
		}

		if(!instance.IsValid()) {
			throw SPXParseException("SPXPlotConfiguration::AddConfigurationInstance: Could not add configuration instance: Instance is invalid");
		}

		//Add the instance to the instance vector
		configurationInstances.push_back(instance);

		//Add the pairs of filenames with the PCI to the map (this is used to look up PCI's by the corresponding filename)
		std::string pdfFilename = instance.pdfSteeringFile.GetFilename();
		std::string dataFilename = instance.dataSteeringFile.GetFilename();
		SPXPlotConfigurationInstance pci = instance;

		StringPCIPair_T dataPair(dataFilename, instance);
		StringPCIPair_T pdfPair(pdfFilename, instance);

		filenameToPCIMap.insert(dataPair);
		filenameToPCIMap.insert(pdfPair);

		if(debug) std::cout << focn << mn << "Successfully added a configuration instance to the instance vector" << std::endl;
	}

	void Parse(std::map<std::string, std::vector<std::string> > & options);
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

	unsigned int GetID(void) const {
		return id;
	}

	SPXPlotType & GetPlotType(void) {
		return plotType;
	}

	std::string & GetDescription(void) {
		return description;
	}

	SPXDisplayStyle & GetDisplayStyle(void) {
		return displayStyle;
	}

	SPXOverlayStyle & GetOverlayStyle(void) {
		return overlayStyle;
	}

	std::string & GetRatioTitle(void) {
		return ratioTitle;
	}

	unsigned int GetNumberOfRatios(void) const {
		return ratios.size();
	}

	SPXRatioStyle & GetRatioStyle(unsigned int index) {
		if((index + 1) > ratios.size()) {
			int top = ratios.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXPlotConfiguration::GetRatioStyle: Index out of range");
		}

		return ratioStyles.at(index);
	}

	std::string & GetRatio(unsigned int index) {
		if((index + 1) > ratios.size()) {
			int top = ratios.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXPlotConfiguration::GetRatio: Index out of range");
		}

		return ratios.at(index);
	}

	bool IsXLog(void) const {
		return xLog;
	}

	bool IsYLog(void) const {
		return yLog;
	}

	unsigned int GetNumberOfConfigurationInstances(void) const {
		return configurationInstances.size();
	}

 	std::vector<SPXPlotConfigurationInstance> & GetPlotConfigurationInstances(void) {
		return configurationInstances;
	}

	SPXPlotConfigurationInstance & GetPlotConfigurationInstance(unsigned int index) {
		std::string mn = "GetPlotConfigurationInstance: ";

		if((index + 1) > configurationInstances.size()) {
			int top = configurationInstances.size() - 1;
			throw SPXOutOfRangeException(top, index, focn + mn + "Index out of range");
		}

		return configurationInstances.at(index);
	}

	SPXPlotConfigurationInstance & GetPlotConfigurationInstance(std::string filename) {
		std::string mn = "GetPlotConfigurationInstance: ";

		if(debug) std::cout << focn << mn << "Filename Key: [" << filename << "]" << std::endl;

		if(filenameToPCIMap.count(filename) == 0) {
			throw SPXGraphException(focn + mn + "Invalid key: filenameToPCIMap[" + filename + "]");
		}

		return filenameToPCIMap[filename];
	}

private:
	static bool debug;
	unsigned int id;
	bool xLog;
	bool yLog;
	std::string description;
	SPXPlotType plotType;
	SPXDisplayStyle displayStyle;
	SPXOverlayStyle overlayStyle;
	std::string ratioTitle;
	std::vector<SPXRatioStyle> ratioStyles;
	std::vector<std::string> ratios;

	std::vector<SPXPlotConfigurationInstance> configurationInstances;

	StringPCIMap_T filenameToPCIMap;

	void SetDefaults(void) {
		std::string mn = "SetDefaults: ";

		xLog = false;
		yLog = false;
		description.clear();
		ratioTitle.clear();
		ratioStyles.clear();
		ratios.clear();
		configurationInstances.clear();
	}
};

#endif
