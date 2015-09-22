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
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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

#include "SPXUtilities.h"
#include "SPXException.h"

const int PC_EMPTY_STYLE = -1;
const int PC_EMPTY_COLOR = -1;
const int PC_EMPTY_EDGE = 0;


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

	int gridMarkerStyle;
	int gridFillStyle;
	int gridMarkerColor;

	int totalFillColor;
	int totalFillStyle;
	int totalEdgeColor;
	int totalEdgeStyle;
	int totalMarkerStyle;

	int pdfFillColor;
	int pdfFillStyle;
	int pdfEdgeColor;
	int pdfEdgeStyle;
	int pdfMarkerStyle;

	int scaleFillColor;
	int scaleFillStyle;
	int scaleEdgeColor;
	int scaleEdgeStyle;
	int scaleMarkerStyle;

	int AlternativeScaleChoiceFillColor;
	int AlternativeScaleChoiceFillStyle;
	int AlternativeScaleChoiceEdgeColor;
	int AlternativeScaleChoiceEdgeStyle;
	int AlternativeScaleChoiceMarkerStyle;

	int alphasFillColor;
	int alphasFillStyle;
	int alphasEdgeColor;
	int alphasEdgeStyle;
	int alphasMarkerStyle;

	int beamuncertaintyFillColor;
	int beamuncertaintyFillStyle;
	int beamuncertaintyEdgeColor;
	int beamuncertaintyEdgeStyle;
	int beamuncertaintyMarkerStyle;

	int correctionsFillColor;
	int correctionsFillStyle;
	int correctionsEdgeColor;
	int correctionsEdgeStyle;
	int correctionsMarkerStyle;

	double xScale;
	double yScale;
	unsigned int id;

        std::vector<std::string> systematicsclasses;      // groups to display systematics
        std::vector<int>         systematicsclassescolor; // groups to display systematics fill colors
        std::vector<int>         systematicsclassesedgecolor; // groups to display systematics edge colors
        std::vector<int>         systematicsclassesedgestyle; // groups to display systematics edge style
        std::vector<int>         systematicsclassesedgewidth; // groups to display systematics edge line width

        std::vector<std::string> removesystematicsclasses;      // groups to remove systematics
        std::vector<std::string> containsystematicsclasses;     // groups to contain systematics


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

		gridMarkerStyle = PC_EMPTY_STYLE;
		gridFillStyle = 1001;
		gridMarkerColor = PC_EMPTY_COLOR;

		totalFillStyle = PC_EMPTY_STYLE;
		totalFillColor = PC_EMPTY_COLOR;
		totalEdgeStyle = PC_EMPTY_STYLE;
		totalEdgeColor = PC_EMPTY_COLOR;
		totalMarkerStyle = PC_EMPTY_STYLE;

		pdfFillStyle = PC_EMPTY_STYLE;
		pdfFillColor = PC_EMPTY_COLOR;
		pdfEdgeStyle = PC_EMPTY_STYLE;
		pdfEdgeColor = PC_EMPTY_COLOR;
		pdfMarkerStyle = PC_EMPTY_STYLE;

		scaleFillStyle = PC_EMPTY_STYLE;
		scaleFillColor = PC_EMPTY_COLOR;
		scaleEdgeStyle = PC_EMPTY_STYLE;
		scaleEdgeColor = PC_EMPTY_COLOR;
		scaleMarkerStyle = PC_EMPTY_STYLE;

		AlternativeScaleChoiceFillStyle = PC_EMPTY_STYLE;
		AlternativeScaleChoiceFillColor = PC_EMPTY_COLOR;
		AlternativeScaleChoiceEdgeStyle = PC_EMPTY_STYLE;
		AlternativeScaleChoiceEdgeColor = PC_EMPTY_COLOR;
		AlternativeScaleChoiceMarkerStyle = PC_EMPTY_STYLE;

		alphasFillStyle = PC_EMPTY_STYLE;
		alphasFillColor = PC_EMPTY_COLOR;
		alphasEdgeStyle = PC_EMPTY_STYLE;
		alphasEdgeColor = PC_EMPTY_COLOR;
		alphasMarkerStyle = PC_EMPTY_STYLE;

		beamuncertaintyFillStyle = PC_EMPTY_STYLE;
		beamuncertaintyFillColor = PC_EMPTY_COLOR;
		beamuncertaintyEdgeStyle = PC_EMPTY_STYLE;
		beamuncertaintyEdgeColor = PC_EMPTY_COLOR;
		beamuncertaintyMarkerStyle = PC_EMPTY_STYLE;

		correctionsFillStyle = PC_EMPTY_STYLE;
		correctionsFillColor = PC_EMPTY_COLOR;
		correctionsEdgeStyle = PC_EMPTY_STYLE;
		correctionsEdgeColor = PC_EMPTY_COLOR;
		correctionsMarkerStyle = PC_EMPTY_STYLE;
		xScale = 1.0;
		yScale = 1.0;

		//systematicsclasses.clear();     
		//systematicsclassescolor.clear();
		//systematicsclassesedgecolor.clear();
		//systematicsclassesedgestyle.clear(); 
		//systematicsclassesedgewidth.clear(); 

		//removesystematicsclasses.clear();
                //containsystematicsclasses.clear(); 
	}

	//Returns true if ALL required fields are empty
	bool IsEmpty(void) const {
		std::string mn = "IsEmpty: ";

		if (!dataSteeringFile.GetFilename().empty()) {
		 if (debug) std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Data steering file = " << dataSteeringFile.GetFilename() << std::endl;
		 //return false;
		}

		if (!gridSteeringFile.GetFilename().empty()) {
		  if (debug) std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Grid steering file = " << gridSteeringFile.GetFilename() << std::endl;
		  //return false;
		}

		if (!pdfSteeringFile.GetFilename().empty()) {
		  if (debug) std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: PDF steering file = " << pdfSteeringFile.GetFilename() << std::endl;
		  //return false;
		}

		if (dataSteeringFile.GetFilename().empty() && 
                  (gridSteeringFile.GetFilename().empty()||pdfSteeringFile.GetFilename().empty())) {
		  if (debug) {
		   if(dataSteeringFile.GetFilename().empty())
                    std::cout << "SPXPlotConfigurationInstance::IsEmpty: "<< " no data steering  " << std::endl;

		   if (gridSteeringFile.GetFilename().empty())
                   std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << " no grid steering  " << std::endl;

		   if (pdfSteeringFile.GetFilename().empty())
                   std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << " no PDF steering " << std::endl;
                  }
		  return false;
                }

		if (gridFillStyle != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Grid fill style = " << dataFillStyle << std::endl;
			return false;
		}

		if (gridMarkerStyle != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Grid Marker style = " << gridMarkerStyle << std::endl;
			return false;
		}

		if (gridMarkerColor != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Grid Marker color = " << dataMarkerColor << std::endl;
			return false;
		}


		if (dataMarkerStyle != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Data Marker style = " << dataMarkerStyle << std::endl;
			return false;
		}

		if (dataMarkerColor != PC_EMPTY_STYLE) {
			//std::cout << "SPXPlotConfigurationInstance::IsEmpty: " << "Not empty: Data Marker color = " << dataMarkerColor << std::endl;
			return false;
		}

		if (debug) std::cout << foicn << mn << "SPXPlotConfigurationInstance object is EMPTY" << std::endl;
		return true;
	}

	//Returns true ONLY if all required fields are valid
	bool IsValid(void) const {
		std::string mn = "IsValid: ";

		//Empty, but valid 
		if (this->IsEmpty()) {
		  return true;
		}

		//Not empty, but fields are missing
		if (dataSteeringFile.GetFilename().empty()) {
			if(debug) std::cout << mn << "Data Steering Filename field missing from PlotConfigurationInstance Object" << std::endl;
			//return false;
		}

		if (gridSteeringFile.GetFilename().empty()) {
			if(debug) std::cout << mn << "Grid Steering Filename field missing from PlotConfigurationInstance Object" << std::endl;
			//return false;
		}

		if (pdfSteeringFile.GetFilename().empty()) {
			if(debug) std::cout << mn << "PDF Steering Filename field missing from PlotConfigurationInstance Object" << std::endl;
			//return false;
		}

                if (!dataSteeringFile.GetFilename().empty() ||
                   (!(gridSteeringFile.GetFilename().empty()&&pdfSteeringFile.GetFilename().empty()))){

		 if (debug) std::cout << mn << "SPXPlotConfigurationInstance object is VALID" << std::endl;
		 return true;
                }
	}

	const std::string ToString(void) const {
		std::ostringstream tmp;

		tmp << "\t: " << dataMarkerStyle << std::endl;
		tmp << "\t Data Marker Color: " << dataMarkerColor << std::endl;

		tmp << "\t Grid Marker Style: " << gridMarkerStyle << std::endl;
		tmp << "\t Grid Fill Style: "   << gridFillStyle << std::endl;
		tmp << "\t Grid Marker Color: " << gridMarkerColor << std::endl;

		tmp << "\t Total Fill Style: "   << totalFillStyle << std::endl;
		tmp << "\t Total Fill Color: "   << totalFillColor << std::endl;
		tmp << "\t Total Edge Style: "   << totalEdgeStyle << std::endl;
		tmp << "\t Total Edge Color: "   << totalEdgeColor << std::endl;
		tmp << "\t Total Marker Style: " << totalMarkerStyle << std::endl;

		tmp << "\t PDF Fill Style: "   << pdfFillStyle << std::endl;
		tmp << "\t PDF Fill Color: "   << pdfFillColor << std::endl;
		tmp << "\t PDF Edge Style: "   << pdfEdgeStyle << std::endl;
		tmp << "\t PDF Edge Color: "   << pdfEdgeColor << std::endl;
		tmp << "\t PDF Marker Style: " << pdfMarkerStyle << std::endl;

		tmp << "\t Scale Fill Style: "   << scaleFillStyle << std::endl;
		tmp << "\t Scale Fill Color: "   << scaleFillColor << std::endl;
		tmp << "\t Scale Edge Style: "   << scaleEdgeStyle << std::endl;
		tmp << "\t Scale Edge Color: "   << scaleEdgeColor << std::endl;
		tmp << "\t Scale Marker Style: " << scaleMarkerStyle << std::endl;

		tmp << "\t AlternativeScaleChoice Fill Style: "   << AlternativeScaleChoiceFillStyle << std::endl;
		tmp << "\t AlternativeScaleChoice Fill Color: "   << AlternativeScaleChoiceFillColor << std::endl;
		tmp << "\t AlternativeScaleChoice Edge Style: "   << AlternativeScaleChoiceEdgeStyle << std::endl;
		tmp << "\t AlternativeScaleChoice Edge Color: "   << AlternativeScaleChoiceEdgeColor << std::endl;
		tmp << "\t AlternativeScaleChoice Marker Style: " << AlternativeScaleChoiceMarkerStyle << std::endl;

		tmp << "\t AlphaS Fill Style: "   << alphasFillStyle << std::endl;
		tmp << "\t AlphaS Fill Color: "   << alphasFillColor << std::endl;
		tmp << "\t AlphaS Edge Style: "   << alphasEdgeStyle << std::endl;
		tmp << "\t AlphaS Edge Color: "   << alphasEdgeColor << std::endl;
		tmp << "\t AlphaS Marker Style: " << alphasMarkerStyle << std::endl;

		tmp << "\t BeamUncertainty Fill Style: "   << beamuncertaintyFillStyle << std::endl;
		tmp << "\t BeamUncertainty Fill Color: "   << beamuncertaintyFillColor << std::endl;
		tmp << "\t BeamUncertainty Edge Style: "   << beamuncertaintyEdgeStyle << std::endl;
		tmp << "\t BeamUncertainty Edge Color: "   << beamuncertaintyEdgeColor << std::endl;
		tmp << "\t BeamUncertainty Marker Style: " << beamuncertaintyMarkerStyle << std::endl;

		tmp << "\t Corrections Fill Style: "  << correctionsFillStyle << std::endl;
		tmp << "\t Corrections Fill Color: "  << correctionsFillColor << std::endl;
		tmp << "\t Corrections Edge Style: "  << correctionsEdgeStyle << std::endl;
		tmp << "\t Corrections Edge Color: "  << correctionsEdgeColor << std::endl;
		tmp << "\t Corrections Marker Style: "<< correctionsMarkerStyle << std::endl;

		tmp << "\t X Scale: " << xScale << std::endl;
		tmp << "\t Y Scale: " << yScale << std::endl;

                for (int i=0; i<systematicsclasses.size(); i++)
		  tmp << "\t systematicsclasses["<<i<<"]: " << systematicsclasses.at(i) << std::endl;

                for (int i=0; i<systematicsclassescolor.size(); i++)
		  tmp << "\t systematicsclassescolor["<<i<<"]: " << systematicsclassescolor.at(i) << std::endl;

                for (int i=0; i<systematicsclassesedgecolor.size(); i++)
		  tmp << "\t systematicsclassesedgecolor["<<i<<"]: " << systematicsclassesedgecolor.at(i) << std::endl;

                for (int i=0; i<systematicsclassesedgestyle.size(); i++)
		  tmp << "\t systematicsclassesedgestyle["<<i<<"]: " << systematicsclassesedgestyle.at(i) << std::endl;

                for (int i=0; i<systematicsclassesedgewidth.size(); i++)
		  tmp << "\t systematicsclassesedgewidth["<<i<<"]: " << systematicsclassesedgewidth.at(i) << std::endl;

                for (int i=0; i<removesystematicsclasses.size(); i++)
		  tmp << "\t removesystematicsclasses["<<i<<"]: " << removesystematicsclasses.at(i) << std::endl;

                for (int i=0; i<containsystematicsclasses.size(); i++)
		  tmp << "\t containssystematicsclasses["<<i<<"]: " << containsystematicsclasses.at(i) << std::endl;

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
		if (debug) SPXUtilities::PrintMethodHeader(focn, mn);

		if (instance.IsEmpty()) {
		  //throw SPXParseException("SPXPlotConfiguration::AddConfigurationInstance: Could not add configuration instance: Instance is empty");
		  std::cout<<mn<<"INFO instance is EMPTY "<<std::endl;
		}

		if (!instance.IsValid()) {
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

		if (debug) std::cout << focn << mn << "Successfully added a configuration instance to the instance vector" << std::endl;
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

	  //std::cout<<"GetRatioStyle index= "<<index<<std::endl;
	  //std::cout<<"GetRatioStyle  ratio.size()= "<<ratios.size()<<std::endl;

		if(index  > ratios.size()) {
		 int top = ratios.size() - 1;
		 throw SPXOutOfRangeException(top, index, "SPXPlotConfiguration::GetRatioStyle: Index out of range");
		}
 
                if (ratios.size()==0) { 
                 return tmp;
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

		if (debug) std::cout << focn << mn << "Filename Key: [" << filename << "]" << std::endl;

		if(filenameToPCIMap.count(filename) == 0) {
			throw SPXGraphException(focn + mn + "Invalid key: filename Key: [" + filename + "]");
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

	SPXRatioStyle tmp;

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
