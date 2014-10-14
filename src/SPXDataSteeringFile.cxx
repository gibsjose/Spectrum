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
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		06.10.2014
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
	
	if(debug) std::cout << cn << mn << "Setting default Data Steering File data" << std::endl;
	
	name.clear();
	if(debug) std::cout << cn << mn << "name set to default: \" \"" << std::endl;

	year = 0;
	if(debug) std::cout << cn << mn << "year set to default: \" \"" << std::endl;

	experiment.clear();
	if(debug) std::cout << cn << mn << "experiment set to default: \" \"" << std::endl;

	reaction.clear();
	if(debug) std::cout << cn << mn << "reaction set to default: \" \"" << std::endl;

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

	xScale = 1.0;
	if(debug) std::cout << cn << mn << "xScale set to default: \"1.0\"" << std::endl;
	
	yScale = 1.0;
	if(debug) std::cout << cn << mn << "yScale set to default: \"1.0\"" << std::endl;

	frameXLog = false;
	if(debug) std::cout << cn << mn << "frameXLog set to default: \"false\"" << std::endl;

	frameYLog = false;
	if(debug) std::cout << cn << mn << "frameYLog set to default: \"false\"" << std::endl;
	

	/*	@TODO Figure out why frame X/Y Min/Max is here...
	frameXMin = 0.0;
	if(debug) std::cout << cn << mn << "frameXMin set to default: \"0.0\"" << std::endl;
	
	frameXMax = 0.0;
	if(debug) std::cout << cn << mn << "frameXMax set to default: \"0.0\"" << std::endl;
	*/

	jetAlgorithmLabel.clear();
	if(debug) std::cout << cn << mn << "jetAlgorithmLabel set to default: \" \"" << std::endl;

	jetAlgorithmNumber = 0;
	if(debug) std::cout << cn << mn << "jetAlgorithmNumber set to default: \" \"" << std::endl;

	dataFormat = SPXDataFormat(DF_SPECTRUM_T1S);
	if(debug) std::cout << cn << mn << "dataFormat set to default: \"Spectrum T1S\"" << std::endl;

	dataFilepath.clear();
	if(debug) std::cout << cn << mn << "dataFilepath set to default: \" \"" << std::endl;

	dividedByBinWidth = false;
	if(debug) std::cout << cn << mn << "dividedByBinWidth set to default: \"false\"" << std::endl;

	normalizedToTotalSigma = false;
	if(debug) std::cout << cn << mn << "normalizedToTotalSigma set to default: \"false\"" << std::endl;

	errorInPercent = false;
	if(debug) std::cout << cn << mn << "errorInPercent set to default: \"false\"" << std::endl;
}

//Print the Data Steering File Data in a nice format
void SPXDataSteeringFile::Print(void) {	
	std::cout << "Data Steering File: " << filename << std::endl;
	std::cout << "\t General Options [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl << std::endl;
	std::cout << "\t Description [DESC]" << std::endl;
	std::cout << "\t\t Name: " << name << std::endl;
	std::cout << "\t\t Year: " << year << std::endl;
	std::cout << "\t\t Reaction: " << reaction << std::endl << std::endl;
	std::cout << "\t Graphing Options [GRAPH]" << std::endl;
	std::cout << "\t\t Sqrt(s): " << sqrtS << std::endl;
	std::cout << "\t\t X Label: " << xLabel << std::endl;
	std::cout << "\t\t Y Label: " << yLabel << std::endl;
	std::cout << "\t\t X Units: " << xUnits << std::endl;
	std::cout << "\t\t Y Units: " << yUnits << std::endl;
	std::cout << "\t\t X Scale: " << xScale << std::endl;
	std::cout << "\t\t Y Scale: " << yScale << std::endl;
	std::cout << "\t\t Logarithmic X-Axis: " << (frameXLog ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Logarithmic Y-Axis: " << (frameYLog ? "ON" : "OFF") << std::endl;
	//std::cout << "\t\t Frame X Min: " << frameXMin << std::endl;
	//std::cout << "\t\t Frame Y Min: " << frameYMin << std::endl;
	std::cout << "\t\t Jet Algorithm Label: " << jetAlgorithmLabel << std::endl;
	std::cout << "\t\t Jet Algorithm Number: " << jetAlgorithmNumber << std::endl << std::endl;
	std::cout << "\t Data Options [DATA]" << std::endl;
	std::cout << "\t\t Data Format: " << dataFormat.ToString() << std::endl;
	std::cout << "\t\t Data File: " << dataFilepath << std::endl;
	std::cout << "\t\t Data Divided by Bin Width? " << (dividedByBinWidth ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Data Normalized to Total Sigma? " << (normalizedToTotalSigma ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Errors given in percentages? " << (errorInPercent ? "YES" : "NO") << std::endl << std::endl;
}

//@TODO Create Correlation Matrix Class and parse correlation matrix here if there is one (correlation_matrix = true)!
//@TODO Update documentation with correlation matrix syntax and behavior
void SPXDataSteeringFile::Parse(void) {
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

	year = reader->GetInteger("DESC", "year", year);

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

	xScale = reader->GetReal("GRAPH", "x_scale", xScale);
	yScale = reader->GetReal("GRAPH", "y_scale", yScale);

	frameXLog = reader->GetBoolean("GRAPH", "frame_x_log", frameXLog);
	frameYLog = reader->GetBoolean("GRAPH", "frame_y_log", frameYLog);

	/*
	frameXMin = reader->GetReal("GRAPH", "frame_x_min", frameXMin);
	frameXMax = reader->GetReal("GRAPH", "frame_x_max", frameXMax);
	*/

	tmp = reader->Get("GRAPH", "jet_algorithm_label", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Jet Algorithm Label was not specified" << std::endl;
	} else {
		jetAlgorithmLabel = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Jet Algorithm Label: " << jetAlgorithmLabel << std::endl;
	}

	jetAlgorithmNumber = reader->GetInteger("GRAPH", "jet_algorithm_number", jetAlgorithmNumber);

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
}
