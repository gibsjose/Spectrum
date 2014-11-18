//************************************************************/
//
//	Data Steering File Header
//
//	Outlines the SPXDataSteeringFile class, which standardizes the
//	format and I/O of the Data portion of the
//	Steering File Format (SFF), a file format that allows for
//	interfacing between Grids, Data, and Cross Sections with
//	the MyAnalysis Program
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		06.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXDATASTEERINGFILE_H
#define SPXDATASTEERINGFILE_H

#include <iostream>

#include "INIReader.h"

#include "SPXDataFormat.h"

class SPXDataSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;

	//[DESC]
	std::string name;				//Name of Steering Data
	unsigned long year;				//Year
	std::string experiment;			//Experiment
	std::string reaction;			//Reaction name

	//[GRAPH]
	double sqrtS;					//Sqrt(s) value for dataset
	std::string legendLabel; 		//Legend label describing the data set
	std::string xLabel;				//X-Axis label
	std::string yLabel;				//Y-Axis label
	std::string xUnits;				//X-Axis units
	std::string yUnits;				//Y-Axis units
	std::string yBinWidthUnits;		//Y-Axis Bin Width Units used when normalizing, if DividedByBinWidth is true
									// NOTE: For some data the units of the data is different for the y-axis and the x-axis
									//			EG: Y-Axis is in [1/TeV], but X-Axis is GeV...
	std::string jetAlgorithmLabel;	//Jet algorithm label
	int jetAlgorithmRadius;			//Jet algorithm radius

	//[DATA]
	SPXDataFormat dataFormat;		//The format of the data: Current supported formats are: Spectrum, HERAPDF
	std::string dataFilepath;		//The data filepath
	bool dividedByBinWidth;			//Flag to indicate that data has been divided by the bin width
	bool normalizedToTotalSigma;	//Flag to indicate that cross section data is normalized to the total cross section
	bool errorInPercent;			//Flag to indicate the errors in the data file are given in percents

	void SetDefaults(void);

public:

	void Print(void);
	void Parse(void);

	SPXDataSteeringFile(void) : filename(""), debug(false) {}

	explicit SPXDataSteeringFile(std::string filename) : debug(false) {
		this->filename = filename;
	}

	const std::string & GetFilename(void) const {
		return filename;
	}

	const std::string & GetName(void) const {
		return name;
	}

	unsigned long GetYear(void) const {
		return year;
	}

	const std::string & GetReaction(void) const {
		return reaction;
	}

	double GetSqrtS(void) const {
		return sqrtS;
	}

	const std::string & GetLegendLabel(void) const {
		return legendLabel;
	}

	const std::string & GetXLabel(void) const {
		return xLabel;
	}

	const std::string & GetYLabel(void) const {
		return yLabel;
	}

	const std::string & GetXUnits(void) const {
		return xUnits;
	}

	const std::string & GetYUnits(void) const {
		return yUnits;
	}

	const std::string & GetYBinWidthUnits(void) const {
		return yBinWidthUnits;
	}

	/*
	double GetFrameXMin(void) const {
		return frameXMin;
	}

	double GetFrameYMin(void) const {
		return frameYMin;
	}
	*/

	const std::string & GetJetAlgorithmLabel(void) const {
		return jetAlgorithmLabel;
	}

	int GetJetAlgorithmRadius(void) const {
		return jetAlgorithmRadius;
	}

	const SPXDataFormat & GetDataFormat(void) const {
		return dataFormat;
	}

	const std::string GetDataFile(void) const {
		return dataFilepath;
	}

	bool IsDividedByBinWidth(void) const {
		return dividedByBinWidth;
	}

	bool IsNormalizedToTotalSigma(void) const {
		return normalizedToTotalSigma;
	}

	bool IsErrorInPercent(void) const {
		return errorInPercent;
	}
};

#endif
