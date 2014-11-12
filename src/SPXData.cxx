//************************************************************/
//
//	Data Implementation
//
//	Implements the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iomanip>
#include <string.h> //malloc

#include "SPXData.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXData::";

//Must define the static debug variable in the implementation
bool SPXData::debug;

//Public helper method to parse the data file based on the type
void SPXData::Parse(void) {
	std::string mn = "Parse: ";

	if(debug) std::cout << cn << mn << "Parsing data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

	try {
		OpenDataFile();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXFileIOException(cn + mn + "Unable to open data file");
	}

	if(dataFormat.IsSpectrumT1S()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrumT1S();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(dataFormat.IsSpectrumT1A()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrumT1A();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(dataFormat.IsSpectrumT2S()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrumT2S();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(dataFormat.IsSpectrumT2A()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrumT2A();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(dataFormat.IsSpectrumT3S()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrumT3S();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(dataFormat.IsSpectrumT3A()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrumT3A();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else if(dataFormat.IsHERAFitter()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseHERAFitter();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
	} else {
		throw SPXParseException("DataSteeringFile " + pci.dataSteeringFile.GetFilename() + " has invalid data format");
	}
}

void SPXData::ParseSpectrumT1S(void) {
	std::string mn = "ParseSpectrumT1S: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst", syst));
}

void SPXData::ParseSpectrumT1A(void) {

	std::string mn = "ParseSpectrumT1A: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));
}

void SPXData::ParseSpectrumT2S(void) {
	std::string mn = "ParseSpectrumT2S: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

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

			//Check for systematic errors
			if(line.find("+ syst_") == 0) {

				std::string name;
				std::string sign;
				iss >> sign; //Get rid of the '+'
				iss >> name; //Actual name

				double val = 0;
				std::vector<double> tmp_syst;

				//Keep reading in values (one for each bin)
				while(iss >> val) {
					//if(debug) std::cout << cn << mn << "Val = " << val << std::endl;
					tmp_syst.push_back(val);
				}

				//Add it to the map
				StringDoubleVectorPair_T pair(name, tmp_syst);
				individualSystematics.insert(pair);
			}

			//Not a systematic error
			else {
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

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst", syst));
}

void SPXData::ParseSpectrumT2A(void) {
	std::string mn = "ParseSpectrumT2A: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

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

			//Check for systematic errors
			if(line.find(" syst_") == 1) {

				std::string sign;
				std::string name;

				iss >> sign; //Sign
				iss >> name; //Actual name

				//Append '_p' or '_n' depending on sign
				if(!sign.compare("+")) {
					name += "_p";
				} else if(!sign.compare("-")) {
					name += "_n";
				}

				//if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;

				double val = 0;
				std::vector<double> tmp_syst;

				//Keep reading in values (one for each bin)
				while(iss >> val) {
					//if(debug) std::cout << cn << mn << "Val = " << val << std::endl;
					tmp_syst.push_back(val);
				}

				//Add it to the map
				StringDoubleVectorPair_T pair(name, tmp_syst);
				individualSystematics.insert(pair);
			}

			//Not a systematic error
			else {
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

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));
}

void SPXData::ParseSpectrumT3S(void) {
	std::string mn = "ParseSpectrumT3S: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

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

			//Check for systematic errors
			if(line.find("+ syst_") == 0) {

				std::string sign;
				std::string name;
				iss >> sign; //Get rid of the '+'
				iss >> name; //Actual name

				//if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;

				double val = 0;
				std::vector<double> tmp_syst;

				//Keep reading in values (one for each bin)
				while(iss >> val) {
					//if(debug) std::cout << cn << mn << "Val = " << val << std::endl;
					tmp_syst.push_back(val);
				}

				//Add it to the map
				StringDoubleVectorPair_T pair(name, tmp_syst);
				individualSystematics.insert(pair);
			}

			//Not a systematic error
			else {
				//Parse each line into temp variables
				iss >> xm_t >> xlow_t >> xhigh_t >> sigma_t >> stat_t;

				//Fill vectors with temp variables
				xm.push_back(xm_t);
				xlow.push_back(xlow_t);
				xhigh.push_back(xhigh_t);
				sigma.push_back(sigma_t);
				stat.push_back(stat_t);
			}
		}
	}

	//Check vector sizes: all vectors should be the same size
	int masterSize = xm.size();
	if(debug) std::cout << cn << mn << "Master size set to size of \"xm\" vector: " << masterSize << std::endl;

	//Compute total systematics for each bin using the individual systematic errors
	for(int i = 0; i < masterSize; i++) {
		std::vector<double> errors;
		errors.clear();

		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			std::vector<double> &systematic = it->second;
			errors.push_back(systematic.at(i));
		}

		syst_t = SPXMathUtilities::AddErrorsInQuadrature(errors);
		syst.push_back(syst_t);

		if(debug) std::cout << cn << mn << "Total systematic error for bin " << i << " calculated as: " << syst_t << std::endl;
	}

	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors.." << std::endl;

	try {
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat, "stat", masterSize);
		CheckVectorSize(syst, "syst", masterSize);

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst", syst));
}

void SPXData::ParseSpectrumT3A(void) {
	std::string mn = "ParseSpectrumT3A: ";

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

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

			//Check for systematic errors
			if(line.find(" syst_") == 1) {

				std::string sign;
				std::string name;

				iss >> sign; //Sign
				iss >> name; //Actual name

				//Append '_p' or '_n' depending on sign
				if(!sign.compare("+")) {
					name += "_p";
				} else if(!sign.compare("-")) {
					name += "_n";
				}

				//if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;

				double val = 0;
				std::vector<double> tmp_syst;

				//Keep reading in values (one for each bin)
				while(iss >> val) {
					//if(debug) std::cout << cn << mn << "Val = " << val << std::endl;
					tmp_syst.push_back(val);
				}

				//Add it to the map
				StringDoubleVectorPair_T pair(name, tmp_syst);
				individualSystematics.insert(pair);
			}

			//Not a systematic error
			else {
				//Parse each line into temp variables
				iss >> xm_t >> xlow_t >> xhigh_t >> sigma_t >> stat_t;

				//Fill vectors with temp variables
				xm.push_back(xm_t);
				xlow.push_back(xlow_t);
				xhigh.push_back(xhigh_t);
				sigma.push_back(sigma_t);
				stat.push_back(stat_t);
			}
		}
	}

	//Check vector sizes: all vectors should be the same size
	int masterSize = xm.size();
	if(debug) std::cout << cn << mn << "Master size set to size of \"xm\" vector: " << masterSize << std::endl;

	//Compute total positive/negative systematics for each bin using the individual systematic errors
	for(int i = 0; i < masterSize; i++) {
		std::vector<double> p_errors;
		std::vector<double> n_errors;
		p_errors.clear();
		n_errors.clear();

		StringDoubleVectorMap_T::iterator p_it = individualSystematics.begin();
		StringDoubleVectorMap_T::iterator n_it = individualSystematics.begin();

		while((p_it != individualSystematics.end()) && (n_it != individualSystematics.end())) {

			//Increment the Negative iterator first to offset it from the positive by 1
			if(++n_it == individualSystematics.end()) {
				break;
			}

			std::vector<double> &p_syst = p_it->second;
			std::vector<double> &n_syst = n_it->second;
			p_errors.push_back(p_syst.at(i));
			n_errors.push_back(n_syst.at(i));

			//Increment Positive iterator by 2
			if( ++p_it == individualSystematics.end()) {
				break;
			}
			if( ++p_it == individualSystematics.end()) {
				break;
			}

			//Increment the Negative iterator by 1, making a total increment of 2
			if( ++n_it == individualSystematics.end()) {
				break;
			}
		}

		syst_p_t = SPXMathUtilities::AddErrorsInQuadrature(p_errors);
		syst_n_t = SPXMathUtilities::AddErrorsInQuadrature(n_errors);
		syst_p.push_back(syst_p_t);
		syst_n.push_back(syst_n_t);

		if(debug) std::cout << cn << mn << "Total positive systematic error for bin " << i << " calculated as: " << syst_p_t << std::endl;
		if(debug) std::cout << cn << mn << "Total negative systematic error for bin " << i << " calculated as: " << syst_n_t << std::endl;
	}

	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors.." << std::endl;

	try {
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat, "stat", masterSize);
		CheckVectorSize(syst_p, "syst_p", masterSize);
		CheckVectorSize(syst_n, "syst_n", masterSize);

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));
}

