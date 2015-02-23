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
void SPXCrossSection::Create(SPXSteeringFile *mainsteeringfile) {
 std::string mn = "Create: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 mainsteeringFile=mainsteeringfile;
 if(!mainsteeringFile)
  throw SPXParseException(cn+mn+"main steering file not found !");

 //Attempt to create the Grid
 try {
  grid = new SPXGrid(pci);
 } catch(const SPXException &e) {
  throw;
 }

 if (debug) std::cout<<cn<<mn<<"Attached the GRID "<<endl;

 //Attempt to create the PDF object and perform convolution
 try {
  //pdf = new SPXPDF(psf, grid->GetGridName());
  pdf = new SPXPDF(psf, grid);
 } catch(const SPXException &e) {
  throw;
 }

 //@TODO What to do when the grid IS divided but the reference is NOT?
 dividedByBinWidth = this->pci->gridSteeringFile.IsGridDividedByBinWidth();

 int bncorr=mainsteeringFile->GetNumberofCorrectionToBand();

 if (debug) {

  std::cout<<cn<<mn<<"Created the PDF-class "<<endl;
  std::cout<<cn<<mn<<"dividedByBinWidth= " <<dividedByBinWidth<<std::endl;

  std::cout<<cn<<mn<<"GetBandwithPDF= "   <<mainsteeringFile->GetBandwithPDF()<<std::endl;
  std::cout<<cn<<mn<<"GetBandwithAlphas= "<<mainsteeringFile->GetBandwithAlphaS()<<std::endl;
  std::cout<<cn<<mn<<"GetBandwithScales= "<<mainsteeringFile->GetBandwithScales()<<std::endl;
  std::cout<<cn<<mn<<"GetBandTotal= "     <<mainsteeringFile->GetBandTotal()<<std::endl;
  for (int i=0; i<bncorr; i++) {
   if (mainsteeringFile->GetGridCorrectionToBand(i))
    std::cout<<cn<<mn<<"Grid correction i= "<<i<<" is ON -> include to Map "<<std::endl;
   else
    std::cout<<cn<<mn<<"Grid correction i= "<<i<<" is OFF  "<<std::endl;
  }
 }

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 if (debug){
  std::cout<<cn<<mn<<"Number of corrections from main steering "<<bncorr<<std::endl;
  std::cout<<cn<<mn<<"Number of corrections from grid steering "<< ncorr<<std::endl;
 }

 if (bncorr>ncorr) {
  std::cout<<cn<<mn<<"Number of grid correction in Grid file "<<ncorr
           <<" and main steering file "<<bncorr<<" do not match"<<std::endl;
  }

  // The logic below only works, if we invert the defaults set in
  // SPXSteering
  //
  if (mainsteeringFile->GetBandwithPDF()==false) 
   pdf->SetDoPDFBand (mainsteeringFile->GetBandwithPDF()); 
  if (mainsteeringFile->GetBandwithAlphaS()==true) 
   pdf->SetDoAlphaS  (mainsteeringFile->GetBandwithAlphaS());
  if (mainsteeringFile->GetBandwithScales()==false) 
   pdf->SetDoScale   (mainsteeringFile->GetBandwithScales());
  if (mainsteeringFile->GetBandTotal()==false) 
   pdf->SetDoTotError(mainsteeringFile->GetBandTotal());

  std::vector<double> RenScales=mainsteeringFile->GetRenScales();
  std::vector<double> FacScales=mainsteeringFile->GetFacScales();

  if (RenScales.size()!=FacScales.size()) {
   std::ostringstream oss;
   oss << cn << mn << "Different number of values given for rennormalisation and factorisation scale";
   throw SPXParseException(oss.str());
  }

  if (RenScales.size()!=0)
   pdf->SetScales(RenScales,FacScales);

  if (debug) std::cout<<cn<<mn<<" Initialize the PDF "<<std::endl;

  pdf->Initialize();
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

void SPXCrossSection::UpdateBandandHisto() {
 std::string mn = "UpdateBandandHisto: ";
 // calculate total uncertainties
 
 pdf->CalcTotalErrors();

 //Convert reference and nominal histograms to graphs and save them
 if (!pdf->GetPDFNominal()) {
  throw SPXParseException(cn+mn+"Nominal PDF histogram not found !");
 }

 if (!grid->GetReference()) {
  std::cout<<cn<<mn<<"WARNING: reference histogram not found ! "<<endl;
 }

 SPXGraphUtilities::HistogramToGraph(gridReference, grid->GetReference());
 SPXGraphUtilities::HistogramToGraph(nominal,       pdf->GetPDFNominal());
 
 return;
}

void SPXCrossSection::ApplyCorrections() {
 std::string mn = "ApplyCorrections: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if(!mainsteeringFile)
  throw SPXParseException(cn+mn+"main steering file not found !");

 if(mainsteeringFile->ApplyGridCorr()) {
   if (debug) std::cout << cn << mn << "INFO: grid corrections will be applied to cross sections !" << std::endl;
 } else {
   if (debug) std::cout << cn << mn << "INFO: No grid correction asked for in steering file ! return " << std::endl;
   return;
 }

 if (debug) std::cout << cn << mn << "Parse grid corrections" << std::endl;
 this->ParseCorrections();

 int bncorr=mainsteeringFile->GetNumberofCorrectionToBand();

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 if(ncorr == 0) {
  std::cout << cn << mn << "WARNING: No grid correction file specified, but ApplyCorrection called !" << std::endl;
  return;
 }

 if (bncorr>ncorr) {
  std::cout<<cn<<mn<<"WARNING: Number of grid correction in Grid file "<<ncorr
                  <<" and main steering file "<<bncorr<<" do not match"<<std::endl;
 }

 if (debug) std::cout << cn << mn << ">>>>>>>>>>>>>>>>> APPLY CORRECTIONS <<<<<<<<<<<<<<<<<" << std::endl;

 if (debug) {
  std::cout << cn << mn << "Available corrections from Grid ncorr= " << ncorr << " requested to include bncorr= "<<bncorr<< std::endl;
 }

 if (corrections==0)
  std::cout << cn << mn << "WARNING: No correction found but ncorr= " << ncorr << std::endl;

 for (int i=0; i<ncorr; i++) {
  std::string filename = pci->gridSteeringFile.GetCorrectionFile(i);
  std::string corrLabel= pci->gridSteeringFile.GetCorrectionFileLabel(i);
  std::string griddir  = pci->gridDirectory;
  
  filename=griddir+"/"+filename;
  std::cout<<cn<<mn<<"Apply corrections from filename= "<<filename<<" label= "<<corrLabel.c_str()<<std::endl;

  TGraphAsymmErrors *gcorr=corrections->GetCorrectionGraph(filename);
  if (!gcorr) {
   throw SPXGraphException(cn + mn + "Correction graph not found for filename= "+filename);
  }
  if (debug) {
   std::cout<<cn<<mn<<"Print correction graph: "<<gcorr->GetName()<<std::endl;
   gcorr->Print();
  }

  bool includeinband=mainsteeringFile->GetGridCorrectionToBand(i);
  pdf->ApplyBandCorrection(gcorr,corrLabel,includeinband);
 }

 if (debug) std::cout << cn << mn << "APPLY CORRECTIONS finished !" << std::endl;
}


std::vector<std::string> SPXCrossSection::GetCorrectionLabels() {
 std::string mn = "GetCorrectionLabels: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 correctionlabels.clear();

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 if(ncorr == 0) {
  std::cout << cn << mn << "WARNING: No grid correction file specified, but ApplyCorrection called !" << std::endl;
  return correctionlabels;
 }

 if (debug) {
  std::cout << cn << mn <<"corrections ncorr= " << ncorr << std::endl;
 }
 
 correctionlabels.clear();

 for (int i=0; i<ncorr; i++) {
  std::string filename = pci->gridSteeringFile.GetCorrectionFile(i);
  std::string corrLabel= pci->gridSteeringFile.GetCorrectionFileLabel(i);
  std::string griddir  = pci->gridDirectory;
  
  correctionlabels.push_back(corrLabel);

  if (debug) {
   std::cout<<cn<<mn<<"corrLabel= "<<corrLabel.c_str()<<std::endl;
  } 
 }
 return correctionlabels;
}
void SPXCrossSection::MatchBinning(StringGraphMap_T dataFileGraphMap) {
 std::string mn = "MatchBinning: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 //Match binning of all graphs within each PCI, if matchBinning set
 if(mainsteeringFile->GetMatchBinning()) {

  TGraphAsymmErrors *master=0;

  if(debug) std::cout << cn << mn << "Matched binning is ON" << std::endl;

  if(debug) std::cout << cn << mn << "Matching cross section bands to data master" << std::endl;

  std::string dataKey = pci->dataSteeringFile.GetFilename();
  std::string gridKey = pci->gridSteeringFile.GetFilename();
  std::string pdfKey  = pci->pdfSteeringFile.GetFilename();

  StringPair_T convoluteKey(gridKey, pdfKey);

  if(debug) std::cout << cn << mn << "dataKey= "<<dataKey
                                  <<" gridKey= "<<gridKey <<" pdfKey= "<<pdfKey<< std::endl;

  master = dataFileGraphMap[dataKey];
  if (!master) {
   std::ostringstream oss;
   oss << cn << mn << "Data master graph not found ! ";
   throw SPXParseException(oss.str());
  }

  if (debug)
   std::cout << cn << mn << "data master file name "<<master->GetName()<<std::endl;

  //
  // Check if data master is divided by bin width
  bool dividedByBinWidth = false;
  //TC if(pci.dataSteeringFile.IsDividedByBinWidth() && !pci.gridSteeringFile.IsGridDividedByBinWidth()) {
  if(pci->dataSteeringFile.IsDividedByBinWidth() && pci->gridSteeringFile.IsGridDividedByBinWidth()) {
   dividedByBinWidth = true;
  }
                                
  // loop over all Band in pdf
  int nbands=pdf->GetNBands();
  if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;

  for (int iband=0; iband<nbands; iband++) {
   TGraphAsymmErrors * gband   =pdf->GetBand(iband);
   if (!gband) {
    std::ostringstream oss;
    oss << cn <<mn<<"get bands "<<"Band "<<iband<<" not found at index "<<iband;
    throw SPXGeneralException(oss.str());
   }
   if (debug) std::cout << cn <<mn<<"Match binning for graph "<<gband->GetName()<<std::endl;
   //if (debug) if (dividedByBinWidth)  std::cout << cn <<mn<<"Divided by binwidth is ON "<<std::endl;

   SPXGraphUtilities::MatchBinning(master, gband, dividedByBinWidth);
   if (debug) {
    std::cout << cn <<mn<<"After matching pdf graph "<<gband->GetName()<<" to data "<<master->GetName()<<std::endl;
    gband->Print();
   }
  }
 }
}

