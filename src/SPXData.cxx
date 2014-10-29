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
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst", syst));

	//Print formatted table
	// if(debug) {
	// 	PrintSpectrumT1S();
	// }
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
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst_p", syst_p));
	data.insert(std::pair<std::string, std::vector<double> >("syst_n", syst_n));

	//Print formatted table
	// if(debug) {
	// 	PrintSpectrumT1A();
	// }
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

	//Vector containing
	std::vector<std::vector<double> > individualSystematicErrors;

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
				iss >> name; //Get rid of the '+'
				iss >> name; //Actual name

				//if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;

				double val = 0;
				std::vector<double> tmp_syst;

				//Keep reading in values (one for each bin)
				while(iss >> val) {
					//if(debug) std::cout << cn << mn << "Val = " << val << std::endl;
					tmp_syst.push_back(val);
				}

				//Add the name to the object's list of individual systematic error names
				individualSystematicErrorNames.push_back(name);

				//Add the data to the temporary list of individual systematic errors
				individualSystematicErrors.push_back(tmp_syst);
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
		for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
			CheckVectorSize(individualSystematicErrors.at(i), individualSystematicErrorNames.at(i), masterSize);
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
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst", syst));

	//Add individual systematic errors to map
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
		data.insert(std::pair<std::string, std::vector<double> >(individualSystematicErrorNames.at(i), individualSystematicErrors.at(i)));
	}

	//Print formatted table
	// if(debug) {
	// 	PrintSpectrumT2S();
	// }
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

	//Vector containing
	std::vector<std::vector<double> > individualSystematicErrors;

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

				//Add the name to the object's list of individual systematic error names
				individualSystematicErrorNames.push_back(name);

				//Add the data to the temporary list of individual systematic errors
				individualSystematicErrors.push_back(tmp_syst);
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
		for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
			CheckVectorSize(individualSystematicErrors.at(i), individualSystematicErrorNames.at(i), masterSize);
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
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst_p", syst_p));
	data.insert(std::pair<std::string, std::vector<double> >("syst_n", syst_n));

	//Add individual systematic errors to map
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
		data.insert(std::pair<std::string, std::vector<double> >(individualSystematicErrorNames.at(i), individualSystematicErrors.at(i)));
	}

	//Print formatted table
	// if(debug) {
	// 	PrintSpectrumT2A();
	// }
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

	//Vector containing
	std::vector<std::vector<double> > individualSystematicErrors;

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
				iss >> name; //Get rid of the '+'
				iss >> name; //Actual name

				//if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;

				double val = 0;
				std::vector<double> tmp_syst;

				//Keep reading in values (one for each bin)
				while(iss >> val) {
					//if(debug) std::cout << cn << mn << "Val = " << val << std::endl;
					tmp_syst.push_back(val);
				}

				//Add the name to the object's list of individual systematic error names
				individualSystematicErrorNames.push_back(name);

				//Add the data to the temporary list of individual systematic errors
				individualSystematicErrors.push_back(tmp_syst);
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
		for(int j = 0; j < individualSystematicErrors.size(); j++) {
			errors.push_back(individualSystematicErrors[j][i]);
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
		for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
			CheckVectorSize(individualSystematicErrors.at(i), individualSystematicErrorNames.at(i), masterSize);
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
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst", syst));

	//Add individual systematic errors to map
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
		data.insert(std::pair<std::string, std::vector<double> >(individualSystematicErrorNames.at(i), individualSystematicErrors.at(i)));
	}

	//Print formatted table
	// if(debug) {
	// 	PrintSpectrumT3S();
	// }
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

	//Vector containing
	std::vector<std::vector<double> > individualSystematicErrors;

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

				//Add the name to the object's list of individual systematic error names
				individualSystematicErrorNames.push_back(name);

				//Add the data to the temporary list of individual systematic errors
				individualSystematicErrors.push_back(tmp_syst);
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
		for(int j = 0; j < individualSystematicErrors.size(); j+=2) {
			p_errors.push_back(individualSystematicErrors.at(j).at(i));
			n_errors.push_back(individualSystematicErrors.at(j + 1).at(i));
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
		for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
			CheckVectorSize(individualSystematicErrors.at(i), individualSystematicErrorNames.at(i), masterSize);
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
	data.insert(std::pair<std::string, std::vector<double> >("xm", xm));
	data.insert(std::pair<std::string, std::vector<double> >("xlow", xlow));
	data.insert(std::pair<std::string, std::vector<double> >("xhigh", xhigh));
	data.insert(std::pair<std::string, std::vector<double> >("sigma", sigma));
	data.insert(std::pair<std::string, std::vector<double> >("stat", stat));
	data.insert(std::pair<std::string, std::vector<double> >("syst_p", syst_p));
	data.insert(std::pair<std::string, std::vector<double> >("syst_n", syst_n));

	//Add individual systematic errors to map
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {
		data.insert(std::pair<std::string, std::vector<double> >(individualSystematicErrorNames.at(i), individualSystematicErrors.at(i)));
	}

	//Print formatted table
	// if(debug) {
	// 	PrintSpectrumT3A();
	// }
}

