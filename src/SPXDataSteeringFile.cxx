//************************************************************/
//
//	Data Steering File Implementation
//
//	Implements the SPXDataSteeringFile class, which standardizes the
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

#include <iostream>
#include <fstream>
#include <sstream>

#include "SPXDataSteeringFile.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXDataSteeringFile::";

void SPXDataSteeringFile::SetDefaults(void) {
	std::string mn = "SetDefaults: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Setting default Data Steering File data" << std::endl;

	name.clear();
	if(debug) std::cout << cn << mn << "name set to default: \" \"" << std::endl;

	comments.clear();
	if(debug) std::cout << cn << mn << "comments set to default: \" \"" << std::endl;

	experiment.clear();
	if(debug) std::cout << cn << mn << "experiment set to default: \" \"" << std::endl;

	reaction.clear();
	if(debug) std::cout << cn << mn << "reaction set to default: \" \"" << std::endl;

	datasetYear.clear();
	if(debug) std::cout << cn << mn << "datasetYear set to default: \" \"" << std::endl;

	datasetLumi.clear();
	if(debug) std::cout << cn << mn << "datasetLumi set to default: \" \"" << std::endl;

	referenceJournalName.clear();
	if(debug) std::cout << cn << mn << "referenceJournalName set to default: \" \"" << std::endl;

	referenceJournalYear.clear();
	if(debug) std::cout << cn << mn << "referenceJournalYear set to default: \" \"" << std::endl;

	referenceArXivNumber.clear();
	if(debug) std::cout << cn << mn << "referenceArXivNumber set to default: \" \"" << std::endl;

	referenceArXivYear.clear();
	if(debug) std::cout << cn << mn << "referenceArXivYear set to default: \" \"" << std::endl;

	publicationStatus.clear();
	if(debug) std::cout << cn << mn << "publicationStatus set to default: \" \"" << std::endl;

	sqrtS = 0;
	if(debug) std::cout << cn << mn << "sqrtS set to default: \" \"" << std::endl;

	legendLabel.clear();
	if(debug) std::cout << cn << mn << "legendLabel set to default: \" \"" << std::endl;

	xLabel.clear();
	if(debug) std::cout << cn << mn << "xLabel set to default: \" \"" << std::endl;

	yLabel.clear();
	if(debug) std::cout << cn << mn << "yLabel set to default: \" \"" << std::endl;

	xUnits.clear();
	if(debug) std::cout << cn << mn << "xUnits set to default: \" \"" << std::endl;

	yUnits.clear();
	if(debug) std::cout << cn << mn << "yUnits set to default: \" \"" << std::endl;

	yBinWidthUnits.clear();
	if(debug) std::cout << cn << mn << "yBinWidthUnits set to default: \" \"" << std::endl;

	jetAlgorithmLabel.clear();
	if(debug) std::cout << cn << mn << "jetAlgorithmLabel set to default: \" \"" << std::endl;

	jetAlgorithmRadius = 0;
	if(debug) std::cout << cn << mn << "jetAlgorithmRadius set to default: \" \"" << std::endl;

	dataFormat = SPXDataFormat(DF_SPECTRUM);
	if(debug) std::cout << cn << mn << "dataFormat set to default: \"Spectrum\"" << std::endl;

	dataFilepath.clear();
	if(debug) std::cout << cn << mn << "dataFilepath set to default: \" \"" << std::endl;

	dividedByBinWidth = false;
	if(debug) std::cout << cn << mn << "dividedByBinWidth set to default: \"false\"" << std::endl;

	normalizedToTotalSigma = false;
	if(debug) std::cout << cn << mn << "normalizedToTotalSigma set to default: \"false\"" << std::endl;

	errorInPercent = false;
	if(debug) std::cout << cn << mn << "errorInPercent set to default: \"false\"" << std::endl;

	lumiScaleFactor = 1.0;
	if(debug) std::cout << cn << mn << "lumiScaleFactor set to default: \"1.0\"" << std::endl;
}

