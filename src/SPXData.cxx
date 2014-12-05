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

//Class name for debug statements
const std::string cn = "SPXData::";

//Must define the static debug variable in the implementation
bool SPXData::debug;

//Public helper method to parse the data file based on the type
void SPXData::Parse(void) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Parsing data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

	try {
		OpenDataFile();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXFileIOException(cn + mn + "Unable to open data file");
	}

	if(dataFormat.IsSpectrum()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrum();
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

void SPXData::ParseSpectrum(void) {
	std::string mn = "ParseSpectrum: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

	if(!(*dataFile)) {
		throw SPXFileIOException("Something went awry with the dataFile ifstream...");
	}

	//Fixed column indices
	const unsigned int XM_COL = 		0;
	const unsigned int XLOW_COL = 		1;
	const unsigned int XHIGH_COL = 		2;
	const unsigned int SIGMA_COL = 		3;
	const unsigned int STAT_COL = 		4;
	const unsigned int SYST_COL = 		5;	//For symmetric total systematics
	const unsigned int SYST_P_COL =		5;	//For asymmetric total systematics
	const unsigned int SYST_N_COL = 	6;	//...

	const unsigned int REQ_COLS =		5;	//Required columns (xm - stat)

	std::string line;
	double xm_t, xlow_t, xhigh_t, sigma_t, stat_t, syst_p_t, syst_n_t;
	std::vector<double> xm;				//Mean x
	std::vector<double> xlow;			//X min
	std::vector<double> xhigh;			//X max
	std::vector<double> sigma;			//Sigma (cross-section)
	std::vector<double> stat;			//Statistical error
	std::vector<double> syst_p;			//Total systematic error (+)
	std::vector<double> syst_n;			//Total systematic error (-)

	//Individual systematic error namess
	std::vector<std::string> names;

	//Count the number of positive/negative systematic errors to check balance
	unsigned int pos_count = 0;
	unsigned int neg_count = 0;

	//Bin count
	unsigned int bin_count = 0;

	//Number of columns (used to determine symmetric, asymmetric, or no total systematic error)
	unsigned int numberOfColumns = 0;

	while(dataFile->good()) {
		std::getline(*dataFile, line);

		//Skip empty lines
		if(line.empty()) {
			continue;
		}

		std::istringstream iss(line);

		//Skip comments
		if(!line.empty() && (line[0] == ';')) {
			continue;
		} else if(!line.empty()) {

			//Check for systematic errors (line contains 'syst_')
			if(line.find("syst_") != std::string::npos) {

				//Split the line into systematic name and data
				std::string name;
				std::vector<double> tmp_syst;

				//Parse
				iss >> name;
				std::string formatted_line = SPXStringUtilities::RemoveFirstSubstring(line, name);
				formatted_line = SPXStringUtilities::ReplaceAll(formatted_line, "\t", " ");
				tmp_syst = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

				//Symmetric Error: Create both + and - and add them to map
				if((name.find("+") == std::string::npos) && (name.find("-") == std::string::npos)) {
					//Increment both positive and negative count
					pos_count++;
					neg_count++;

					std::string p_name = name + "+";
					std::string n_name = name + "-";

					if(debug) std::cout << std::endl;
					if(debug) std::cout << cn << mn << "Found new symmetric systematic error: " << name << std::endl;
					if(debug) std::cout << cn << mn << "Converted to asymmetric errors: " << p_name << " and " << n_name << std::endl;
					if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

					//Add to map
					StringDoubleVectorPair_T p_pair(p_name, tmp_syst);
					StringDoubleVectorPair_T n_pair(n_name, tmp_syst);
					individualSystematics.insert(p_pair);
					individualSystematics.insert(n_pair);
				}

				//Asymmetric Error: If '-', make sure there was already a '+', else issue warning
				else {
					if(name.find("+") != std::string::npos) {
						pos_count++;

						if(debug) std::cout << std::endl;
						if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;
						if(debug) std::cout << cn << mn << "Line: " << line << std::endl;
					}
					else if(name.find("-") != std::string::npos) {
						neg_count++;

						if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

						if(individualSystematics.count(SPXStringUtilities::ReplaceAll(name, "-", "+")) == 0) {
							std::cerr << cn << mn << "WARNING: Unbalanced sytematic error: " << SPXStringUtilities::RemoveCharacters(name, "+-") << std::endl;
						}
					}

					//Add it to the map
					StringDoubleVectorPair_T pair(name, tmp_syst);
					individualSystematics.insert(pair);
				}
			}

			//Not a systematic error: Read as data if it starts with a number (if first non-whitespace character is a digit)
			else if(isdigit((int)SPXStringUtilities::LeftTrim(line).at(0))) {

				if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

				//Parse line into data vector
				//Convert all tabs to spaces
				std::string formatted_line = SPXStringUtilities::ReplaceAll(line, "\t", " ");
				std::vector<double> tmp_data = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

				//Make sure there are at least 5 columns
				if(tmp_data.size() < REQ_COLS) {
					throw SPXParseException(cn + mn + "There must be at least 5 data columns (xm, xlow, xhigh, sigma, stat)");
				}

				//Set number of columns to the size of the 0th bin
				if(!bin_count) {
					//Set number of columns
					numberOfColumns = tmp_data.size();

					//Warn user that the aforementioned columns will be assumed for the remaining bins
					std::cout << cn << mn << "WARNING: The remaining bins MUST also have exactly " << numberOfColumns << " columns" << std::endl;
				}
				//After the 0th bin, make sure all other bins have the exact same number of columns
				else {
					if(tmp_data.size() != numberOfColumns) {
						std::ostringstream oss;
						oss << cn << mn << "Number of columns for bin " << bin_count + 1<< " (" << tmp_data.size() << \
							") does NOT match expected (" << numberOfColumns << ")" << std::endl;
						throw SPXParseException(oss.str());
					}
				}

				//Parse out required data
				xm_t = tmp_data[XM_COL];
				xlow_t = tmp_data[XLOW_COL];
				xhigh_t = tmp_data[XHIGH_COL];
				sigma_t = tmp_data[SIGMA_COL];
				stat_t = tmp_data[STAT_COL];

				//Check for symmetric, asymmetric, or no total systematic error (set numberOfColumns)
				if(numberOfColumns == 5) {
					if(!bin_count && debug) std::cout << cn << mn << "Assuming No Total Systematic Errors: Will sum individuals to calculate total" << std::endl;
				} else if(numberOfColumns == 6) {
					if(!bin_count && debug) std::cout << cn << mn << "Assuming Symmetric Systematic Errors: Will sum individuals to compare against total, if they are given" << std::endl;
					syst_p_t = tmp_data[SYST_COL];
					syst_n_t = tmp_data[SYST_COL];
					syst_p.push_back(syst_p_t);
					syst_n.push_back(syst_n_t);
				} else if(numberOfColumns == 7) {
					if(!bin_count && debug) std::cout << cn << mn << "Assuming Asymmetric Systematic Errors: Will sum individuals to compare against total, if they are given" << std::endl;
					syst_p_t = tmp_data[SYST_P_COL];
					syst_n_t = tmp_data[SYST_N_COL];
					syst_p.push_back(syst_p_t);
					syst_n.push_back(syst_n_t);
				}

				//Increment bin count
				bin_count++;

				//Fill required vectors with temp variables
				xm.push_back(xm_t);
				xlow.push_back(xlow_t);
				xhigh.push_back(xhigh_t);
				sigma.push_back(sigma_t);
				stat.push_back(stat_t);
			}
		}
	}

	//Check for no bin count...
	if(!bin_count) {
		throw SPXParseException("No data bins found in data file: Make sure it is not empty");
	}

	//Check positive/negative individual systematic count
	if(pos_count != neg_count) {
		std::cerr << cn << mn << "WARNING: Different number of positive/negative systematic errors: Data errors could be skewed" << std::endl;
	}

	//Set master size to number of bins
	int masterSize = bin_count;
	if(debug) std::cout << cn << mn << "Master size set to size of \"bin_count\": " << masterSize << std::endl;

	//If no total sytematics were given but there are no individual errors given either, bail out
	if((numberOfColumns == 5) && !pos_count) {
		throw SPXParseException("No total systematic error provided AND no individuals to sum");
	}

	//If necessary, compute/compare total positive/negative systematics for each bin using the individual systematic errors
	if(pos_count) {

		//The acceptable percent difference between given total and summed individuals (0.10 = 10%)
		const double INDIV_VS_TOT_ACCEPTABLE_ERROR = 0.10;

		for(int i = 0; i < masterSize; i++) {
			std::vector<double> p_errors;
			std::vector<double> n_errors;
			p_errors.clear();
			n_errors.clear();

			for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); ++it) {

				const std::string &name = it->first;
				std::vector<double> &syst = it->second;

				//Positive systematic
				if(name.find("+") != std::string::npos) {
					p_errors.push_back(syst.at(i));
				}

				//Negative systematic
				else if(name.find("-") != std::string::npos) {
					n_errors.push_back(syst.at(i));
				}

				//Symmetric systematic: +/- are the same
				else {
					p_errors.push_back(syst.at(i));
					n_errors.push_back(syst.at(i));
				}
			}

			syst_p_t = SPXMathUtilities::AddErrorsInQuadrature(p_errors);
			syst_n_t = SPXMathUtilities::AddErrorsInQuadrature(n_errors);

			if(debug) std::cout << std::endl;
			if(debug) std::cout << cn << mn << "Total positive systematic error for bin " << i << " calculated as: " << syst_p_t << std::endl;
			if(debug) std::cout << cn << mn << "Total negative systematic error for bin " << i << " calculated as: " << syst_n_t << std::endl;

			//No totals given, just use sum of individuals
			if(numberOfColumns == 5) {
				syst_p.push_back(syst_p_t);
				syst_n.push_back(syst_n_t);
			}

			//Otherwise, compare the summed individuals against the total within the acceptable percent difference
			else if((numberOfColumns == 6) || (numberOfColumns == 7)) {
				double given_total_p = syst_p.at(i);
				double given_total_n = syst_n.at(i);
				double delta_p = abs(given_total_p - syst_p_t);
				double delta_n = abs(given_total_n - syst_n_t);
				double average_p = (given_total_p + syst_p_t) / 2;
				double average_n = (given_total_n + syst_n_t) / 2;

				double percentDifference_p = delta_p / average_p;
				double percentDifference_n = delta_n / average_n;

				if(percentDifference_p > INDIV_VS_TOT_ACCEPTABLE_ERROR) {
					std::cerr << cn << mn << "WARNING: Bin " << i << ": Sum of POSITIVE individual errors (" << syst_p_t << \
					 	") does not agree within " << (INDIV_VS_TOT_ACCEPTABLE_ERROR * 100) << \
							"% with the given POSITIVE total systematic error (" << given_total_p << ")" << std::endl;
				}

				if(percentDifference_n > INDIV_VS_TOT_ACCEPTABLE_ERROR) {
					std::cerr << cn << mn << "WARNING: Bin " << i << ": Sum of NEGATIVE individual errors (" << syst_n_t << \
						") does not agree within " << (INDIV_VS_TOT_ACCEPTABLE_ERROR * 100) << \
							"% with the given NEGATIVE total systematic error (" << given_total_n << ")" << std::endl;
				}
			}
		}
	}

	//Check vector sizes: all vectors should be the same size
	if(debug) std::cout << std::endl;
	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors..." << std::endl;

	try {
		CheckVectorSize(xm, "xm", masterSize);
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
	numberOfBins = masterSize;
	if(debug) std::cout << cn << mn << "Number of Bins set to match master size: " << numberOfBins << std::endl;

	//Add all data to map
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));

	if(debug) std::cout << cn << mn << "Successfully added data to map" << std::endl;
}

