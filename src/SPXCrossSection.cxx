//************************************************************/
//
//	CrossSection Implementation
//
//	Implements the SPXCrossSection class, which performs the
//	convolution and all math required to calculate the cross-
//	section from a PDF and a grid file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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
bool SPXCrossSection::debug=false;
 
//Create the CrossSection
void SPXCrossSection::Create(SPXSteeringFile *mainsteeringfile) {
 std::string mn = "Create: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 mainsteeringFile=mainsteeringfile;
 if (!mainsteeringFile)
  throw SPXParseException(cn+mn+"main steering file not found !");

 numberofgridcorrection=0;

 //Attempt to create the Grid
 try {

  if (debug) std::cout<<cn<<mn<<"Create grid with name= "<<gridname.c_str()<<std::endl;

  grid = new SPXGrid(pci);

  if (debug) std::cout<<cn<<mn<<"Created grid with name= "<<gridname.c_str()<<std::endl;

  grid->SetGridName(gridname);
  grid->CreateGrid();

 } catch(const SPXException &e) {
  throw;
 }

 if (debug) std::cout<<cn<<mn<<"Attach the GRID "<<std::endl;
 //Attempt to create the PDF object and perform convolution
 try {
  pdf = new SPXPDF(psf, grid);
 } catch(const SPXException &e) {
  throw;
 }

 if (debug) std::cout<<cn<<mn<<"Attached the GRID "<<std::endl;

 //@TODO What to do when the grid IS divided but the reference is NOT?
 dividedByBinWidth = this->pci->gridSteeringFile.IsGridDividedByBinWidth();

 if (debug) {

  std::cout<<cn<<mn<<"Created the PDF-class "<<std::endl;
  std::cout<<cn<<mn<<"dividedByBinWidth= " <<(dividedByBinWidth? "ON" : "OFF")<<std::endl;

  std::cout<<cn<<mn<<"GetBandwithPDF= "    <<(mainsteeringFile->GetBandwithPDF() ? "ON" : "OFF") <<std::endl;
  std::cout<<cn<<mn<<"GetBandwithAlphas= " <<(mainsteeringFile->GetBandwithAlphaS()? "ON" : "OFF")<<std::endl;
  std::cout<<cn<<mn<<"GetBandwithScales= " <<(mainsteeringFile->GetBandwithScales()? "ON" : "OFF")<<std::endl;
  std::cout<<cn<<mn<<"GetBandwithAlternativeScaleChoice= " <<(mainsteeringFile->GetBandwithAlternativeScaleChoice()? "ON" : "OFF")<<std::endl;
  std::cout<<cn<<mn<<"GetBeamUncertainty= "<<(mainsteeringFile->GetBeamUncertainty()? "ON" : "OFF")<<std::endl;
  std::cout<<cn<<mn<<"GetBandTotal= "      <<(mainsteeringFile->GetBandTotal()? "ON" : "OFF")<<std::endl;

 }

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 if (debug){                                                      // start counting at 0
  std::cout<<cn<<mn<<"Number of corrections from grid steering "<< ncorr+1<<std::endl;
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

 if (mainsteeringFile->GetBandwithAlternativeScaleChoice()==true) 
  pdf->SetDoAlternativeScaleChoice(mainsteeringFile->GetBandwithAlternativeScaleChoice());

 if (mainsteeringFile->GetBeamUncertainty()!=1.) 
  pdf->SetChangeSqrtS (mainsteeringFile->GetBeamUncertainty());

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

 if (mainsteeringFile->GetParameterScan()) 
  pdf->SetParameterScan(mainsteeringFile->GetParameterScan());

 if (debug) std::cout<<cn<<mn<<"Initialize the PDF "<<std::endl;

 pdf->Initialize();

 //Convert reference and nominal histograms to graphs and save them
 if (!pdf->GetPDFNominal()) {
  throw SPXParseException(cn+mn+"Nominal PDF histogram not found !");
 }

 if (!grid->GetReference()) {
  std::cout<<cn<<mn<<"WARNING: reference histogram not found ! "<<std::endl;
 }

 SPXGraphUtilities::HistogramToGraph(gridReference, grid->GetReference());
 SPXGraphUtilities::HistogramToGraph(nominal,       pdf->GetPDFNominal());
 TString nomname="Nominal";
 nomname+=nominal->GetName();
 nominal->SetName(nomname);

 if (debug) {
  std::cout<<cn<<mn<<"Nominal graph "<<nominal->GetName()<<" Nbins= "<<nominal->GetN()<<std::endl;
  nominal->Print();
 }
}


void SPXCrossSection::ParseCorrections(void) {
 std::string mn = "ParseCorrections: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);



 //Check if grid contains corrections
 if (pci->gridSteeringFile.GetNumberOfCorrectionFiles() != 0) {
  try {
   gridcorrection = new SPXGridCorrections(*pci);
   gridcorrection->Parse();

   if (debug) {
    std::cout<<cn<<mn<<"Sucessfully read in corrections: "<<std::endl;
    gridcorrection->Print();
   }
  } catch(const SPXException &e) {
   throw;
  }
 }
}

void SPXCrossSection::PrintTotalCrossSection() {
 std::string mn = "PrintTotalCrossSection: ";
 SPXUtilities::PrintMethodHeader(cn, mn);

 int nbands=pdf->GetNBands();
 if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;

 for (int iband=0; iband<nbands; iband++) {
  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
  TString gname=gband->GetName();
  if (!gband) {
   std::ostringstream oss;
   oss << cn <<mn<<"get bands "<<"Band "<<iband<<" not found at index "<<iband;
   throw SPXGeneralException(oss.str());
  }
  if (gname.Contains("_total_")) {
   double sigtot=0.;
   for (int i=0; i<gband->GetN(); i++) {
    double xlow  = gband->GetErrorXlow(i);
    double xhigh = gband->GetErrorXhigh(i);
    double binw=xlow+xhigh;
    double x_val, y_val;
    gband->GetPoint(i, x_val, y_val);
    sigtot+=y_val*binw;
   }
   std::cout<<cn<<mn<<"Theory cross section "<<gridname.c_str()<<std::endl;
   std::cout<<cn<<mn<<"Band name= "<<gname.Data()<<std::endl;
   std::cout<<cn<<mn<<"Total integrated cross section= "<<sigtot<<std::endl;

  }
 }

 return;
}

void SPXCrossSection::UpdateBand() {
 std::string mn = "UpdateBandandHisto: ";
 //
 // calculate total uncertainties
 //
 pdf->CalcTotalErrors();

 return;
}

void SPXCrossSection::ApplyCorrections() {
 std::string mn = "ApplyCorrections: ";
 //debug=true;
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if(!mainsteeringFile)
  throw SPXParseException(cn+mn+"Main steering file not found !");

 if(mainsteeringFile->ApplyGridCorr()) {
  if (debug) std::cout<<cn<<mn<<"INFO: grid corrections will be applied to cross sections !" << std::endl;
 } else {
  if (debug) std::cout<<cn<<mn<<"INFO: No grid correction asked for in steering file ! return " << std::endl;
  return;
 }

 if (debug) std::cout<<cn<<mn<<"Parse grid corrections" << std::endl;
 this->ParseCorrections();

 int bncorr=mainsteeringFile->GetNumberofCorrectionToBand();

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 if (ncorr == 0) {
  std::cout<<cn<<mn<<"WARNING: No grid correction file specified, but ApplyCorrection called !"<<std::endl;
  return;
 }

 if (bncorr>ncorr) {
  std::cout<<cn<<mn<<"WARNING: Number of grid correction in Grid file "<<ncorr
                  <<" and main steering file "<<bncorr<<" do not match"<<std::endl;
 }

 if (!mainsteeringFile->KeepGridCorr().empty()) {
   std::cout<<cn<<mn<<"INFO: Only keep systematics that contains "<<mainsteeringFile->KeepGridCorr()<<std::endl;
 }

 if (debug) std::cout<<cn<<mn<<">>>>>>>>>>>>>>>>> APPLY CORRECTIONS <<<<<<<<<<<<<<<<<"<<std::endl;

 if (debug) {
  std::cout<<cn<<mn<<"Available corrections from Grid ncorr= "<<ncorr<<" requested to include bncorr= "<<bncorr<<std::endl;
 }

 if (gridcorrection==0)
  std::cout<<cn<<mn<<"WARNING: gridcorrection not defined ncorr= "<<ncorr<<std::endl;

 for (int i=0; i<ncorr; i++) {
  std::string filename = pci->gridSteeringFile.GetCorrectionFile(i);
  std::string corrLabel= pci->gridSteeringFile.GetCorrectionFileLabel(i);
  std::string griddir  = pci->gridDirectory;
  
  filename=griddir+"/"+filename;

  if (!mainsteeringFile->KeepGridCorr().empty()) {
     std::cout<<cn<<mn<<"Test if filename= "<<filename<<" contains= "<<mainsteeringFile->KeepGridCorr()<<std::endl;
    if (TString(filename).Contains(mainsteeringFile->KeepGridCorr())){
     std::cout<<cn<<mn<<"Apply corrections from filename= "<<filename<<" label= "<<corrLabel.c_str()<<std::endl;
    } else {
     std::cout<<cn<<mn<<"Do NOT apply corrections filename= "<<filename<<" label= "<<corrLabel.c_str()<<std::endl;
     continue;
    }
  } else {
   std::cout<<cn<<mn<<"Apply corrections from filename= "<<filename<<" label= "<<corrLabel.c_str()<<std::endl;
  }

  numberofgridcorrection++; //count number of correction actually applied
  bool includeinband=true;
  if (numberofgridcorrection>1) includeinband=false;

  if (debug) {
   std::cout<<cn<<mn<<"includeinband= "<<(includeinband ? "ON" : "OFF")
            <<" numberofgridcorrection= "<<numberofgridcorrection<<std::endl;
  }
  TGraphAsymmErrors *gcorr=gridcorrection->GetCorrectionGraph(filename);
  if (!gcorr) {
   throw SPXGraphException(cn+mn+"Correction graph not found for filename= "+filename);
  }

  if (debug) {
   std::cout<<cn<<mn<<"Print correction graph gcorr: "<<gcorr->GetName()<<std::endl;
   gcorr->Print();
  }

  if (mainsteeringFile->KeepGridCorr().empty()) {
   includeinband=mainsteeringFile->GetGridCorrectionToBand(i);
   if (debug) {
    std::cout<<cn<<mn<<"includeinband= "<<(includeinband ? "ON" : "OFF")<<" i= "<<i<<std::endl;
    std::cout<<cn<<mn<<"contain_apply_grid overwrites band_with_grid "<<std::endl;
   }
  }

  if (mainsteeringFile->ApplyNominalCorr()){
   if (debug) std::cout<<cn<<mn<<"Apply correction to nominal "<<std::endl;
   //correct nominal graph
   SPXGraphUtilities::MatchBinning(nominal, gcorr, true);
   SPXGraphUtilities::Multiply(nominal,gcorr,1);
  }

  // correct PDF band
  if (debug) std::cout<<cn<<mn<<"Now correct PDF band includeinband="<<(includeinband ? "ON" : "OFF")<<std::endl;
  pdf->ApplyBandCorrection(gcorr,corrLabel,includeinband);
 }

 if (debug) std::cout<<cn<<mn<<"finished !"<<std::endl;
 return;
}


std::vector<std::string> SPXCrossSection::GetCorrectionLabels() {
 std::string mn = "GetCorrectionLabels: ";
 //if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 correctionlabels.clear();

 int ncorr=pci->gridSteeringFile.GetNumberOfCorrectionFiles(); 
 //int ncorr=numberofgridcorrection;    
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
  
  if (debug) std::cout<<cn<<mn<<"filename= "<<filename<<std::endl; 

  if (!mainsteeringFile->KeepGridCorr().empty()) {
   if (TString(filename).Contains(mainsteeringFile->KeepGridCorr())){
    if (debug) std::cout<<cn<<mn<<"Put corrections filename= "<<filename<<" to label "<<std::endl;
   } else {
    if (debug) std::cout<<cn<<mn<<"Do NOT put corrections filename= "<<filename<<" to label "<<std::endl;
    continue;
   }
  }
  
  correctionlabels.push_back(corrLabel);
 
 }

 if (debug) {
  for (int i=0; i<correctionlabels.size(); i++)
    std::cout<<cn<<mn<<"corrLabel["<<i<<"]= "<<correctionlabels.at(i)<<std::endl;
 }

 return correctionlabels;
}
void SPXCrossSection::MatchBinning(StringGraphMap_T dataFileGraphMap) {
 std::string mn = "MatchBinning: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 //Match binning of all graphs within each PCI, if matchBinning set
 if (mainsteeringFile->GetMatchBinning()) {

  TGraphAsymmErrors *master=0;

  if (debug) std::cout << cn << mn << "Matched binning is ON" << std::endl;

  if (debug) std::cout << cn << mn << "Matching cross section bands to data master" << std::endl;

  std::string dataKey = pci->dataSteeringFile.GetFilename();
  //std::string gridKey = pci->gridSteeringFile.GetFilename();
  //std::string pdfKey  = pci->pdfSteeringFile.GetFilename();

  //StringPair_T convoluteKey(gridKey, pdfKey);

  //if(debug) std::cout << cn << mn << "dataKey= "<<dataKey
  //                                <<" gridKey= "<<gridKey <<" pdfKey= "<<pdfKey<< std::endl;

  if (debug) std::cout << cn << mn << "dataKey= "<<dataKey<< std::endl;

  master = dataFileGraphMap[dataKey];
  if (!master) {
   std::ostringstream oss;
   oss << cn << mn << "Data master graph not found ! ";
   throw SPXParseException(oss.str());
  }

  if (debug)
   std::cout << cn << mn << "master file name "<<master->GetName()<<std::endl;

  //
  // Check if data master is divided by bin width
  //
  bool dividedByBinWidth = false;
  //
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
   if (debug) std::cout << cn <<mn<<"Match binning for slave graph "<<gband->GetName()<<std::endl;
   //if (debug) if (dividedByBinWidth)  std::cout << cn <<mn<<"Divided by binwidth is ON "<<std::endl;

   SPXGraphUtilities::MatchBinning(master, gband, dividedByBinWidth);
   if (debug) {
    std::cout << cn <<mn<<"After matching pdf graph "<<gband->GetName()<<" to data "<<master->GetName()<<std::endl;
    gband->Print();
   }
  }

  if (!nominal) {
   throw SPXGeneralException(cn+mn+"Nominal graph not found");
  }

  if (debug) std::cout << cn <<mn<<"Match binning for nominal graph "<<nominal->GetName()<<std::endl; 
  SPXGraphUtilities::MatchBinning(master, nominal, dividedByBinWidth);
  if (debug) {
   std::cout << cn <<mn<<"After matching nominal graph "<<nominal->GetName()<<" to data "<<master->GetName()<<std::endl;

   nominal->Print();
   std::cout << cn <<mn<<"Number of bins "<<nominal->GetN()<<std::endl;
  }

  // Now also match the individual histograms

  int npdfcomponents=pdf->GetNumberOfIndividualPDFComponents();
  if (debug) {
   std::cout<<cn<<mn<<"Number of individual PDF components  " << npdfcomponents << std::endl;
  }
  for (int ipdf=0; ipdf<npdfcomponents; ipdf++) {
   TH1D *hcomp= pdf->GetIndividualPDFComponent(ipdf);
   if (!hcomp) {std::cout<<cn<<mn<<"Histogram for component ipdf= "<<ipdf<<" not found "<<std::endl; continue;}
  
   TH1D* hnew=SPXGraphUtilities::MatchBinning(master, hcomp, true);  
   if (debug) {
    std::cout<<cn<<mn<<"After MatchBinning for Histogram "<<hnew->GetName()<<std::endl;   
    hnew->Print("all");
   }
   pdf->SetIndividualPDFComponent(ipdf,hnew); 
  }

  int nscalecomponents=pdf->GetNumberOfIndividualScaleVariations();
  if (debug) {
   std::cout<<cn<<mn<<"Number of individual scale variations  " << nscalecomponents << std::endl;
  }
  for (int iscale=0; iscale<nscalecomponents; iscale++) {
   TH1D *hcomp= pdf->GetIndividualScaleVariation(iscale);
   if (!hcomp) {std::cout<<cn<<mn<<"Histogram for component iscale= "<<iscale<<" not found ! "<<std::endl; continue;}
   TH1D* hnew=SPXGraphUtilities::MatchBinning(master, hcomp, true);  
   if (debug) {
    std::cout<<cn<<mn<<"After MatchBinning for Histogram "<<hnew->GetName()<<std::endl;   
    hnew->Print("all");
   }
   pdf->SetIndividualScaleVariation(iscale,hnew); 
  }
  // read alphas uncertainty components
  int nalphascomponents=pdf->GetNumberOfIndividualAlphaSVariations();
  if (debug) {
   std::cout<<cn<<mn<<"Number of individual alphas variations  " << nalphascomponents << std::endl;
  }

  for (int ialphas=0; ialphas<nalphascomponents; ialphas++) {
   TH1D *hcomp= pdf->GetIndividualAlphaSVariation(ialphas);
   if (!hcomp) {std::cout<<cn<<mn<<"Histogram for component ialphas= "<<ialphas<<" not found ! "<<std::endl; continue;}
   TH1D* hnew=SPXGraphUtilities::MatchBinning(master, hcomp, true);  
   if (debug) {
    std::cout<<cn<<mn<<"After MatchBinning for Histogram "<<hnew->GetName()<<std::endl;   
    hnew->Print("all");
   }
   pdf->SetIndividualAlphaSVariation(ialphas,hnew); 
  }
 }
}

