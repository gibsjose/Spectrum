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

enum PDFErrorPropagation_t {
EigenvectorSymmetricHessian=0,      // add in quadrature and symmetrize
EigenvectorAsymmetricHessian,       // add in quadrature, keep asymetic uncertainties
StyleHeraPDF,                      // HERAPDF type Eigenvector, QUAD MAX
StyleNNPDF                         // NNPDF type 
};

class SPXPDFSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;

	//[DESC]
	std::string name;
	std::string nameVar;
	std::string type;
	unsigned int order;
	int numberOfMembers;
	std::string pdfSetPath;

	//[STYLE]
	int fillStyle;
	int fillColor;
	int markerStyle;

	//[PDF]
	int defaultID;
	int defaultIDVar;
	int firstEig;
	int lastEig;
	int firstQuad;
	int lastQuad;
	int firstMax;
	int lastMax;
	bool includeEig;
	bool includeQuad;
	bool includeMax;
        bool is90Percent;
        int ErrorPropagationType;
	SPXPDFBandType bandType;
	SPXPDFErrorType errorType;
	//SPXPDFErrorSize errorSize;
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

	SPXPDFSteeringFile(void) : filename(""), debug(false) {}

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

	const std::string & GetNameVar(void) const {
		return this->nameVar;
	}

	const std::string & GetType(void) const {
		return this->type;
	}

	unsigned int GetNumberOfMembers(void) const {
		return this->numberOfMembers;
	}

	unsigned int GetOrder(void) const {
		return order;
	}

	unsigned int GetNumberOfLoops(void) const {
		return GetOrder();
	}

	const std::string & GetPDFSetPath(void) const {
		return this->pdfSetPath;
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

	int GetDefaultID(void) const {
		return this->defaultID;
	}

	int GetDefaultIDVar(void) const {
		return this->defaultIDVar;
	}

	int GetFirstEig(void) const {
		return this->firstEig;
	}

	int GetLastEig(void) const {
		return this->lastEig;
	}

	int GetFirstQuad(void) const {
		return this->firstQuad;
	}

	int GetLastQuad(void) const {
		return this->lastQuad;
	}

	int GetFirstMax(void) const {
		return this->firstMax;
	}

	int GetLastMax(void) const {
		return this->lastMax;
	}

	bool GetIncludeEig(void) const {
		return this->includeEig;
	}

	bool GetIncludeQuad(void) const {
		return this->includeQuad;
	}

	bool GetIncludeMax(void) const {
		return this->includeMax;
	}

	bool GetIs90Percent(void) const {
		return this->is90Percent;
	}


        int GetErrorPropagationType(void) const {
         return this->ErrorPropagationType;
        }

	SPXPDFBandType & GetBandType(void) {
		return this->bandType;
	}

	SPXPDFErrorType & GetErrorType(void) {
		return this->errorType;
	}

	//SPXPDFErrorSize & GetErrorSize(void) {
	//	return this->errorSize;
	//}

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
