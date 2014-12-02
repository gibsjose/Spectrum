//************************************************************/
//
//	Data Header
//
//	Outlines the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXDATA_H
#define SPXDATA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>

#include "SPXROOT.h"
#include "TMatrixTLazy.h"
#include "TMatrixT.h"

#include "SPXPlotConfiguration.h"
#include "SPXException.h"
#include "SPXUtilities.h"

class SPXData {

public:
	explicit SPXData(const SPXPlotConfigurationInstance &pci) {
		this->pci = pci;
		this->dataFormat = pci.dataSteeringFile.GetDataFormat();
		dividedByBinWidth = this->pci.dataSteeringFile.IsDividedByBinWidth();
		std::cout << ">>>>>>" << pci.dataDirectory << std::endl;
		std::cout << pci.dataSteeringFile.GetFilename() << std::endl;
	}

	void Parse(void);
	void Print(void);
	void Draw(void);

	//CreateGraphs should always be called BEFORE Get_____Graph() is called
	void CreateGraphs(void);

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	const SPXDataFormat & GetDataFormat(void) {
		return pci.dataSteeringFile.GetDataFormat();
	}

	unsigned int GetNumberOfBins(void) {
		return numberOfBins;
	}

	bool IsDividedByBinWidth(void) const {
		return dividedByBinWidth;
	}

	unsigned int GetNumberOfIndividualSystematicErrors(void) const {
		return individualSystematics.size();
	}

	std::vector<double> GetDataColumn(const std::string &column) {
		return data[column];
	}

	std::map<std::string, std::vector<double> > & GetDataMap(void) {
		return data;
	}

	std::vector<double> & GetXmVector(void) {
		return data["xm"];
	}

	std::vector<double> & GetXlowVector(void) {
		return data["xlow"];
	}

	std::vector<double> & GetXhighVector(void) {
		return data["xhigh"];
	}

	std::vector<double> & GetSigmaVector(void) {
		return data["sigma"];
	}

	std::vector<double> & GetStatisticalErrorVector(void) {
		return data["stat"];
	}

	//NOTE: Returns only the POSITIVE vector for asymmetric formats
	std::vector<double> & GetSystematicErrorVector(void) {
		return data["syst_p"];
	}

	std::vector<double> & GetPositiveSystematicErrorVector(void) {
		return data["syst_p"];
	}

	std::vector<double> & GetNegativeSystematicErrorVector(void) {
		return data["syst_n"];
	}

	TGraphAsymmErrors * GetStatisticalErrorGraph(void) {

		if(!this->statisticalErrorGraph) {
			throw SPXGraphException("Statistical Error Graph pointer is NULL: You MUST call ::CreateGraphs() before obtaining the graph");
		}

		return this->statisticalErrorGraph;
	}

	TGraphAsymmErrors * GetSystematicErrorGraph(void) {

		if(!this->statisticalErrorGraph) {
			throw SPXGraphException("Systematic Error Graph pointer is NULL: You MUST call ::CreateGraphs() before obtaining the graph");
		}

		return this->systematicErrorGraph;
	}

	TGraphAsymmErrors * GetTotalErrorGraph(void) {

		if(!this->totalErrorGraph) {
			throw SPXGraphException("Total Error Graph pointer is NULL: You MUST call ::CreateGraphs() before obtaining the graph");
		}

		return this->totalErrorGraph;
	}

private:
	static bool debug;								//Flag indicating debug mode
	std::ifstream *dataFile;						//Must declare as pointer... ifstream's copy constructor is private
	SPXPlotConfigurationInstance pci;				//Frame options instance which contains the data steering file as well as the plot options
	SPXDataFormat dataFormat;						//Format of this data: Included directly simply to cut down on syntax in implementation
	bool dividedByBinWidth;							//Flag indicating whether the initial data was already divided by the bin width

	//Number of bins in data map
	unsigned int numberOfBins;

	//Individual Systematic Errors
	StringDoubleVectorMap_T individualSystematics;

	//Actual data map
	StringDoubleVectorMap_T data;

	//Graph for plotting statistical error ticks
	TGraphAsymmErrors *statisticalErrorGraph;

	//Graph containing systematic errors
	TGraphAsymmErrors *systematicErrorGraph;

	//Actual data graph containing total errors
	TGraphAsymmErrors *totalErrorGraph;

	void ParseSpectrum(void);
	void ParseHERAFitter(void);

	void PrintSpectrum(void);
	void PrintHERAFitter(void);

	void OpenDataFile(void) {
		std::string filepath = pci.dataSteeringFile.GetDataFile();

		if(filepath.empty()) {
			throw SPXFileIOException("Data Filepath is empty");
		}

		try {
			dataFile = new std::ifstream();
			dataFile->open(filepath.c_str());

			if(*dataFile) {
				if(debug) std::cout << "SPXData::OpenDataFile: Successfully opened data file: " << filepath << std::endl;
			} else {
				throw SPXFileIOException(filepath, "Unable to open data file");
			}
		} catch(const std::exception &e) {
			std::cerr << e.what() << std::endl;

			throw SPXFileIOException(filepath, "Unable to open data file");
		}
	}

	void CheckVectorSize(const std::vector<double> & vector, const std::string & name, unsigned int masterSize) {
		if(vector.size() != masterSize) {
			std::ostringstream oss;
			oss << "Size error: \"" << name << "\" vector has different size (" << vector.size() << ") than master size (" << masterSize << ")" << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), oss.str());
		} else {
			if(debug) std::cout << "SPXData::" << "CheckVectorSize: " << "\t -->  Success: \"" << name << "\" vector size matches master size" << std::endl;
		}
	}
};

#endif
