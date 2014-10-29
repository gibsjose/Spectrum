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

class SPXData {

public:
	explicit SPXData(const SPXPlotConfigurationInstance &pci) {
		this->pci = pci;
		this->dataFormat = pci.dataSteeringFile.GetDataFormat();
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

	unsigned int GetNumberOfIndividualSystematicErrors(void) const {
		return individualSystematicErrorNames.size();
	}

	const std::string & GetIndividualSystematicErrorName(unsigned int index) const {
		if(index > (individualSystematicErrorNames.size() - 1)) {
			throw SPXOutOfRangeException((individualSystematicErrorNames.size() - 1), index, "SPXData::GetIndividualSystematicErrorName: Index out of range");
		}

		return individualSystematicErrorNames.at(index);
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
		if(dataFormat.IsSymmetric()) {
			return data["syst"];
		} else {
			return data["sys_p"];
		}
	}

	std::vector<double> & GetPositiveSystematicErrorVector(void) {
		if(dataFormat.IsSymmetric()) {
			return data["syst"];
		} else {
			return data["syst_p"];
		}
	}

	std::vector<double> & GetNegativeSystematicErrorVector(void) {
		if(dataFormat.IsSymmetric()) {
			return data["syst"];
		} else {
			return data["syst_n"];
		}
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

private:
	static bool debug;								//Flag indicating debug mode
	std::ifstream *dataFile;						//Must declare as pointer... ifstream's copy constructor is private
	SPXPlotConfigurationInstance pci;				//Frame options instance which contains the data steering file as well as the plot options
	SPXDataFormat dataFormat;						//Format of this data: Included directly simply to cut down on syntax in implementation

	//Number of bins in data map
	unsigned int numberOfBins;

	//Stores a copy of all individual systematic error names
	// for T2S: There is a single copy of each name
	// for T2A: There are 2 copies of each name, each appended with '_p' for the positive errors
	//				and '_n' for the negative errors
	std::vector<std::string> individualSystematicErrorNames;

	//Actual data map
	std::map<std::string, std::vector<double> > data;

	//Graph for plotting statistical error bars
	TGraphAsymmErrors *statisticalErrorGraph;

	//Graph for plotting systematic error and actual point markers
	TGraphAsymmErrors *systematicErrorGraph;

	void ParseSpectrumT1S(void);
	void ParseSpectrumT1A(void);
	void ParseSpectrumT2S(void);
	void ParseSpectrumT2A(void);
	void ParseSpectrumT3S(void);
	void ParseSpectrumT3A(void);
	void ParseHERAFitter(void);

	void PrintSpectrumT1S(void);
	void PrintSpectrumT1A(void);
	void PrintSpectrumT2S(void);
	void PrintSpectrumT2A(void);
	void PrintSpectrumT3S(void);
	void PrintSpectrumT3A(void);
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
