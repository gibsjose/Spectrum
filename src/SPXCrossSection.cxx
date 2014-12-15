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
bool SPXCrossSection::debug=true;

//Create the CrossSection
void SPXCrossSection::Create(void) {
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


	if (debug) std::cout<<cn<<mn<<"Created the PDF-class "<<endl;
        pdf->SetDoPDFBand(true);
        pdf->SetDoAlphaS(false);
        pdf->SetDoScale(false);
        pdf->SetDoTotError(false);

        std::vector<double> RenScales;
        std::vector<double> FacScales;
        RenScales.push_back(1.);
        FacScales.push_back(1.);
        RenScales.push_back(0.5);
        FacScales.push_back(0.5);
        RenScales.push_back(2.0);
        FacScales.push_back(2.0);
        RenScales.push_back(0.5);
        FacScales.push_back(1.0);
        RenScales.push_back(1.0);
        FacScales.push_back(0.5);

        pdf->SetScales(RenScales,FacScales);

        pdf->Initialize();

	//Convert reference and nominal histograms to graphs and save them
        if (!pdf->GetPDFNominal()) std::cout<<cn<<mn<<"nominal PDF not found ! "<<endl;
        if (!grid->GetReference()) std::cout<<cn<<mn<<"reference histogram not found ! "<<endl;

	SPXGraphUtilities::HistogramToGraph(gridReference, grid->GetReference());
	SPXGraphUtilities::HistogramToGraph(nominal, pdf->GetPDFNominal());

	//Set the name of the convolution graphs appropriately
	//Create name strings
	TString name;
	TString pdfName;
	TString alphaSName;
	TString scaleName;

	//Check if name exists
	if(!pci->gridSteeringFile.GetName().empty()) {
		name = TString(pci->gridSteeringFile.GetName());
		pdfName = name + "_pdf";
		alphaSName = name + "_alpha_s";
		scaleName = name + "_scale";
	}

	//Grid steering file has no [DESC]:name
	//Default to filename
	else {
		if(debug) std::cout << cn << mn << "Grid steering file has no name value: using filename instead" << std::endl;
		name = pci->gridSteeringFile.GetFilename();
		name.ReplaceAll(TString(".txt"), TString(""));
		pdfName = name + "_pdf";
		alphaSName = name + "_alpha_s";
		scaleName = name + "_scale";
	}

	//Set the graph names
	//
	if (debug) std::cout<<cn<<mn<<"Set graph names h_PDFBand_results pdfNames= "<<pdfName<<endl;
	if(pdf->h_PDFBand_results) pdf->h_PDFBand_results->SetName(pdfName);
        else if (debug) std::cout<<cn<<mn<<"h_PDFBand_results not found ! "<<endl;

	if (debug) std::cout<<cn<<mn<<"Set graph h_AlphaS_results alphasName "<<alphaSName<<endl;
	if(pdf->h_AlphaS_results) pdf->h_AlphaS_results->SetName(alphaSName);
        else if (debug) std::cout<<cn<<mn<<"h_AlphaS_results not found ! "<<endl;

	if (debug) std::cout<<cn<<mn<<"Set graph h_Scale_result scaleName "<<scaleName<<endl;
	if(pdf->h_Scale_results) pdf->h_Scale_results->SetName(scaleName);
        else if (debug) std::cout<<cn<<mn<<"h_Scale_results not found ! "<<endl;

	if (debug) std::cout<<cn<<mn<<" finished "<<std::endl;
}

void SPXCrossSection::ParseCorrections(void) {
	std::string mn = "ParseCorrections: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