void SPXData::ParseHERAFitter(void) {
	std::string mn = "ParseHERAFitter: ";

	//Fixed column indices (starting at Column 0)
	const unsigned int XLOW_COL = 1;
	const unsigned int XHIGH_COL = 2;
	const unsigned int SIGMA_COL = 5;
	const unsigned int STAT_P_COL = 6;
	const unsigned int STAT_N_COL = 7;
	const unsigned int SYST_BEGIN_COL = 8;

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

	if(!(*dataFile)) {
		throw SPXFileIOException("Something went awry with the dataFile ifstream...");
	}

	unsigned int numberOfBins = 0;
	unsigned int numberOfColumns = 0;
	unsigned int numberOfSystematics = 0;

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

		//Temp string
		std::string tmp;
		tmp.clear();

		if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

		//Skip empty lines
		if(line.empty()) {
			continue;
		}

		//Skip comments
		if(!line.empty() && (line[0] == ';')) {
			continue;
		} else if(!line.empty()) {

			//Get the number of bins
			if(line.find("NDATA") != std::string::npos) {
				iss >> tmp;	//NDATA
				iss >> tmp;	//=
				iss >> tmp;
				numberOfBins = atoi(tmp);
				if(debug) std::cout << cn << mn << "Number of Bins set to " << numberOfBins << std::endl;
			}

			//Get number of columns and initialize individual vector
			if(line.find("NColumn") != std::string::npos) {
				iss >> tmp;
				iss >> tmp;
				iss >> tmp;
				numberOfColumns = atoi(tmp);
				if(debug) std::cout << cn << mn << "Number of Columns set to "  << numberOfColumns << std::endl;

				//Determine number of individual systematics
				numberOfSystematics = numberOfColumns - SYST_BEGIN_COL;
				if(debug) std::cout << cn << mn << "Number of Systematics set to " << numberOfSystematics << std::endl;
			}

			//Compile the list of all individual systematic error names
			if(line.find("ColumnName") == 1) {

				std::vector<std::string> names;

				tmp = line;

				//Get rid of 'ColumnName = '
				tmp = SPXStringUtilities::ReplaceFirstSubstring(tmp, "ColumnName = ");

				//Remove all single quotes and delimit with "," returning a vector of tokens
				tmp = SPXStringUtilities::RemoveCharacters(tmp, "\'");
				names = SPXStringUtilities::SplitString(tmp, ",");

				//Remove the non-systematic error names
				std::vector<names::value_type>(names.begin() + SYST_BEGIN_COL, names.end()).swap(names);

				//DEBUG: Print all names
				if(debug) {
					std::cout << cn << mn << "names.size() = " << names.size() << std::endl;
					std::cout << cn << mn << "Names: " << std::endl;
					for(int i = 0; i < names.size(); i++) {
						std::cout << "\t " << names.at(i) << std::endl;
					}
				}

				//Check size against calculated number of columns
				if(names.size() != numberOfSystematics) {
					std::ostringstream oss;
					oss << cn << mn << "Error parsing systematic names: Name vector size (" << names.size() << \
						") is not equal to the expected number of systematics (" << numberOfSystematics << ")" << std::endl;
					throw SPXParseException(oss.str());
				}

				//@TODO For each SYMMETRIC name, make _p and _n with same value AND INCREMENT numberOfSystematics
				// //Append '_p' or '_n' depending on sign
				// if(!sign.compare("+")) {
				// 	name += "_p";
				// } else if(!sign.compare("-")) {
				// 	name += "_n";
				// }

	/////////////DEBUG//////////////
				while(1);
	////////////////////////////////

				//Initialize a data vector for each name found
				for(int i = 0; i < numberOfSystematics; i++) {
					std::vector<double> tmp_v;
					individualSystematics[names[i]].push_back(tmp_v);
				}
			}

			//Data begins with numeric character at position 0 (and # bins and # columns have already been read correctly)
			if(isdigit((int)line.at(0)) && (numberOfBins) && (numberOfDataColumns)) {

				//Parse line into vector of doubles
				try {
					std::vector<double> dataVector = SPXStringUtilities::ParseStringToDoubleVector(line, ' ');
				} catch(const SPXException &e) {
					std::cerr << e.what() << std::endl;
					throw SPXParseException(cn + mn + "Could not parse HERAFitter data file " + pci.dataSteeringFile.GetDataFile());
				}

				if(dataVector.size() != numberOfColumns) {
					std::ostringstream oss;
					oss << cn << mn << "Error parsing data: line's data vector size (" << dataVector.size() << \
						") is not equal to the expected number of columns (" << numberOfColumns << ")" << std::endl;
					throw SPXParseException(oss.str());
				}

				xlow_t = dataVector[XLOW_COL];
				xhigh_t = dataVector[XHIGH_COL];
				xm_t = sqrt(pow(xlow_t, 2.0) + pow(xhigh_t, 2.0));	//@TODO How to calculate Xm for HERAFitter? Geometric mean?
				sigma_t = dataVector[SIGMA_COL];
				double stat_p = dataVector[STAT_P_COL];
				double data_n = dataVector[STAT_N_COL];
				stat_t = sqrt(pow(stat_p, 2.0) + pow(stat_n, 2.0));	//@TODO How to calculate Stat for HERAFitter? Geometric mean?

				//Read in systematics: Must use names vector instead of map keys to ensure correct order
				for(int i = SYST_BEGIN_COL; i < numberOfColumns; i++) {
					individualSystematics[names[i]].push_back(dataVector.at(i));
				}

				//Fill vectors with temp variables
				xm.push_back(xm_t);
				xlow.push_back(xlow_t);
				xhigh.push_back(xhigh_t);
				sigma.push_back(sigma_t);
				stat.push_back(stat_t);
			}
		}
	}

	//Check vector sizes: all vectors should be the same size
	int masterSize = xm.size();
	if(debug) std::cout << cn << mn << "Master size set to size of \"xm\" vector: " << masterSize << std::endl;

	//Compute total positive/negative systematics for each bin using the individual systematic errors
	for(int i = 0; i < masterSize; i++) {
		std::vector<double> p_errors;
		std::vector<double> n_errors;
		p_errors.clear();
		n_errors.clear();

		StringDoubleVectorMap_T::iterator p_it = individualSystematics.begin();
		StringDoubleVectorMap_T::iterator n_it = individualSystematics.begin();

		while((p_it != individualSystematics.end()) && (n_it != individualSystematics.end())) {

			//Increment the Negative iterator first to offset it from the positive by 1
			if(++n_it == individualSystematics.end()) {
				break;
			}

			std::vector<double> &p_syst = p_it->second;
			std::vector<double> &n_syst = n_it->second;
			p_errors.push_back(p_syst.at(i));
			n_errors.push_back(n_syst.at(i));

			//Increment Positive iterator by 2
			if( ++p_it == individualSystematics.end()) {
				break;
			}
			if( ++p_it == individualSystematics.end()) {
				break;
			}

			//Increment the Negative iterator by 1, making a total increment of 2
			if( ++n_it == individualSystematics.end()) {
				break;
			}
		}

		syst_p_t = SPXMathUtilities::AddErrorsInQuadrature(p_errors);
		syst_n_t = SPXMathUtilities::AddErrorsInQuadrature(n_errors);
		syst_p.push_back(syst_p_t);
		syst_n.push_back(syst_n_t);

		if(debug) std::cout << cn << mn << "Total positive systematic error for bin " << i << " calculated as: " << syst_p_t << std::endl;
		if(debug) std::cout << cn << mn << "Total negative systematic error for bin " << i << " calculated as: " << syst_n_t << std::endl;
	}

	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors.." << std::endl;

	try {
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat, "stat", masterSize);
		CheckVectorSize(syst_p, "syst_p", masterSize);
		CheckVectorSize(syst_n, "syst_n", masterSize);

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

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
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));
}

