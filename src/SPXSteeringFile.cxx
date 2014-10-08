//************************************************************/
//
//	Steering File Class
//
//	Implements the SPXSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		19.09.2014
//
//************************************************************/

#include "SPXSteeringFile.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXSteeringFile::";

void SPXSteeringFile::SetDefaults(void) {
	std::string mn = "SetDefaults: ";
	
	if(debug) std::cout << cn << mn << "Setting default Steering File data" << std::endl;
	
	numberOfFrames = 0;
	if(debug) std::cout << cn << mn << "numberOfFrames set to default: \"0\"" << std::endl;
	
	pdfDirectory = ".";
	if(debug) std::cout << cn << mn << "pdfDirectory set to default: \".\"" << std::endl;

	dataDirectory = ".";
	if(debug) std::cout << cn << mn << "dataDirectory set to default: \".\"" << std::endl;
	
	gridDirectory = ".";
	if(debug) std::cout << cn << mn << "gridDirectory set to default: \".\"" << std::endl;
	
	plotBand = false;
	if(debug) std::cout << cn << mn << "plotBand set to default: \"false\"" << std::endl;
	
	plotErrorTicks = false;
	if(debug) std::cout << cn << mn << "plotErrorTicks set to default: \"false\"" << std::endl;
	
	plotMarker = false;
	if(debug) std::cout << cn << mn << "plotMarker set to default: \"false\"" << std::endl;
	
	plotStaggered = false;
	if(debug) std::cout << cn << mn << "plotStaggered set to default: \"false\"" << std::endl;
	
	labelSqrtS = false;
	if(debug) std::cout << cn << mn << "labelSqrtS set to default: \"false\"" << std::endl;
	
	xLegend = 0.9;
	if(debug) std::cout << cn << mn << "xLegend set to default: \"0.9\"" << std::endl;
	
	yLegend = 0.9;
	if(debug) std::cout << cn << mn << "yLegend set to default: \"0.9\"" << std::endl;
	
	ratioTitle = std::string();
	if(debug) std::cout << cn << mn << "ratioTitle set to default: \" \"" << std::endl;
	
	ratioStyle = SPXRatioStyle();
	if(debug) std::cout << cn << mn << "ratioStyle has no default: initialized as empty" << std::endl;
	
	overlayStyle = SPXOverlayStyle(OS_DATA | OS_REFERENCE);
	if(debug) std::cout << cn << mn << "overlayStyle set to default: \"" << overlayStyle.ToString() << "\"" << std::endl;
	
	displayStyle = SPXDisplayStyle(DS_OVERLAY | DS_RATIO);
	if(debug) std::cout << cn << mn << "displayStyle set to default: \"" << displayStyle.ToString() << "\"" << std::endl;
	
	yOverlayMin = 0;
	if(debug) std::cout << cn << mn << "yOverlayMin set to default: \"0\"" << std::endl;
	
	yOverlayMax = 0;
	if(debug) std::cout << cn << mn << "yOverlayMax set to default: \"0\"" << std::endl;
	
	yRatioMin = 0;
	if(debug) std::cout << cn << mn << "yRatioMin set to default: \"0\"" << std::endl;
	
	yRatioMax = 0;
	if(debug) std::cout << cn << mn << "yRatioMax set to default: \"0\"" << std::endl;
	
	pdfSteeringFiles.clear();
	if(debug) std::cout << cn << mn << "pdfSteeringFiles set to default: \" \"" << std::endl;
	
	pdfFillStyle = STYLE_EMPTY;
	if(debug) std::cout << cn << mn << "pdfFillStyle set to default: \" \"" << std::endl;
	
	pdfFillColor = COLOR_EMPTY;
	if(debug) std::cout << cn << mn << "pdfFillColor set to default: \" \"" << std::endl;
	
	pdfMarkerStyle = STYLE_EMPTY;
	if(debug) std::cout << cn << mn << "pdfMarkerStyle set to default: \" \"" << std::endl;
	
	pdfBandType = SPXPDFBandType();
	if(debug) std::cout << cn << mn << "pdfBandType set to default: \" \"" << std::endl;
	
	pdfErrorType = SPXPDFErrorType();
	if(debug) std::cout << cn << mn << "pdfErrorType set to default: \" \"" << std::endl;
	
	pdfErrorSize = SPXPDFErrorSize();
	if(debug) std::cout << cn << mn << "pdfErrorSize set to default: \" \"" << std::endl;
}

