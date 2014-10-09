//************************************************************/
//
//	Data Implementation
//
//	Implements the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		08.10.2014
//
//************************************************************/

#include "SPXData.h"

//Class name for debug statements
const std::string cn = "SPXData::";

//Must define the static debug variable in the implementation
bool SPXData::debug;

//Public helper method to parse the data file based on the type
void SPXData::Parse(void) {
	std::string mn = "Parse: ";
	
	if(debug) std::cout << cn << mn << "Parsing data file: " << dataSteeringFile.GetDataFile() << std::endl;

	try {
		OpenDataFile();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXFileIOException(cn + mn + "Unable to open data file");
	}

	SPXDataFormat df = dataSteeringFile.GetDataFormat();

	if(df.IsSpectrumT1S()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;

		try {
			ParseSpectrumT1S();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(df.IsSpectrumT1A()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;

		try {
			ParseSpectrumT1A();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(df.IsSpectrumT2S()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;

		try {
			ParseSpectrumT2S();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(df.IsSpectrumT2A()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;

		try {
			ParseSpectrumT2A();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(df.IsHERAFitter()) {
		if(debug) std::cout << cn << mn << "Data format is " << df.ToString() << std::endl;

		try {
			ParseHERAFitter();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else {
		throw SPXParseException("DataSteeringFile " + dataSteeringFile.GetFilename() + " has invalid data format");
	}
}

void SPXData::ParseSpectrumT1S(void) {
	std::string mn = "ParseSpectrumT1S: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << dataSteeringFile.GetDataFile() << std::endl;

	if(!(*dataFile)) {
		throw SPXFileIOException("Something went awry with the dataFile ifstream...");
	}

	std::string line;
	double xm_t, xlow_t, xhigh_t, sigma_t, stat_t, syst_t;
	std::vector<double> xm;				//Mean x
	std::vector<double> xlow;			//X min
	std::vector<double> xhigh;			//X max
	std::vector<double> sigma;			//Sigma (cross-section)
	std::vector<double> stat;			//Statistical error
	std::vector<double> syst;			//Total systematic error

	while(dataFile->good()) {
		std::getline(*dataFile, line);
		
		//String stream to parse the individual lines
		std::istringstream iss(line);

		if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

		//Skip empty lines
		if(line.empty()) {
			continue;
		}

		//Skip comments
		if(!line.empty() && (line[0] == ';')) {
			continue;
		} else if(!line.empty()) {

			//Parse each line into temp variables
			iss >> xm_t >> xlow_t >> xhigh_t >> sigma_t >> stat_t >> syst_t;

			//Fill vectors with temp variables
			xm.push_back(xm_t);
			xlow.push_back(xlow_t);
			xhigh.push_back(xhigh_t);
			sigma.push_back(sigma_t);
			stat.push_back(stat_t);
			syst.push_back(syst_t);
		}
	}

	//Check vector sizes: all vectors should be the same size
	int masterSize = xm.size();
	if(debug) std::cout << cn << mn << "Master size set to size of \"xm\" vector: " << masterSize << std::endl;

	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors.." << std::endl;

	try {
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat, "stat", masterSize);
		CheckVectorSize(syst, "syst", masterSize);
	} catch(const SPXException &e) {
		throw;
	}

	//All vectors passed size check
	if(debug) std::cout << cn << mn << "Success: All vector sizes match master size" << std::endl;

	//Set numberOfBins based on master size
	if(debug) std::cout << cn << mn << "Setting numberOfBins to match master size" << std::endl;
	numberOfBins = masterSize;
	if(debug) std::cout << cn << mn << "numberOfBins = " << numberOfBins << std::endl;

	//Add to data map
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst", syst));

	//Print formatted table
	if(debug) {
		PrintSpectrumT1S();
	}
}

void SPXData::ParseSpectrumT1A(void) {

	std::string mn = "ParseSpectrumT1A: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << dataSteeringFile.GetDataFile() << std::endl;

	if(!(*dataFile)) {
		throw SPXFileIOException("Something went awry with the dataFile ifstream...");
	}

	std::string line;
	double xm_t, xlow_t, xhigh_t, sigma_t, stat_t, syst_p_t, syst_n_t;
	std::vector<double> xm;				//Mean x
	std::vector<double> xlow;			//X min
	std::vector<double> xhigh;			//X max
	std::vector<double> sigma;			//Sigma (cross-section)
	std::vector<double> stat;			//Statistical error
	std::vector<double> syst_p;			//Total systematic error (+)
	std::vector<double> syst_n;			//Total systematic error (-)

	while(dataFile->good()) {
		std::getline(*dataFile, line);
		
		//String stream to parse the individual lines
		std::istringstream iss(line);

		if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

		//Skip empty lines
		if(line.empty()) {
			continue;
		}

		//Skip comments
		if(!line.empty() && (line[0] == ';')) {
			continue;
		} else if(!line.empty()) {

			//Parse each line into temp variables
			iss >> xm_t >> xlow_t >> xhigh_t >> sigma_t >> stat_t >> syst_p_t >> syst_n_t;

			//Fill vectors with temp variables
			xm.push_back(xm_t);
			xlow.push_back(xlow_t);
			xhigh.push_back(xhigh_t);
			sigma.push_back(sigma_t);
			stat.push_back(stat_t);
			syst_p.push_back(syst_p_t);
			syst_n.push_back(syst_n_t);
		}
	}

	//Check vector sizes: all vectors should be the same size
	int masterSize = xm.size();
	if(debug) std::cout << cn << mn << "Master size set to size of \"xm\" vector: " << masterSize << std::endl;

	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors.." << std::endl;

	try {
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat, "stat", masterSize);
		CheckVectorSize(syst_p, "syst_p", masterSize);
		CheckVectorSize(syst_n, "syst_n", masterSize);
	} catch(const SPXException &e) {
		throw;
	}

	//All vectors passed size check
	if(debug) std::cout << cn << mn << "Success: All vector sizes match master size" << std::endl;

	//Set numberOfBins based on master size
	if(debug) std::cout << cn << mn << "Setting numberOfBins to match master size" << std::endl;
	numberOfBins = masterSize;
	if(debug) std::cout << cn << mn << "numberOfBins = " << numberOfBins << std::endl;

	//Add to data map
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst_p", syst_p));
	data.insert(std::pair<std::string, std::vector<double> >("syst_n", syst_n));

	//Print formatted table
	if(debug) {
		PrintSpectrumT1A();
	}
}

void SPXData::ParseSpectrumT2S(void) {
	std::string mn = "ParseSpectrumT2S: ";
}

void SPXData::ParseSpectrumT2A(void) {
	std::string mn = "ParseSpectrumT2A: ";
}

void SPXData::ParseHERAFitter(void) {
	std::string mn = "ParseHERAFitter: ";
}

//Helper method to choose correct print method based on data format
void SPXData::Print(void) {

	SPXDataFormat df = dataSteeringFile.GetDataFormat();

	if(df.IsSpectrumT1S()) {
		PrintSpectrumT1S();
	} else if(df.IsSpectrumT1A()) {
		PrintSpectrumT1A();
	} else if(df.IsSpectrumT2S()) {
		PrintSpectrumT2S();
	} else if(df.IsSpectrumT2A()) {
		PrintSpectrumT2A();
	} else if(df.IsHERAFitter()) {
		PrintHERAFitter();
	}
}

void SPXData::PrintSpectrumT1S(void) {

	std::cout << std::endl << "Spectrum T1S Data File: " << dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "===============================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |       syst |" << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//std::pair<std::string, std::vector<double> > p;

		//Show 4 decimal places
		std::cout << std::fixed;
		std::cout.precision(4);

		std::cout << "| ";
		std::cout.width(10); 
		std::cout << data["xm"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xlow"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xhigh"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["sigma"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["stat"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst"][i]; 
		std::cout << " |" << std::endl;
	}

	std::cout << "===============================================================================" << std::endl << std::endl;
}

void SPXData::PrintSpectrumT1A(void) {

	std::cout << std::endl << "Spectrum T1A Data File: " << dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "============================================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |     syst + |     syst - |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//std::pair<std::string, std::vector<double> > p;

		//Show 4 decimal places
		std::cout << std::fixed;
		std::cout.precision(4);

		std::cout << "| ";
		std::cout.width(10); 
		std::cout << data["xm"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xlow"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xhigh"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["sigma"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["stat"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst_p"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst_n"][i]; 
		std::cout << " |" << std::endl;
	}

	std::cout << "============================================================================================" << std::endl;
}

void SPXData::PrintSpectrumT2S(void) {

}

void SPXData::PrintSpectrumT2A(void) {

}

void SPXData::PrintHERAFitter(void) {

}