//Helper method to choose correct print method based on data format
void SPXData::Print(void) {
	if(dataFormat.IsSpectrumT1S()) {
		PrintSpectrumT1S();
	} else if(dataFormat.IsSpectrumT1A()) {
		PrintSpectrumT1A();
	} else if(dataFormat.IsSpectrumT2S()) {
		PrintSpectrumT2S();
	} else if(dataFormat.IsSpectrumT2A()) {
		PrintSpectrumT2A();
	} else if(dataFormat.IsSpectrumT3S()) {
		PrintSpectrumT3S();
	} else if(dataFormat.IsSpectrumT3A()) {
		PrintSpectrumT3A();
	} else if(dataFormat.IsHERAFitter()) {
		PrintHERAFitter();
	}
}

void SPXData::PrintSpectrumT1S(void) {

	std::cout << std::endl << "Spectrum T1S Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "===============================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |       syst |" << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << std::endl << "Spectrum T1A Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "============================================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |     syst + |     syst - |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << std::endl << "Spectrum T2S Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "===============================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |       syst |" << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << "This data has " << individualSystematics.size() << " Individual Systematic Errors:" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
		const std::string &syst_name = it->first;
		std::vector<double> &systematic = it->second;

		std::cout << std::left << std::setw(24) << syst_name << "  ";
		std::cout << std::fixed;
		std::cout.precision(4);
		for(int j = 0; j < systematic.size(); j++) {
			std::cout.width(10);
			std::cout << systematic.at(j) << " ";
		}
		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}