//Print the Data Steering File Data in a nice format
void SPXDataSteeringFile::Print(void) {
	std::cout << "Data Steering File: " << filename << std::endl;
	std::cout << "\t General Options [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl << std::endl;
	std::cout << "\t Description [DESC]" << std::endl;
	std::cout << "\t\t Name: " << name << std::endl;
	std::cout << "\t\t Comments: " << comments << std::endl;
	std::cout << "\t\t Reaction: " << reaction << std::endl;
	std::cout << "\t\t Dataset Year: " << datasetYear << std::endl;
	std::cout << "\t\t Dataset Luminosity: " << datasetLumi << std::endl;
	std::cout << "\t\t Reference Journal Name: " << referenceJournalName << std::endl;
	std::cout << "\t\t Reference Journal Year: " << referenceJournalYear << std::endl;
	std::cout << "\t\t Reference arXiv Number: " << referenceArXivNumber << std::endl;
	std::cout << "\t\t Reference arXiv Year: " << referenceArXivYear << std::endl;
	std::cout << "\t\t Publication Status: " << publicationStatus << std::endl << std::endl;
	std::cout << "\t Graphing Options [GRAPH]" << std::endl;
	std::cout << "\t\t Sqrt(s): " << sqrtS << std::endl;
	std::cout << "\t\t X Label: " << xLabel << std::endl;
	std::cout << "\t\t Y Label: " << yLabel << std::endl;
	std::cout << "\t\t X Units: " << xUnits << std::endl;
	std::cout << "\t\t Y Units: " << yUnits << std::endl;
	std::cout << "\t\t Y Bin Width Units: " << yBinWidthUnits << std::endl;
	std::cout << "\t\t Jet Algorithm Label: " << jetAlgorithmLabel << std::endl;
	std::cout << "\t\t Jet Algorithm Radius: " << jetAlgorithmRadius << std::endl << std::endl;
	std::cout << "\t Data Options [DATA]" << std::endl;
	std::cout << "\t\t Data Format: " << dataFormat.ToString() << std::endl;
	std::cout << "\t\t Data File: " << dataFilepath << std::endl;
	std::cout << "\t\t Data Divided by Bin Width? " << (dividedByBinWidth ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Data Normalized to Total Sigma? " << (normalizedToTotalSigma ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Errors given in percentages? " << (errorInPercent ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Luminosity Scale Factor: " << lumiScaleFactor << std::endl << std::endl;
}

//@TODO Create Correlation Matrix Class and parse correlation matrix here if there is one (correlation_matrix = true)!
//@TODO Update documentation with correlation matrix syntax and behavior
void SPXDataSteeringFile::Parse(void) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
	if(filename.empty()) {
		throw SPXFileIOException(filename, "Empty file string \"\" was given");
	}

	//Initialize reader
	reader = new INIReader(filename);

	if(reader->ParseError() < 0) {
		delete reader;

    	throw SPXFileIOException(filename, "INIReader::INIReader(): ParseError generated when parsing file");
	}

	//General Options [GEN]
	debug = reader->GetBoolean("GEN", "debug", debug);
	if(debug) std::cout << cn << mn << "Debug is ON" << std::endl;

	//Set Defaults
	this->SetDefaults();

	std::string tmp;

	//Description [DESC]
	tmp = reader->Get("DESC", "name", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Data name was not specified" << std::endl;
	} else {
		name = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Data Name: " << name << std::endl;
	}

	tmp = reader->Get("DESC", "comments", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Comments were not specified" << std::endl;
	} else {
		comments = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Comments: " << comments << std::endl;
	}

	tmp = reader->Get("DESC", "experiment", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Experiment was not specified" << std::endl;
	} else {
		experiment = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Experiment: " << experiment << std::endl;
	}

	tmp = reader->Get("DESC", "reaction", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reaction was not specified" << std::endl;
	} else {
		reaction = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Reaction: " << reaction << std::endl;
	}

	tmp = reader->Get("DESC", "data_set_year", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Dataset Year was not specified" << std::endl;
	} else {
		datasetYear = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Dataset Year: " << datasetYear << std::endl;
	}

	tmp = reader->Get("DESC", "data_set_lumi", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Dataset Luminosity was not specified" << std::endl;
	} else {
		datasetLumi = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Dataset Luminosity: " << datasetLumi << std::endl;
	}

	tmp = reader->Get("DESC", "reference_journal_name", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reference Journal Name was not specified" << std::endl;
	} else {
		referenceJournalName = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Reference Journal Name: " << referenceJournalName << std::endl;
	}

	tmp = reader->Get("DESC", "reference_journal_year", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reference Journal Year was not specified" << std::endl;
	} else {
		referenceJournalYear = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Reference Journal Year: " << referenceJournalYear << std::endl;
	}

	tmp = reader->Get("DESC", "reference_arxiv_number", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reference arXiv Number was not specified" << std::endl;
	} else {
		referenceArXivNumber = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Reference arXiv Number: " << referenceArXivNumber << std::endl;
	}

	tmp = reader->Get("DESC", "reference_arxiv_year", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reference arXiv Year was not specified" << std::endl;
	} else {
		referenceArXivYear = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Reference arXiv Year: " << referenceArXivYear << std::endl;
	}

	tmp = reader->Get("DESC", "publication_status", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Publication Status was not specified" << std::endl;
	} else {
		publicationStatus = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Publication Status: " << publicationStatus << std::endl;
	}

	//Graping Options [GRAPH]
	tmp = reader->Get("GRAPH", "sqrt_s", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "sqrt_s", "You MUST specify the sqrt_s");
	} else {
		sqrtS = reader->GetReal("GRAPH", "sqrt_s", sqrtS);
		if(debug) std::cout << cn << mn << "Successfully read Sqrt(s): " << sqrtS << std::endl;
	}

	tmp = reader->Get("GRAPH", "legend_label", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "legend_label", "You MUST specify the legend_label");
	} else {
		legendLabel = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Legend Label: " << legendLabel << std::endl;
	}

	tmp = reader->Get("GRAPH", "x_label", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "x_label", "You MUST specify the x_label");
	} else {
		xLabel = tmp;
		if(debug) std::cout << cn << mn << "Successfully read X Label: " << xLabel << std::endl;
	}

	tmp = reader->Get("GRAPH", "y_label", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "y_label", "You MUST specify the y_label");
	} else {
		yLabel = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Y Label: " << yLabel << std::endl;
	}

	tmp = reader->Get("GRAPH", "x_units", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "x_units", "You MUST specify the x_units");
	} else {
		xUnits = tmp;
		if(debug) std::cout << cn << mn << "Successfully read X Units: " << xUnits << std::endl;
	}

	tmp = reader->Get("GRAPH", "y_units", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "y_units", "You MUST specify the y_units");
	} else {
		yUnits = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Y Units: " << yUnits << std::endl;
	}

	tmp = reader->Get("GRAPH", "y_bin_width_units", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "y_bin_width_units", "You MUST specify the y_bin_width_units");
	} else {
		yBinWidthUnits = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Y Units: " << yBinWidthUnits << std::endl;
	}

	tmp = reader->Get("GRAPH", "jet_algorithm_label", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Jet Algorithm Label was not specified" << std::endl;
	} else {
		jetAlgorithmLabel = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Jet Algorithm Label: " << jetAlgorithmLabel << std::endl;
	}

	jetAlgorithmRadius = reader->GetInteger("GRAPH", "jet_algorithm_radius", jetAlgorithmRadius);

	//Data Options [DATA]
	tmp = reader->Get("DATA", "data_format", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("DATA", "data_format", "You MUST specify the data_format");
	} else {
		try {
			dataFormat.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;

			throw SPXINIParseException("DATA", "data_format", "Invalid data format string: " + tmp);
		}
	}

	tmp = reader->Get("DATA", "data_file", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("DATA", "data_file", "You MUST specify the data_file");
	} else {
		dataFilepath = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Data File: " << dataFilepath << std::endl;
	}

	dividedByBinWidth = reader->GetBoolean("DATA", "divided_by_bin_width", false);
	if(debug) std::cout << cn << mn << "Divided By Bin Width set to: " << (dividedByBinWidth ? "ON" : "OFF") << std::endl;

	normalizedToTotalSigma = reader->GetBoolean("DATA", "normalized_to_total_sigma", false);
	if(debug) std::cout << cn << mn << "Normalized to Total Sigma set to: " << (normalizedToTotalSigma ? "ON" : "OFF") << std::endl;

	errorInPercent = reader->GetBoolean("DATA", "error_in_percent", false);
	if(debug) std::cout << cn << mn << "Error in Percent set to: " << (errorInPercent ? "ON" : "OFF") << std::endl;

	lumiScaleFactor = reader->GetReal("DATA", "lumi_scale_factor", lumiScaleFactor);
	if(lumiScaleFactor == 0) {
		std::cerr << cn << mn << "WARNING: Luminosity Scale Factor read as \"0.0\", data/convolute may be scaled to zero" << std::endl;
	}
}
