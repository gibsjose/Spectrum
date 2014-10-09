//************************************************************/
//
//	Steering File Header
//
//	Outlines the SPXSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		19.09.2014
//
//************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "INIReader.h"

#include "SPXRatioStyle.h"
#include "SPXDisplayStyle.h"
#include "SPXOverlayStyle.h"
#include "SPXPDFBandType.h"
#include "SPXPDFErrorType.h"
#include "SPXPDFErrorSize.h"
#include "SPXFrameOptions.h"

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
	unsigned int numberOfFrames;

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
	bool labelSqrtS;				//Flag to indicate that the Sqrt(s) value should be shown in the legend
	double xLegend;					//X Position of the legend, based on top-right corner
	double yLegend;					//Y Position of the legend, based on top-right corner
	std::string ratioTitle;			//Force Ratio Section Y-Axis Label to a certain value
	SPXRatioStyle ratioStyle;		//What to plot in the ratio section
	SPXOverlayStyle overlayStyle;	//What to plot in the overlay section
	SPXDisplayStyle displayStyle;	//What to plot: overlay AND/OR ratio
	double yOverlayMin;				//Force Y-Axis minimum for overlay section
	double yOverlayMax;				//Force Y-Axis maximum for overlay section
	double yRatioMin;				//Force Y-Axis minimum for ratio section
	double yRatioMax;				//Force Y-Axis maximum for ratio section
	
	//[PDF]
	std::vector<SPXPDFSteeringFile> pdfSteeringFiles;	//PDF Steering Files
	int pdfFillStyle;				//Override PDF fill style
	int pdfFillColor;				//Override PDF fill color
	int pdfMarkerStyle;				//Override PDF marker style
	SPXPDFBandType pdfBandType;		//Override PDF band type
	SPXPDFErrorType pdfErrorType;	//Override PDF error type
	SPXPDFErrorSize pdfErrorSize;	//Override PDF error size
	
	//[FRAME_n]
	std::vector<SPXFrameOptions> frameOptions;			//Options for each Frame to be plotted
	
	void SetDefaults(void);
	unsigned int ParseNumberOfFrames(void);
	void ParseFrameOptions(unsigned int numFrames);
	
public:

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
	
	double GetXLegend(void) const {
		return this->xLegend;
	}
	
	double GetYLegend(void) const {
		return this->yLegend;
	}
	
	const std::string & GetRatioTitle(void) const {
		return this->ratioTitle;
	}
	
	const SPXRatioStyle & GetRatioStyle(void) const {
		return this->ratioStyle;
	}
	
	const SPXOverlayStyle & GetOverlayStyle(void) const {
		return this->overlayStyle;
	}
	
	const SPXDisplayStyle & GetDisplayStyle(void) const {
		return this->displayStyle;
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
	
	const std::vector<std::string> GetPDFSteeringFilepaths(void) const {
		std::vector<std::string> tmpVector;
		for(int i = 0; i < pdfSteeringFiles.size(); i++) {
			tmpVector.push_back(pdfSteeringFiles.at(i).GetFilename());
		}
		return tmpVector;
	}

	unsigned int GetNumberOfPDFSteeringFiles(void) const {
		return this->pdfSteeringFiles.size();
	}
	
	const std::vector<SPXPDFSteeringFile> & GetPDFSteeringFiles(void) const {
		return this->pdfSteeringFiles;
	}
	
	const SPXPDFSteeringFile & GetPDFSteeringFile(unsigned int index) const {
		if((index + 1) > pdfSteeringFiles.size()) {
			int top = pdfSteeringFiles.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXSteeringFile::GetPDFSteeringFile: Index out of range");
		}
		
		return pdfSteeringFiles.at(index);
	}
	
	int GetPDFFillStyle(void) const {
		return this->pdfFillStyle;
	}
	
	int GetPDFFillColor(void) const {
		return this->pdfFillColor;
	}	
	
	int GetPDFMarkerStyle(void) const {
		return this->pdfMarkerStyle;
	}
	
	const SPXPDFBandType & GetPDFBandType(void) const {
		return this->pdfBandType;
	}
	
	const SPXPDFErrorType & GetPDFErrorType(void) const {
		return this->pdfErrorType;
	}
	
	const SPXPDFErrorSize & GetPDFErrorSize(void) const {
		return this->pdfErrorSize;
	}
	
	unsigned int GetNumberOfFrames(void) const {
		return this->frameOptions.size();
	}
	
	const std::vector<SPXFrameOptions> & GetFrameOptionsVector(void) const {
		return this->frameOptions;
	}
	
	const SPXFrameOptions & GetFrameOptions(unsigned int index) const {
		
		if((index + 1) > frameOptions.size()) {
			int top = frameOptions.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXSteeringFile::GetFrameOptions: Index out of range");
		}
		
		return frameOptions.at(index);
	}

	SPXFrameOptionsInstance & GetFrameOptionsInstance(unsigned int frameOptionsIndex, unsigned int frameOptionsInstanceIndex) {
		return frameOptions.at(frameOptionsIndex).GetFrameOptionsInstance(frameOptionsInstanceIndex);
	}
	
	SPXDataSteeringFile & GetDataSteeringFile(unsigned int frameOptionsIndex, unsigned int frameOptionsInstanceIndex) {
		return frameOptions.at(frameOptionsIndex).GetFrameOptionsInstance(frameOptionsInstanceIndex).dataSteeringFile;
	}

	SPXGridSteeringFile & GetGridSteeringFile(unsigned int frameOptionsIndex, unsigned int frameOptionsInstanceIndex) {
		return frameOptions.at(frameOptionsIndex).GetFrameOptionsInstance(frameOptionsInstanceIndex).gridSteeringFile;
	}
	
};

#endif
