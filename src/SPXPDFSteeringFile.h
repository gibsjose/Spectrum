//************************************************************/
//
//	PDF Steering File Header
//
//	Outlines the SPXPDFSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		30.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "INIReader.h"

#include "SPXPDFBandType.h"
#include "SPXPDFErrorType.h"
#include "SPXPDFErrorSize.h"

#ifndef SPXPDFSTEERINGFILE_H
#define SPXPDFSTEERINGFILE_H

const int NUM_MEMBERS_EMPTY = -1;
const int PDF_STYLE_EMPTY = -1;
const int PDF_COLOR_EMPTY = -1;
const int ALPHA_S_ERROR_NUM_EMPTY = -1;

class SPXPDFSteeringFile {

	//Currently implemented as just a string...
	//Any reason to use enum?
	/*
	typedef enum PDFOrder_t {
		LO = 0,
		NLO = 1,
		NNLO = 2
	} PDFOrder_t;
	*/
	
private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;
	
	//[DESC]
	std::string name;
	std::string type;
	std::string order;
	int numberOfMembers;
	
	//[STYLE]
	int fillStyle;
	int fillColor;
	int markerStyle;
	
	//[PDF]
	SPXPDFBandType bandType;
	SPXPDFErrorType errorType;
	SPXPDFErrorSize errorSize;
	int alphaSErrorNumberUp;
	int alphaSErrorNumberDown;
	std::string alphaSPDFNameUp;
	std::string alphaSPDFNameDown;
	std::string alphaSPDFHistogramNameUp;
	std::string alphaSPDFHistogramNameDown;
	
	void SetDefaults(void);
	
public:

	void Print(void);
	void Parse(void);
	
	explicit SPXPDFSteeringFile(const std::string &filename) : debug(false) {	
		this->filename = filename;		
	}
	
	std::string GetFilename(void) const {
		return this->filename;
	}
	
	void SetDebug(bool b) {
		debug = b;
	}
	
	bool GetDebug(void) const {
		return this->debug;
	}
	
	const std::string & GetName(void) const {
		return this->name;
	}
	
	const std::string & GetType(void) const {
		return this->type;
	}
	
	const std::string & GetOrder(void) const {
		return this->order;
	}
	
	unsigned int GetNumberOfMembers(void) const {
		return this->numberOfMembers;
	}
	
	int GetFillStyle(void) const {
		return this->fillStyle;
	}
	
	int GetFillColor(void) const {
		return this->fillColor;
	}	
	
	int GetMarkerStyle(void) const {
		return this->markerStyle;
	}
	
	const SPXPDFBandType & GetBandType(void) const {
		return this->bandType;
	}
	
	const SPXPDFErrorType & GetErrorType(void) const {
		return this->errorType;
	}
	
	const SPXPDFErrorSize & GetErrorSize(void) const {
		return this->errorSize;
	}
	
	int GetAlphaSErrorNumberUp(void) const {
		return this->alphaSErrorNumberUp;
	}
	
	int GetAlphaSErrorNumberDown(void) const {
		return this->alphaSErrorNumberDown;
	}
	
	const std::string & GetAlphaSPDFNameUp(void) const {
		return this->alphaSPDFNameUp;
	}
	
	const std::string & GetAlphaSPDFNameDown(void) const {
		return this->alphaSPDFNameDown;
	}	
	
	const std::string & GetAlphaSPDFHistogramNameUp(void) const {
		return this->alphaSPDFHistogramNameUp;
	}
	
	const std::string & GetAlphaSPDFHistogramNameDown(void) const {
		return this->alphaSPDFHistogramNameDown;
	}
};

#endif