void SPXData::ParseHERAFitter(void) {
	std::string mn = "ParseHERAFitter: ";
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

	std::cout << std::endl << "Spectrum T1A Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

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

	std::cout << std::endl << "Spectrum T2S Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

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

	std::cout << "This data has " << individualSystematicErrorNames.size() << " Individual Systematic Errors:" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {

		std::cout << std::left << std::setw(24) << individualSystematicErrorNames.at(i) << "  ";

		std::cout << std::fixed;
		std::cout.precision(4);

		for(int j = 0; j < numberOfBins; j++) {
			std::cout.width(10);
			std::cout << data[individualSystematicErrorNames.at(i)][j] << " ";
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

	std::cout << "This data has " << (individualSystematicErrorNames.size() / 2) << " Individual Systematic Errors (+/-):" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {

		std::cout << std::left << std::setw(24) << individualSystematicErrorNames.at(i) << "  ";

		std::cout << std::fixed;
		std::cout.precision(4);

		for(int j = 0; j < numberOfBins; j++) {
			std::cout.width(10);
			std::cout << data[individualSystematicErrorNames.at(i)][j] << " ";
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

	std::cout << "This data has " << individualSystematicErrorNames.size() << " Individual Systematic Errors:" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {

		std::cout << std::left << std::setw(24) << individualSystematicErrorNames.at(i) << "  ";

		std::cout << std::fixed;
		std::cout.precision(4);

		for(int j = 0; j < numberOfBins; j++) {
			std::cout.width(10);
			std::cout << data[individualSystematicErrorNames.at(i)][j] << " ";
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

	std::cout << "This data has " << (individualSystematicErrorNames.size() / 2) << " Individual Systematic Errors (+/-):" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(int i = 0; i < individualSystematicErrorNames.size(); i++) {

		std::cout << std::left << std::setw(24) << individualSystematicErrorNames.at(i) << "  ";

		std::cout << std::fixed;
		std::cout.precision(4);

		for(int j = 0; j < numberOfBins; j++) {
			std::cout.width(10);
			std::cout << data[individualSystematicErrorNames.at(i)][j] << " ";
		}

		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}

void SPXData::PrintHERAFitter(void) {

}

void SPXData::CreateGraphs(void) {
	std::string mn = "CreateGraphs: ";

	//Create name strings
	TString name;
	TString statName;
	TString systName;

	//Check if name exists
	if(!pci.dataSteeringFile.GetName().empty()) {
		name = TString(pci.dataSteeringFile.GetName());
		statName = name + "_stat";
		systName = name + "_syst";
	}

	//Data steering file has no [DESC]:name
	//Default to filename
	else {
		if(debug) std::cout << cn << mn << "Data steering file has no name value: using filename instead" << std::endl;
		name = pci.dataSteeringFile.GetFilename();
		name.ReplaceAll(TString(".txt"), TString(""));
		statName = name + "_stat";
		systName = name + "_syst";
	}

	//double x[numberOfBins]; 		//xm
	//double y[numberOfBins];		//sigma
	double exl[numberOfBins];		// = (xm - ((xh + xl) / 2) + ((xh - xl) / 2))
	double exh[numberOfBins];		// = (((xh + xl) / 2) + ((xh - xl) / 2) - xm)
	//double eyl[numberOfBins];		//either stat - or syst - (depending on graph)
	//double eyh[numberOfBins];		//either stat + or syst + (depending on graph)

	double *x = &data["xm"][0];		//x = xm
	double *y = &data["sigma"][0];	//y = sigma

	//Calculate exl and exh
	for(int i = 0; i < numberOfBins; i++) {
		double m, n;

		m = (data["xhigh"][i] + data["xlow"][i]) / 2;
		n = (data["xhigh"][i] - data["xlow"][i]) / 2;

		exl[i] = x[i] - m + n;
		exh[i] = m + n - x[i];
	}

	//Errors for statistical are always symmetrical
	double *eyl_stat = &data["stat"][0];
	double *eyh_stat = &data["stat"][0];

	//Must initialize beforehand
	double *eyl_syst;
	double *eyh_syst;

	//Symmetric
	if(dataFormat.IsSymmetric()) {
		eyl_syst = &data["syst"][0];
		eyh_syst = &data["syst"][0];
	}
	//Asymmetric
	else if(dataFormat.IsAsymmetric()) {
		eyl_syst = &data["syst_n"][0];
		eyh_syst = &data["syst_p"][0];
	}

	//@TODO MUST IMPLEMENT CORRELATION MATRIX

	//Convert to raw number if errors are given in percent
	if(pci.dataSteeringFile.IsErrorInPercent()) {
		if(debug) std::cout << cn << mn << "Errors were given in percent: Converting to raw numbers" << std::endl;

		for(int i = 0; i < numberOfBins; i++) {
			eyl_stat[i] *= y[i] / 100.0;
			eyh_stat[i] *= y[i] / 100.0;
			eyl_syst[i] *= y[i] / 100.0;
			eyh_syst[i] *= y[i] / 100.0;
		}
	}

	//Create statistical error graph
	statisticalErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, eyl_stat, eyh_stat);

	//Create systematic error graph
	systematicErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, eyl_syst, eyh_syst);

	//Modify names
	statisticalErrorGraph->SetName(statName);
	systematicErrorGraph->SetName(systName);

	if(debug) {
		std::cout << cn << mn << "Statistical Error Graph created with name: " << statName << std::endl;
		statisticalErrorGraph->Print();
		std::cout << std::endl;
		std::cout << cn << mn << "Systematic Error Graph created with name: " << systName << std::endl;
		systematicErrorGraph->Print();
		std::cout << std::endl;
	}
}

/*
void SPXData::Draw(void) {
	std::string mn = "Draw: ";

	//Create the graphs
	//@TODO User should have to call CreateGraphs and then Draw, this way things aren't calculated everytime draw is called
	CreateGraphs();

	if(!statisticalErrorGraph) {
		throw SPXGraphException("Invalid statistical error graph");
	}

	if(!systematicErrorGraph) {
		throw SPXGraphException("Invalid systematic error graph");
	}

	TCanvas *canvas = new TCanvas("canvas", "Test Canvas", 200, 10, 700, 500);
	canvas->SetFillColor(0);
	canvas->SetGrid();

	//Draw the frame (xmin, ymin, xmax, ymax)
	canvas->DrawFrame(0, -10, 3000, 10);

	//Draw
	systematicErrorGraph->Draw("P");
	statisticalErrorGraph->Draw("||");

	canvas->Update();
}
*/
