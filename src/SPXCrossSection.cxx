//************************************************************/
//
//	CrossSection Implementation
//
//	Implements the SPXCrossSection class, which performs the
//	convolution and all math required to calculate the cross-
//	section from a PDF and a grid file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXCrossSection.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXCrossSection::";

//Must define the static debug variable in the implementation
bool SPXCrossSection::debug;

//Create the CrossSection
void SPXCrossSection::Create(void) {

	//Attempt to create the Grid
	try {
		grid = new SPXGrid(pci);
	} catch(const SPXException &e) {
		throw;
	}

	//Attempt to create the PDF object
	try {
		pdf = new SPXPDF(psf, grid->GetGridName());
	} catch(const SPXException &e) {
		throw;
	}
}

//@TODO Already done in SPXPDF::Initialize()
/*
void SPXCrossSection::ConfigureStyle(void) {

	//Set marker style
	pdf->h_PDFBand_results->SetMarkerStyle(psf->GetMarkerStyle());

	//Set marker color
	pdf->h_PDFBand_results->SetMarkerColor(psf->GetMarkerColor());

	//Set marker size
	pdf->h_PDFBand_results->SetMarkerSize(1.0);
	
	//Set line color
	pdf->h_PDFBand_results->SetLineColor(1);

	//Set line width
	pdf->h_PDFBand_results->SetLineWidth(1);
}
*/