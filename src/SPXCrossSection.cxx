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

	//Attempt to create the PDF object and perform convolution
	try {
		pdf = new SPXPDF(psf, grid->GetGridName());
	} catch(const SPXException &e) {
		throw;
	}
}

void SPXCrossSection::ParseCorrections(void) {
	//Check if grid contains corrections
	if(pci->gridSteeringFile.GetNumberOfCorrectionFiles() != 0) {
		try {
			corrections = new SPXGridCorrections(*pci);
			corrections->Parse();
			corrections->Print();
		} catch(const SPXException &e) {
			throw;
		}
	}
}

void SPXCrossSection::ApplyCorrections(void) {
	std::string mn = "ApplyCorrections: ";

	if(pci->gridSteeringFile.GetNumberOfCorrectionFiles() == 0) {
		return;
	}

	std::cout << cn << mn << ">>>>>>>>>>>>>>>>> APPLY CORRECTIONS <<<<<<<<<<<<<<<<<" << std::endl;

	//Loop over the band bins and make sure they match, if not just do nothing

	unsigned int nBins = pdf->h_PDFBand_results->GetN();
	double *x = pdf->h_PDFBand_results->GetX();
	double *y = pdf->h_PDFBand_results->GetY();
	double *exl = pdf->h_PDFBand_results->GetErrorXlow();
	double *exh = pdf->h_PDFBand_results->GetErrorXhigh();
	double *eyl = pdf->h_PDFBand_results->GetErrorYlow();
	double *eyh = pdf->h_PDFBand_results->GetErrorYhigh();

	unsigned int nBinsCorr = corrections->GetNumberOfBins();
	double *c_x = &(corrections->GetTotalX().at(0));
	double *c_exl = &(corrections->GetTotalEXL().at(0));
	double *c_exh = &(corrections->GetTotalEXH().at(0));
	double *c_y = &(corrections->GetTotalYCorrections().at(0));
	double *c_eyl = &(corrections->GetTotalEYLCorrections().at(0));
	double *c_eyh = &(corrections->GetTotalEYHCorrections().at(0));

	//Loop over the smallest of the two
	unsigned int n = (nBins < nBinsCorr ? nBins : nBinsCorr);

	for(int i = 0; i < nBins; i++) {
		for(int j = 0; j < nBinsCorr; j++) {
			//Check for bin match
			if(((x[i] - exl[i]) == c_exl[j]) && ((x[i] + exh[i]) == c_exh[j])) {

				//Bins match; Scale y, eyl, and eyh
				y[i] *= c_y[j];
				eyl[i] *= c_eyl[j];
				eyh[i] *= c_eyh[j];

				break;
			}
		}
	}

	//@TODO Do the same for Alpha S and Scale uncertainty bands
}