//Print the Steering File Data in a nice format
void SPXSteeringFile::Print(void) {
	std::cout << "Steering File: " << filename << std::endl;
	std::cout << "\t General Options [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t PDF Directory: " << pdfDirectory << std::endl;
	std::cout << "\t\t Data Directory: " << dataDirectory << std::endl;
	std::cout << "\t\t Grid Directory: " << gridDirectory << std::endl << std::endl;
	std::cout << "\t Graphing Options [GRAPH]" << std::endl;
	std::cout << "\t\t Plot Band is: " << (plotBand ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Error Ticks is: " << (plotErrorTicks ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Marker is: " << (plotMarker ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Staggered is: " << (plotStaggered ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Label Sqrt(s) on Leggend: " << (labelSqrtS ? "YES" : "NO") << std::endl;
	std::cout << "\t\t X Legend: " << xLegend << std::endl;
	std::cout << "\t\t Y Legend: " << yLegend << std::endl;
	std::cout << "\t\t Ratio Title: " << ratioTitle << std::endl;
	std::cout << "\t\t Ratio Style: " << ratioStyle.ToString() << std::endl;
	std::cout << "\t\t Overlay Style: " << overlayStyle.ToString() << std::endl;
	std::cout << "\t\t Display Style: " << displayStyle.ToString() << std::endl;
	std::cout << "\t\t Y Overlay Min: " << yOverlayMin << std::endl;
	std::cout << "\t\t Y Overlay Max: " << yOverlayMax << std::endl;
	std::cout << "\t\t Y Ratio Min: " << yRatioMin << std::endl;
	std::cout << "\t\t Y Ratio Max: " << yRatioMax << std::endl << std::endl;
	std::cout << "\t PDF Options [PDF]" << std::endl;
	std::vector<std::string> tmpVector = this->GetPDFSteeringFilepaths();
	std::cout << "\t\t PDF Steering Files: " << SPXUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
	std::cout << "\t\t PDF Fill Style: " << (pdfFillStyle == STYLE_EMPTY ? "UNSET: " : "") << pdfFillStyle << std::endl;
	std::cout << "\t\t PDF Fill Color: " << (pdfFillColor == COLOR_EMPTY ? "UNSET: " : "") << pdfFillColor << std::endl;
	std::cout << "\t\t PDF Marker Style: " << (pdfMarkerStyle == STYLE_EMPTY ? "UNSET: " : "") << pdfMarkerStyle << std::endl;
	std::cout << "\t\t PDF Band Type: " << pdfBandType.ToString() << std::endl;
	std::cout << "\t\t PDF Error Type: " << pdfErrorType.ToString() << std::endl;
	std::cout << "\t\t PDF Error Size: " << pdfErrorSize.ToString() << std::endl;
	std::cout << "" << std::endl;
	std::cout << "\t Frame Options" << std::endl;
	std::cout << "\t\t Number of Frames: " << numberOfFrames << std::endl << std::endl;
	
	for(int i = 0; i < numberOfFrames; i++) {
		std::cout << "\t\t Frame " << i << " Options [FRAME_" << i << "]" << std::endl;
		std::cout << "\t\t\t Description: " << frameOptions[i].GetDescription() << std::endl << std::endl;
		
		for(int j = 0; j < frameOptions[i].GetNumberOfOptionsInstances(); j++) {
			SPXFrameOptionsInstance tmp;
			
			try {
				tmp = frameOptions[i].GetFrameOptionsInstance(j);
			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;
				
				//@TODO An SPXOutOfRangeException occurred here... what do I do now?
			}
			std::cout << "\t\t\t Data Steering File " << j << ": " << tmp.dataSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t Grid Steering File " << j << ": " << tmp.gridSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t Marker Style " << j << ": " << tmp.markerStyle << std::endl;
			std::cout << "\t\t\t Marker Color " << j << ": " << tmp.markerColor << std::endl;
			std::cout << "\t\t\t Reference Line Style " << j << ": " << tmp.refLineStyle << std::endl;
			std::cout << "\t\t\t Reference Line Color " << j << ": " << tmp.refLineColor << std::endl << std::endl;
		}
	}			
}

unsigned int SPXSteeringFile::ParseNumberOfFrames(void) {
	std::string mn = "ParseNumberOfFrames: ";
	
	unsigned int frameNumber = 0;
	bool noMoreFrames = false;
	std::string tmp;
	std::string frameSection;
	
	if(debug) std::cout << cn << mn << "Starting to look for frame sections..." << std::endl;
	
	do {
		std::ostringstream intStream;
		intStream << "FRAME_" << frameNumber;
		frameSection = intStream.str();
		if(debug) std::cout << cn << mn << "Formed frame section string: " << frameSection << std::endl;
		if(debug) std::cout << cn << mn << "Checking for existence of frame section: " << frameSection << std::endl;
		
		tmp = reader->Get(frameSection, "data_steering_files", "EMPTY");
		
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "Frame section: " << frameSection << " was not found. Number of frames found: " << frameNumber << std::endl;
			noMoreFrames = true;
			break;
		} else {
			frameNumber++;
			if(debug) std::cout << cn << mn << "Frame section: " << frameSection << " found. Current number of frames: " << frameNumber << std::endl;
		}	
	
	} while(!noMoreFrames);
	
	if(debug) std::cout << cn << mn << "Done searching for frames: " << frameNumber << " frames found" << std::endl;
	
	return frameNumber;
}

void SPXSteeringFile::ParseFrameOptions(unsigned int numFrames) {
	std::string mn = "ParseFrameOptions: ";
	
	std::string tmp;
	std::string desc;
	std::string frameSection;
	std::vector<std::string> tmpVector;
	std::vector<std::vector<std::string> > options;
	int numberOfOptionsInstances = 0;
	
	//Create frame options object for all frames found
	for(int i = 0; i < numFrames; i++) {
		
		desc.clear();
		tmp.clear();
		tmpVector.clear();
		options.clear();
		numberOfOptionsInstances = 0;
		
		std::ostringstream intStream;
		
		//Form the section name from the frame number
		intStream << "FRAME_" << i;
		frameSection = intStream.str();
		if(debug) std::cout << cn << mn << "Formed frame section string: " << frameSection << std::endl;
		
		//Get the data_steering_files
		tmp = reader->Get(frameSection, "data_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(frameSection, "data_steering_files", "You MUST specify the data_steering_files");
		} else {
			//Parse into vector
			tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}
			
			//Prepend data directory onto all data steering files
			for(int j = 0; j < tmpVector.size(); j++) {
				if(debug) std::cout << cn << mn << "Prepending: \"" << dataDirectory << "\" to \"" << tmpVector[j] << "\"" << std::endl;
				tmpVector[j] = dataDirectory + "/" + tmpVector[j];
				if(debug) std::cout << cn << mn << "Now: " << tmpVector[j] << std::endl;
			}
			
			//Add to Options vector
			options.push_back(tmpVector);
			if(debug) std::cout << cn << mn << "options[0] = " << SPXUtilities::VectorToCommaSeparatedList(options[0]) << std::endl;
			
			//Set the numberOfOptionsInstances based on the number of dataSteeringFiles
			//	NOTE: ALL OPTIONS HENCEFORTH MUST HAVE THE SAME SIZE VECTOR, OR AN ERROR IS SIGNALED WHEN PARSING IN SPXFrameOptions::Parse
			numberOfOptionsInstances = tmpVector.size();
			if(debug) std::cout << cn << mn << "Number of frame options instances for frame " << i << " has been set to: " << numberOfOptionsInstances << std::endl;
		}
		
		//Get the grid_steering_files
		tmp = reader->Get(frameSection, "grid_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(frameSection, "grid_steering_files", "You MUST specify the grid_steering_files");
		} else {
			//Parse into vector
			tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "grid_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}
			
			//Prepend grid directory onto all grid steering files
			for(int j = 0; j < tmpVector.size(); j++) {
				if(debug) std::cout << cn << mn << "Prepending: \"" << gridDirectory << "\" to \"" << tmpVector[j] << "\"" << std::endl;
				tmpVector[j] = gridDirectory + "/" + tmpVector[j];
				if(debug) std::cout << cn << mn << "Now: " << tmpVector[j] << std::endl;
			}
			
			//Add to Options vector
			options.push_back(tmpVector);
			if(debug) std::cout << cn << mn << "options[1] = " << SPXUtilities::VectorToCommaSeparatedList(options[1]) << std::endl;
		}
		
		//Get the marker_style
		tmp = reader->Get(frameSection, "marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(frameSection, "marker_style", "You MUST specify the marker_style");
		} else {
			//Parse into vector
			tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "marker_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}
			
			//Add to Options vector
			options.push_back(tmpVector);
			if(debug) std::cout << cn << mn << "options[2] = " << SPXUtilities::VectorToCommaSeparatedList(options[2]) << std::endl;
		}
		
		//Get the marker_color
		tmp = reader->Get(frameSection, "marker_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(frameSection, "marker_color", "You MUST specify the marker_color");
		} else {
			//Parse into vector
			tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "marker_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}
			
			//Add to Options vector
			options.push_back(tmpVector);
			if(debug) std::cout << cn << mn << "options[3] = " << SPXUtilities::VectorToCommaSeparatedList(options[3]) << std::endl;
		}
		
		//Get the ref_line_style
		tmp = reader->Get(frameSection, "ref_line_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No frame option for ref_line_style was specified" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "ref_line_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}
			
			//Add to Options vector
			options.push_back(tmpVector);
			if(debug) std::cout << cn << mn << "options[4] = " << SPXUtilities::VectorToCommaSeparatedList(options[4]) << std::endl;
		}
		
		//Get the ref_line_color
		tmp = reader->Get(frameSection, "ref_line_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No frame option for ref_line_color was specified" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "ref_line_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}
			
			//Add to Options vector
			options.push_back(tmpVector);
			if(debug) std::cout << cn << mn << "options[5] = " << SPXUtilities::VectorToCommaSeparatedList(options[5]) << std::endl;
		}
		
		//Get the description
		tmp = reader->Get(frameSection, "desc", "EMPTY");
		if(tmp.compare("EMPTY") != 0) {
			desc = tmp;
			if(debug) std::cout << cn << mn << "Description successfully parsed: \"" << desc << "\"" << std::endl;
		} else {
			desc.clear();
			if(debug) std::cout << cn << mn << "No frame option for desc was specified" << std::endl;
		}
		
		//Create a new frameOptions object with options vector (FrameOptions constructor will parse it)
		try {
			SPXFrameOptions fo = SPXFrameOptions(options, desc, numberOfOptionsInstances);
			
			//Make sure it is valid and non-empty, and add it to frame vector
			if(!fo.IsEmpty() && fo.IsValid()) {
				frameOptions.push_back(fo);
				if(debug) std::cout << cn << mn << "Successfully added " << frameSection << " to the frameOptions vector" << std::endl;
			} else {
				throw SPXINIParseException(frameSection, "SPXFrameOptions object is empty or invalid");
			}
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXINIParseException(frameSection, "Unable to create/parse new frame options object");
		}
	}
	
	if(debug) std::cout << cn << mn << "Successfully parsed and added " << numFrames << " frame options" << std::endl;
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
	
	//General Options [GEN]
	debug = reader->GetBoolean("GEN", "debug", debug);
	
	//Set Defaults
	this->SetDefaults();
	
	pdfDirectory = reader->Get("GEN", "pdf_directory", pdfDirectory);
	dataDirectory = reader->Get("GEN", "data_directory", dataDirectory);
	gridDirectory = reader->Get("GEN", "grid_directory", gridDirectory);
	
	//Enable all debug options if debug is on
	if(debug) {
		std::cout << cn << mn << "Debug is ON" << std::endl;
		SPXRatioStyle::SetDebug(true);
		SPXOverlayStyle::SetDebug(true);
		SPXDisplayStyle::SetDebug(true);
		SPXPDFBandType::SetDebug(true);
		SPXPDFErrorType::SetDebug(true);
		SPXPDFErrorSize::SetDebug(true);
		SPXFrameOptions::SetDebug(true);
	}
	
	//Graphing Options [GRAPH]
	plotBand = reader->GetBoolean("GRAPH", "plot_band", plotBand);
	plotErrorTicks = reader->GetBoolean("GRAPH", "plot_error_ticks", plotErrorTicks);
	plotMarker = reader->GetBoolean("GRAPH", "plot_marker", plotMarker);
	plotStaggered = reader->GetBoolean("GRAPH", "plot_staggered", plotStaggered);
	labelSqrtS = reader->GetBoolean("GRAPH", "label_sqrt_s", labelSqrtS);
	xLegend = reader->GetReal("GRAPH", "x_legend", xLegend);
	yLegend = reader->GetReal("GRAPH", "y_legend", xLegend);
	ratioTitle = reader->Get("GRAPH", "ratio_title", ratioTitle);
	
	//Parse Ratio Style
	tmp = reader->Get("GRAPH", "ratio_style", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "ratio_style", "You MUST specify the ratio_style");
	} else {
		//Attempt to parse ratio style
		try {
			ratioStyle.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			std::ostringstream s;
			s << "Invalid ratio style: Numerator = " << ratioStyle.GetNumerator() << " Denominator = " << ratioStyle.GetDenominator() << ": Check configuration string: ratio_style = " << tmp;
			throw SPXINIParseException("GRAPH", "ratio_style", s.str());
		}
	}	
	
	//Parse Overlay Style
	tmp = reader->Get("GRAPH", "overlay_style", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "overlay_style", "You MUST specify the overlay_style");
	} else {
		//Attempt to parse overlay style
		try {
			overlayStyle.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
		
			std::ostringstream s;
			s << "Invalid overlay style: Style = " << overlayStyle.GetStyle() << ": Check configuration string: overlay_style = " << tmp;
			throw SPXINIParseException("GRAPH", "overlay_style", s.str());
		}
	}
	
	//Parse Display Style
	tmp = reader->Get("GRAPH", "display_style", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Display style configuration string empty: Defaulting to \"overlay, ratio\"" << std::endl;
	} else {
		//Attempt to parse display style		
		try {
			displayStyle.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			std::ostringstream s;
			s << "Invalid display style: Style = " << displayStyle.GetStyle() << ": Check configuration string: display_style = " << tmp;
			throw SPXINIParseException("GRAPH", "display_style", s.str());
		}
	}
	
	yOverlayMin = reader->GetReal("GRAPH", "y_overlay_min", yOverlayMin);
	yOverlayMax = reader->GetReal("GRAPH", "y_overlay_max", yOverlayMax);
	yRatioMin = reader->GetReal("GRAPH", "y_ratio_min", yRatioMin);
	yRatioMax = reader->GetReal("GRAPH", "y_ratio_max", yRatioMax);
	
	//PDF Options [PDF]

	//Parse PDF Steering Filepaths
	tmp = reader->Get("PDF", "pdf_steering_files", "EMPTY");
	if(!tmp.compare("EMPTY")){
		//Required if Overlay -> Convolute is plotted
		if(displayStyle.ContainsOverlay() && overlayStyle.ContainsConvolute()) {
			throw SPXINIParseException("PDF", "pdf_steering_files", "You MUST specify at least one PDF Steering File if Overlay -> Convolute is plotted");
		} else {
			if(debug) std::cout << cn << mn << "No PDF Steering Files were specified" << std::endl;
		}
	} else {
		std::vector<std::string> tmpVector = SPXUtilities::CommaSeparatedListToVector(tmp);

		//Prepend PDF directory onto all PDF steering files
		for(int i = 0; i < tmpVector.size(); i++) {
			if(debug) std::cout << cn << mn << "Prepending: \"" << pdfDirectory << "\" to \"" << tmpVector[i] << "\"" << std::endl;
			tmpVector[i] = pdfDirectory + "/" + tmpVector[i];
			if(debug) std::cout << cn << mn << "Now: " << tmpVector[i] << std::endl;

			//Create a PDF steering file with the full filepath
			SPXPDFSteeringFile pdfSteeringFile = SPXPDFSteeringFile(tmpVector[i]);

			//Add the PDF steering file to the vector
			pdfSteeringFiles.push_back(pdfSteeringFile);
		}
	}
	
	pdfFillStyle = reader->GetInteger("PDF", "pdf_fill_style", pdfFillStyle);
	pdfFillColor = reader->GetInteger("PDF", "pdf_fill_color", pdfFillColor);
	pdfMarkerStyle = reader->GetInteger("PDF", "pdf_marker_style", pdfMarkerStyle);
	
	//Parse PDF Band Type
	tmp = reader->Get("PDF", "pdf_band_type", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		std::cout << cn << mn << "No PDF Band Type override specified" << std::endl;
	} else {		
		//Attempt to parse the PDF band type
		try {
			pdfBandType.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			std::ostringstream s;
			s << "Invalid PDF Band Type: Type = " << pdfBandType.GetType() << ": Check configuration string: pdf_band_type = " << tmp;
			throw SPXINIParseException("PDF", "pdf_band_type", s.str());
		}
	}
	
	//Parse PDF Error Type
	tmp = reader->Get("PDF", "pdf_error_type", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		std::cout << cn << mn << "No PDF Error Type override specified" << std::endl;
	} else {
		//Attempt to parse the PDF error type
		try {
			pdfErrorType.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			std::ostringstream s;
			s << "Invalid PDF Error Type: Type = " << pdfErrorType.GetType() << ": Check configuration string: pdf_error_type = " << tmp;
			throw SPXINIParseException("PDF", "pdf_error_type", s.str());
		}
	}
	
	//Parse PDF Error Size
	tmp = reader->Get("PDF", "pdf_error_size", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		std::cout << cn << mn << "No PDF Error Size override specified" << std::endl;
	} else {
		//Attempt to parse the PDF error size
		try {
			pdfErrorSize.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			std::ostringstream s;
			s << "Invalid PDF Error Size: Type = " << pdfErrorSize.GetType() << ": Check configuration string: pdf_error_size = " << tmp;
			throw SPXINIParseException("PDF", "pdf_error_size", s.str());
		}
	}
	
	//Parse Frames
	numberOfFrames = this->ParseNumberOfFrames();
	
	if(numberOfFrames == 0) {
		throw SPXParseException("No frame options found: Nothing will be plotted");
	}
	
	//Attempt to parse frame options
	try {
		this->ParseFrameOptions(numberOfFrames);
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		
		throw SPXParseException("Could not parse frames: Verify correct frame configuration syntax");
	}
}