void SPXData::PrintSpectrumT2A(void) {

	std::cout << std::endl << "Spectrum T2A Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "============================================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |     syst + |     syst - |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << "This data has " << individualSystematics.size() / 2 << " Individual Systematic Errors:" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
		const std::string &syst_name = it->first;
		std::vector<double> &systematic = it->second;

		std::cout << std::left << std::setw(24) << syst_name << "  ";
		std::cout << std::fixed;
		std::cout.precision(4);
		for(int j = 0; j < systematic.size(); j++) {
			std::cout.width(10);
			std::cout << systematic.at(j) << " ";
		}
		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}

void SPXData::PrintSpectrumT3S(void) {

	std::cout << std::endl << "Spectrum T3S Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "===============================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |       syst |" << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << "This data has " << individualSystematics.size() << " Individual Systematic Errors:" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
		const std::string &syst_name = it->first;
		std::vector<double> &systematic = it->second;

		std::cout << std::left << std::setw(24) << syst_name << "  ";
		std::cout << std::fixed;
		std::cout.precision(4);
		for(int j = 0; j < systematic.size(); j++) {
			std::cout.width(10);
			std::cout << systematic.at(j) << " ";
		}
		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}

void SPXData::PrintSpectrumT3A(void) {

	std::cout << std::endl << "Spectrum T3A Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "============================================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |     syst + |     syst - |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << "This data has " << individualSystematics.size() / 2<< " Individual Systematic Errors:" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
		const std::string &syst_name = it->first;
		std::vector<double> &systematic = it->second;

		std::cout << std::left << std::setw(24) << syst_name << "  ";
		std::cout << std::fixed;
		std::cout.precision(4);
		for(int j = 0; j < systematic.size(); j++) {
			std::cout.width(10);
			std::cout << systematic.at(j) << " ";
		}
		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}

