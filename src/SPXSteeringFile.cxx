//************************************************************/
//
//	Steering File Class
//
//	Implements the SPXSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		19.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXSteeringFile.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXSteeringFile::";

const int DEFAULT_DATA_MARKER_STYLE = 20;	//Circle
const int DEFAULT_DATA_MARKER_COLOR = 1;	//Black

void SPXSteeringFile::SetDefaults(void) {
	std::string mn = "SetDefaults: ";

	if(debug) std::cout << cn << mn << "Setting default Steering File data" << std::endl;

	plotBand = false;
	if(debug) std::cout << cn << mn << "plotBand set to default: \"false\"" << std::endl;

	plotErrorTicks = false;
	if(debug) std::cout << cn << mn << "plotErrorTicks set to default: \"false\"" << std::endl;

	plotMarker = true;
	if(debug) std::cout << cn << mn << "plotMarker set to default: \"true\"" << std::endl;

	plotStaggered = false;
	if(debug) std::cout << cn << mn << "plotStaggered set to default: \"false\"" << std::endl;

	matchBinning = true;
	if(debug) std::cout << cn << mn << "matchBinning set to default: \"true\"" << std::endl;

	gridCorr = false;
	if(debug) std::cout << cn << mn << "gridCorr set to default: \"false\"" << std::endl;

	labelSqrtS = false;
	if(debug) std::cout << cn << mn << "labelSqrtS set to default: \"false\"" << std::endl;

	xLegend = 0.9;
	if(debug) std::cout << cn << mn << "xLegend set to default: \"0.9\"" << std::endl;

	yLegend = 0.9;
	if(debug) std::cout << cn << mn << "yLegend set to default: \"0.9\"" << std::endl;

	yOverlayMin = MIN_EMPTY;
	if(debug) std::cout << cn << mn << "yOverlayMin set to default: \"" << MIN_EMPTY << "\"" << std::endl;

	yOverlayMax = MAX_EMPTY;
	if(debug) std::cout << cn << mn << "yOverlayMax set to default: \"" << MAX_EMPTY << "\"" << std::endl;

	yRatioMin = MIN_EMPTY;
	if(debug) std::cout << cn << mn << "yRatioMin set to default: \"" << MIN_EMPTY << "\"" << std::endl;

	yRatioMax = MAX_EMPTY;
	if(debug) std::cout << cn << mn << "yRatioMax set to default: \"" << MAX_EMPTY << "\"" << std::endl;
}

void SPXSteeringFile::PrintAll(void) {
	this->Print();
	this->PrintPDFSteeringFiles();
	this->PrintDataSteeringFiles();
	this->PrintGridSteeringFiles();
}

void SPXSteeringFile::ParseAll(bool print) {

	try {
		this->Parse();
		if(print) this->Print();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing Steering File");
	}

	try {
		this->ParsePDFSteeringFiles();
		if(print) this->PrintPDFSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing PDF Steering Files");
	}

	try {
		this->ParseDataSteeringFiles();
		if(print) this->PrintDataSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing Data Steering Files");
	}

	try {
		this->ParseGridSteeringFiles();
		if(print) this->PrintGridSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing Grid Steering Files");
	}
}

