//************************************************************/
//
//	PDF Steering File Implementation
//
//	Implements the SPXPDFSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		30.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXPDFSteeringFile.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXPDFSteeringFile::";

void SPXPDFSteeringFile::SetDefaults(void) {
	std::string mn = "SetDefaults: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Setting default PDF Steering File data" << std::endl;

	name.clear();
	if(debug) std::cout << cn << mn << "name set to default: \" \"" << std::endl;

	nameVar.clear();
	if(debug) std::cout << cn << mn << "nameVar set to default: \" \"" << std::endl;

	type.clear();
	if(debug) std::cout << cn << mn << "type set to default: \" \"" << std::endl;

	order = 1;
	if(debug) std::cout << cn << mn << "order set to default: \"NLO\" (1)" << std::endl;

	numberOfMembers = NUM_MEMBERS_EMPTY;
	if(debug) std::cout << cn << mn << "numberOfMembers set to default: \"-1\"" << std::endl;

	pdfSetPath.clear();
	if(debug) std::cout << cn << mn << "pdfSetPath set to default: \" \"" << std::endl;

	fillStyle = PDF_STYLE_EMPTY;
	if(debug) std::cout << cn << mn << "fillStyle set to default: \"-1\"" << std::endl;

	fillColor = PDF_COLOR_EMPTY;
	if(debug) std::cout << cn << mn << "fillColor set to default: \"-1\"" << std::endl;

	markerStyle = PDF_STYLE_EMPTY;
	if(debug) std::cout << cn << mn << "markerStyle set to default: \"-1\"" << std::endl;

	defaultID = 0;
	if(debug) std::cout << cn << mn << "markerStyle set to default: \"0\"" << std::endl;

	defaultIDVar = -1;
	if(debug) std::cout << cn << mn << "markerStyle set to default: \"-1\"" << std::endl;

	firstEig = 1;
	if(debug) std::cout << cn << mn << "firstEig set to default: \"1\"" << std::endl;

	lastEig = 1;
	if(debug) std::cout << cn << mn << "lastEig set to default: \"1\"" << std::endl;

	firstQuad = 1;
	if(debug) std::cout << cn << mn << "firstQuad set to default: \"1\"" << std::endl;

	lastQuad = 1;
	if(debug) std::cout << cn << mn << "lastQuad set to default: \"1\"" << std::endl;

	firstMax = 1;
	if(debug) std::cout << cn << mn << "firstMax set to default: \"1\"" << std::endl;

	lastMax = 1;
	if(debug) std::cout << cn << mn << "lastMax set to default: \"1\"" << std::endl;

	includeEig = false;
	if(debug) std::cout << cn << mn << "includeEig set to default: \"false\"" << std::endl;

	includeQuad = false;
	if(debug) std::cout << cn << mn << "includeQuad set to default: \"false\"" << std::endl;

	includeMax = false;
	if(debug) std::cout << cn << mn << "includeMax set to default: \"false\"" << std::endl;

	bandType = SPXPDFBandType();
	if(debug) std::cout << cn << mn << "bandType set to default: \" \"" << std::endl;

	errorType = SPXPDFErrorType();
	if(debug) std::cout << cn << mn << "errorType set to default: \" \"" << std::endl;

	errorSize = SPXPDFErrorSize();
	if(debug) std::cout << cn << mn << "errorSize set to default: \" \"" << std::endl;

	alphaSErrorNumberUp = ALPHA_S_ERROR_NUM_EMPTY;
	if(debug) std::cout << cn << mn << "alphaSErrorNumberUp set to default: \"-1\"" << std::endl;

	alphaSErrorNumberDown = ALPHA_S_ERROR_NUM_EMPTY;
	if(debug) std::cout << cn << mn << "alphaSErrorNumberDown set to default: \"-1\"" << std::endl;

	alphaSPDFNameUp.clear();
	if(debug) std::cout << cn << mn << "alphaSPDFNameUp set to default: \" \"" << std::endl;

	alphaSPDFNameDown.clear();
	if(debug) std::cout << cn << mn << "alphaSPDFNameDown set to default: \" \"" << std::endl;

	alphaSPDFHistogramNameUp.clear();
	if(debug) std::cout << cn << mn << "alphaSPDFHistogramNameUp set to default: \" \"" << std::endl;

	alphaSPDFHistogramNameDown.clear();
	if(debug) std::cout << cn << mn << "alphaSPDFHistogramNameDown set to default: \" \"" << std::endl;
}

