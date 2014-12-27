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
	 std::cout<<cn<<mn<<"dividedByBinWidth= " <<dividedByBinWidth<<std::endl;

	 std::cout<<cn<<mn<<"GetBandwithPDF= "   <<mainsteeringFile->GetBandwithPDF()<<std::endl;
	 std::cout<<cn<<mn<<"GetBandwithAlphas= "<<mainsteeringFile->GetBandwithAlphaS()<<std::endl;
	 std::cout<<cn<<mn<<"GetBandwithScales= "<<mainsteeringFile->GetBandwithScales()<<std::endl;
	 std::cout<<cn<<mn<<"GetBandTotal= "     <<mainsteeringFile->GetBandTotal()<<std::endl;
	}

        // The logic below only works, if we invert the defaults set in
        // SPXSteering
        //
        if (mainsteeringFile->GetBandwithPDF()==false) 
	 pdf->SetDoPDFBand (mainsteeringFile->GetBandwithPDF()); 
        if (mainsteeringFile->GetBandwithAlphaS()==true) 
         pdf->SetDoAlphaS  (mainsteeringFile->GetBandwithAlphaS());
        if (mainsteeringFile->GetBandwithScales()==true) 
         pdf->SetDoScale   (mainsteeringFile->GetBandwithScales());
        if (mainsteeringFile->GetBandTotal()==false) 
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

void SPXCrossSection::ApplyCorrections(TGraphAsymmErrors* g) {

 std::string mn = "ApplyCorrections: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (!g) {
  std::ostringstream oss;
  oss << cn << mn << "ERROR Graph to apply corrections not found "; 
  throw SPXParseException(oss.str());
 }

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 if(ncorr == 0) {
  if (debug) std::cout << cn << mn << "no correction file specified" << std::endl;
  return;
 }

 std::cout << cn << mn << ">>>>>>>>>>>>>>>>> APPLY CORRECTIONS <<<<<<<<<<<<<<<<<" << std::endl;
 if (debug) {
  std::cout << cn << mn << " available corrections" << std::endl;
  for (int i=0; i<ncorr; i++) {
   std::string filename = pci->gridSteeringFile.GetCorrectionFile(i);
   std::cout<<mn<<cn<<"filename= "<<filename<<std::endl;
  }
 }

 //for (int i=0; i<ncorr; i++) {
 // std::string filename = pci->gridSteeringFile.GetCorrectionFile(i);
 // if (debug) std::cout<<mn<<cn<<"filename= "<<filename<<std::endl;
 // TGraphAsymmErrors *gcorr=corrections->GetCorrectionGraph(filename);
 // SPXGraphUtilities::Multiply(g,gcorr);
 //}

}