void SPXSteeringFile::PrintPDFSteeringFiles(void) {
	for(int i = 0; i < pdfSteeringFiles.size(); i++) {
		pdfSteeringFiles.at(i).Print();
	}
}

void SPXSteeringFile::ParsePDFSteeringFiles(void) {
	std::string mn = "ParsePDFSteeringFiles: ";
	
	//Parse each PDF Steering File in the vector
	for(int i = 0; i < pdfSteeringFiles.size(); i++) {

		SPXPDFSteeringFile & pdfSteeringFile = pdfSteeringFiles.at(i);

		//Attempt to parse the PDF steering file
		try {
			pdfSteeringFile.Parse();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			
			std::ostringstream oss;
			oss << "Unable to parse the PDF Steering File: " << pdfSteeringFile.GetFilename() << ": Aborting further parsing of remaining " << pdfSteeringFiles.size() - i << " files";
			throw SPXParseException(oss.str());
		}
	}
}

void SPXSteeringFile::PrintDataSteeringFiles(void) {
	for(int i = 0; i < frameOptions.size(); i++) {
		for(int j = 0; j < frameOptions.at(i).GetNumberOfOptionsInstances(); j++) {
			SPXDataSteeringFile &dataSteeringFile = frameOptions.at(i).GetFrameOptionsInstance(j).dataSteeringFile;
			dataSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParseDataSteeringFiles(void) {
	std::string mn = "ParseDataSteeringFiles: ";
	
	//Loop through all frame options instances for each frame option
	for(int i = 0; i < frameOptions.size(); i++) {
		for(int j = 0; j < frameOptions.at(i).GetNumberOfOptionsInstances(); j++) {
			
			SPXDataSteeringFile &dataSteeringFile = frameOptions.at(i).GetFrameOptionsInstance(j).dataSteeringFile;
			
			//Attempt to parse the Grid Steering File
			try {
				dataSteeringFile.Parse();
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
	for(int i = 0; i < frameOptions.size(); i++) {
		for(int j = 0; j < frameOptions.at(i).GetNumberOfOptionsInstances(); j++) {
			SPXGridSteeringFile &gridSteeringFile = frameOptions.at(i).GetFrameOptionsInstance(j).gridSteeringFile;
			gridSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParseGridSteeringFiles(void) {
	std::string mn = "ParseGridSteeringFiles: ";
	
	//Loop through all frame options instances for each frame option
	for(int i = 0; i < frameOptions.size(); i++) {
		for(int j = 0; j < frameOptions.at(i).GetNumberOfOptionsInstances(); j++) {
			
			SPXGridSteeringFile &gridSteeringFile = frameOptions.at(i).GetFrameOptionsInstance(j).gridSteeringFile;
			
			//Attempt to parse the Grid Steering File
			try {
				gridSteeringFile.Parse();
			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;
				
				std::ostringstream oss;
				oss << "Unable to parse the Grid Steering File: " << gridSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}	
}