//Print the PDF Steering File Data in a nice format
void SPXPDFSteeringFile::Print(void) {
	std::cout << "PDF Steering File: " << filename << std::endl;
	std::cout << "\t General Options [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl << std::endl;
	std::cout << "\t Description [DESC]" << std::endl;
	std::cout << "\t\t Name: " << name << std::endl;
	std::cout << "\t\t Name Variation: " << nameVar << std::endl;
	std::cout << "\t\t Type: " << type << std::endl;
	std::cout << "\t\t Order: " << order << std::endl;
	std::cout << "\t\t Number of Members: " << numberOfMembers << std::endl;
	std::cout << "\t\t PDF Set Path: " << pdfSetPath << std::endl << std::endl;
	std::cout << "\t Style Options [STYLE]" << std::endl;
	std::cout << "\t\t Fill Style: " << (fillStyle == PDF_STYLE_EMPTY ? "UNSET: " : "") << fillStyle << std::endl;
	std::cout << "\t\t Fill Color: " << (fillColor == PDF_COLOR_EMPTY ? "UNSET: " : "") << fillColor << std::endl;
	std::cout << "\t\t Marker Style: " << (markerStyle == PDF_STYLE_EMPTY ? "UNSET: " : "") << markerStyle << std::endl << std::endl;
	std::cout << "\t PDF Options [PDF]" << std::endl;
	std::cout << "\t\t Default ID: " << defaultID << std::endl;
	std::cout << "\t\t Default ID Var: " << defaultIDVar << std::endl;
	std::cout << "\t\t First Eigenvector: " << firstEig << std::endl;
	std::cout << "\t\t Last Eigenvector: " << lastEig << std::endl;
	std::cout << "\t\t First Quadrature: " << firstQuad << std::endl;
	std::cout << "\t\t Last Quadrature: " << lastQuad << std::endl;
	std::cout << "\t\t First Max: " << firstMax << std::endl;
	std::cout << "\t\t Last Max: " << lastMax << std::endl;
	std::cout << "\t\t Include Eigenvectors? " << (includeEig ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Include Quadrature? " << (includeQuad ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Include Max? " << (includeMax ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Band Type: " << bandType.ToString() << std::endl;
	std::cout << "\t\t Error Type: " << errorType.ToString() << std::endl;
	std::cout << "\t\t Error Size: " << errorSize.ToString() << std::endl;
	std::cout << "\t\t Alpha S Error Number Up: " << alphaSErrorNumberUp << std::endl;
	std::cout << "\t\t Alpha S Error Number Down: " << alphaSErrorNumberDown << std::endl;
	std::cout << "\t\t Alpha S PDF Name Up: " << alphaSPDFNameUp << std::endl;
	std::cout << "\t\t Alpha S PDF Name Down: " << alphaSPDFNameDown << std::endl;
	std::cout << "\t\t Alpha S PDF Histogram Name Up: " << alphaSPDFHistogramNameUp << std::endl;
	std::cout << "\t\t Alpha S PDF Histogram Name Down: " << alphaSPDFHistogramNameDown << std::endl << std::endl;
}

void SPXPDFSteeringFile::Parse(void) {
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

	std::string tmp;

	//General Options [GEN]
	debug = reader->GetBoolean("GEN", "debug", debug);

	//Set Defaults
	this->SetDefaults();

	//Enable all debug options if debug is on
	if(debug) {
		std::cout << cn << mn << "Debug is ON" << std::endl;
		SPXPDFBandType::SetDebug(true);
		SPXPDFErrorType::SetDebug(true);
		SPXPDFErrorSize::SetDebug(true);
	}

	//Description [DESC]
	name = reader->Get("DESC", "name", "EMPTY");
	if(!name.compare("EMPTY")) {
		throw SPXINIParseException("DESC", "name", "You MUST specify the name");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read PDF Name: " << name << std::endl;
	}

	nameVar = reader->Get("DESC", "name_var", "EMPTY");
	if(!nameVar.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No name variation specified: defaulting to \" \"" << std::endl;
		nameVar.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read PDF Name Variation: " << nameVar << std::endl;
	}

	type = reader->Get("DESC", "type", "EMPTY");
	if(!type.compare("EMPTY")) {
		throw SPXINIParseException("DEC", "type", "You MUST specify the type");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read PDF Type: " << type << std::endl;
	}

	order = (unsigned int)reader->GetInteger("DESC", "order", order);
	if(order > 2) {
		std::cerr << "WARNING: Order number unrecognized: " << order << " defaulting to NLO (1)" << std::endl;
		order = 1;
	} else {
		if(debug) std::cout << cn << mn << "Successfully read PDF Order: " << order << std::endl;
	}

	numberOfMembers = reader->GetInteger("DESC", "num_members", NUM_MEMBERS_EMPTY);
	if(numberOfMembers == NUM_MEMBERS_EMPTY) {
		throw SPXINIParseException("DESC", "num_members", "You MUST specify the num_members");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Number of Members: " << numberOfMembers << std::endl;
	}

	pdfSetPath = reader->Get("DESC", "pdf_set_path", "EMPTY");
	if(!pdfSetPath.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No PDF Set Path configuration found" << std::endl;
		pdfSetPath.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read PDF Set Path: " << pdfSetPath << std::endl;
	}

	//Style Options [STYLE]
	fillStyle = reader->GetInteger("STYLE", "fill_style", PDF_STYLE_EMPTY);
	if(fillStyle == PDF_STYLE_EMPTY) {
		throw SPXINIParseException("STYLE", "fill_style", "You MUST specify the fill_style");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Fill Style: " << fillStyle << std::endl;
	}

	fillColor = reader->GetInteger("STYLE", "fill_color", PDF_COLOR_EMPTY);
	if(fillColor == PDF_COLOR_EMPTY) {
		throw SPXINIParseException("STYLE", "fill_color", "You MUST specify the fill_color");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Fill Color: " << fillColor << std::endl;
	}

	markerStyle = reader->GetInteger("STYLE", "marker_style", PDF_STYLE_EMPTY);
	if(markerStyle == PDF_STYLE_EMPTY) {
		throw SPXINIParseException("STYLE", "marker_style", "You MUST specify the marker_style");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Marker Style: " << markerStyle << std::endl;
	}

	//PDF Options [PDF]
	defaultID = reader->GetInteger("PDF", "default_id", -1);
	if(defaultID == -1) {
		throw SPXINIParseException("PDF", "default_id", "You MUST specify the default_id");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Default ID: " << defaultID << std::endl;
	}

	defaultIDVar = reader->GetInteger("PDF", "default_id_var", -1);
	if(defaultIDVar == -1) {
		throw SPXINIParseException("PDF", "default_id_var", "You MUST specify the default_id");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Default ID Variation: " << defaultIDVar << std::endl;
	}

	firstEig = reader->GetInteger("PDF", "first_eig", -1);
	if(firstEig == -1) {
		throw SPXINIParseException("PDF", "first_eig", "You MUST specify the first_eig");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read First Eigenvector: " << firstEig << std::endl;
	}

	lastEig = reader->GetInteger("PDF", "last_eig", -1);
	if(lastEig == -1) {
		throw SPXINIParseException("PDF", "last_eig", "You MUST specify the last_eig");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Last Eigenvector: " << lastEig << std::endl;
	}

	firstQuad = reader->GetInteger("PDF", "first_quad", -1);
	if(firstQuad == -1) {
		throw SPXINIParseException("PDF", "first_quad", "You MUST specify the first_quad");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read First Quadrature: " << firstQuad << std::endl;
	}

	lastQuad = reader->GetInteger("PDF", "last_quad", -1);
	if(lastQuad == -1) {
		throw SPXINIParseException("PDF", "last_quad", "You MUST specify the last_quad");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Last Quadrature: " << lastQuad << std::endl;
	}

	firstMax = reader->GetInteger("PDF", "first_max", -1);
	if(firstMax == -1) {
		throw SPXINIParseException("PDF", "first_max", "You MUST specify the first_max");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read First Max: " << firstMax << std::endl;
	}

	lastMax = reader->GetInteger("PDF", "last_max", -1);
	if(lastMax == -1) {
		throw SPXINIParseException("PDF", "last_max", "You MUST specify the last_max");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Last Max: " << lastMax << std::endl;
	}

	includeEig = reader->GetBoolean("PDF", "include_eig", includeEig);
	includeQuad = reader->GetBoolean("PDF", "include_quad", includeQuad);
	includeMax = reader->GetBoolean("PDF", "include_max", includeMax);

	//Parse Band Type
	tmp = reader->Get("PDF", "band_type", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "band_type", "You MUST specify the band_type");
	} else {
		//Attempt to parse the band type
		try {
			bandType.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;

			std::ostringstream s;
			s << "Invalid Band Type: Type = " << bandType.GetType() << ": Check configuration string: band_type = " << tmp;
			throw SPXINIParseException("PDF", "band_type", s.str());
		}
	}

	//Parse Error Type
	tmp = reader->Get("PDF", "error_type", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "error_type", "You MUST specify the error_type");
	} else {
		//Attempt to parse the error type
		try {
			errorType.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;

			std::ostringstream s;
			s << "Invalid Error Type: Type = " << errorType.GetType() << ": Check configuration string: error_type = " << tmp;
			throw SPXINIParseException("PDF", "error_type", s.str());
		}
	}

	//Parse Error Size
	tmp = reader->Get("PDF", "error_size", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "error_size", "You MUST specify the error_size");
	} else {
		//Attempt to parse the error size
		try {
			errorSize.Parse(tmp);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;

			std::ostringstream s;
			s << "Invalid Error Size: Type = " << errorSize.GetType() << ": Check configuration string: error_size = " << tmp;
			throw SPXINIParseException("PDF", "error_size", s.str());
		}
	}

	alphaSErrorNumberUp = reader->GetInteger("PDF", "alpha_s_error_number_up", ALPHA_S_ERROR_NUM_EMPTY);
	if(alphaSErrorNumberUp == ALPHA_S_ERROR_NUM_EMPTY) {
		throw SPXINIParseException("PDF", "alpha_s_error_number_up", "You MUST specify the alpha_s_error_number_up");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Alpha S Error Number Up: " << alphaSErrorNumberUp << std::endl;
	}

	alphaSErrorNumberDown = reader->GetInteger("PDF", "alpha_s_error_number_down", ALPHA_S_ERROR_NUM_EMPTY);
	if(alphaSErrorNumberDown == ALPHA_S_ERROR_NUM_EMPTY) {
		throw SPXINIParseException("PDF", "alpha_s_error_number_down", "You MUST specify the alpha_s_error_number_down");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Alpha S Error Number Down: " << alphaSErrorNumberDown << std::endl;
	}

	alphaSPDFNameUp = reader->Get("PDF", "alpha_s_pdf_name_up", "EMPTY");
	if(!alphaSPDFNameUp.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "alpha_s_pdf_name_up", "You MUST specify the alpha_s_pdf_name_up");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Alpha S PDF Name Up: " << alphaSPDFNameUp << std::endl;
	}

	alphaSPDFNameDown = reader->Get("PDF", "alpha_s_pdf_name_down", "EMPTY");
	if(!alphaSPDFNameDown.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "alpha_s_pdf_name_down", "You MUST specify the alpha_s_pdf_name_down");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Alpha S PDF Name Down: " << alphaSPDFNameDown << std::endl;
	}

	alphaSPDFHistogramNameUp = reader->Get("PDF", "alpha_s_pdf_histogram_name_up", "EMPTY");
	if(!alphaSPDFHistogramNameUp.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "alpha_s_pdf_histogram_name_up", "You MUST specify the alpha_s_pdf_histogram_name_up");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Alpha S PDF Histogram Name Up: " << alphaSPDFHistogramNameUp << std::endl;
	}

	alphaSPDFHistogramNameDown = reader->Get("PDF", "alpha_s_pdf_histogram_name_down", "EMPTY");
	if(!alphaSPDFHistogramNameDown.compare("EMPTY")) {
		throw SPXINIParseException("PDF", "alpha_s_pdf_histogram_name_down", "You MUST specify the alpha_s_pdf_histogram_name_down");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Alpha S PDF Histogram Name Down: " << alphaSPDFHistogramNameDown << std::endl;
	}

	delete reader;
}