void SPXData::ParseHERAFitter(void) {
	std::string mn = "ParseHERAFitter: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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

	//Individual systematic error namess
	std::vector<std::string> names;

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
				numberOfBins = atoi(tmp.c_str());
				if(debug) std::cout << cn << mn << "Number of Bins set to " << numberOfBins << std::endl;
			}

			//Get number of columns and initialize individual vector
			if(line.find("NColumn") != std::string::npos) {
				iss >> tmp;
				iss >> tmp;
				iss >> tmp;
				numberOfColumns = atoi(tmp.c_str());
				if(debug) std::cout << cn << mn << "Number of Columns set to "  << numberOfColumns << std::endl;

				//Determine number of individual systematics
				numberOfSystematics = numberOfColumns - SYST_BEGIN_COL;
				if(debug) std::cout << cn << mn << "Number of Systematics set to " << numberOfSystematics << std::endl;
			}

			//Compile the list of all individual systematic error names
			if(line.find("ColumnName") != std::string::npos) {

				tmp = line;

				//Get rid of 'ColumnName = '
				tmp = SPXStringUtilities::RemoveFirstSubstring(tmp, "ColumnName = ");

				//Remove all single quotes and spaces and delimit with "," returning a vector of tokens
				tmp = SPXStringUtilities::RemoveCharacters(tmp, "\' ");
				names = SPXStringUtilities::SplitString(tmp, ",");

				//Remove the non-systematic error names
				names.erase(names.begin(), names.begin() + SYST_BEGIN_COL);

				//DEBUG: Print all names
				if(debug) {
					std::cout << cn << mn << "names.size() = " << names.size() << std::endl;
					std::cout << cn << mn << "Individual Systematic Error Names: " << std::endl;
				}

				unsigned int pos_count = 0;
				unsigned int neg_count = 0;

				//Make sure there are the same number of +/- errors, if not issue a warning
				for(int i = 0; i < names.size(); i++) {
					if(debug) std::cout << "\t " << names.at(i) << std::endl;

					if(names.at(i).find("+") != std::string::npos) {
						pos_count++;
					}
					else if(names.at(i).find("-") != std::string::npos) {
						neg_count++;
					}
				}

				if(pos_count != neg_count) {
					std::cerr << cn << mn << "WARNING: Different number of positive/negative systematic errors: Data errors could be skewed" << std::endl;
				}

				//Check size against calculated number of columns
				if(names.size() != numberOfSystematics) {
					std::ostringstream oss;
					oss << cn << mn << "Error parsing systematic names: Name vector size (" << names.size() << \
						") is not equal to the expected number of systematics (" << numberOfSystematics << ")" << std::endl;
					throw SPXParseException(oss.str());
				}

				//Initialize a data vector for each name found
				for(int i = 0; i < numberOfSystematics; i++) {
					individualSystematics[names[i]] = std::vector<double>();
				}
			}

			//Data begins with numeric character at position 0 (and # bins and # columns have already been read correctly)
			if(isdigit((int)line.at(0)) && (numberOfBins) && (numberOfColumns)) {

				//Parse line into vector of doubles
				std::vector<double> dataVector;

				try {
					dataVector = SPXStringUtilities::ParseStringToDoubleVector(line, ' ');
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
				xm_t = (xlow_t + xhigh_t) / 2;
				sigma_t = dataVector[SIGMA_COL];
				double stat_p = dataVector[STAT_P_COL];
				double stat_n = dataVector[STAT_N_COL];
				stat_t = (stat_p + stat_n) / 2;

				//Read in systematics: Must use names vector instead of map keys to ensure correct order
				for(int i = SYST_BEGIN_COL; i < numberOfSystematics; i++) {
					if(debug) std::cout << "[" << i << "]" << " --> Adding error for " << names.at(i) << ": " << dataVector.at(i) << std::endl;

					if(individualSystematics.count(names.at(i)) == 0) {
						throw SPXParseException(cn + mn + "Name \"" + names.at(i) + "\" not found in systematics map");
					} else {
						std::vector<double> &v = individualSystematics[names.at(i)];
						v.push_back(dataVector.at(i));
					}
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

	//Set master size
	int masterSize = numberOfBins;
	if(debug) std::cout << cn << mn << "Master size set to size of \"NDATA\" " << masterSize << std::endl;

	//Compute total positive/negative systematics for each bin using the individual systematic errors
	for(int i = 0; i < masterSize; i++) {
		std::vector<double> p_errors;
		std::vector<double> n_errors;
		p_errors.clear();
		n_errors.clear();

		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); ++it) {

			const std::string &name = it->first;
			std::vector<double> &syst = it->second;

			//Positive systematic
			if(name.find("+") != std::string::npos) {
				p_errors.push_back(syst.at(i));
			}

			//Negative systematic
			else if(name.find("-") != std::string::npos) {
				n_errors.push_back(syst.at(i));
			}

			//Symmetric systematic: +/- are the same
			else {
				p_errors.push_back(syst.at(i));
				n_errors.push_back(syst.at(i));
			}
		}

		syst_p_t = SPXMathUtilities::AddErrorsInQuadrature(p_errors);
		syst_n_t = SPXMathUtilities::AddErrorsInQuadrature(n_errors);
		syst_p.push_back(syst_p_t);
		syst_n.push_back(syst_n_t);

		if(debug) std::cout << cn << mn << "Total positive systematic error for bin " << i << " calculated as: " << syst_p_t << std::endl;
		if(debug) std::cout << cn << mn << "Total negative systematic error for bin " << i << " calculated as: " << syst_n_t << std::endl;
	}

	//Check vector sizes: all vectors should be the same size
	if(debug) std::cout << cn << mn << "Checking sizes of all vectors.." << std::endl;

	try {
		CheckVectorSize(xm, "xm", masterSize);
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
	this->numberOfBins = masterSize;
	if(debug) std::cout << cn << mn << "numberOfBins = " << this->numberOfBins << std::endl;

	//Add to data map
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));

	PrintHERAFitter();
	while(1);
}

