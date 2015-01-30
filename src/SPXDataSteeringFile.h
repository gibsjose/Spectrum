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

#include "SPXUtilities.h"

class SPXDataSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;

	//[DESC]
	std::string name;					//Name of Steering Data
	std::string comments;				//Data comments
	std::string experiment;				//Experiment
	std::string reaction;				//Reaction name
	std::string datasetYear;			//Year of dataset
	std::string datasetLumi;			//Dataset luminosity string
	std::string referenceJournalName;	//Name of journal
	std::string referenceJournalYear;	//Year of publication in journal
	std::string referenceArXivNumber;	//ID number of arXiv
	std::string referenceArXivYear;		//Year of arXiv
	std::string publicationStatus;		//Status of publication

	//[GRAPH]
	double sqrtS;				//Sqrt(s) value for dataset
	std::string legendLabel; 		//Legend label describing the data set
	std::string xLabel;		        //X-Axis label
	std::string yLabel;			//Y-Axis label
	std::string xUnits;			//X-Axis units
	std::string yUnits;			//Y-Axis units
	std::string yBinWidthUnits;		//Y-Axis Bin Width Units used when normalizing, if DividedByBinWidth is true
	// NOTE: For some data the units of the data is different for the y-axis and the x-axis
	//			EG: Y-Axis is in [1/TeV], but X-Axis is GeV...
	std::string jetAlgorithmLabel;	        //Jet algorithm label
	int jetAlgorithmRadius;			//Jet algorithm radius

	//[DATA]
	SPXDataFormat dataFormat;		//The format of the data: Current supported formats are: Spectrum, HERAPDF
	std::string dataFilepath;		//The data filepath
        std::string corrtotalfilename;          // name of correlation file
        std::string corrstatfilename;           // name of correlation file

	bool dividedByBinWidth;			//Flag to indicate that data has been divided by the bin width
	bool normalizedToTotalSigma;	//Flag to indicate that cross section data is normalized to the total cross section
	bool errorInPercent;			//Flag to indicate the errors in the data file are given in percents
	double lumiScaleFactor;			//Luminosity scale factor: Default to 1.0

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

	const std::string & GetComments(void) const {
		return comments;
	}

	const std::string & GetExperiment(void) const {
		return experiment;
	}

	const std::string & GetReaction(void) const {
		return reaction;
	}

	const std::string & GetDatasetYear(void) const {
		return datasetYear;
	}

	const std::string & GetDatasetLumi(void) const {
		return datasetLumi;
	}

	const std::string & GetReferenceJournalName(void) const {
		return referenceJournalName;
	}

	const std::string & GetReferenceJournalYear(void) const {
		return referenceJournalYear;
	}

	const std::string & GetReferenceArXivNumber(void) const {
		return referenceArXivNumber;
	}

	const std::string & GetReferenceArXivYear(void) const {
		return referenceArXivYear;
	}

	const std::string & GetPublicationStatus(void) const {
		return publicationStatus;
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

	const std::string & GetJetAlgorithmLabel(void) const {
		return jetAlgorithmLabel;
	}


	int GetJetAlgorithmRadius(void) const {
		return jetAlgorithmRadius;
	}

	const SPXDataFormat & GetDataFormat(void) const {
		return dataFormat;
	}


	const std::string & GetDataFile(void) const {
		return dataFilepath;
	}

	void SetDataFile(std::string & filepath) {
		dataFilepath = filepath;
	}

	void PrependDataFile(std::string & path) {
		dataFilepath = path + "/" + dataFilepath;
	}


	const std::string & GetTotalCorrellationFileName(void) const {
		return corrtotalfilename;
	}

	void SetTotalCorrelationFileName(std::string & filepath) {
		corrtotalfilename = filepath;
	}

	void PrependTotalCorrelationFileName(std::string & path) {
		corrtotalfilename = path + "/" + corrtotalfilename;
	}

	const std::string & GetStatCorrellationFileName(void) const {
		return corrstatfilename;
	}

	void SetStatCorrelationFileName(std::string & filepath) {
		corrstatfilename = filepath;
	}

	void PrependStatCorrelationFileName(std::string & path) {
		corrstatfilename = path + "/" + corrstatfilename;
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

	double GetLumiScaleFactor(void) const {
		return lumiScaleFactor;
	}
};

#endif