void SPXData::PrintHERAFitter(void) {

	std::cout << std::endl << "HERAFitter Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "============================================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |     syst + |     syst - |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

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

	std::cout << "This data has " << individualSystematics.size() << " Individual Systematic Errors (including asymmetries):" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
		const std::string &syst_name = it->first;
		std::vector<double> &systematic = it->second;

		std::cout << std::left << std::setw(24) << syst_name << "  ";
		std::cout << std::fixed;
		std::cout.precision(4);
		for(int j = 0; j < systematic.size(); j++) {
			std::cout.width(10);
			std::cout << systematic.at(j) << " ";
		}
		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}

void SPXData::CreateGraphs(void) {
	std::string mn = "CreateGraphs: ";

	//Create name strings
	TString name;
	TString statName;
	TString systName;
	TString totName;

	//Check if name exists
	if(!pci.dataSteeringFile.GetName().empty()) {
		name = TString(pci.dataSteeringFile.GetName());
		statName = name + "_stat";
		systName = name + "_syst";
		totName = name + "_tot";
	}

	//Data steering file has no [DESC]:name
	//Default to filename
	else {
		if(debug) std::cout << cn << mn << "Data steering file has no name value: using filename instead" << std::endl;
		name = pci.dataSteeringFile.GetFilename();
		name.ReplaceAll(TString(".txt"), TString(""));
		statName = name + "_stat";
		systName = name + "_syst";
		totName = name + "_tot";
	}

	double exl[numberOfBins];		// = (xm - ((xh + xl) / 2) + ((xh - xl) / 2))
	double exh[numberOfBins];		// = (((xh + xl) / 2) + ((xh - xl) / 2) - xm)

	//Calculate exl and exh
	for(int i = 0; i < numberOfBins; i++) {
		double m, n;

		m = (data["xhigh"][i] + data["xlow"][i]) / 2;
		n = (data["xhigh"][i] - data["xlow"][i]) / 2;

		exl[i] = data["xm"][i] - m + n;
		exh[i] = m + n - data["xm"][i];
	}

	//@TODO MUST IMPLEMENT CORRELATION MATRIX

	//Convert to raw number if errors are given in percent
	if(pci.dataSteeringFile.IsErrorInPercent()) {
		if(debug) std::cout << cn << mn << "Errors were given in percent: Converting to raw numbers" << std::endl;

		for(int i = 0; i < numberOfBins; i++) {
			//if(debug) std::cout << "data[\"sigma\"][" << i << "] = " << data["sigma"][i] << std::endl;
			//if(debug) std::cout << "data[\"stat\"][" << i << "] = " << data["stat"][i] << std::endl;

			//Convert stat errors
			data["stat"][i] *= data["sigma"][i] / 100;
			//if(debug) std::cout << "New = data[\"stat\"][" << i << "] * (y[" << i << "] / 100) = " << data["stat"][i] << std::endl;

			//Convert syst errors
			if(dataFormat.IsSymmetric()) {
				//if(debug) std::cout << "data[\"syst\"][" << i << "] = " << data["syst"][i] << std::endl;
				data["syst"][i] *= data["sigma"][i] / 100;
				//if(debug) std::cout << "New = data[\"syst\"][" << i << "] * (y[" << i << "] / 100) = " << data["syst"][i] << std::endl;
			}
			else if(dataFormat.IsAsymmetric()) {
				//if(debug) std::cout << "data[\"syst_p\"][" << i << "] = " << data["syst_p"][i] << std::endl;
				//if(debug) std::cout << "data[\"syst_n\"][" << i << "] = " << data["syst_n"][i] << std::endl;
				data["syst_p"][i] *= data["sigma"][i] / 100;
				data["syst_n"][i] *= data["sigma"][i] / 100;
				//if(debug) std::cout << "New = data[\"syst_p\"][" << i << "] * (y[" << i << "] / 100) = " << data["syst_p"][i] << std::endl;
				//if(debug) std::cout << "New = data[\"syst_n\"][" << i << "] * (y[" << i << "] / 100) = " << data["syst_n"][i] << std::endl;
			}
		}
	}

	double *x = &data["xm"][0];
	double *y = &data["sigma"][0];
	double *stat = &data["stat"][0];
	double *eyl_syst;
	double *eyh_syst;

	if(dataFormat.IsSymmetric()) {
		eyh_syst = eyl_syst = &data["syst"][0];
	}
	else if(dataFormat.IsAsymmetric()) {
		eyh_syst = &data["syst_p"][0];
		eyl_syst = &data["syst_n"][0];
	}

	std::vector<double> eyl_tot_v;
	std::vector<double> eyh_tot_v;

	//Calculate total errors = sqrt(stat^2 + syst^2)
	for(int i = 0; i < numberOfBins; i++) {
		double eylt;
		double eyht;

		eylt = sqrt(pow(stat[i], 2.0) + pow(eyl_syst[i], 2.0));
		eyht = sqrt(pow(stat[i], 2.0) + pow(eyh_syst[i], 2.0));
		eyl_tot_v.push_back(eylt);
		eyh_tot_v.push_back(eyht);
	}

	double *eyl_tot = &eyl_tot_v[0];
	double *eyh_tot = &eyh_tot_v[0];

	//Create statistical error graph
	statisticalErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, stat, stat);

	//Create systematic error graph
	systematicErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, eyl_syst, eyh_syst);

	//Create total error graph
	totalErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, eyl_tot, eyh_tot);

	//Modify names
	statisticalErrorGraph->SetName(statName);
	systematicErrorGraph->SetName(systName);
	totalErrorGraph->SetName(totName);

	if(debug) {
		std::cout << cn << mn << "Statistical Error Graph created with name: " << statName << std::endl;
		statisticalErrorGraph->Print();
		std::cout << std::endl << std::endl;
		std::cout << cn << mn << "Systematic Error Graph created with name: " << systName << std::endl;
		systematicErrorGraph->Print();
		std::cout << std::endl << std::endl;
		std::cout << cn << mn << "Total Error Graph created with name: " << totName << std::endl;
		totalErrorGraph->Print();

		std::cout << std::endl;
	}
}