//Helper method to choose correct print method based on data format
void SPXData::Print(void) {

	if(dataFormat.IsSpectrum()) {
		PrintSpectrum();
	} else if(dataFormat.IsHERAFitter()) {
		PrintHERAFitter();
	}
}

void SPXData::PrintSpectrum(void) {
	std::string mn = "PrintSpectrum: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	std::cout << std::endl << "Spectrum Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;

	std::cout << "============================================================================================" << std::endl;
	std::cout << "|         xm |       xlow |      xhigh |      sigma |       stat |     syst + |     syst - |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
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
		std::cout << std::endl;
	}

	std::cout << std::endl << std::endl;
}

void SPXData::PrintHERAFitter(void) {
	std::string mn = "PrintHERAFitter: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
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
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
			//Convert stat errors
			data["stat"][i] *= data["sigma"][i] / 100;

			//Convert syst errors
			data["syst_p"][i] *= data["sigma"][i] / 100;
			data["syst_n"][i] *= data["sigma"][i] / 100;
		}
	}

	double *x = &data["xm"][0];
	double *y = &data["sigma"][0];
	double *stat = &data["stat"][0];
	double *eyl_syst = &data["syst_n"][0];
	double *eyh_syst = &data["syst_p"][0];

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
		std::cout << std::endl;
		std::cout << cn << mn << "Statistical Error Graph created with name: " << statName << std::endl;
		statisticalErrorGraph->Print();
		std::cout << std::endl;
		std::cout << cn << mn << "Systematic Error Graph created with name: " << systName << std::endl;
		systematicErrorGraph->Print();
		std::cout << std::endl << std::endl;
		std::cout << cn << mn << "Total Error Graph created with name: " << totName << std::endl;
		totalErrorGraph->Print();
		std::cout << std::endl;
	}
}