//Print the Steering File Data in a nice format
void SPXSteeringFile::Print(void) {
	std::cout << "Steering File: " << filename << std::endl;
	std::cout << "\t General configurations [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl;
	std::cout << "\t Graphing configurations [GRAPH]" << std::endl;
	std::cout << "\t\t Plot Band is: " << (plotBand ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Error Ticks is: " << (plotErrorTicks ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Marker is: " << (plotMarker ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Staggered is: " << (plotStaggered ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Match Binning is: " << (matchBinning ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Grid Corrections are: " << (gridCorr ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Label Sqrt(s) on Leggend: " << (labelSqrtS ? "YES" : "NO") << std::endl;
	std::cout << "\t\t X Legend: " << xLegend << std::endl;
	std::cout << "\t\t Y Legend: " << yLegend << std::endl;
	std::cout << "\t\t Y Overlay Min: " << yOverlayMin << std::endl;
	std::cout << "\t\t Y Overlay Max: " << yOverlayMax << std::endl;
	std::cout << "\t\t Y Ratio Min: " << yRatioMin << std::endl;
	std::cout << "\t\t Y Ratio Max: " << yRatioMax << std::endl << std::endl;
	std::cout << "\t Plot Configurations" << std::endl;
	std::cout << "\t\t Number of Plots: " << GetNumberOfPlotConfigurations() << std::endl << std::endl;

	for(int i = 0; i < GetNumberOfPlotConfigurations(); i++) {
		std::cout << "\t\t Plot " << i << " Configuration [PLOT_" << i << "]" << std::endl;
		std::cout << "\t\t Description: " << plotConfigurations[i].GetDescription() << std::endl;
		std::cout << "\t\t Plot Type: " << plotConfigurations[i].GetPlotType().ToString() << std::endl;
		std::cout << "\t\t Display Style: " << plotConfigurations[i].GetDisplayStyle().ToString() << std::endl;
		std::cout << "\t\t Overlay Style: " << plotConfigurations[i].GetOverlayStyle().ToString() << std::endl;
		std::cout << "\t\t X Log: " << (plotConfigurations[i].IsXLog() ? "YES" : "NO") << std::endl;
		std::cout << "\t\t Y Log: " << (plotConfigurations[i].IsYLog() ? "YES" : "NO") << std::endl;
		std::cout << "\t\t Ratio Title: " << plotConfigurations[i].GetRatioTitle() << std::endl;
		std::cout << "\t\t Number of Ratios: " << plotConfigurations[i].GetNumberOfRatios() << std::endl;
		for(int j = 0; j < plotConfigurations[i].GetNumberOfRatios(); j++) {
			std::cout << "\t\t\t Ratio Style " << j << ": " << plotConfigurations[i].GetRatioStyle(j).ToString() << std::endl;
			std::cout << "\t\t\t Ratio " << j << ": " << plotConfigurations[i].GetRatio(j) << std::endl;
		}
		std::cout << std::endl;

		std::cout << "\t\t Number of Configuration Instances for Plot " << i << ": " << \
			plotConfigurations[i].GetNumberOfConfigurationInstances() << std::endl << std::endl;

		for(int j = 0; j < plotConfigurations[i].GetNumberOfConfigurationInstances(); j++) {
			SPXPlotConfigurationInstance tmp;
			tmp = plotConfigurations[i].GetPlotConfigurationInstance(j);

			std::cout << "\t\t\t PDF Directory: " << tmp.pdfDirectory << std::endl;
			std::cout << "\t\t\t Data Directory: " << tmp.dataDirectory << std::endl;
			std::cout << "\t\t\t Grid Directory: " << tmp.gridDirectory << std::endl << std::endl;
			std::cout << "\t\t\t Data Steering File " << j << ": " << tmp.dataSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t Grid Steering File " << j << ": " << tmp.gridSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t PDF Steering File " << j << ": " << tmp.pdfSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t Data Marker Style " << j << ": " << tmp.dataMarkerStyle << std::endl;
			std::cout << "\t\t\t Data Marker Color " << j << ": " << tmp.dataMarkerColor << std::endl;
			std::cout << "\t\t\t PDF Fill Style " << j << ": " << tmp.pdfFillStyle << std::endl;
			std::cout << "\t\t\t PDF Fill Color " << j << ": " << tmp.pdfFillColor << std::endl;
			std::cout << "\t\t\t PDF Marker Style " << j << ": " << tmp.pdfMarkerStyle << std::endl;
			std::cout << "\t\t\t X Scale " << j << ": " << tmp.xScale << std::endl;
			std::cout << "\t\t\t Y Scale " << j << ": " << tmp.yScale << std::endl << std::endl;
		}
	}
}

unsigned int SPXSteeringFile::ParseNumberOfPlots(void) {
	std::string mn = "ParseNumberOfPlots: ";

	unsigned int plotNumber = 0;
	bool noMorePlots = false;
	std::string tmp;
	std::string plotSection;

	if(debug) std::cout << cn << mn << "Starting to look for plot sections..." << std::endl;

	do {
		std::ostringstream intStream;
		intStream << "PLOT_" << plotNumber;
		plotSection = intStream.str();
		if(debug) std::cout << cn << mn << "Formed plot section string: " << plotSection << std::endl;
		if(debug) std::cout << cn << mn << "Checking for existence of plot section: " << plotSection << std::endl;

		tmp = reader->Get(plotSection, "data_steering_files", "EMPTY");

		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "plot section: " << plotSection << " was not found. Number of plots found: " << plotNumber << std::endl;
			noMorePlots = true;
			break;
		} else {
			plotNumber++;
			if(debug) std::cout << cn << mn << "plot section: " << plotSection << " found. Current number of plots: " << plotNumber << std::endl;
		}

	} while(!noMorePlots);

	if(debug) std::cout << cn << mn << "Done searching for plots: " << plotNumber << " plots found" << std::endl;

	return plotNumber;
}

unsigned int SPXSteeringFile::ParseNumberOfRatios(unsigned int plotNumber) {
	std::string mn = "ParseNumberOfRatios: ";

	unsigned int ratioNumber = 0;
	bool noMoreRatios = false;
	std::string tmpStyle;
	std::string tmp;
	std::string plotSection;
	std::string ratioStyle;
	std::string ratio;

	std::ostringstream pss;
	pss << "PLOT_" << plotNumber;
	plotSection = pss.str();

	if(debug) std::cout << cn << mn << "Looking for ratios under plot [" << plotSection << "]" << std::endl;

	do {
		std::ostringstream rsss;
		rsss << "ratio_style_" << ratioNumber;
		ratioStyle = rsss.str();

		std::ostringstream rss;
		rss << "ratio_" << ratioNumber;
		ratio = rss.str();

		if(debug) std::cout << cn << mn << "Checking existence of " << ratioStyle << " AND " << ratio << std::endl;

		tmpStyle = reader->Get(plotSection, ratioStyle, "EMPTY");
		tmp = reader->Get(plotSection, ratio, "EMPTY");

		if(!tmpStyle.compare("EMPTY") || !tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "Ratio " << ratio << " or Ratio Style " << ratioStyle << " was not found. Number of ratios found: " << ratioNumber << std::endl;
			noMoreRatios = true;
			break;
		} else {
			ratioNumber++;
			if(debug) std::cout << cn << mn << "Ratio " << ratio << " found. Current number of ratios: " << ratioNumber << std::endl;
		}

	} while(!noMoreRatios);

	if(debug) std::cout << cn << mn << "Done searching for ratios for plot [" << plotSection << "]: " << ratioNumber << " ratios found" << std::endl;

	return ratioNumber;
}

void SPXSteeringFile::ParsePlotConfigurations(void) {
	std::string mn = "ParsePlotConfigurations: ";

	std::string tmp;
	bool xLog = false;
	bool yLog = false;

	std::string plotSection;
	std::vector<std::string> tmpVector;
	std::map<std::string, std::vector<std::string> > configurations;

	unsigned int numPlots = ParseNumberOfPlots();

	//Create plot configurations object for all plots found
	for(int i = 0; i < numPlots; i++) {

		tmp.clear();
		tmpVector.clear();
		configurations.clear();

		std::ostringstream intStream;

		//Form the section name from the plot number
		intStream << "plot_" << i;
		plotSection = intStream.str();
		if(debug) std::cout << cn << mn << "Formed plot section string: " << plotSection << std::endl;

		//Get the plot_type
		tmp = reader->Get(plotSection, "plot_type", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "plot_type", "You MUST specify the plot_type");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("plot_type", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[plot_type] = " << configurations["plot_type"].at(0) << std::endl;
		}

		//Get the data_directory
		tmp = reader->Get(plotSection, "data_directory", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No directory specified: Defaulting to \".\"" << std::endl;
			tmp = ".";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				if(debug) {
					std::cout << cn << mn << "data_directory configuration string: " << tmp << " parsed into:" << std::endl;
					for(int j = 0; j < tmpVector.size(); j++) {
						std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
					}
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_directory", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[data_directory] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_directory"]) << std::endl;
		}

		//Get the grid_directory
		tmp = reader->Get(plotSection, "grid_directory", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No directory specified: Defaulting to \".\"" << std::endl;
			tmp = ".";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				if(debug) {
					std::cout << cn << mn << "grid_directory configuration string: " << tmp << " parsed into:" << std::endl;
					for(int j = 0; j < tmpVector.size(); j++) {
						std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
					}
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_directory", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[grid_directory] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_directory"]) << std::endl;
		}

		//Get the pdf_directory
		tmp = reader->Get(plotSection, "pdf_directory", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No directory specified: Defaulting to \".\"" << std::endl;
			tmp = ".";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				if(debug) {
					std::cout << cn << mn << "pdf_directory configuration string: " << tmp << " parsed into:" << std::endl;
					for(int j = 0; j < tmpVector.size(); j++) {
						std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
					}
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_directory", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[pdf_directory] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_directory"]) << std::endl;
		}

		//Get the data_steering_files
		tmp = reader->Get(plotSection, "data_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "data_steering_files", "You MUST specify the data_steering_files");
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_steering_files", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[data_steering_files] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_steering_files"]) << std::endl;
		}

		//Get the grid_steering_files
		tmp = reader->Get(plotSection, "grid_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "grid_steering_files", "You MUST specify the grid_steering_files");
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "grid_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_steering_files", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[grid_steering_files] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_steering_files"]) << std::endl;
		}

		//Get the pdf_steering_files
		tmp = reader->Get(plotSection, "pdf_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "pdf_steering_files", "You MUST specify the pdf_steering_files");
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_steering_files", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_steering_files] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_steering_files"]) << std::endl;
		}

		//Get the data_marker_style
		tmp = reader->Get(plotSection, "data_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			std::cout << cn << mn << "WARNING: No plot option for data_marker_color found: Defaulting to pre-defined settings (" << DEFAULT_DATA_MARKER_STYLE << ")" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_marker_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[data_marker_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_marker_style"]) << std::endl;
		}

		//Get the data_marker_color
		tmp = reader->Get(plotSection, "data_marker_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			std::cout << cn << mn << "WARNING: No plot option for data_marker_color found: Defaulting to pre-defined settings (" << DEFAULT_DATA_MARKER_COLOR << ")" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_marker_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_marker_color", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[data_marker_color] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_marker_color"]) << std::endl;
		}

		//Get the pdf_fill_style
		tmp = reader->Get(plotSection, "pdf_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(plotBand) {
				std::cout << cn << mn << "WARNING: No plot option for pdf_fill_style found, but plot_band = true: Defaulting to pdf steering file settings" << std::endl;
			} else {
				if(debug) std::cout << cn << mn << "No plot option for pdf_fill_style found" << std::endl;
			}
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_fill_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_fill_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_fill_style"]) << std::endl;
		}

		//Get the pdf_fill_color
		tmp = reader->Get(plotSection, "pdf_fill_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(plotBand) {
				std::cout << cn << mn << "WARNING: No plot option for pdf_fill_color found, but plot_band = true: Defaulting to pdf steering file settings" << std::endl;
			} else {
				if(debug) std::cout << cn << mn << "No plot options for pdf_fill_color found" << std::endl;
			}
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_fill_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_fill_color", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_fill_color] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_fill_color"]) << std::endl;
		}

		//Get the pdf_marker_style
		tmp = reader->Get(plotSection, "pdf_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(plotMarker) {
				std::cout << cn << mn << "WARNING: No plot option for pdf_marker_style found, but plot_marker = true: Defaulting to pdf steering file settings" << std::endl;
			} else {
				if(debug) std::cout << cn << mn << "No plot option for pdf_marker_style found" << std::endl;
			}
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_marker_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_marker_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_marker_style"]) << std::endl;
		}

		//Get the x_scale
		tmp = reader->Get(plotSection, "x_scale", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No plot option for x_scale was specified" << std::endl;
			tmp = "1.0";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "x_scale configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("x_scale", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[x_scale] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["x_scale"]) << std::endl;
		}

		//Get the y_scale
		tmp = reader->Get(plotSection, "y_scale", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No plot option for y_scale was specified" << std::endl;
			tmp = "1.0";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "y_scale configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			if(Test::TestFeatures) std::cin.get();

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("y_scale", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[y_scale] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["y_scale"]) << std::endl;
		}

		//Get the x_log
		tmp = reader->Get(plotSection, "x_log", "EMPTY");
		if(tmp.compare("EMPTY") != 0) {
			xLog = reader->GetBoolean(plotSection, "x_log", false);
			if(debug) std::cout << cn << mn << "X Log successfully parsed: " << (xLog ? "ON" : "OFF") << std::endl;
		} else {
			xLog = false;
			if(debug) std::cout << cn << mn << "X Log not specified: defaulting to OFF" << std::endl;
		}
		{
			if(xLog) {
				tmp = std::string("true");
			} else {
				tmp = std::string("false");
			}

			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("x_log", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[x_log] = " << configurations["x_log"].at(0) << std::endl;
		}

		//Get the y_log
		tmp = reader->Get(plotSection, "y_log", "EMPTY");
		if(tmp.compare("EMPTY") != 0) {
			yLog = reader->GetBoolean(plotSection, "y_log", false);
			if(debug) std::cout << cn << mn << "Y Log successfully parsed: " << (yLog ? "ON" : "OFF") << std::endl;
		} else {
			yLog = false;
			if(debug) std::cout << cn << mn << "Y Log not specified: defaulting to OFF" << std::endl;
		}
		{
			if(yLog) {
				tmp = std::string("true");
			} else {
				tmp = std::string("false");
			}

			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("y_log", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[y_log] = " << configurations["y_log"].at(0) << std::endl;
		}

		//Get the desc
		tmp = reader->Get(plotSection, "desc", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "desc", "You MUST specify the desc");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("desc", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[desc] = " << configurations["desc"].at(0) << std::endl;
		}

		//Get the display_style
		tmp = reader->Get(plotSection, "display_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "display_style", "You MUST specify the display_style");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("display_style", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[display_style] = " << configurations["display_style"].at(0) << std::endl;
		}

		//Get the overlay_style
		tmp = reader->Get(plotSection, "overlay_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "overlay_style", "You MUST specify the overlay_style");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("overlay_style", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[overlay_style] = " << configurations["overlay_style"].at(0) << std::endl;
		}

		//Get the ratio_title
		tmp = reader->Get(plotSection, "ratio_title", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "ratio_title was not specified: Defaulting to \"Ratio\"" << std::endl;
			tmp = std::string("Ratio");
		}
		{
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("ratio_title", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[ratio_title] = " << configurations["ratio_title"].at(0) << std::endl;
		}

		//Get the ratio styles and ratios
		unsigned int numberOfRatios = ParseNumberOfRatios(i);
		std::string ratioStyle;
		std::string ratio;
		std::vector<std::string> rsVector;
		std::vector<std::string> rVector;

		if(debug) std::cout << cn << mn << "numberOfRatios = " << numberOfRatios << std::endl;

		for(int j = 0; j < numberOfRatios; j++) {

			std::ostringstream rsss;
			std::ostringstream rss;
			rsss << "ratio_style_" << j;
			rss << "ratio_" << j;
			ratioStyle = rsss.str();
			ratio = rss.str();

			if(debug) std::cout << cn << mn << "Getting [" << plotSection << "]: " << ratioStyle << std::endl;
			tmp = reader->Get(plotSection, ratioStyle, "EMPTY");
			if(debug) std::cout << "\t Found: " << tmp << std::endl;
			rsVector.push_back(tmp);

			if(debug) std::cout << cn << mn << "Getting [" << plotSection << "]: " << ratio << std::endl;
			tmp = reader->Get(plotSection, ratio, "EMPTY");
			if(debug) std::cout << "\t Found: " << tmp << std::endl;
			rVector.push_back(tmp);
		}

		//Insert the ratio and ratio styles vectors into the map
		configurations.insert(std::pair<std::string, std::vector<std::string> >("ratio_style", rsVector));
		configurations.insert(std::pair<std::string, std::vector<std::string> >("ratio", rVector));

		//Create a new plotConfigurations object with configurations vector (plotconfigurations constructor will parse it)
		try {
			SPXPlotConfiguration pc = SPXPlotConfiguration(configurations, i);

			//Make sure it is valid and non-empty, and add it to plot vector
			if(!pc.IsEmpty() && pc.IsValid()) {
				plotConfigurations.push_back(pc);
				if(debug) std::cout << cn << mn << "Successfully added " << plotSection << " to the plotConfigurations vector" << std::endl;
			} else {
				throw SPXINIParseException(plotSection, "SPXPlotConfiguration object is empty or invalid");
			}
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			std::ostringstream oss;
			oss << "[PLOT_" << i << "]";
			throw SPXParseException("Unable to parse plot configuration: " + oss.str());
		}
	}

	if(debug) std::cout << cn << mn << "Successfully parsed and added " << numPlots << " plot configurations" << std::endl;
}

void SPXSteeringFile::Parse(void) {
	std::string mn = "Parse: ";

	if(filename.empty()) {
		throw SPXFileIOException(filename, "Empty file string \"\" was given");
	}

	//Initialize reader
	reader = new INIReader(filename);

	if(reader->ParseError() < 0) {
		delete reader;

    	throw SPXFileIOException(filename, "INIReader::INIReader(): ParseError generated when parsing file");
	}

	std::string tmp;

	//General configurations [GEN]
	debug = reader->GetBoolean("GEN", "debug", debug);

	//Set Defaults
	this->SetDefaults();

	//Enable all debug configurations if debug is on
	if(debug) {
		std::cout << cn << mn << "Debug is ON" << std::endl;
		SPXPlotType::SetDebug(true);
		SPXPlotConfiguration::SetDebug(true);
		SPXRatioStyle::SetDebug(true);
		SPXOverlayStyle::SetDebug(true);
		SPXDisplayStyle::SetDebug(true);
		SPXPDFBandType::SetDebug(true);
		SPXPDFErrorType::SetDebug(true);
		SPXPDFErrorSize::SetDebug(true);
	}

	//Graphing configurations [GRAPH]
	plotBand = reader->GetBoolean("GRAPH", "plot_band", plotBand);
	plotErrorTicks = reader->GetBoolean("GRAPH", "plot_error_ticks", plotErrorTicks);
	plotMarker = reader->GetBoolean("GRAPH", "plot_marker", plotMarker);
	plotStaggered = reader->GetBoolean("GRAPH", "plot_staggered", plotStaggered);
	matchBinning = reader->GetBoolean("GRAPH", "match_binning", matchBinning);
	gridCorr = reader->GetBoolean("GRAPH", "grid_corr", gridCorr);
	labelSqrtS = reader->GetBoolean("GRAPH", "label_sqrt_s", labelSqrtS);
	xLegend = reader->GetReal("GRAPH", "x_legend", xLegend);
	yLegend = reader->GetReal("GRAPH", "y_legend", xLegend);

	yOverlayMin = reader->GetReal("GRAPH", "y_overlay_min", yOverlayMin);
	yOverlayMax = reader->GetReal("GRAPH", "y_overlay_max", yOverlayMax);
	yRatioMin = reader->GetReal("GRAPH", "y_ratio_min", yRatioMin);
	yRatioMax = reader->GetReal("GRAPH", "y_ratio_max", yRatioMax);

	//Attempt to parse plot configurations
	try {
		this->ParsePlotConfigurations();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXParseException("Could not parse plots: Verify correct plot configuration syntax");
	}
}

void SPXSteeringFile::PrintDataSteeringFiles(void) {
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {
			SPXDataSteeringFile &dataSteeringFile = plotConfigurations.at(i).GetPlotConfigurationInstance(j).dataSteeringFile;
			dataSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParseDataSteeringFiles(void) {
	std::string mn = "ParseDataSteeringFiles: ";

	//Loop through all plot configurations instances for each plot option
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {

			SPXPlotConfigurationInstance &pci = plotConfigurations.at(i).GetPlotConfigurationInstance(j);
			SPXDataSteeringFile &dataSteeringFile = pci.dataSteeringFile;

			//Attempt to parse the Data Steering File
			try {
				dataSteeringFile.Parse();

				//Prepend the data directory onto the steering file's data path
				dataSteeringFile.PrependDataFile(pci.dataDirectory);
				if(debug) std::cout << cn << mn << "Successfully prepended directory \"" << pci.dataDirectory << "\" onto data file" << std::endl;
				if(debug) std::cout << cn << mn << "Resulting data filepath: \"" << dataSteeringFile.GetDataFile() << "\"" << std::endl;

				//Use default marker style or marker color if currently empty
				if(pci.dataMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Data Marker Style was empty: Defaulting to Circle (20)" << std::endl;
					pci.dataMarkerStyle = DEFAULT_DATA_MARKER_STYLE;	//DEFAULT TO 20 (circle)
				}

				if(pci.dataMarkerColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Data Marker Color was empty: Defaulting to Blac (1)" << std::endl;
					pci.dataMarkerColor = DEFAULT_DATA_MARKER_COLOR;	//DEFAULT TO BLACK
				}

			//Update PCI with new data
			SPXPlotConfigurationInstance &pcim = plotConfigurations.at(i).GetPlotConfigurationInstance(dataSteeringFile.GetFilename());
			pcim.dataMarkerStyle = pci.dataMarkerStyle;
			pcim.dataMarkerColor = pci.dataMarkerColor;

			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse the Data Steering File: " << dataSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}
}

void SPXSteeringFile::PrintGridSteeringFiles(void) {
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {
			SPXGridSteeringFile &gridSteeringFile = plotConfigurations.at(i).GetPlotConfigurationInstance(j).gridSteeringFile;
			gridSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParseGridSteeringFiles(void) {
	std::string mn = "ParseGridSteeringFiles: ";

	//Loop through all plot configurations instances for each plot option
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {

			SPXPlotConfigurationInstance &pci = plotConfigurations.at(i).GetPlotConfigurationInstance(j);
			SPXGridSteeringFile &gridSteeringFile = pci.gridSteeringFile;

			//Attempt to parse the Grid Steering File
			try {
				gridSteeringFile.Parse();

				gridSteeringFile.PrependGridFilepath(pci.gridDirectory);
				if(debug) std::cout << cn << mn << "Successfully prepended directory \"" << pci.gridDirectory << "\" onto grid file" << std::endl;
				if(debug) std::cout << cn << mn << "Resulting grid filepath: \"" << gridSteeringFile.GetGridFilepath() << "\"" << std::endl;
			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse the Grid Steering File: " << gridSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}
}

void SPXSteeringFile::PrintPDFSteeringFiles(void) {
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {
			SPXPDFSteeringFile &pdfSteeringFile = plotConfigurations.at(i).GetPlotConfigurationInstance(j).pdfSteeringFile;
			pdfSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParsePDFSteeringFiles(void) {
	std::string mn = "ParsePDFSteeringFiles: ";

	//Loop through all plot configurations instances for each plot option
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {

			SPXPlotConfigurationInstance &pci = plotConfigurations.at(i).GetPlotConfigurationInstance(j);
			SPXPDFSteeringFile &pdfSteeringFile = pci.pdfSteeringFile;

			//Attempt to parse the PDF Steering File
			try {
				pdfSteeringFile.Parse();

				//Use default pdfFillStyle, pdfFillColor, and pdfMarkerStyle if currently empty
				if(pci.pdfFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" PDF Fill Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillStyle() << std::endl;
					pci.pdfFillStyle = pdfSteeringFile.GetFillStyle();
				}

				if(pci.pdfFillColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" PDF Fill Color was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillColor() << std::endl;
					pci.pdfFillColor = pdfSteeringFile.GetFillColor();
				}

				if(pci.pdfMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" PDF Marker Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetMarkerStyle() << std::endl;
					pci.pdfMarkerStyle = pdfSteeringFile.GetMarkerStyle();
				}

				//Update PCI with new data
				SPXPlotConfigurationInstance &pcim = plotConfigurations.at(i).GetPlotConfigurationInstance(pdfSteeringFile.GetFilename());
				pcim.pdfFillStyle = pci.pdfFillStyle;
				pcim.pdfFillColor = pci.pdfFillColor;
				pcim.pdfMarkerStyle = pci.pdfMarkerStyle;

			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse the PDF Steering File: " << pdfSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}
}
