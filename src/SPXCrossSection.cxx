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
		grid = new SPXGrid(*pci);
	} catch(const SPXException &e) {
		throw;
	}

	//Attempt to create the PDF object and perform convolution
	try {
		pdf = new SPXPDF(psf, grid->GetGridName());
	} catch(const SPXException &e) {
		throw;
	}

	//Check if grid contains corrections
	if(pci->gridSteeringFile.GetNumberOfCorrectionFiles() != 0) {
		try {
			corrections = new SPXGridCorrections(pci);
			corrections->Parse();
			corrections->Print();
		} catch(const SPXException &e) {
			throw;
		}
	}
}

void SPXCrossSection::ApplyCorrections(void) {
	if(pci.gridSteeringFile.GetNumberOfCorrectionFiles() == 0) {
		return;
	}

	//Loop over the band bins and make sure they match, if not just do nothing

	// unsigned int nBins = pdf->h_PDFBand_results->GetN();
	// double *x = pdf->h_PDFBand_results->GetX();
	// double *y = pdf->h_PDFBand

	//@TODO Should calculate the TOTAL scale for sigma, dsig+, dsig- and have it accessible as a vector from the SPXGridCorrections class
	//		then only need to loop over bins and make sure there is a corresponding bin for each total scale, and if there is, apply it.
	//		The SPXGridCorrections class should do the check to see whether the different corrections have the same binning

	// //PDF Band
	// for(int i = 0; i < pci.gridSteeringFile.GetNumberOfCorrectionFiles(); i++) {
	// 	std::string &key = pci.gridSteeringFile.GetCorrectionFile(i);
	// 	std::vector<std::vector<double> > &matrix;
	//
	// 	matrix = corrections.GetCorrections(key);
	//
	//
	// 	//Loop over bins in band and apply corrections if the bins match up
	// 	for(int j = 0; j < nBins; j++) {
	//
	//
	// 		if(matrix[1] != )
	// 	}
	// }

	//@TODO Do the same for Alpha S and Scale uncertainty bands
}
