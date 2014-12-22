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
void SPXCrossSection::Create(SPXSteeringFile *mainsteeringFile) {
	std::string mn = "Create: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Attempt to create the Grid
	try {
		grid = new SPXGrid(pci);
	} catch(const SPXException &e) {
	        throw;
	 }

	if (debug) std::cout<<cn<<mn<<"Attached the GRID "<<endl;

	//Attempt to create the PDF object and perform convolution
	try {
	  pdf = new SPXPDF(psf, grid->GetGridName());
	} catch(const SPXException &e) {
		throw;
	}


        //@TODO What to do when the grid IS divided but the reference is NOT?
	dividedByBinWidth = this->pci->gridSteeringFile.IsGridDividedByBinWidth();

	if (debug) {
         std::cout<<cn<<mn<<"Created the PDF-class "<<endl;
	 std::cout<<cn<<mn<<" dividedByBinWidth= " <<dividedByBinWidth<<std::endl;

	 std::cout<<cn<<mn<<" GetBandwithPDF= "   <<mainsteeringFile->GetBandwithPDF()<<std::endl;
	 std::cout<<cn<<mn<<" GetBandwithAlphas= "<<mainsteeringFile->GetBandwithAlphaS()<<std::endl;
	 std::cout<<cn<<mn<<" GetBandwithScales= "<<mainsteeringFile->GetBandwithScales()<<std::endl;
	 std::cout<<cn<<mn<<" GetBandTotal= "     <<mainsteeringFile->GetBandTotal()<<std::endl;
	}

        // Set which uncertainty to calculate in PDF file
        //default setting is in PDF class PDF true, rest false
        if (pdf->GetDoPDFBand()==false || mainsteeringFile->GetBandwithPDF()==false) 
         pdf->SetDoPDFBand (mainsteeringFile->GetBandwithPDF()); 
        pdf->SetDoAlphaS  (mainsteeringFile->GetBandwithAlphaS());
        pdf->SetDoScale   (mainsteeringFile->GetBandwithScales());
        pdf->SetDoTotError(mainsteeringFile->GetBandTotal());

	//@JJG 16.12.14
	//
	//	These should only be called if the steeringFile actually sets them, right? Currently for old steering files
	//	this causes them to have all the scales off, which then gives an error
	//
	//	Another option might be to just skip overriding the PDFBand from the top-level steering, since this is almost always wanted
	//	and if it is not wanted they can modify the PDF steering file

        std::vector<double> RenScales=mainsteeringFile->GetRenScales();
	std::vector<double> FacScales=mainsteeringFile->GetFacScales();

	if (RenScales.size()!=FacScales.size()) {
         std::ostringstream oss;
         oss << cn << mn << "Different number of values given for rennormalisation and factorisation scale";
         throw SPXParseException(oss.str());
	}

        if (RenScales.size()!=0)
         pdf->SetScales(RenScales,FacScales);

        pdf->Initialize();

	//Convert reference and nominal histograms to graphs and save them
        if (!pdf->GetPDFNominal()) std::cout<<cn<<mn<<"nominal PDF not found ! "<<endl;
        if (!grid->GetReference()) std::cout<<cn<<mn<<"reference histogram not found ! "<<endl;

	SPXGraphUtilities::HistogramToGraph(gridReference, grid->GetReference());
	SPXGraphUtilities::HistogramToGraph(nominal,       pdf->GetPDFNominal());

	//Set the name of the convolution graphs appropriately
	//Create name strings
	//TString name;
	//TString pdfName;
	//TString alphaSName;
	//TString scaleName;

	if(debug) std::cout << cn << mn << "Grid steering file has no name value: using filename instead" << std::endl;
}

void SPXCrossSection::ParseCorrections(void) {
	std::string mn = "ParseCorrections: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Check if grid contains corrections
	if(pci->gridSteeringFile.GetNumberOfCorrectionFiles() != 0) {
	 try {
	  corrections = new SPXGridCorrections(*pci);
	  corrections->Parse();
  	  if (debug) {
           std::cout<<cn<<mn<<"Sucessfully read in corrections: "<<std::endl;
	   corrections->Print();
          }
	 } catch(const SPXException &e) {
	  throw;
	 }
	}
}

void SPXCrossSection::ApplyCorrections(void) {
	std::string mn = "ApplyCorrections: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(pci->gridSteeringFile.GetNumberOfCorrectionFiles() == 0) {
		return;
	}

	std::cout << cn << mn << ">>>>>>>>>>>>>>>>> APPLY CORRECTIONS <<<<<<<<<<<<<<<<<" << std::endl;

	//Loop over the band bins and make sure they match, if not just do nothing

	unsigned int nBins = pdf->h_PDFBand_results->GetN();
	double *x   = pdf->h_PDFBand_results->GetX();
	double *y   = pdf->h_PDFBand_results->GetY();
	double *exl = pdf->h_PDFBand_results->GetEXlow();
	double *exh = pdf->h_PDFBand_results->GetEXhigh();
	double *eyl = pdf->h_PDFBand_results->GetEYlow();
	double *eyh = pdf->h_PDFBand_results->GetEYhigh();

	unsigned int nBinsCorr = corrections->GetNumberOfBins();
	double *c_x   = &(corrections->GetTotalX().at(0));
	double *c_exl = &(corrections->GetTotalEXL().at(0));
	double *c_exh = &(corrections->GetTotalEXH().at(0));
	double *c_y   = &(corrections->GetTotalYCorrections().at(0));
	double *c_eyl = &(corrections->GetTotalEYLCorrections().at(0));
	double *c_eyh = &(corrections->GetTotalEYHCorrections().at(0));

	//Loop over the smallest of the two
	unsigned int n = (nBins < nBinsCorr ? nBins : nBinsCorr);

	for(int i = 0; i < nBins; i++) {
		for(int j = 0; j < nBinsCorr; j++) {

			//Check for bin match
			if(((x[i] - exl[i]) == c_exl[j]) && ((x[i] + exh[i]) == c_exh[j])) {

				if(debug) std::cout << cn << mn << "Bins Match (i, j): (" << i << ", " << j << ")" << std::endl;

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
