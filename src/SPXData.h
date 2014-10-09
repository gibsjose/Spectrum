//************************************************************/
//
//	Data Header
//
//	Outlines the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		08.10.2014
//
//************************************************************/

#ifndef SPXDATA_H
#define SPXDATA_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "SPXDataSteeringFile.h"
#include "SPXException.h"

class SPXData {

public:
	explicit SPXData (const SPXDataSteeringFile & dataSteeringFile) {
		this->dataSteeringFile = dataSteeringFile;
	}
	
	void Parse(void);
	void Print(void);

	void Draw(void);
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}
	
	const SPXDataFormat & GetDataFormat(void) {
		return dataSteeringFile.GetDataFormat();
	}

private:
	static bool debug;
	std::ifstream *dataFile;	//Must declare as pointer... ifstream's copy constructor is private
	SPXDataSteeringFile dataSteeringFile;
	std::map<std::string, std::vector<double> > data;

	void ParseSpectrumT1S(void);
	void ParseSpectrumT1A(void);
	void ParseSpectrumT2S(void);
	void ParseSpectrumT2A(void);
	void ParseHERAFitter(void);

	void OpenDataFile(void) {
		std::string filepath = dataSteeringFile.GetDataFile();

		if(filepath.empty()) {
			throw SPXFileIOException("Data Filepath is empty");
		}

		try {
			dataFile = new std::ifstream();
			dataFile->open(filepath.c_str());
			if(debug) std::cout << "SPXData::OpenDataFile: Successfully opened data file: " << filepath << std::endl;
		} catch(const std::exception &e) {
			std::cerr << e.what() << std::endl;

			throw SPXFileIOException(filepath, "Unable to open data file");
		}
	}
};

#endif
