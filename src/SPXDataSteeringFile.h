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
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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

	//[GEN]
	bool debug;

	std::string filename;                   // name of steering file

	//[DESC]
	std::string name;			// Name of Steering Data
	std::string comments;			// Data comments
	std::string experiment;			// Experiment
	std::string reaction;			// Reaction name
	std::string datasetYear;		// Year of dataset
	std::string datasetLumilabel;		// Dataset luminosity label string 
	std::string referenceJournalName;	// Name of journal
	std::string referenceJournalYear;	// Year of publication in journal
	std::string referenceArXivNumber;	// ID number of arXiv
	std::string referenceArXivYear;		// Year of arXiv
	std::string publicationStatus;		// Status of publication

	//[GRAPH]
	double sqrtS;				// Sqrt(s) value for dataset
	std::string legendLabel; 		// Legend label describing the data set
	std::string xLabel;		        // X-Axis label
	std::string yLabel;			// Y-Axis label
	std::string xUnits;			// X-Axis units
	std::string yUnits;			// Y-Axis units
	std::string yBinWidthUnits;		// Y-Axis Bin Width Units used when normalizing, if DividedByBinWidth is true
	// NOTE: For some data the units of the data is different for the y-axis and the x-axis
	//			EG: Y-Axis is in [1/TeV], but X-Axis is GeV...
	std::string jetAlgorithmLabel;	        // Jet algorithm label
	int jetAlgorithmRadius;			// Jet algorithm radius

        std::string doublediffBinname;          // name of double differential bin variable
        double doublediffBinValueMin;           // minimum value of double differential variable
        double doublediffBinValueMax;           // maximum value of double differential variable
        double doublediffBinWidth;              // bin width of double differential variable

	//[DATA]
	SPXDataFormat dataFormat;		// The format of the data: Current supported formats are: Spectrum, HERAPDF
	std::string dataFilepath;		// The data filepath
        std::string corrtotalfilename;          // name of correlation file
        std::string corrstatfilename;           // name of correlation file

        bool dividedByDoubleDiffBinWidth;       // Flag to indicate if data are divided by double differential bin width 
	bool dividedByBinWidth;			// Flag to indicate that data has been divided by the bin width
	bool normalizedToTotalSigma;	        // Flag to indicate that cross section data is normalized to the total cross section
	bool errorInPercent;			// Flag to indicate the errors in the data file are given in percents
	double lumiScaleFactor;			// Luminosity scale factor: Default to 1.0
	double lumiValue;			// Luminosity value:      
	double lumiError;			// Luminosity uncertainty 

        bool addLumiSystematic;                 // Add luminosity uncertainty as additional systematic components 

        bool RemoveXbins;                       // if ON points below/above DataCutXmin/DataCutXmax are removed
        double DataCutXmin;                     // Value below which data points are removed if  RemoveXbins=true
        double DataCutXmax;                     // Value above which data points are removed if  RemoveXbins=true

	void SetDefaults(void);

        std::vector<std::string> SystematicsUncorrelatedBetweenBins; // list of systematics that are uncorrelated between bins

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
		return datasetLumilabel;
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

	const std::string & GetDoubleBinVariableName(void) const {
		return doublediffBinname;
	}

	double GetDoubleBinValueMin(void) const {
		return doublediffBinValueMin;
	}

	double GetDoubleBinValueMax(void) const {
		return doublediffBinValueMax;
	}

	bool IsDividedByDoubleDiffBinWidth(void) const {
		return dividedByDoubleDiffBinWidth;
	}

	double GetDoubleBinValueWidth(void) const {
		return doublediffBinWidth;
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

	bool AddLuminosityUncertainyToSystematics(void) const {
		return addLumiSystematic;
	}

	double GetLumiScaleFactor(void) const {
		return lumiScaleFactor;
	}

	double GetDatasetValue(void) const {
		return lumiValue;
	}

	double GetDatasetLumiUncertainty(void) const {
		return lumiError;
	}

        double GetDataCutXmin(){
	 if (!RemoveXbins) std::cout<<"SPXDataSteeringFile::GetDataCutXmin:: WARNING: RemoveXbin flag is not ON "<<std::cout;
	 return DataCutXmin;
        }

        double GetDataCutXmax(){
	 if (!RemoveXbins) std::cout<<"SPXDataSteeringFile::GetDataCutXmin:: WARNING: RemoveXbin flag is not ON "<<std::cout;
	 return DataCutXmax;
        }

        void SetDataCutXmin(double xmin){
          RemoveXbins=true;
          DataCutXmin=xmin;
	  return;
        }

        void SetDataCutXmax(double xmax){
          RemoveXbins=true;
          DataCutXmax=xmax;
	  return;
        }

        bool GetDataRemoveXbinsFlag() {
         return RemoveXbins;
        }

        std::vector<std::string> GetUncertaintyCorrelationTypeVector(){
         return SystematicsUncorrelatedBetweenBins; 
        }

};

#endif
