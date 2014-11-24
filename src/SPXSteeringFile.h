//************************************************************/
//
//	Steering File Header
//
//	Outlines the SPXSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		19.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "INIReader.h"

#include "SPXPlotType.h"
#include "SPXRatioStyle.h"
#include "SPXDisplayStyle.h"
#include "SPXOverlayStyle.h"
#include "SPXPlotConfiguration.h"

#include "SPXPDFSteeringFile.h"
#include "SPXDataSteeringFile.h"
#include "SPXGridSteeringFile.h"

#include "SPXException.h"

#ifndef SPXSTEERINGFILE_H
#define SPXSTEERINGFILE_H

const int STYLE_EMPTY = -1;
const int COLOR_EMPTY = -1;

class SPXSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;						//Flag to indicate debug mode
	std::string pdfDirectory;		//Directory prepended to PDF steering files
	std::string dataDirectory;		//Directory prepended to data steering files
	std::string gridDirectory;		//Directory prepended to grid steering files

	//[GRAPH]
	bool plotBand;					//Flag to indicate that error bands should be plotted
	bool plotErrorTicks;			//Flag to indicate that tick marks should be used at the end of error bars
	bool plotMarker;				//Flag to indicate that data point markers should be plotted
	bool plotStaggered;				//Flag to indicate that points should be staggered horizontally for readability
	bool matchBinning;				//Flag to indicate that the binning of all graphs on the plot should be matched
	bool labelSqrtS;				//Flag to indicate that the Sqrt(s) value should be shown in the legend
	double xLegend;					//X Position of the legend, based on top-right corner
	double yLegend;					//Y Position of the legend, based on top-right corner
	double yOverlayMin;				//Force Y-Axis minimum for overlay section
	double yOverlayMax;				//Force Y-Axis maximum for overlay section
	double yRatioMin;				//Force Y-Axis minimum for ratio section
	double yRatioMax;				//Force Y-Axis maximum for ratio section

	//[FRAME_n]
	std::vector<SPXPlotConfiguration> plotConfigurations;	//Options for each plot

	void SetDefaults(void);
	unsigned int ParseNumberOfPlots(void);
	unsigned int ParseNumberOfRatios(unsigned int plotNumber);
	void ParsePlotConfigurations();

public:

	//These functions simplify the main file's interface to parsing
	// and printing the Steering Files
	void PrintAll(void);
	void ParseAll(bool print);

	void Print(void);
	void PrintPDFSteeringFiles(void);
	void PrintDataSteeringFiles(void);
	void PrintGridSteeringFiles(void);

	void Parse(void);
	void ParsePDFSteeringFiles(void);
	void ParseDataSteeringFiles(void);
	void ParseGridSteeringFiles(void);

	explicit SPXSteeringFile(const std::string &filename) : debug(false){
		//Set filename
		this->filename = filename;
	}

	const std::string & GetFilename(void) const {
		return this->filename;
	}

	void SetDebug(bool b) {
		debug = b;
	}

	bool GetDebug(void) const {
		return this->debug;
	}

	const std::string & GetPDFDirectory(void) const {
		return this->pdfDirectory;
	}

	const std::string & GetDataDirectory(void) const {
		return this->dataDirectory;
	}

	const std::string & GetGridDirectory(void) const {
		return this->gridDirectory;
	}

	bool GetPlotBand(void) const {
		return this->plotBand;
	}

	bool GetPlotErrorTicks(void) const {
		return this->plotErrorTicks;
	}

	bool GetPlotMarker(void) const {
		return this->plotMarker;
	}

	bool GetPlotStaggered(void) const {
		return this->plotStaggered;
	}

	bool GetMatchBinning(void) const {
		return this->matchBinning;
	}

	bool GetLabelSqrtS(void) const {
		return this->labelSqrtS;
	}

	double GetXLegend(void) const {
		return this->xLegend;
	}

	double GetYLegend(void) const {
		return this->yLegend;
	}

	double GetYOverlayMin(void) const {
		return this->yOverlayMin;
	}

	double GetYOverlayMax(void) const {
		return this->yOverlayMax;
	}

	double GetYRatioMin(void) const {
		return this->yRatioMin;
	}

	double GetYRatioMax(void) const {
		return this->yRatioMax;
	}

	unsigned int GetNumberOfPlotConfigurations(void) const {
		return this->plotConfigurations.size();
	}

	std::vector<SPXPlotConfiguration> & GetPlotConfigurationsVector(void) {
		return this->plotConfigurations;
	}

	SPXPlotConfiguration & GetPlotConfiguration(unsigned int index) {

		if((index + 1) > plotConfigurations.size()) {
			int top = plotConfigurations.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXSteeringFile::GetPlotConfiguration: Index out of range");
		}

		return plotConfigurations.at(index);
	}

	unsigned int GetNumberOfConfigurationInstances(unsigned int index) {
		return plotConfigurations.at(index).GetNumberOfConfigurationInstances();
	}

	SPXPlotConfigurationInstance & GetPlotConfigurationInstance(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii);
	}

	SPXDataSteeringFile & GetDataSteeringFile(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii).dataSteeringFile;
	}

	SPXGridSteeringFile & GetGridSteeringFile(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii).gridSteeringFile;
	}

	SPXPDFSteeringFile & GetPDFSteeringFile(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii).pdfSteeringFile;
	}

};

#endif
