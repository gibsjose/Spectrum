//************************************************************/
//
//  PDF Implementation
//
//  Implements the SPXPDF class, which manages the PDF data
//
//  @Author:  J. Gibson, C. Embree, T. Carli - CERN ATLAS
//  @Date:    01.06.2013 - Renamed 15.10.2014
//  @Email:   gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <sstream>

#include "SPXPDF.h"

//Patch for faulty G++ compiler <string> guards...
// Somewhere in <string> there is an issue where there are some #ifdef guards
// that are incorrectly not being passed to allow the std::to_string function
// to be properly build and linked...
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

//Class name for debug statements
const std::string cn = "SPXPDF::";

//Must define the static debug variable in the implementation
bool SPXPDF::debug;


std::string SPXPDF::GetEnv( const std::string & var ) {
		const char* res= getenv( var.c_str() );

		std::string s = res!=NULL? res:"";
		if (debug) std::cout<<"s: "<<s<<std::endl;
		return s;
}

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
// version 6
LHAPDF::PDF* mypdf=0;
//void getPDF(const double &X, const double &Q, double xfs[13]) {
void getPDF(const double &X, const double &Q, double *xfs) {
  std::vector <double> xf; xf.resize(13);

  //std::cout<<"getpdf: X= "<<X<<" Q= "<<Q<<std::endl;
  if (!mypdf) std::cout<<" getpdf mypdf not found !"<<std::endl;

  double x=X;
  if (x>=1) x-=1.e-12;
  mypdf->xfxQ(x, Q, xf);
 
  //std::cout<<"LHAPDF6 getpdf: number of pdfs= "<<xf.size()<<std::endl;

  for (int i=0; i<xf.size(); i++) {
   xfs[i]=xf.at(i);
  //std::cout<<cn<<"getpdf: xfs["<<i<<"]= "<<xfs[i]<<std::endl;
  }
  return; 
}
double alphasPDF(const double & Q) {
  
  return mypdf->alphasQ(Q); 
}
#else
//version 5
void getPDF(const double& x, const double& Q, double* xf) {
  //std::cout<<cn<<" LHAPDF5: getpdf: x= "<<x<<" Q= "<<Q<<std::endl;
  evolvepdf_(&x, &Q, xf);
}

#endif


/******************************************************************
 ** Method Implementations
 ******************************************************************/

SPXPDF::SPXPDF(SPXPDFSteeringFile *psf, int iflpdf, double Q2value, TH1D* h1)
{
 //
 // constructor to make PDF only plots
 // iflpdf convention [-6,0,6] zero is gluon
 //
 SetVariablesDefault();
 gridName="";
 nLoops = 1; //default is NLO, steering will overwrite

 ReadPDFSteeringFile(psf);

 hpdf=h1;
 if (!hpdf) std::cout<<cn<<" histograme not found "<<std::endl;

 ifl=iflpdf;
 Q2=Q2value;

 std::cout<<cn<<"Get PDF for Q2= "<<Q2<<" ifl= "<<ifl<<std::endl;
 if (debug) {
  std::cout<<cn<<" Get PDF for Q2= "<<Q2<<" ifl= "<<ifl<<std::endl;
  std::cout<<cn<<" Histogram: "<<std::endl;
  hpdf->Print("all");
 }


 if (debug) std::cout<<cn<<"SPXPDF: end constructor PDF-only "<<std::endl;
}

SPXPDF::SPXPDF(SPXPDFSteeringFile *psf, const std::string &_gridName)
{
  //
  spxgrid=0;
  gridName=_gridName;

  std::cout<<cn<<"constructor called via name "<<gridName.c_str()<<std::endl;

  SetUpParameters(psf);
}


SPXPDF::SPXPDF(SPXPDFSteeringFile *psf, SPXGrid *grid)
{

  spxgrid=grid;
  gridName=grid->GetGridName();
  //gridName=SPXStringUtilities::ReplaceAll(gridName,".txt","");
  std::cout<<cn<<"constructor called via SPXGrid name= "<<gridName.c_str()<<std::endl;

  SetUpParameters(psf);
}

void SPXPDF::SetUpParameters(SPXPDFSteeringFile *psf) {
 std::string mn = "SetUpParameters: ";
 if (debug) std::cout<<cn<<mn<<" Start overloaded constructor"<<std::endl;

 SetVariablesDefault();

 RenScales.clear();
 FacScales.clear();

 ReadPDFSteeringFile(psf);

 h_PDF_results=0;
 h_AlphaS_results=0;
 h_Scale_results=0;
 h_BeamUncertainty_results=0;
 h_Total_results=0;

}

//This function replaces the ::ReadSteering(), since the PDF Steering file has already been parsed by the
//Top-Level Steering File parser, and has been passed directly to the SPXPDF constructor, and thus this function
void SPXPDF::ReadPDFSteeringFile(SPXPDFSteeringFile *psf) {
 std::string mn = "ReadPDFSteeringFile: ";

 steeringFileName = psf->GetFilename();

 SPXPDF::debug = psf->GetDebug();
 if (debug) std::cout<<cn<<mn<<" Debug turned on "<< std::endl;

 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
 if(debug) std::cout<<cn<<mn<<"processing pre-parsed steering file named: "<< steeringFileName << std::endl;

  pdfSetPath = defaultPDFSetPath;
  std::string pdfSetDefaultPath = GetEnv("LHAPATH");

  if (pdfSetDefaultPath.size()>0) {
   if (pdfSetDefaultPath.find_last_of("/") == pdfSetDefaultPath.size()-1) {
    pdfSetDefaultPath = pdfSetDefaultPath.substr(0,pdfSetDefaultPath.size()-1); //remove trailing slashes if there are any
   }

   pdfSetPath=pdfSetDefaultPath;
   if (debug) std::cout<<cn<<mn<<"LHAPATH environment variable found, using path: "<<pdfSetPath<<std::endl;
  } else {
   if (debug) std::cout<<cn<<mn<<"LHAPATH environment variable not set, using default: "<<pdfSetPath<<std::endl;
  }

  PDFtype = psf->GetType();
  PDFname = psf->GetName();
  PDFnamevar = psf->GetNameVar();

  n_PDFMembers = psf->GetNumberOfMembers();
  defaultpdfid = psf->GetDefaultID();
  defaultpdfidvar = psf->GetDefaultIDVar();

  firsteig = psf->GetFirstEig();
  lasteig = psf->GetLastEig();
  firstquadvar = psf->GetFirstQuad();
  lastquadvar = psf->GetLastQuad();
  firstmaxvar = psf->GetFirstMax();
  lastmaxvar = psf->GetLastMax();

  includeEIG = psf->GetIncludeEig();
  includeQUAD = psf->GetIncludeQuad();
  includeMAX = psf->GetIncludeMax();

  nLoops = psf->GetNumberOfLoops();

  fillStyleCode = psf->GetFillStyle();
  fillColorCode = psf->GetFillColor();
  markerStyle   = psf->GetMarkerStyle();
  ErrorPropagationType= psf->GetErrorPropagationType();
  if (debug) std::cout<<cn<<mn<<"ErrorPropagationType= "<<ErrorPropagationType<<std::endl;

  PDFBandType = psf->GetBandType().ToString();

  PDFErrorType = psf->GetErrorType().ToString();
  if(psf->GetErrorType().IsAlphaS()) {
   if(debug) std::cout<<cn<<mn<<"turn do_AlphaS ON "<< std::endl;
   do_AlphaS = true;
  } else if(psf->GetErrorType().IsPDFBand()) {
   if(debug) std::cout<<cn<<mn<<"turn do_PDFBand ON "<< std::endl;
   do_PDFBand = true;
  } else if(psf->GetErrorType().IsScaleBand()) {
   if(debug) std::cout<<cn<<mn<<"turn do_Scale ON "<< std::endl;
   do_Scale = true;
  }

  //PDFErrorSize = psf->GetErrorSize().ToString();
  f_PDFErrorSize90Percent=psf->GetIs90Percent();
  if (debug) {
   if (f_PDFErrorSize90Percent) std::cout<<cn<<mn<<"is90Percent ON "<< std::endl;
   else                         std::cout<<cn<<mn<<"is90Percent OFF "<< std::endl;
  }

  if(!psf->GetPDFSetPath().empty()) {
   pdfSetPath = psf->GetPDFSetPath();
  }

  AlphaSmemberNumDown  = psf->GetAlphaSErrorNumberDown();
  AlphaSmemberNumUp    = psf->GetAlphaSErrorNumberUp();
  AlphaSPDFSetNameDown = psf->GetAlphaSPDFNameDown();
  AlphaSPDFSetNameUp   = psf->GetAlphaSPDFNameUp();

  if (TString(AlphaSPDFSetNameUp).Contains("none") ||
      TString(AlphaSPDFSetNameDown).Contains("none") ){
    std::cout<<cn<<mn<<"WARNING: Need to provide AlphaSPDFSetNameUp and AlphaSPDFSetNameDown to evaluate alphas uncertainty, do_Alphas turned off ";
    do_AlphaS=false;
  }

  if (debug) {
   std::cout<<cn<<mn<<"finished"<< std::endl;
  }
  return;
}

#ifdef TIMER
class quick_timer { 
public:
  quick_timer() : _timer(appl_timer_start()) {  }
  double time()     { return appl_timer_stop(_timer); }
private:
  struct timeval _timer;
};
#endif

TH1D *  SPXPDF::GetHisto(double renscale, double facscale){
 std::string mn = "GetHisto: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 //bool varyscale=false;
 //if (renscale!=1. || facscale!=1) varyscale=true;
 //if (varyscale) {
 //  if (debug) std::cout<<cn<<mn<<"Scale variation renscale= "<<renscale<<" facscale= "<<facscale<<std::endl;
 //}

 TString name="xsec_pdf_"+default_pdf_set_name;
 if (debug) std::cout<<cn<<mn<<"Number of grids= "<<ngrid<<std::endl;
 TH1D* htmpsum=0;
 // if beam uncertainty should be calculated, here we need the default cross section
 double xEscale=1.;
 if (!do_Escale) xEscale=Escale; 

 if (debug) std::cout<<cn<<mn<<"Beam/cms energy defEscale= "<<xEscale<<std::endl;    

 if (ngrid==1) {
 
  my_grid=spxgrid->GetGrid(0);
  gridName=spxgrid->GetGridName();
  if (!my_grid) {
   std::cout<<cn<<mn<<"No applgrid found ! "<<std::endl;
   applgridok=false;
   if (debug) std::cout<<cn<<mn<<"No applgrid found gridname= "<<TString(gridName).Data()<<std::endl;
   applgridok=false;
  } else {
   applgridok=true;
  }

  if (debug) std::cout<<cn<<mn<<"nLoops= "<<nLoops<<" renscale= "<<renscale<<" facscale= "<<facscale<<" xEscale= "<<xEscale<<std::endl;

  htmpsum= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops, renscale, facscale, xEscale);

  if (debug) std::cout<<cn<<mn<<"Set htmpsum name to "<<name.Data()<<std::endl;
  htmpsum->SetName(name);

 } else {
  applgridok=false;
  TH1D *htmp=0;
  for (int igrid=0; igrid<ngrid; igrid++) {
   my_grid=spxgrid->GetGrid(igrid);
   if (!my_grid) { 
    if (debug) std::cout<<cn<<mn<<"igrid= "<<igrid<<"No applgrid found gridname= "<<TString(gridName).Data()<<std::endl;
    throw SPXParseException(cn+mn+"Grid not found !");
   }
   //
   gridName=spxgrid->GetName();
   //

   htmp= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops, renscale, facscale, xEscale);

   htmp->SetName(TString(gridName));
   if (debug) {
    std::cout<<cn<<mn<<" igrid= "<<igrid<<" adding "<<TString(gridName).Data()<<std::endl;

    std::cout<<cn<<mn<<"Print "<<htmp->GetName()<<std::endl;
    htmp->Print("all");
   }

   if (igrid==0) htmpsum=htmp;
   else {
    SPXGraphUtilities::Add(htmpsum,htmp);
   }
   if (debug) {
    std::cout<<cn<<mn<<"igrid= "<<igrid<<" Print "<<htmpsum->GetName()<<std::endl;
    htmpsum->Print("all");
   }
  }
 }
  
 if (!htmpsum) {
  throw SPXParseException(cn+mn+"Histgram not found !");
  applgridok=true; 
 } else {
  applgridok=true; 
 }

 if (debug) {
  if (applgridok) std::cout<<cn<<mn<<"applgridok flag is ON "<<std::endl;
  else            std::cout<<cn<<mn<<"applgridok flag is OFF "<<std::endl;
  std::cout<<cn<<mn<<"Returning absolute"<<htmpsum->GetName()<<std::endl;
  htmpsum->Print("all");
  //
  //TString hname=TString("hratio_")+htmpsum->GetName()+TString("_divided_by_");
  //hname+=htmpsum->GetName();
  //TH1D * hratio=(TH1D*) hvar->Clone(hname);
  //hratio->Divide(hpdfdefault);
  //std::cout<<cn<<mn<<"Returning relative"<<hratio->GetName()<<std::endl;
  //hratio->Print("all");

 }
 return htmpsum;
};                 
  //
// perform any additional work after constructors but before the object is available for use
void SPXPDF::Initialize()
{
 std::string mn = "Initialize: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

#ifdef TIMER
 quick_timer t0;
#endif

 if (debug) {
  std::cout<<cn<<mn<<"Performing Initialization"<<std::endl;

  if (do_PDFBand) std::cout<<cn<<mn<<"do_PDFBand ON" <<std::endl;
  else            std::cout<<cn<<mn<<"do_PDFBand OFF"<<std::endl;
  if (do_AlphaS ) std::cout<<cn<<mn<<"do_AlphaS ON " <<std::endl;
  else            std::cout<<cn<<mn<<"do_AlphaS OFF" <<std::endl;
  if (do_Scale)   std::cout<<cn<<mn<<"do_Scale  ON " <<std::endl;
  else            std::cout<<cn<<mn<<"do_Scale  OFF" <<std::endl;
  if (do_Escale)  std::cout<<cn<<mn<<"do_Escale  ON " <<std::endl;
  else            std::cout<<cn<<mn<<"do_Escale  OFF" <<std::endl;
  if (do_Total)   std::cout<<cn<<mn<<"do_Total  ON" <<std::endl;
  else            std::cout<<cn<<mn<<"do_Total  OFF"<<std::endl;
 }

 //if (!do_PDFBand && !do_AlphaS && !do_Scale) {
 // std::ostringstream oss;
 // oss << cn << mn << "All theory uncertainties are disabled: Check settings in steering file";
 // throw SPXParseException(oss.str());
 //}

 if (do_Scale) {
  if (RenScales.size()!=FacScales.size()){
   //std::cout<<cn<<mn<<"ERROR Same number of renormalisation and factorisation scales are needed !"<<std::endl;
   //std::cout<<cn<<mn<<"ERROR RenScales.size() = "<<RenScales.size()<<" FacScales.size() = "<<FacScales.size()<<std::endl;
   std::ostringstream oss;
   oss << cn << mn << "ERROR Same number of renormalisation and factorisation scales are needed ! RenScales.size() = "<<RenScales.size()<<" FacScales.size() = "<<FacScales.size();
   throw SPXParseException(oss.str());
  }
  if (debug) {
   std::cout<<cn<<mn<<"RenScales.size()="<<RenScales.size()<<std::endl;
   for (int i=0; i<RenScales.size(); i++) {
    std::cout<<cn<<mn<<"RenScales["<<i<<"]= "<<RenScales[i]<<std::endl;
   }
   for (int i=0; i<FacScales.size(); i++) {
    std::cout<<cn<<mn<<"FacScales["<<i<<"]= "<<FacScales[i]<<std::endl;
   }
  }
 }

 if (debug) std::cout<<cn<<mn<<"Initialize: gridName= "<<gridName.c_str()<<std::endl;

 do_Escale=false;
 // If Escale!=1 calculate beam enery uncertainty
 if (debug)  std::cout<<cn<<mn<<"Change beam energy ? Escale= "<<Escale<<std::endl;
 if (Escale!=1.) {
  do_Escale=true;
  if (debug)  std::cout<<cn<<mn<<"Calculate beam energy uncertainy using Escale= "<<Escale<<std::endl;
 }

 // If beam energy comes from Grid steering change everything
 double GridEscale=spxgrid->GetChangeSqrtS();

 if (GridEscale!=1.) {
  Escale=GridEscale;
  if (!do_Escale) if (debug) std::cout<<cn<<mn<<"Calculate all predictions with different beam energy Escale= "<<Escale<<std::endl;   
  else {
   std::cout<<cn<<mn<<"GridEscale= "<<GridEscale<<std::endl;
   throw SPXParseException(cn+mn+"Can not change beam energy for all predictions and calculate beam uncertainty band");
  } 
 }

 if (spxgrid) {

#ifdef TIMER
  std::cout<<cn<<mn<<"TIMER set GetGrid running... "<< std::endl;  
  quick_timer t;
#endif
  // my_grid=spxgrid->GetGrid();
  ngrid=spxgrid->GetNumberofGrids();
  std::cout<<cn<<mn<<"Number of Grids= " << ngrid << std::endl;  
  my_grid=spxgrid->GetGrid(0);

#ifdef TIMER
   std::cout<<cn<<mn<<"TIMER done GetGrid " << t.time() << " [ms]" << std::endl;  
#endif

 } else {

  std::cout<<"INFO No SPX grid found open via name "<<std::endl;
  if (gridName.size()==0) {
   std::cout<<cn<<mn<<"WARNING: no gridname given "<<std::endl;
  } else {
   if (debug) std::cout<<cn<<mn<<"onstruct gridname= "<<TString(gridName).Data()<<std::endl;
#ifdef TIMER
   std::cout<<cn<<mn<<"TIMER reading grid() running..." << std::endl;     
   quick_timer t;
#endif
   my_grid = new appl::grid(gridName.c_str());

#ifdef TIMER
   std::cout<<cn<<mn<<"TIMER done grid " << t.time() << " [ms]" << std::endl;
#endif
  }
 }

 if (!my_grid) {
  std::cout<<cn<<mn<<"No applgrid found ! "<<std::endl;
  applgridok=false;
  if (debug) std::cout<<cn<<"No applgrid found gridname= "<<TString(gridName).Data()<<std::endl;
  applgridok=false;
  } else {
   if (debug) std::cout<<cn<<"applgrid ok gridname= "<<TString(gridName).Data()<<std::endl;
   applgridok=true;
  }

//
//#ifdef TIMER
//   std::cout<<cn<<mn<<"TIMER trimming running.... "<< std::endl;  
//   quick_timer t1;
//#endif
// trimming done during reading
// if (applgridok) my_grid->trim();
//#ifdef TIMER
// std::cout<<cn<<mn<<"TIMER done trimming " << t1.time() << " [ms]" << std::endl;
//#endif

 static const int nFlavours = 5;
 h_errors_PDF.clear();
 h_errors_AlphaS.clear();
 h_errors_Scale.clear();
 h_errors_Scale.clear();
 h_errors_BeamUncertainty.clear();

 TH1D* temp_hist=0;

 if (debug)
  std::cout<<cn<<mn<<"Fill PDF errors for PDFType: "
      <<PDFtype<<", PDFName: "<<PDFname<<", from PDFPath: "<<pdfSetPath<<std::endl;

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
 default_pdf_set_name = (std::string) (PDFname);
#else
 default_pdf_set_name = (std::string) (pdfSetPath+"/"+PDFname+".LHgrid");
#endif

 std::cout<<cn<<mn<<"PDFset looking up: "<<default_pdf_set_name<<" default-id= "<<defaultpdfid<<std::endl;

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
 //
 bool verbosity=false;
 
 if (!verbosity) {
  const LHAPDF::PDFInfo info(default_pdf_set_name.c_str(), 2);
 //
  LHAPDF::Info& cfg = LHAPDF::getConfig();
 //
  cfg.set_entry("Verbosity",0);
 //
  if (debug) {
   std::cout<<"  "<<std::endl;
   std::cout<<cn<<mn<<"Running on LHAPDF6: "<<std::endl;
   std::cout<<"  "<<std::endl;
  //
  //std::cout<<cn<<mn<<"Read PDF= "<<default_pdf_set_name.c_str()<<std::endl;
  //
  cfg.set_entry("Verbosity",1);
  //
  if (info.has_key("PdfDesc")) std::cout << "PdfDesc: " << info.get_entry("PdfDesc") << std::endl;
   std::cout <<cn<<mn<< "PdfType: " << info.get_entry("PdfType") << std::endl;
   std::cout <<cn<<mn<< "Verbosity from PDF: " << info.get_entry("Verbosity") << std::endl;
   //vector<int> pids = info.get_entry_as< vector<int> >("Flavors");
  }
 }
 //mypdf=LHAPDF::mkPDF(default_pdf_set_name.c_str(),defaultpdfid);

 // std::cout<<mypdf.description()<<std::endl;
 // double x=0.1, Q=10.;
 // std::cout << "xf_g = " << mypdf->xfxQ(21, x, Q) << std::endl;

 //const LHAPDF::PDFSet set(default_pdf_set_name.c_str());
 //int npdfmemberfromset = set.size();

 //if (debug) {
 // std::cout<<cn<<mn<<"Number of PDF members from steering=  "<<n_PDFMembers<<" from PDF set= "<<npdfmemberfromset <<std::endl;
 //}

 //if (npdfmemberfromset!=n_PDFMembers  && !(ErrorPropagationType==StyleHeraPDF)) {

 // std::cerr<<cn<<mn<<"ERROR: Number of PDF members from steering=  "<<n_PDFMembers<<" from PDF set= "<<npdfmemberfromset <<std::endl;
 // std::cout<<cn<<mn<<"ERROR: Number of PDF members from steering=  "<<n_PDFMembers<<" from PDF set= "<<npdfmemberfromset <<std::endl;
 // std::cout<<cn<<mn<<"ERROR: Correct the steering file !" <<std::endl;
 //}
 
 //const std::vector<LHAPDF::PDF*> pdfs = set.mkPDFs();
 //mypdf=pdfs[defaultpdfid];
 //vector<double> xgAll, xuAll;
 //for (size_t imem = 0; imem <= nmem; imem++) {
 // xgAll.push_back(pdfs[imem]->xfxQ(21,x,Q)); // gluon distribution
 // xuAll.push_back(pdfs[imem]->xfxQ(2,x,Q)); // up-quark distribution
 //}
 //#else
 //if (debug) {
 // std::cout<<cn<<mn<<"  "<<std::endl;
 // std::cout<<cn<<mn<<"Running on LHAPDF5: "<<std::endl;
 // std::cout<<cn<<mn<<"  "<<std::endl;
 //}

 //std::cout<<cn<<mn<<"init PDF set called: "<<default_pdf_set_name.c_str()<<std::endl;
 //LHAPDF::initPDFSet(default_pdf_set_name.c_str(), defaultpdfid);
#endif

 this->SetLHAPDFPDFset(default_pdf_set_name,defaultpdfid);

 if (debug) {
  std::cout<<cn<<mn<<"...finished PDF set up: name= "<<default_pdf_set_name<<std::endl;
 }

 if (applgridok) {
#ifdef TIMER
  std::cout<<cn<<mn<<"TIMER convolution running... " << std::endl;
  quick_timer t2;
#endif

  TString name="xsec_pdf_"+default_pdf_set_name;

  temp_hist=this->GetHisto(); // default renscale=1 facscale=1
  if (!temp_hist) {
   std::cout<<cn<<mn<<"WARNING temp_hist not found ! "<<std::endl;
  }

#ifdef TIMER
 std::cout<<cn<<mn<<"TIMER done convolution " << t2.time() << " [ms]" << std::endl;
#endif

 } else {
  if (debug) std::cout<<cn<<mn<<"Histogram from PDF not applgrid ! "<<std::endl;
  temp_hist=this->FillPdfHisto();
  std::cout<<cn<<mn<<" hpdf= "<<hpdf->GetName()<<std::endl;
  temp_hist->SetName(hpdf->GetName());
 }

 if (!temp_hist) std::cout<<cn<<mn<<"ERROR could not do the convolution; temp_hist not found ! "<<std::endl;

 if (debug) {
  std::cout<<cn<<mn<<"\nDefault Cross section after convolution: "<<std::endl;
  temp_hist->Print("all");
 }

 hpdfdefault=temp_hist;
 // set-up defaults
 if (do_AlphaS) {
  h_errors_AlphaS.push_back(temp_hist);
  h_AlphaS_results=SPXGraphUtilities::TH1TOTGraphAsymm(temp_hist);
  TString name="xsec_alphas_"+default_pdf_set_name;
  if (spxgrid) name+="_"+gridName;;
  if (h_AlphaS_results) h_AlphaS_results->SetName(name);
 }
 if (do_Scale)   {
  h_Scale_results=SPXGraphUtilities::TH1TOTGraphAsymm(temp_hist);
  TString name="xsec_scale_"+default_pdf_set_name;
  if (spxgrid) name+="_"+gridName;;
  if (h_Scale_results) h_Scale_results->SetName(name);
 }
 if (do_PDFBand) {
  h_PDF_results=SPXGraphUtilities::TH1TOTGraphAsymm(temp_hist);
  TString name="xsec_pdf_"+default_pdf_set_name;
  if (spxgrid) name+="_"+gridName;
  if (h_PDF_results) h_PDF_results->SetName(name);
 }
 if (do_Escale) {
  h_BeamUncertainty_results=SPXGraphUtilities::TH1TOTGraphAsymm(temp_hist);
  TString name="xsec_BeamUncertainty_"+default_pdf_set_name;
  if (spxgrid) name+="_"+gridName;
  if (h_BeamUncertainty_results) h_BeamUncertainty_results->SetName(name);
 }
 // Now do the scale variations
 if (debug)
  if (do_Scale) std::cout<<cn<<mn<<"do_Scale is ON "<<std::endl;
  else          std::cout<<cn<<mn<<"do_Scale is OFF "<<std::endl;

 if (do_Scale) {
  if (applgridok) {

   if (RenScales.size()==0 || FacScales.size()==0){
    std::ostringstream oss;
    oss << cn<< mn << "ERROR Scale calculation on, but RenScales.size() = "<<RenScales.size()<<" FacScales.size() = "<<FacScales.size();
    throw SPXParseException(oss.str());
   }

   if (RenScales.size()!=FacScales.size()) {
    std::ostringstream oss;
    oss << cn<< mn << "ERROR Same number of renormalisation and factorisation scales are needed ! RenScales.size() = "<<RenScales.size()<<" FacScales.size() = "<<FacScales.size();
    throw SPXParseException(oss.str());
   }

   if (RenScales.size()!=0 || FacScales.size()!=0){
    if (debug) {
     std::cout<<cn<<mn<<"Number of ren scale variations= "<<RenScales.size()<<std::endl;
     std::cout<<cn<<mn<<"Number of fac scale variations= "<<FacScales.size()<<std::endl;
    }
    if (RenScales.size()!=FacScales.size())
     std::cout<<cn<<mn<<" Something is wrong #RenScales != #FacScales "
              <<" #RenScales= "<< RenScales.size() <<" #FacScale= "<<FacScales.size() << std::endl;
    for (int iscale=0; iscale<RenScales.size(); iscale++){
     //
     TH1D* h_scale_temp=this->GetHisto(RenScales[iscale],FacScales[iscale]);
     char rs[100];
     sprintf(rs,"%s #xi_{R}=%3.1f   #xi_{F}=%3.1f",PDFname.c_str(),RenScales[iscale],FacScales[iscale]);
     if (debug) std::cout<<cn<<mn<<iscale<<" "<<rs<<std::endl;
     char rname[100];
     sprintf(rname,"%s_muR%3.1f_muF%3.1f",PDFname.c_str(),RenScales[iscale],FacScales[iscale]);
     h_scale_temp->SetTitle(rs);
     h_scale_temp->SetName(rname);
     //if (debug) {
     // std::cout<<iscale<<" print histogram: "<<rname<<std::endl;
     // h_scale_temp->Print("all");
     //}

     //double chi2=h_scale_temp->Chi2Test(temp_hist,"WWP");
     //if (chi2<0.99) {
     h_errors_Scale.push_back(h_scale_temp);
    }
   } 

  } else {
   if (debug) std::cout<<cn<<mn<<"Histogram from PDF not applgrid ! "<<std::endl;
   temp_hist=this->FillPdfHisto();
   std::cout<<cn<<mn<<" hpdf= "<<hpdf->GetName()<<std::endl;
   //temp_hist->SetName((TString) ("h_pdf_default"));
   temp_hist->SetName(hpdf->GetName());
  }

  if (debug) {
   std::cout<<cn<<mn<<"Scale variations"<<std::endl;
   for (int iscale=0; iscale<h_errors_Scale.size(); iscale++){
    std::cout<<cn<<mn<<iscale<<" Name= "<<h_errors_Scale[iscale]->GetName()<<std::endl;
    TH1D *hdef=h_errors_Scale[0];
    if (!hdef) std::cout<<cn<<mn<<"Histogram hdef not found "<<std::endl;
    if (iscale!=0) {
     TH1D *hvar=h_errors_Scale[iscale];
     if (!hvar) std::cout<<cn<<mn<<"hvariation["<<iscale<<"] Histogram hvar not found "<<std::endl;
     TString hname=TString("hratio_")+hdef->GetName()+TString("_divided_by_");
     hname+=hvar->GetName();
     if (spxgrid) hname+="_"+gridName;;
     TH1D * hratio=(TH1D*) hvar->Clone(hname);
     hratio->Divide(hdef);
     //std::cout<<cn<<mn<<" hvariation["<<iscale<<"]="<<hvar->GetName()<<std::endl;
     hratio->Print("all");
    }
   }
  }
 }

 if (debug) {
  if (do_AlphaS) std::cout<<cn<<mn<<"do_AlphaS is ON: calculate Alphas uncertainty "<<std::endl;
  else           std::cout<<cn<<mn<<"do_AlphaS is OFF "<<std::endl;
 }


 if (AlphaSPDFSetNameUp.compare("")==0   || AlphaSPDFSetNameUp.compare("none")==0 ||
     AlphaSPDFSetNameDown.compare("")==0 || AlphaSPDFSetNameDown.compare("none")==0 ) {

  std::cout<<cn<<mn<<"WARNING: can not calculated AlphaS uncertainty, no PDF member found. Switch of Alphas calculation "<<std::endl; 
  do_AlphaS=false;

 }

 if (do_AlphaS) {
  //check for necessary names before continuing
  if(AlphaSmemberNumDown==DEFAULT) {
   std::ostringstream oss;
   oss << cn << mn << "WARNING 'AlphaSmemberNumDown' not provided in steer file: "<<steeringFileName<<" switching off alphas uncertainty calculation";
   //throw SPXParseException(oss.str());
   std::cerr<<oss.str()<<std::endl;
   std::cout<<oss.str()<<std::endl;
   do_AlphaS=false;
  }

  if (AlphaSmemberNumUp==DEFAULT) {
   std::ostringstream oss;
   oss << cn << mn << "WARNING 'AlphaSmemberNumUp' not provided in steer file: "<<steeringFileName<<" switching off alphas uncertainty calculation";
   //throw SPXParseException(oss.str());
   std::cerr<<oss.str()<<std::endl;
   std::cout<<oss.str()<<std::endl;
   do_AlphaS=false;
  }

  if (AlphaSPDFSetNameUp.compare("")==0) {
   std::ostringstream oss;
   oss << cn << mn << "WARNING 'AlphaSPDFSetNameUp' not provided in steer file: "<<steeringFileName<<" switching off alphas uncertainty calculation";
   //throw SPXParseException(oss.str());
   std::cerr<<oss.str()<<std::endl;
   std::cout<<oss.str()<<std::endl;
   do_AlphaS=false;
  }

  if (AlphaSPDFSetNameDown.compare("")==0) {
   std::ostringstream oss;
   oss << cn << mn << "WARNING 'AlphaSPDFSetNameDown' not provided in steer file: "<<steeringFileName<<" switching off alphas uncertainty calculation";
   //throw SPXParseException(oss.str());
   std::cerr<<oss.str()<<std::endl;
   std::cout<<oss.str()<<std::endl;
   do_AlphaS=false;
  }

  if (do_AlphaS) {
   // alphaS central
   std::cout<<cn<<mn<<"PDFset getting alphaS uncertainty for "<<default_pdf_set_name<<" PDF with Scale= "<<alphaS_scale_worldAverage<<std::endl;
   this->SetLHAPDFPDFset(default_pdf_set_name, defaultpdfid);
   if (debug&&applgridok) {
    temp_hist= this->GetHisto();
    std::cout<<cn<<mn<<"Print default from convolution:= "<<default_pdf_set_name<<std::endl;
    temp_hist->Print("all");
   }

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
   double value_alphaS=mypdf->alphasQ(alphaS_scale_worldAverage);
#else
   double value_alphaS=LHAPDF::alphasPDF(alphaS_scale_worldAverage);
#endif
   if (debug) std::cout<<cn<<mn<<"alphas cent PDFname= "<<default_pdf_set_name<<" member= "<<defaultpdfid<<" value= "<<value_alphaS<<std::endl;
   alphaS_variations.push_back(value_alphaS);
   if (debug) std::cout <<cn<<mn<< "Added central histogram with alphaS value: " << value_alphaS << std::endl ;

// alphaS down
   this->SetLHAPDFPDFset(AlphaSPDFSetNameDown, AlphaSmemberNumDown);

//  if (debug) std::cout<<cn<<mn<<"Setting up alphas down PDF-name= "<<AlphaSPDFSetNameDown<<" member= "<<AlphaSmemberNumDown<<std::endl;
//#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
//  mypdf=LHAPDF::mkPDF(AlphaSPDFSetNameDown.c_str(),AlphaSmemberNumDown);
//  if (!mypdf) std::cout<<"PDF not found name= "<<AlphaSPDFSetNameDown.c_str()<<" member= "<<AlphaSmemberNumDown<<std::endl;
  //else if (debug) mypdf->print();
//#else
// LHAPDF::initPDFSet(((std::string) (pdfSetPath+"/"+AlphaSPDFSetNameDown+".LHgrid")).c_str(), AlphaSmemberNumDown);
//#endif

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
   double value_alphaS_down=mypdf->alphasQ(alphaS_scale_worldAverage);
   if (debug) mypdf->print();
#else
   double value_alphaS_down=LHAPDF::alphasPDF(alphaS_scale_worldAverage);
#endif

  // For NNPDF there is a problem reading out the alphas values (acknowledged by J Rojo), need to fix this by hand
   if (TString(AlphaSPDFSetNameDown).Contains("NNPDF30_nlo_as_0117_hera") 
    || TString(AlphaSPDFSetNameDown).Contains("NNPDF30_nlo_as_0117_nojet") 
    || TString(AlphaSPDFSetNameDown).Contains("NNPDF30_nlo_as_0117_nolhc") ) {
    value_alphaS_down=0.117;
    std::ostringstream oss;
    oss << cn << mn << "INFO: Set "<<AlphaSPDFSetNameDown<<" alphas value value by hand to "<<value_alphaS_down<<std::endl;
    std::cerr<<cn<<mn<<oss.str()<<std::endl;
    std::cout<<cn<<mn<<oss.str()<<std::endl;
   }

   TString AlphaSPDFSetHistNameDown=AlphaSPDFSetNameDown+"_value_alphas= ";
   TString namedown; namedown.Form("%3.3f",value_alphaS_down);
   AlphaSPDFSetHistNameDown+=namedown;

   if (debug) std::cout<<cn<<mn<<"alphas down PDFname= "<<AlphaSPDFSetNameDown<<" member= "<<AlphaSmemberNumDown<<" value= "<<value_alphaS_down<<std::endl;

   if (applgridok) {
    //
    temp_hist= this->GetHisto();
    temp_hist->SetName((TString) ("xsec_alphas_pdfset"+AlphaSPDFSetHistNameDown));
   } else {
    if (debug) std::cout<<cn<<mn<<"applgrid not found; histogram from PDF not applgrid ! "<<std::endl;
    temp_hist=this->FillPdfHisto();
    temp_hist->SetName((TString) ("pdfonly_"+AlphaSPDFSetHistNameDown));
   }

   if (!temp_hist) std::cout<<cn<<mn<<"temp_hist not found ! "<<std::endl;
   if (debug) {
    std::cout<<cn<<mn<<"Print hist "<<temp_hist->GetName()<<std::endl;
    temp_hist->Print("all");
   }

   h_errors_AlphaS.push_back(temp_hist);

   alphaS_variations.push_back(value_alphaS_down);
   if (debug) std::cout << cn<<mn<<"Added down variation histogram with alphaS value: " << value_alphaS_down <<std::endl ;

// alphaS up

   if (debug) std::cout<<cn<<mn<<"Setting up alphas up   PDF-name= "<<AlphaSPDFSetNameUp<<" member= "<<AlphaSmemberNumUp<<std::endl;

   this->SetLHAPDFPDFset(AlphaSPDFSetNameUp, AlphaSmemberNumUp);
 //#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
 //mypdf=LHAPDF::mkPDF(AlphaSPDFSetNameUp.c_str(),AlphaSmemberNumUp);
 //#else
 // LHAPDF::initPDFSet(((std::string) (pdfSetPath+"/"+AlphaSPDFSetNameUp+".LHgrid")).c_str(), AlphaSmemberNumUp);
  // Peter's comment: for LHAPDF v 6.1, this line has to be used
  //LHAPDF::initPDFSet(((std::string) (AlphaSPDFSetNameUp+".LHgrid")).c_str(), AlphaSmemberNumUp);
  //}
 //#endif
#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
   double value_alphaS_up=mypdf->alphasQ(alphaS_scale_worldAverage);
   if (debug) mypdf->print();
#else
   double value_alphaS_up=LHAPDF::alphasPDF(alphaS_scale_worldAverage);
#endif

  // For NNPDF there is a problem reading out the alphas values (acknowledged by J Rojo), need to fix this by hand
   if (TString(AlphaSPDFSetNameUp).Contains("NNPDF30_nlo_as_0119_hera") 
    || TString(AlphaSPDFSetNameUp).Contains("NNPDF30_nlo_as_0119_nojet") 
    || TString(AlphaSPDFSetNameUp).Contains("NNPDF30_nlo_as_0119_nolhc") ) {
    value_alphaS_up=0.119;
    std::ostringstream oss;
    oss << cn << mn << "INFO: Set "<<AlphaSPDFSetNameUp<<" alphas value value by hand to "<<value_alphaS_up<<std::endl;
    std::cerr<<cn<<mn<<oss.str()<<std::endl;
    std::cout<<cn<<mn<<oss.str()<<std::endl;
   }

   TString AlphaSPDFSetHistNameUp=AlphaSPDFSetNameUp+"_value_alphas= ";
   TString nameup; nameup.Form("%3.3f",value_alphaS_up);
   AlphaSPDFSetHistNameUp+=nameup;

   temp_hist= this->GetHisto();
   if (applgridok) {
    //
    temp_hist->SetName((TString) ("xsec_alphas_pdfset_"+AlphaSPDFSetHistNameUp));
   } else {
    if (debug) std::cout<<cn<<mn<<"Histogram from PDF not applgrid ! "<<std::endl;
    temp_hist=this->FillPdfHisto();
    temp_hist->SetName((TString) ("pdfonly_"+AlphaSPDFSetHistNameUp));
   }

   if (debug) {
    std::cout<<cn<<mn<<"Print hist "<<temp_hist->GetName()<<std::endl;
    temp_hist->Print("all");
   }

   h_errors_AlphaS.push_back(temp_hist);

   if (debug) std::cout<<cn<<mn<<"alphas up   PDFname= "<<AlphaSPDFSetNameUp<<" member= "<<AlphaSmemberNumUp<<" value= "<<value_alphaS_up<<std::endl;
   alphaS_variations.push_back(value_alphaS_up);
   if (debug) std::cout <<cn<<mn<<"Added up variation histogram with alphaS value: " << value_alphaS_up << std::endl ;

   if (debug) {
    std::cout<<" alphaS_variations: "<<std::endl;
    for (int i=0; i<alphaS_variations.size(); i++){
      std::cout<<i<<" value= "<<alphaS_variations[i]<<std::endl;
    }
   }

   if (alphaS_variations.size()>2) {
    if(alphaS_variations.at(0)==alphaS_variations.at(1) ||
       alphaS_variations.at(1)==alphaS_variations.at(2) ||
       alphaS_variations.at(0)==alphaS_variations.at(2) ) {
     std::cout<<cn<<mn<<"WARNING: alphaS_variations should be different ! "<< h_errors_AlphaS.at(0)->GetName()<<std::endl;
     for (int i=0; i<3; i++){
       std::cout<<cn<<mn<<i<<" value= "<<alphaS_variations.at(i)<<std::endl;
     }
     std::cerr<<cn<<mn<<"WARNING: alphaS_variations should be different ! "<<h_errors_AlphaS.at(0)->GetName()<<std::endl;
    }
   }
  }
 }

// reset to default
 if (debug) std::cout<<cn<<mn<<"Reset to PDF default "<<std::endl;
 this->SetLHAPDFPDFset(default_pdf_set_name,defaultpdfid);
//
 if (debug)
  if (do_PDFBand) std::cout<<cn<<mn<<"do_PDFBand ON"<<std::endl;
  else            std::cout<<cn<<mn<<"do_PDFBand OFF"<<std::endl;

 // Calculate PDF errors using standard PDF error band
 if (do_PDFBand) {
  if (debug) std::cout<<cn<<mn<<"Calculate PDF errors"<<std::endl;
/*
  std::cout<<cn<<mn<<"Calculate PDF errors for: "<<default_pdf_set_name<<" w/ defaultpdfid: "<<defaultpdfid<<std::endl;

#ifdef TIMER
  std::cout<<cn<<mn<<"TIMER call initPDF/mkPDF default PDF= "<<default_pdf_set_name.c_str()<<" id= "<< defaultpdfid<<" running..." << std::endl; 
  quick_timer ti;
#endif

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
  if (debug) {
   std::cout<<cn<<mn<<" "<<std::endl;
   std::cout<<cn<<mn<<"Running LHAPDF6 mkPDF for "<<default_pdf_set_name.c_str()<<std::endl;
  }
  mypdf=LHAPDF::mkPDF(default_pdf_set_name.c_str(), defaultpdfid);
  
  if (debug) std::cout<<cn<<mn<<"...mkPDF finished for PDFBand"<<std::endl;
#else
  LHAPDF::initPDFSet(default_pdf_set_name.c_str(), defaultpdfid);
  if (debug) std::cout<<cn<<mn<<"...initPDFSet finished for PDFBand"<<std::endl;
#endif

#ifdef TIMER
  std::cout<<cn<<mn<<"TIMER done call initPDF/mkPDF t=" << ti.time() <<" [ms]"<< std::endl;
#endif
// 
*/
  TH1D *hdefault=0;
  for (int pdferri = 0; pdferri < n_PDFMembers; pdferri++) {
   if (debug) std::cout<<cn<<mn<<"pdferri: "<<pdferri<<" of "<<n_PDFMembers<<" pdftype="<<PDFtype.c_str()<<std::endl;

   if (ErrorPropagationType==StyleHeraPDF) {
    if (!includeEIG&&!includeQUAD&&!includeMAX) std::cout<<cn<<mn<<"No error band included !! "<<std::endl;

    if (defaultpdfidvar<0) {
     std::cout<<cn<<mn<<"WARNING: No default PDF id found in steering. Check steering for missing 'defaultpdfidvar'. pdferri= "<<pdferri<<std::endl;
     std::cerr<<cn<<mn<<"WARNING: No default PDF id found in steering. Check steering for missing 'defaultpdfidvar'. pdferri= "<<pdferri<<std::endl;
    }

    if (pdferri <= lasteig ) {
#ifdef TIMER
     quick_timer ti;
     std::cout<<cn<<mn<<"TIMER call initPDF/mkPDF pdferri <= lasteig PDF= "<<PDFname.c_str()<<" pdferri= "<<pdferri<<" running..." << std::endl; 
     quick_timer t0;
#endif
     this->SetLHAPDFPDFset(PDFname, pdferri);
     //#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
     // if (debug) std::cout<<cn<<mn<<"...mkPDF "<<PDFname.c_str()<<" pdferri= "<<pdferri<<std::endl;
     // mypdf=LHAPDF::mkPDF(PDFname.c_str(), pdferri);
     //#else
     // if (debug) std::cout<<cn<<mn<<"...initPDFSet "<<PDFname.c_str()<<" pdferri= "<<pdferri<<std::endl;
     // LHAPDF::initPDF(pdferri);
     //#endif

#ifdef TIMER
      std::cout<<cn<<mn<<"TIMER done call initPDF/mkPDF t0= " << t0.time()<<" [ms]"<<std::endl;
#endif

    } else if( pdferri == lasteig+1 ) {
     //account for PDF set "ATLAS.txt" and "ATLAS3jet" with no error bands?
     //if (debug) std::cout<<cn<<mn<<"A 'PDFnamevar' was "
     //			 <<(PDFnamevar.empty()? "":"NOT")<<" found name= "<<PDFnamevar.c_str()<<std::endl;
     //
     //Band-aid for accounting for no error bands - This needs to be handled better
     if(PDFnamevar.empty() == false) {

#ifdef TIMER
     quick_timer ti;
     std::cout<<cn<<mn<<"TIMER call initPDF/mkPDF pdferri==lasteig+1 PDF= "<<PDFname.c_str()<<" pdferri= "<<pdferri<<" running..." << std::endl; 
     quick_timer t0;
#endif

     this->SetLHAPDFPDFset(PDFnamevar, defaultpdfidvar);
     //#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
     // if (debug) std::cout<<cn<<mn<<"Initialize: mkPDF name= "<<PDFnamevar.c_str()<<" set= "<<defaultpdfidvar<<std::endl;
     // mypdf=LHAPDF::mkPDF(PDFnamevar.c_str(), defaultpdfidvar);
     //#else
     // TString pdfname=TString(pdfSetPath)+"/"+PDFnamevar+".LHgrid";
     // LHAPDF::initPDFSet(pdfname.Data(), defaultpdfidvar);
     //#endif

#ifdef TIMER
      std::cout<<cn<<mn<<"TIMER done call initPDF/mkPDF t0= " << t0.time()<<" [ms]"<<std::endl;
#endif

     } else {
      std::cout<<cn<<mn<<"WARNING: Can not intitalize name= "<<PDFnamevar.c_str()<<" set= "<<defaultpdfidvar<<std::endl;
      std::cerr<<cn<<mn<<"WARNING: Can not intitalize name= "<<PDFnamevar.c_str()<<" set= "<<defaultpdfidvar<<std::endl;
     }
    } else if ( pdferri > lasteig+1 ) {
      //>> modification P Berta 28.8.14>>
     int pdfset=pdferri - lasteig-1;

#ifdef TIMER     
     std::cout<<cn<<mn<<"TIMER initPDF/mkPDF  pdferri>lasteig+1 running PDF= "<<PDFnamevar.c_str()<<" pdfset= "<<pdfset<<std::endl;
     quick_timer t0;
#endif

     if (debug) std::cout<<cn<<mn<<"Initialize: pdferri> lasteig+1 initPDF name= "<<PDFnamevar.c_str()<<" set= "<<pdfset<<std::endl;
     this->SetLHAPDFPDFset(PDFnamevar, pdfset);
     //#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
     // mypdf=LHAPDF::mkPDF(PDFnamevar.c_str(), pdfset);
     //#else
     //LHAPDF::initPDF(pdfset);
     //#endif

#ifdef TIMER     
     std::cout<<cn<<mn<<"TIMER done initPDF/mkPDF t0= "<<  t0.time()<<" [ms]"<< std::endl;
#endif

    }
   } else {
    if (debug) std::cout<<cn<<mn<<"Initialize normal PDF set= "<<pdferri<<std::endl;

#ifdef TIMER     
    std::cout<<cn<<mn<<"TIMER call initPDF/mkPDF normal PDF set PDF= "<<default_pdf_set_name.c_str()<<" pdferri= "<<pdferri<<" running... " << std::endl;
     quick_timer t0;
#endif

     this->SetLHAPDFPDFset(default_pdf_set_name, pdferri);
     //#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
     //if (debug) {
     //std::cout<<cn<<mn<<" "<<std::endl;
     //std::cout<<cn<<mn<<"Running LHAPDF6 mkPDF for "<<default_pdf_set_name.c_str()<<std::endl;
     //}
     //mypdf=LHAPDF::mkPDF(default_pdf_set_name.c_str(), pdferri);
     //#else
     //LHAPDF::initPDF(pdferri);
     //#endif

#ifdef TIMER     
    std::cout<<cn<<mn<<"TIMER call initPDF/mkPDF done "<<t0.time()<<" [ms]"<< std::endl;
#endif
   }

   TH1D* temp_hist = 0;
   if (applgridok) {
    if (debug) std::cout<<cn<<mn<<"Setting up convolute "<<std::endl;

#ifdef TIMER     
    std::cout<<cn<<mn<<"TIMER convolute pdferri= "<<pdferri<<" running... " << std::endl;
    quick_timer t0;
#endif

    //
    temp_hist = this->GetHisto();
#ifdef TIMER     
    std::cout<<cn<<mn<<"TIMER convolute done t0= "<< t0.time()<<" [ms]"<<std::endl;
#endif

    TString hname=temp_hist->GetName()+TString("_pdf_")+default_pdf_set_name+"_id_";
    hname+=pdferri;
    if (spxgrid) hname+="_"+gridName;
    temp_hist->SetName(hname);

   } else {
    if (debug) std::cout<<cn<<mn<<"Histogram from PDF not applgrid ! "<<std::endl;
    TH1D *tmp=this->FillPdfHisto();
    if (!tmp) std::cout<<cn<<mn<<"tmp histogram not found "<<std::endl;
    temp_hist=(TH1D*)tmp->Clone(tmp->GetName());
    temp_hist->SetName(tmp->GetName());
   }

   if (!temp_hist) std::cout<<cn<<mn<<"temp_hist histogram not found "<<std::endl;
   if (pdferri==defaultpdfid) hdefault=(TH1D*)temp_hist->Clone(temp_hist->GetName());

   // print default cross section and ratio variation/default
   if (debug) {
    if (pdferri==defaultpdfid)  {
     std::cout<<cn<<mn<<"Print defaultpdf histo: "<<std::endl;
     temp_hist->Print("all");
    } else {
     std::cout<<cn<<"Initialize ratio pdferri=  "<<pdferri<<" to default= "<<defaultpdfid<<std::endl;
     TString hname=TString("hratio_")+temp_hist->GetName()+TString("_pdf_");
     hname+=pdferri;
     hname+="_Divided_by_pdf_";
     hname+=defaultpdfid;
     std::cout<<cn<<mn<<"hname=  "<<hname<<std::endl;
     if (!hdefault) std::cout<<cn<<mn<<"hdefault histogram not found "<<std::endl;
     TH1D * hratio=(TH1D*) temp_hist->Clone(hname);
     hratio->Divide(hdefault);
     hratio->Print("all");
    }
   }

   h_errors_PDF.push_back(temp_hist);

  }   /// pdf errors loop

  if (debug) std::cout<<cn<<mn<<"End of PDF errors loop"<<std::endl;
 }  /// do_PDFBAND

// Set back to default PDF
     this->SetLHAPDFPDFset(default_pdf_set_name, defaultpdfid); 
     //#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
     //if (debug) {
     //std::cout<<cn<<mn<<" "<<std::endl;
     //std::cout<<cn<<mn<<"Running LHAPDF6 mkPDF for "<<default_pdf_set_name.c_str()<<std::endl;
     //}
     //mypdf=LHAPDF::mkPDF(default_pdf_set_name.c_str(), defaultpdfid);
     //#else
     //LHAPDF::initPDF(defaultpdfid);
     //#endif

#ifdef TIMER     
    std::cout<<cn<<mn<<"TIMER call CalcSystErrors  running... " << std::endl;
    quick_timer t1;
#endif

 if (debug) std::cout<<cn<<mn<<"Now calling CalcSystErrors running... "<<std::endl;
 this->CalcSystErrors();

#ifdef TIMER     
    std::cout<<cn<<mn<<"TIMER CalcSystErrors  done t0= "<< t1.time()<<" [ms]"<<std::endl;
#endif

 if (debug) std::cout<<cn<<mn<<"Now fill map  Mapallbands "<<std::endl;

 if (do_PDFBand) if(h_PDF_results) {
  if (debug) std::cout<<cn<<mn<<"Fill in map "<<h_PDF_results->GetName()<<std::endl;
  if (Mapallbands.count("pdf")>0) std::cout<<cn<<mn<<"WARNING: Mapallbands[pdf] already filled ! "<<std::endl;
  Mapallbands["pdf"]=h_PDF_results;
 }

 if (do_Scale) if(h_Scale_results){
  if (debug) std::cout<<cn<<mn<<"Fill in map "<<h_Scale_results->GetName()<<std::endl;
  if (Mapallbands.count("scale")>0) std::cout<<cn<<mn<<"WARNING: Mapallbands[scale] already filled ! "<<std::endl;
  Mapallbands["scale"]=h_Scale_results;
 }

 if (do_AlphaS) if (h_AlphaS_results) {
  if (debug) std::cout<<cn<<mn<<"Fill in map "<<h_AlphaS_results->GetName()<<std::endl;
  if (Mapallbands.count("alphas")>0) std::cout<<cn<<mn<<"WARNING: Mapallbands[alphas] already filled ! "<<std::endl;
  Mapallbands["alphas"]=h_AlphaS_results;
 }

 if (do_Escale) if (h_BeamUncertainty_results) {
  if (debug) std::cout<<cn<<mn<<"Fill in map beam= "<<h_BeamUncertainty_results->GetName()<<std::endl;
  if (Mapallbands.count("BeamUncertainty")>0) std::cout<<cn<<mn<<"WARNING: Mapallbands[BeamUncertainty] already filled ! "<<std::endl;
  Mapallbands["BeamUncertainty"]=h_BeamUncertainty_results;
 }

 if (debug) {
  std::cout<<cn<<mn<<"Print Mapallbands "<<std::endl;
  PrintMap(Mapallbands);
 }

/*
 if (debug) {
  std::cout<<cn<<mn<<"After calculation of systematic uncertainties "<<std::endl;
  if (do_AlphaS) {
   std::cout<<cn<<mn<<"h_AlphaS_results "<<std::endl;
   h_AlphaS_results->Print("all");
  }
  if (do_PDFBand) {
   std::cout<<cn<<mn<<"h_PDF_results: "<<std::endl;
   h_PDF_results->Print("all");
   std::cout<<cn<<mn<<"default= "<<defaultpdfid<<std::endl;
   hpdfdefault->Print("all");
  }
  if (do_Scale) {
   std::cout<<cn<<mn<<"Scale variations: "<<std::endl;
   h_Scale_results->Print("all");
  }
  if (do_Escale) {
   std::cout<<cn<<mn<<"Scale variations: "<<std::endl;
   h_BeamUncertainty_results->Print("all");
  }
 }
*/
}

void SPXPDF::CalcSystErrors()
{
 std::string mn = "CalcSystErrors: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
 if (debug) {
  std::cout<<cn<<mn<<"Start systematic error calculation for: "<<PDFtype<<std::endl;
  if (do_Scale)  std::cout<<cn<<mn<<"Calculate Scale uncertainty band "<<std::endl;
  if (do_PDFBand)std::cout<<cn<<mn<<"Calculate PDF uncertainty band "<<std::endl;
  if (do_AlphaS) std::cout<<cn<<mn<<"Calculate AlphaS uncertainty band "<<std::endl;
  if (do_Escale) std::cout<<cn<<mn<<"Calculate beam energy uncertainty band "<<std::endl;
 }

 if (do_PDFBand) CalcPDFBandErrors();
 if (do_AlphaS ) CalcAlphaSErrors();
 if (do_Scale)   CalcScaleErrors();
 if (do_Escale)  CalcBeamEnergyErrors();

 if (debug) std::cout<<cn<<mn<<"End systematic error calculation for: "<<PDFtype<<std::endl;
}


void SPXPDF::CalcBeamEnergyErrors()
{
 // 
 // calculate beam energy uncertainty bands
 //
 // applgrid has Escale * sqrts(s) = old/new
 // Escale=1 is nominal cms energy
 // Escale=1/2 is doubled cms energy

 std::string mn = "CalcBeamEnergyErrors: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);


 if (debug) {
  std::cout<<cn<<mn<<"Calculate beam energy uncertainty for "<<std::endl;
  std::cout<<cn<<mn<<"Escale= "<<Escale<<std::endl;
 }

 double renscale=1.; double facscale=1.;
 // mean is default
 //h_BeamUncertainty_results=SPXGraphUtilities::TH1TOTGraphAsymm(hpdfdefault);
   

 // LHAPDF::initPDF(defaultpdfid);
 if (debug) std::cout<<cn<<mn<<" nLoops= "<<nLoops<<" renscale= "<<renscale<<" facscale= "<<facscale<<" xEscale=1 "<<std::endl;

 TH1D *hnom= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops, renscale, facscale,  1.);
 if (!hnom) {std::cout<<cn<<mn<<"WARNING: Can not convolute nominal beam energy "<<std::endl; return;}
 hnom->SetName("NominalBeamEnergyUncertainy");
 //should be already filled
 //h_BeamUncertainty_results=SPXGraphUtilities::TH1TOTGraphAsymm(hnom);

 if (debug) {
  std::cout<<cn<<mn<<"Nominal "<<std::endl;
  hnom->Print("all");

  std::cout<<cn<<mn<<"Nominal/hpdfdefault "<<std::endl;
  TH1D * hratio=(TH1D*) hnom->Clone("hnom");
  hratio->Divide(hpdfdefault);
  hratio->Print("all");
 }


 TH1D *htmp= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops, renscale, facscale,  1./Escale);
 if (!htmp) {std::cout<<cn<<mn<<"WARNING: Can not convolute up beam energy "<<std::endl; return;}
 TString hname=Form("xsec_BeamUncertainty_%4.3f_%s",Escale,default_pdf_set_name.c_str());
 htmp->SetName(hname);
 h_BeamUncertainty_results->SetName(hname);
 h_errors_BeamUncertainty.push_back(htmp);

 if (debug) {
  std::cout<<cn<<mn<<"After varying beam energy up (ratio to default): "<<std::endl;
  TH1D * hratio=(TH1D*) htmp->Clone(hname);
  hratio->Divide(hnom);
  hratio->Print("all");
 }

 htmp= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops, renscale, facscale, Escale);
 if (!htmp) {std::cout<<cn<<mn<<"WARNING: Can not convolute down beam energy "<<std::endl; return;}
 hname=Form("xsec_BeamUncertainty_%4.3f_%s",1./Escale,default_pdf_set_name.c_str());
 htmp->SetName(hname);
 h_errors_BeamUncertainty.push_back(htmp);

 if (debug) {
  std::cout<<cn<<mn<<"After varying beam energy down: "<<std::endl;
  hname+="ratio";
  TH1D * hratio=(TH1D*) htmp->Clone(hname);
  hratio->Divide(hnom);
  hratio->Print("all");
 }

 for (int ibin=1;ibin<hpdfdefault->GetNbinsX()+1;++ibin){
  double central=hnom->GetBinContent(ibin);

  double max=central;
  double min=central;
  // 
  for (int i=0; i< h_errors_BeamUncertainty.size();++i){
   double content=h_errors_BeamUncertainty[i]->GetBinContent(ibin);
   if (content>max) max=content;
   if (content<min) min=content;
  }

  if (debug) {
   double def=hnom->GetBinContent(ibin);
   std::cout<<"ibin= "<<ibin<<" max= "<<max<<" min= "<<min
                      <<" max/def= "<< max/def<<" min/def= "<<min/def <<std::endl;
  }

  double init_x_val;
  double init_y_val;
 
  h_BeamUncertainty_results->GetPoint(ibin-1, init_x_val, init_y_val);

  if (debug) {
   std::cout<<"ibin= "<<ibin<<" max= "<<max<<" min= "<<min
	    <<" init_y_val= "<<init_y_val<<std::endl;
  }
  //h_BeamUncertainty_results->SetPoint(ibin-1, init_x_val, init_y_val);
  h_BeamUncertainty_results->SetPointEYhigh(ibin-1, max-init_y_val);
  h_BeamUncertainty_results->SetPointEYlow(ibin-1, init_y_val-min);
 } /// ibin

 if (debug) {
  std::cout<<"Print "<< h_BeamUncertainty_results->GetName()<<std::endl;
  h_BeamUncertainty_results->Print();
 }

 h_BeamUncertainty_results->SetFillStyle  (fillStyleCode);
 SPXGraphUtilities::SetColors( h_BeamUncertainty_results,fillColorCode);

 return;

}

void SPXPDF::CalcPDFBandErrors()
{
 // needs some more checking for uncertainty bands
 // calculate uncertainty bands
 //
 std::string mn = "CalcPDFBandErrors: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);
 if (debug) std::cout<<cn<<mn<<"Start calc of PDFBandErrors for: "<<PDFtype<<std::endl;
 if (debug) std::cout<<cn<<mn<<"defaultpdf= "<<defaultpdfid<<std::endl;

 TString defname=PDFtype+"_default_set= ";
 defname+=defaultpdfid;

 if (h_errors_PDF.size()<1) {
  std::ostringstream oss;
  oss << cn << mn << "ERROR: h_error_PDF is too small "<<h_errors_PDF.size(); 
  throw SPXParseException(oss.str());
 }

 hpdfdefault=(TH1D*)h_errors_PDF.at(defaultpdfid)->Clone(defname);

 if (debug) {
  std::cout<<cn<<mn<<"Cross section for defaultpdf: "<<std::endl;
  hpdfdefault->Print("all");

  std::cout<<cn<<mn<<"ErrorPropagationType= "<<ErrorPropagationType<<std::endl;
 }

 for (int bi = 1; bi <= h_errors_PDF.at(0)->GetNbinsX(); bi++) { // loop over bins
  if (debug) std::cout<<cn<<mn<<"bin= "<<bi<<std::endl;

  double this_err_up         = 0.;
  double this_err_down       = 0.;
  double central_val         = 0.;    // needed for MSTW2008nlo and HERAPDF
  double average             = 0.;    // needed for NNPDF
  double extreme_pos_diff    = 0.;    // needed for HERAPDF
  double extreme_neg_diff    = 0.;    // needed for HERAPDF
  double diff_central        = 0.;    // needed for HERAPDF
  double mod_val             = 0.;    // needed for MSTW2008nlo

  if (ErrorPropagationType==StyleNNPDF) {
   // NNPDF are replicatas, calculate the RMS
   // better use formulaa with sum^2 and sum ?
   for (int pdferri = 0; pdferri < (int) h_errors_PDF.size(); pdferri++) {
    average += h_errors_PDF.at(pdferri)->GetBinContent(bi);
   }
   average /= h_errors_PDF.size()-1;
   //
   // update default histogram
   hpdfdefault->SetBinContent(bi,average);

   for (int pdferri = 1; pdferri < (int) h_errors_PDF.size(); pdferri++)  {
    this_err_up += pow(h_errors_PDF.at(pdferri)->GetBinContent(bi)-average, 2.);
   }
   this_err_up = TMath::Sqrt(this_err_up / (h_errors_PDF.size()-1));
   // error with respect to average
   this_err_down = this_err_up;
  } else if (ErrorPropagationType==EigenvectorSymmetricHessian) {
   // Symmetric hessian
   for (int pdferri = 1; pdferri < (int) h_errors_PDF.size()-1; pdferri += 2) {
    this_err_up += pow( h_errors_PDF.at(pdferri)->GetBinContent(bi) - h_errors_PDF.at(pdferri+1)->GetBinContent(bi), 2.);
   }
   // here errors are symmetrized
   this_err_up = 0.5*TMath::Sqrt(this_err_up);
   //if (debug)
   //std::cout<<cn<<mn<<"this_err_up= "<<this_err_up<<" PDFErrorSize= "<<PDFErrorSize.c_str()<<std::endl;
   //if (PDFErrorSize.compare("OneSigma")==0) {
   // if (debug) std::cout<<cn<<mn<<" put to one sigma "<<std::endl;
   // this_err_up /= 1.645;
   //}
   this_err_down = this_err_up;

  } else if (ErrorPropagationType==EigenvectorAsymmetricHessian) {
  //
  // Asymmetric hessian
  //
   central_val = h_errors_PDF.at(defaultpdfid)->GetBinContent(bi);
   //https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TopPdfUncertainty
   //>>>> modifications by P. Berta 28th August
   //
   //for (int pdferri = 1; pdferri < (int) h_errors_PDF.size(); pdferri ++) {
   // mod_val = h_errors_PDF.at(pdferri)->GetBinContent(bi);
   // if (mod_val > central_val ) this_err_up  += pow(mod_val-central_val, 2.);
   // else                        this_err_down+= pow(central_val - mod_val, 2.);
   //}
   for (int pdferri = 1; pdferri < (int) h_errors_PDF.size()-1; pdferri += 2) {
    double delta_up_variation  =h_errors_PDF.at(pdferri)  ->GetBinContent(bi)-central_val;
    double delta_down_variation=h_errors_PDF.at(pdferri+1)->GetBinContent(bi)-central_val;
    if (delta_up_variation>0   && delta_up_variation>delta_down_variation) this_err_up+=pow(delta_up_variation,2.);
    if (delta_down_variation>0 && delta_down_variation>delta_up_variation) this_err_up+=pow(delta_down_variation,2.);
    if (delta_up_variation<0   && delta_up_variation<delta_down_variation) this_err_down+=pow(delta_up_variation,2.);
    if (delta_down_variation<0 && delta_down_variation<delta_up_variation) this_err_down+=pow(delta_down_variation,2.);
   }

   this_err_down = TMath::Sqrt(this_err_down);
   this_err_up   = TMath::Sqrt(this_err_up);
  } else if (ErrorPropagationType==StyleHeraPDF) {
   if (debug) {
    std::cout<<cn<<mn<<"HERA/ATLAS type PDF "<<std::endl;
    std::cout<<cn<<mn<<"PDFname= "<<PDFname.c_str()<<" PDFnamevar= "<<PDFnamevar.c_str()<<std::endl;
    std::cout<<cn<<mn<<"firsteig= "<<firsteig<<" lasteig= "<<lasteig<<std::endl;
    std::cout<<cn<<mn<<"firstquadvar= "<<firstquadvar<<" lastquadvar= "<<lastquadvar<<std::endl;
    std::cout<<cn<<mn<<"firstmaxvar= "<<firstmaxvar<<" lastmaxvar= "<<lastmaxvar<<std::endl;
    std::cout<<cn<<mn<<"defaultpdfid= "<<defaultpdfid<<" defaultpdfidvar= "<<defaultpdfidvar<<std::endl;
   }

   central_val = h_errors_PDF.at(defaultpdfid)->GetBinContent(bi);

   if (debug) std::cout<<cn<<mn<<"Default sample id= "<<defaultpdfid<<" "<<" central_val= "<<central_val<<std::endl;
   if (includeEIG) {
    for (int pdferri = firsteig; pdferri <=lasteig; pdferri += 2) {   //// experimental errors
     this_err_up += pow( 0.5*(h_errors_PDF.at(pdferri+1)->GetBinContent(bi)
                            - h_errors_PDF.at(pdferri)  ->GetBinContent(bi)), 2.);

     if (debug) std::cout<<cn<<mn<<" "<<PDFtype.c_str()
		    <<"pdferri= "<<pdferri<<" , "<<pdferri+1<<" EIG "
		    <<" this_err_up= "<<this_err_up
		    <<" diff/nom= "<<(h_errors_PDF.at(pdferri+1)->GetBinContent(bi)-h_errors_PDF.at(pdferri)  ->GetBinContent(bi))/central_val
		    <<" (var+1)/nom= "<<h_errors_PDF.at(pdferri+1)->GetBinContent(bi)/central_val
		    <<" (var)/nom= "  <<h_errors_PDF.at(pdferri)  ->GetBinContent(bi)/central_val
		    <<std::endl;
    }
   }
 
   if (debug) std::cout<<cn<<mn<<" finished includeEIG "<<std::endl;

   this_err_down = this_err_up;

   int firstvar=lasteig+1; // uncertainties start at last eigenvector previous set + default of variation samle

   central_val = h_errors_PDF.at(firstvar+defaultpdfidvar)->GetBinContent(bi);
   if (debug) std::cout<<cn<<mn<<" variation sample id= "<<firstvar+defaultpdfidvar<<" "<<" central_val= "<<central_val<<std::endl;
   if (includeQUAD) {
    for (int pdferri = firstvar+firstquadvar; pdferri < firstvar+lastquadvar; pdferri++) {// parameterisation errors
     if (h_errors_PDF.at(pdferri)->GetBinContent(bi) > central_val ) {
      this_err_up  += pow( h_errors_PDF.at(pdferri)->GetBinContent(bi) - central_val, 2.);
      this_err_down+= 0.;
     } else  {
      this_err_up  += 0.;
      this_err_down+= pow( central_val - h_errors_PDF.at(pdferri)->GetBinContent(bi), 2.);
     }
     if (debug) std::cout<<cn<<mn<<" "<<PDFtype.c_str()
			 <<" pdferri= "<<pdferri<<" , "<<pdferri+1<<" parameterisation "
			 <<" this_err_up= "<<this_err_up
			 <<" this_err_down= "<<this_err_down
			 <<" (var)/nom= "  <<h_errors_PDF.at(pdferri)  ->GetBinContent(bi)/central_val
			 <<std::endl;
    }
   }

   if (includeMAX) {
    for (int pdferri = firstvar+firstmaxvar; pdferri < firstvar+lastmaxvar; pdferri++) { // model errors
     diff_central = h_errors_PDF.at(pdferri)->GetBinContent(bi) - central_val;
     //if (debug) std::cout<<cn<<mn<<" diff_central= "<<diff_central
     //                    <<" var/nom= "<<h_errors_PDF.at(pdferri)->GetBinContent(bi)/central_val
     //                    <<endl;
     if (diff_central > 0 && diff_central > extreme_pos_diff ) extreme_pos_diff = diff_central;
     if (diff_central < 0 && diff_central < extreme_neg_diff ) extreme_neg_diff = diff_central;

     if (debug) std::cout<<cn<<mn<<" "<<PDFtype.c_str()<<" pdferri= "
		    <<pdferri<<" , "<<pdferri+1<<" model "
		    <<" extreme_pos_diff= "<<extreme_pos_diff
		    <<" extreme_neg_diff= "<<extreme_neg_diff
		    <<std::endl;
    }
    if (extreme_pos_diff > 0. ) this_err_up   += pow(extreme_pos_diff, 2.);
    if (extreme_neg_diff < 0. ) this_err_down += pow(extreme_neg_diff, 2.);
   }

   this_err_up   = TMath::Sqrt(this_err_up);
   this_err_down = TMath::Sqrt(this_err_down);

  } else {
  std::ostringstream oss;
  oss << cn << mn << "ERROR Unsupported pdfCode encountered ErrorPropagationType= "<<ErrorPropagationType
             <<" PDFtype= "<<PDFtype.c_str();
  throw SPXParseException(oss.str());
  }

  if (debug) std::cout<<cn<<mn<<"this_err_up= "  <<this_err_up  <<std::endl;
  if (debug) std::cout<<cn<<mn<<"this_err_down= "<<this_err_down<<std::endl;

  if (f_PDFErrorSize90Percent){
   if (debug) std::cout<<cn<<mn<<"PDF has 90Percent error -> change to 68% "<<std::endl;
   this_err_up  /= 1.645;
   this_err_down/= 1.645;
  }

  if (ErrorPropagationType==StyleNNPDF) {
   double x_val, y_val;
   //
   // replace central value by average
   //
   h_PDF_results->GetPoint(bi-1, x_val, y_val);
   h_PDF_results->SetPoint(bi-1, x_val,average);

   if (h_errors_PDF.size()==0) {
     std::cerr<<cn<<mn<<"WARNING: h_PDF_errors not found. Can not be updated !"<<std::endl;
    } else {
     if (h_errors_PDF.at(0)==0) {    
      std::cerr<<cn<<mn<<"WARNING: h_PDF_errors not found. Can not be updated !"<<std::endl;
     } else {
      if (debug) std::cout<<cn<<mn<<"Update h_errors_PDF[0] "<<h_errors_PDF.at(0)->GetName()<<" with x= "<<x_val<<" average= "<<average<<std::endl;
      h_errors_PDF.at(0)->SetBinContent(bi,x_val,average); 
     }
    } 
   
   //
   // Update also other defaults
   //
   if (do_Scale) {
    if (h_Scale_results)  {
    if (debug) std::cout<<cn<<mn<<"Update h_Scale_results with x= "<<x_val<<" average= "<<average<<std::endl;
    h_Scale_results ->SetPoint(bi-1, x_val,average);
    } else {
     std::cerr<<cn<<mn<<"WARNING: h_Scale_results not found. Can not be updated !"<<std::endl;
    }

    if (h_errors_Scale.size()==0) {
     std::cerr<<cn<<mn<<"WARNING: h_Scale_errors not found. Can not be updated !"<<std::endl;
    } else {
     if (h_errors_Scale.at(0)==0) {    
      std::cerr<<cn<<mn<<"WARNING: h_Scale_errors not found. Can not be updated !"<<std::endl;
     } else {
      if (debug) std::cout<<cn<<mn<<"Update h_errors_Scale[0] "<<h_errors_AlphaS.at(0)->GetName()<<" with x= "<<x_val<<" average= "<<average<<std::endl;
      h_errors_Scale.at(0)->SetBinContent(bi,x_val,average); 
     }
    } 
   }

   if (do_AlphaS) {
    if (h_AlphaS_results) {
     if (debug) std::cout<<cn<<mn<<"Update h_AlphaS_results with x= "<<x_val<<" average= "<<average<<std::endl;
     h_AlphaS_results->SetPoint(bi-1, x_val,average);
    } else {
     std::cerr<<cn<<mn<<"WARNING: h_Alphas_results not found. Can not be updated !"<<std::endl;
    }

    if (h_errors_AlphaS.size()==0) {
     std::cerr<<cn<<mn<<"WARNING: h_Alphas_errors not found. Can not be updated !"<<std::endl;
    } else {
     if (h_errors_AlphaS.at(0)==0) {    
      std::cerr<<cn<<mn<<"WARNING: h_Alphas_errors not found. Can not be updated !"<<std::endl;
     } else {
      if (debug) std::cout<<cn<<mn<<"Update h_errors_AlphaS[0] "<<h_errors_AlphaS.at(0)->GetName()<<" with x= "<<x_val<<" average= "<<average<<std::endl;
      h_errors_AlphaS.at(0)->SetBinContent(bi,x_val,average); 
     }
    }
   }

   if (do_Escale) {
    if (h_BeamUncertainty_results) {
     if (debug) std::cout<<cn<<mn<<"Update h_BeamUncertainty_results with x= "<<x_val<<" average= "<<average<<std::endl;
     h_BeamUncertainty_results->SetPoint(bi-1, x_val,average);
    } else {
     std::cerr<<cn<<mn<<"WARNING: h_BeamUncertainty_results not found. Can not be updated !"<<std::endl;
    }

    if (h_errors_BeamUncertainty.size()==0) {
     std::cerr<<cn<<mn<<"WARNING: h_BeamUncertainty_errors not found. Can not be updated !"<<std::endl;
    } else {
     if (h_errors_BeamUncertainty.at(0)==0) {    
      std::cerr<<cn<<mn<<"WARNING: h_BeamUncertainty_errors not found. Can not be updated !"<<std::endl;
     } else {
      if (debug) std::cout<<cn<<mn<<"Update h_errors_BeamUncertainty[0] "<<h_errors_BeamUncertainty.at(0)->GetName()<<" with x= "<<x_val<<" average= "<<average<<std::endl;
      h_errors_BeamUncertainty.at(0)->SetBinContent(bi,x_val,average); 
     }
    } 

   }
  }
  h_PDF_results->SetPointEYhigh(bi-1, this_err_up);
  h_PDF_results->SetPointEYlow (bi-1, this_err_down);

  if (debug) {
   double x_val;
   double y_val;
   h_PDF_results->GetPoint(bi-1, x_val, y_val);
   if (y_val!=0) {
    std::cout<<cn<<mn<<"bin= "<<bi<<" (y_val+this_err_up)/y_val= "  <<(y_val+this_err_up)/y_val
	     <<" (y_val-this_err_down)/y_val= "<<(y_val-this_err_down)/y_val<<std::endl;
   }
  }
 }  /// loop over bins

 if(debug) std::cout<<cn<<mn<<"End calculation of PDFBandErrors for: "<<PDFtype<<std::endl;
}

void SPXPDF::CalcAlphaSErrors()
{
 std::string mn = "CalcAlphaSErrors: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
 if(debug) std::cout<<cn<<mn<<"Starting calculation of AlphaSErrors for: "<<PDFtype<<std::endl;

 //assert(h_errors_AlphaS.size() == 3);

 double this_default_val = 0.;
 double this_err_down = 0.;
 double this_err_up = 0.;
 double error_up = 0.;
 double error_down = 0.;

 if (debug) std::cout<<cn<<mn<<"Size h_errors_AlphaS= "<<h_errors_AlphaS.size()<<std::endl;

 if (h_errors_AlphaS.size()==0) {
  std::cout<<cn<<mn<<"ERROR h_errors_AlphaS not filled "<<std::endl;
  std::cerr<<cn<<mn<<"ERROR h_errors_AlphaS not filled "<<std::endl;
 }

 for (int i=0; i<h_errors_AlphaS.size(); i++) {
  if (!h_errors_AlphaS[i]) {
    std::cout<<cn<<mn<<"ERROR h_errors_AlphaS["<<i<<"] not found ! "<<std::endl;
    std::cerr<<cn<<mn<<"ERROR h_errors_AlphaS["<<i<<"] not found ! "<<std::endl;
  }

  // if (debug) {
  // std::cout<<cn<<mn<<"AlphaS calculated from histogram: "<<h_errors_AlphaS[i]->GetName()<<std::endl;
  // h_errors_AlphaS[i]->Print("all");
  //}

 }

 if (f_PDFErrorSize90Percent){
  alphaS_absUnc_worldAverage=0.002;
 } else {
  alphaS_absUnc_worldAverage=0.0012;
 }

 if (debug)
  std::cout<<cn<<mn<<"alphaS_absUnc_worldAverage= "<< alphaS_absUnc_worldAverage <<std::endl;
  
 alphaS_relUnc_worldAverage=alphaS_absUnc_worldAverage/alphaS_value_worldAverage;

 if (debug) {
  std::cout<<cn<<mn<<"alphaS_scale_worldAverage = "<<alphaS_scale_worldAverage<<std::endl;
  std::cout<<cn<<mn<<"alphaS_absUnc_worldAverage= "<<alphaS_absUnc_worldAverage<<std::endl;
  std::cout<<cn<<mn<<"alphaS_value_worldAverage = "<<alphaS_value_worldAverage<<std::endl;
  std::cout<<cn<<mn<<"alphaS_relUnc_worldAverage= "<<alphaS_relUnc_worldAverage<<std::endl;
 }

 double alphaS_absUnc=alphaS_relUnc_worldAverage*alphaS_variations.at(0);

 if (debug) std::cout<<cn<<mn<<"alphaS_absUnc= "<<alphaS_absUnc<<std::endl;

 int ifirstup=0; int ifirstdn=0;
 for (int bi = 1; bi <= h_errors_AlphaS.at(0)->GetNbinsX(); bi++) {
  this_default_val = h_errors_AlphaS.at(0)->GetBinContent(bi);
  this_err_down    = h_errors_AlphaS.at(1)->GetBinContent(bi);
  this_err_up      = h_errors_AlphaS.at(2)->GetBinContent(bi);

  if (debug) std::cout<<cn<<mn<<"bin = "<<bi<<", default val = "<<this_default_val
             <<" +"<<this_err_up<<" -"<<this_err_down<<std::endl;

  // now interpolate for alpha_s uncertainty wanted
  // 0 is default variations
  double dxup=alphaS_variations.at(2)-alphaS_variations.at(0);

  if (debug)
   std::cout<<cn<<mn<<"dxup= "<<dxup<<" var2= "<<alphaS_variations.at(2)<<" var0= "<<alphaS_variations.at(0)<<std::endl;

  if (dxup<1.e-12) {
   std::cout<<cn<<mn<<"WARNING dxup= "<<dxup<<" var2= "<<alphaS_variations.at(2)<<" var0= "<<alphaS_variations.at(0)<<std::endl;
   std::cout<<cn<<mn<<"WARNING May be exchanged up and down variation ? "<<std::endl;
  }

  if (fabs(dxup)<1.e-12) {
   ifirstup++;
   if (ifirstup==1) {
    std::cout<<cn<<mn<<"WARNING: No variation in alphas up, can not calculate ALPHAS uncertainty "<<h_errors_AlphaS.at(2)->GetName()<<std::endl;
    std::cerr<<cn<<mn<<"WARNING: No variation in alphas up, can not calculate ALPHAS uncertainty "<<h_errors_AlphaS.at(2)->GetName()<<std::endl;
   }
   error_up=0.;
  } else {
   error_up =   (this_err_up-this_default_val)  *alphaS_absUnc/dxup;
  }

  double dxdn=alphaS_variations.at(0)-alphaS_variations.at(1);

  if (debug)
   std::cout<<cn<<mn<<"dxdn= "<<dxdn<<" var1= "<<alphaS_variations.at(1)<<" var0= "<<alphaS_variations.at(0)<<std::endl;

  if (dxdn<1.e-12) {
   std::cout<<cn<<mn<<"WARNING dxdn= "<<dxdn<<" var1= "<<alphaS_variations.at(1)<<" var0= "<<alphaS_variations.at(0)<<std::endl;
   std::cout<<cn<<mn<<"WARNING May be exchanged up and down variation ? "<<std::endl;
  }

  if (fabs(dxdn)<1.e-12) {
   ifirstdn++;
   if (ifirstdn==1) {
    std::cout<<cn<<mn<<"WARNING: No variation in alphas down, can not calculate ALPHAS uncertainty "<< h_errors_AlphaS.at(1)->GetName()<<std::endl;
    std::cerr<<cn<<mn<<"WARNING: No variation in alphas down, can not calculate ALPHAS uncertainty "<< h_errors_AlphaS.at(1)->GetName()<<std::endl;
   }
   error_down=0.;
  } else {
   error_down = (this_default_val-this_err_down)*alphaS_absUnc/dxdn;
  }

  if (debug) std::cout<<cn<<mn<<"bin = "<<bi<<" error_up = "<<error_up<<" error_down = "<<error_down<<std::endl;

  double init_x_val;
  double init_y_val;

  h_AlphaS_results->GetPoint(bi-1, init_x_val, init_y_val);
  h_AlphaS_results->SetPoint(bi-1, init_x_val, this_default_val);
  h_AlphaS_results->SetPointEYhigh(bi-1, error_up);
  h_AlphaS_results->SetPointEYlow(bi-1, error_down);

 } /// bi

 h_AlphaS_results->SetFillStyle  (fillStyleCode);
 SPXGraphUtilities::SetColors(h_AlphaS_results,fillColorCode);

 if (debug) std::cout<<cn<<mn<<"End AlphaS uncertainty calculation for: "<<PDFtype<<std::endl;
}

void SPXPDF::CalcScaleErrors()
{
 std::string mn = "CalcScaleErrors:";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (debug) std::cout<<cn<<mn<<" Starting calculation of ScaleErrors for: "<<PDFtype<<std::endl;

 if (h_errors_Scale.size()==0){
  std::cout<<cn<<mn<<"Warning: h_errors_Scale not filled ! Not computing scale uncertainty. "<<std::endl; 
  return;
 }

 for (int ibin=1;ibin<h_errors_Scale[0]->GetNbinsX()+1;++ibin){
  double central=h_errors_Scale[0]->GetBinContent(ibin);

  double max=central;
  double min=central;
  // not clear that we want the maximum here
  // could also implement other techniques
  for (int iscale=1;iscale<h_errors_Scale.size();++iscale){
   double content=h_errors_Scale[iscale]->GetBinContent(ibin);
   if (content>max) max=content;
   if (content<min) min=content;
  }

  double init_x_val;
  double init_y_val;
  h_Scale_results->GetPoint(ibin-1, init_x_val, init_y_val);
  // default of ren scale should be the same as for the other bands, otherwise total band is not largest
  //h_Scale_results->SetPoint(ibin-1, init_x_val, central);
  //h_Scale_results->SetPointEYhigh(ibin-1, max-central);
  //h_Scale_results->SetPointEYlow(ibin-1, central-min);
  h_Scale_results->SetPoint(ibin-1, init_x_val, init_y_val);
  h_Scale_results->SetPointEYhigh(ibin-1, max-init_y_val);
  h_Scale_results->SetPointEYlow(ibin-1, init_y_val-min);
 } /// ibin

 //h_Scale_results->SetName(h_errors_Scale[0]->GetName());

 h_Scale_results->SetFillStyle  (fillStyleCode);
 //h_Scale_results->SetMarkerColor(fillColorCode);
 //h_Scale_results->SetLineColor  (fillColorCode);
 //h_Scale_results->SetFillColor  (fillColorCode);

 SPXGraphUtilities::SetColors( h_Scale_results,fillColorCode);

 if (debug) {
   std::cout<<cn<<mn<<" Line Color= "<<h_Scale_results->GetLineColor()<<std::endl;
   std::cout<<cn<<mn<<" Fill Color= "<<h_Scale_results->GetFillColor()<<std::endl;
   std::cout<<cn<<mn<<" h_Scale_results "<< h_Scale_results->GetName()<<std::endl;
  h_Scale_results->Print("all");
 }

 if (debug) std::cout<<cn<<mn<<"End calculation of ScaleErrors for: "<<PDFtype<<std::endl;
}

void SPXPDF::CalcTotalErrors()
{
 std::string mn = "CalcTotalErrors: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (!do_Total) {
  std::cout <<cn<<mn<< "WARNING: do_total is off, will not calculate total error"<< std::endl;
  return;
 }

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can not calculate total uncertainty "<<std::endl;
  return;
 } 

 int nbin=0; 
 int nbinold=(Mapallbands.begin()->second)->GetN();
 for (BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  if (debug) std::cout <<cn<<mn<< "Do: " <<  it->second->GetName() << std::endl;

  nbin=it->second->GetN();

  if (debug)
   std::cout <<cn<<mn<< "map["<<it->first<<"]:"<< " graph name: " << it->second->GetName() <<" nbin= "<<nbin<< std::endl;
  if (nbin!=nbinold)
   std::cout <<cn<<mn<<"WARNING: Number of bins is different" <<" nbin= "<<nbin<<" nbinold= "<<nbinold<< std::endl;

  std::string gtype   =it->first;
  if (gtype.compare(std::string("total"))==0){
   std::ostringstream oss;
   oss << cn << mn << "ERROR Mapallbands already contains total uncertainty, called twice ? "; 
   throw SPXGraphException(oss.str());
  }

 }

 // Create total uncertainty graph
 TString name="xsec_total_"+default_pdf_set_name;
 if (spxgrid) name+="_"+spxgrid->GetName();

 h_Total_results=(TGraphAsymmErrors *) (Mapallbands.begin()->second)->Clone(name);
 if (!h_Total_results) {
  throw SPXGraphException(cn+mn+"Could not create h_Total_results !");
 }

 for (int ibin=0; ibin<nbin; ibin++) {

  double totalError_high=0.;
  double totalError_low =0.;

  double xold=-99999, yold=-99999.;
  int icount=0, ioldx=-1, ioldy=-1;
  for (BandMap_T::const_iterator it = Mapallbands.begin(); it !=Mapallbands.end(); ++it) {
   //
   // add now everything that is enabled in quadrature
 
   double x=0., y=0.;
   it->second->GetPoint(ibin,x,y);
   icount++;
   if (icount==1) { xold=x, yold=y; ioldx=icount, ioldy=icount;}
   else {   
    if (xold!=x) {
     std::cout <<cn<<mn<<"WARNING: Something changed for "<<it->first<<" xold= "<<xold<<" x= "<<x<<" ratio= "<<xold/x<<" iold= "<<ioldx<<" icount= "<<icount<<std::endl;
     std::cerr <<cn<<mn<<"WARNING: Something changed for "<<it->first<<" xold= "<<xold<<" x= "<<x<<" ratio= "<<xold/x<<" iold= "<<ioldx<<" icount= "<<icount<<std::endl;
    }
    if (yold!=y) {
      std::cout <<cn<<mn<<"WARNING: Something changed for "<<it->first<<" yold= "<<yold<<" y= "<<y<<" ratio= "<<yold/y<<" iold= "<<ioldy<<" icount= "<<icount<<std::endl;
      std::cerr <<cn<<mn<<"WARNING: Something changed for "<<it->first<<" yold= "<<yold<<" y= "<<y<<" ratio= "<<yold/y<<" iold= "<<ioldy<<" icount= "<<icount<<std::endl;
    }
   }
   h_Total_results->SetPoint(ibin,x,y);

   double Error_high=it->second->GetErrorYhigh(ibin);
   double Error_low =it->second->GetErrorYlow(ibin);

   totalError_high+=pow(Error_high,2);
   totalError_low +=pow(Error_low ,2);
   //
   //if (debug) {
   //  std::cout <<cn<<mn<<it->first<<" ibin= "<<ibin
   //	             <<" Error_high= " <<Error_high<<" Error_low= " <<Error_low
   //                  <<" totalError_high= " <<totalError_high<<" totalError_low= " <<totalError_low
   //                  << std::endl;
   //}
  }
  totalError_high=sqrt(totalError_high);
  totalError_low =sqrt(totalError_low);

  //h_Total_results->SetPoint(ibin,x,y);
  h_Total_results->SetPointEYhigh(ibin,totalError_high);
  h_Total_results->SetPointEYlow(ibin,totalError_low);

 }

 if (debug) {
  std::cout <<cn<<mn<<"Calculated total uncertainty "<< h_Total_results->GetName()<< std::endl;
  h_Total_results->Print();
 }

 Mapallbands["total"]=h_Total_results;

 h_Total_results->SetFillStyle  (fillStyleCode);
 //h_Total_results->SetMarkerColor(fillColorCode);
 //h_Total_results->SetLineColor  (fillColorCode);
 //h_Total_results->SetFillColor  (fillColorCode);
 SPXGraphUtilities::SetColors( h_Total_results,fillColorCode);
}

void SPXPDF::DrawPDFBand(){
 std::string mn = "GetPDFBand:";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 TH1D* hpdf=this->GetPdfdefault();
 hpdf->Draw("same");

 //h_PDF_results->Print("all");
 h_PDF_results->Draw("E2,same");

 if (!hpdf) std::cout<<cn<<mn<<" test histo not found "<<std::endl;
 hpdf->Draw("same");

 return;

};

void SPXPDF::DrawPDFRatio(int iset1, int iset2){

 TH1D* hratio=this->GetPDFRatio(iset1, iset2);
 hratio->Draw();

}

double SPXPDF::GetMaximum(int iset){
 return h_errors_PDF[iset]->GetMaximum();
}


TH1D* SPXPDF::GetPDFRatio(int iset1, int iset2)
{
 std::string mn = "GetPDFRatio:";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (debug)
  std::cout<<cn<<mn<<" iset2= "<< iset2<<" iset1= "<<iset1
	   <<" n_PDFMembers= "<<n_PDFMembers<<std::endl;

 TString ratio_to_ref_name = (TString) h_PDF_results->GetName() + " Set=";
 ratio_to_ref_name +=iset1;
 ratio_to_ref_name +=" / Set=";
 ratio_to_ref_name +=iset2;

 if (iset1>=n_PDFMembers||iset2>=n_PDFMembers||iset1<0||iset2<0){
  std::ostringstream oss;
  oss << cn << mn << "ERROR iset too large iset2= "<< iset2<<" iset1= "<<iset1;
  throw SPXParseException(oss.str());
 }

 if (iset2!=0) {
  ratio_to_ref_name +=TString("/set");
  ratio_to_ref_name +=iset2;
 }
 if (debug) std::cout<<cn<<mn<<" "<< ratio_to_ref_name.Data()<<std::endl;

 TH1D *hratio=(TH1D*) h_errors_PDF[iset1]->Clone( ratio_to_ref_name);
 TH1D *htmp=(TH1D*) h_errors_PDF[iset2]->Clone("htmp");
 hratio->Divide(htmp);

 hratio->SetLineColor(fillColorCode);

 return hratio;
}


double SPXPDF::GetPDFWeight(int iset1, double x1, double x2){
 std::string mn = "GetPDFWeight: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 double w=1.;

 TH1D* hpdf=this->GetPdfdefault();
 double hpdfx1=hpdf->GetBinContent(hpdf->FindBin(x1));
 double hpdfx2=hpdf->GetBinContent(hpdf->FindBin(x2));

 TH1D *htmp=h_errors_PDF[iset1];
 double htmpx1=htmp->GetBinContent(htmp->FindBin(x1));
 double htmpx2=htmp->GetBinContent(htmp->FindBin(x2));

 // x1,x2,scale,id1,id2,pdf1,pdf2 0.630916 0.734315 3967.7 21 21 0.000859055 0.000121401
 std::cout<<cn<<mn<<" PDF= "<<this->GetPDFName()<<" iset1= "<<iset1<<" ifl= "<<ifl<<std::endl;
 std::cout<<cn<<mn<<" x1= "<<x1<<" x2= "<<x2<<" Q2= "<<Q2<<" pdf1= "<<htmpx1<<" pdf2= "<<htmpx2<<std::endl;

 w=htmpx1*htmpx2;
 w/=hpdfx1*hpdfx2;

 std::cout<<cn<<mn<<" w= "<<w<<std::endl;

 return w;
};


//Print all relevant internal variable values
void SPXPDF::Print()
{
 int w=30;               //arbitrary size that makes the formatting look pretty
 std::string empty="<empty>"; //print this if no input has been provided for that variable
 std::string ON="ON";         //bool true
 std::string OFF="OFF";       //bool false

 std::cout<<"SPXPDF::Print: >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	  <<"\n"<<std::setw(w)<<"debug:"                <<std::setw(w)<<(debug? "ON":"OFF")
     //   <<"\n"<<setw(w)<<"steeringFilePath:"     <<setw(w)<<(steeringFilePath.size()>0? steeringFilePath:empty)
     //   <<"\n"<<setw(w)<<"steeringFileDir:"      <<setw(w)<<(steeringFileDir.size()>0? steeringFileDir:empty)
	  <<"\n"<<std::setw(w)<<"steeringFileName:"     <<std::setw(w)<<(steeringFileName.size()>0? steeringFileName:empty)
	  <<"\n"<<std::setw(w)<<"gridName:"             <<std::setw(w)<<(gridName.size()>0? gridName:empty)
          <<"\n"
	  <<"\n"<<std::setw(w)<<"PDFtype:"              <<std::setw(w)<<(PDFtype.size()>0? PDFtype:empty)
	  <<"\n"<<std::setw(w)<<"PDFname:"              <<std::setw(w)<<(PDFname.size()>0? PDFname:empty)
	  <<"\n"<<std::setw(w)<<"PDFnamevar:"           <<std::setw(w)<<(PDFnamevar.size()>0? PDFname:empty)
	  <<"\n"<<std::setw(w)<<"numPDFMembers:"        <<std::setw(w)<<(n_PDFMembers!=DEFAULT? patch::to_string(n_PDFMembers):empty)
	  <<"\n"<<std::setw(w)<<"nLoops: "              <<std::setw(w)<<(nLoops!=DEFAULT? patch::to_string(nLoops):empty)
	  <<"\n"<<std::setw(w)<<"fillStyleCode:"        <<std::setw(w)<<(fillStyleCode!=DEFAULT? patch::to_string(fillStyleCode):empty)
	  <<"\n"<<std::setw(w)<<"fillColorCode:"        <<std::setw(w)<<(fillColorCode!=DEFAULT? patch::to_string(fillColorCode):empty)
	  <<"\n"<<std::setw(w)<<"ErrorPropagationType: "<<std::setw(w)<<(ErrorPropagationType)
	  <<"\n"<<std::setw(w)<<"PDFBandType:"          <<std::setw(w)<<(PDFBandType.size()>0? PDFBandType:empty)
	  <<"\n"<<std::setw(w)<<"first Eigenvector: "   <<std::setw(w)<<(firsteig!=DEFAULT? patch::to_string(firsteig):empty)
	  <<"\n"<<std::setw(w)<<"last Eigenvector:   "  <<std::setw(w)<<(lasteig!=DEFAULT?  patch::to_string(lasteig):empty)
	  <<"\n"<<std::setw(w)<<"first component added in quadrature"  <<std::setw(w)<<(firstquadvar!=DEFAULT?patch::to_string(firstquadvar):empty)
	  <<"\n"<<std::setw(w)<<"last component added in quadrature"  <<std::setw(w)<<(lastquadvar!=DEFAULT?patch::to_string(lastquadvar):empty)
	  <<"\n"<<std::setw(w)<<"first component to find maximum"  <<std::setw(w)<<(firstmaxvar!=DEFAULT?patch::to_string(firstmaxvar):empty)
	  <<"\n"<<std::setw(w)<<"last component to find maximum"  <<std::setw(w)<<(lastmaxvar!=DEFAULT?patch::to_string(lastmaxvar):empty)
	  <<"\n"<<std::setw(w)<<"f_PDFErrorSize90Percent"         <<std::setw(w)<<(f_PDFErrorSize90Percent? ON:OFF)
	  <<"\n"<<std::setw(w)<<"PDFBand:"              <<std::setw(w)<<(do_PDFBand? ON:OFF)
	  <<"\n"<<std::setw(w)<<"AlphaS:"               <<std::setw(w)<<(do_AlphaS? ON:OFF)
	  <<"\n"<<std::setw(w)<<"Scale:" <<std::setw(w)<<(do_Scale? ON:OFF)
	  <<"\n"<<std::setw(w)<<"includeEIG:"           <<std::setw(w)<<(includeEIG? ON:OFF)
	  <<"\n"<<std::setw(w)<<"includeQUAD:"          <<std::setw(w)<<(includeQUAD? ON:OFF)
	  <<"\n"<<std::setw(w)<<"includeMAX:"           <<std::setw(w)<<(includeMAX? ON:OFF)
     <<"\n SPXPDF::Print:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"<<std::endl;
}

//default values for variables to avoid crashes and check for proper setup before doing anything
void SPXPDF::SetVariablesDefault()
{
 if (debug) std::cout<<"SPXPDF::setVariablesDefault: Start default values being set."<<std::endl;
 std::string defaultString="";
 //int defaultInt=-1;

 DEFAULT=-1;

 debug=false;
 //steeringFileDir=defaultString;
 steeringFileName=defaultString;
 PDFtype=defaultString;
 PDFname=defaultString;
 PDFnamevar=defaultString;
 n_PDFMembers=DEFAULT;
 fillStyleCode=DEFAULT;
 fillColorCode=DEFAULT;
 ErrorPropagationType=-1;
 PDFBandType=defaultString;
 PDFErrorType=defaultString;
 //PDFErrorSize="OneSigma";

 defaultpdfid=0;
 defaultpdfidvar=-1;
 firsteig=1;
 lasteig =1;
 firstquadvar=1;
 lastquadvar=1;
 firstmaxvar=1;
 lastmaxvar =1;
 markerStyle=20; //open circle

 includeEIG=false;
 includeQUAD=false;
 includeMAX=false;

 do_PDFBand=true;
 do_AlphaS=false;
 do_Scale =true;
 do_Escale = false;
 do_Total =true;

 AlphaSmemberNumDown=DEFAULT;
 AlphaSmemberNumUp=DEFAULT;
 AlphaSPDFSetNameDown=defaultString;
 AlphaSPDFSetNameUp=defaultString;
 //AlphaSPDFSetHistNameDown=defaultString;
 //AlphaSPDFSetHistNameUp=defaultString;

 alphaS_value_worldAverage=0.1184;
 //alphaS_absUnc_worldAverage=0.0020;
 alphaS_scale_worldAverage=91.1876;

 Escale=1.;

 if (debug) std::cout<<cn<<"setVariablesDefault: End default values are set."<<std::endl;
}


//mutator methods
void SPXPDF::SetDebug(bool _debug) {
 debug=_debug;
}
void SPXPDF::SetGridName(std::string _gridName) {
 gridName=_gridName;
}

void SPXPDF::SetSteeringFileName(std::string _steeringFileName) {
 steeringFileName=_steeringFileName;
}
void SPXPDF::SetPDFtype(std::string _PDFtype) {
 PDFtype=_PDFtype;
}
void SPXPDF::SetPDFname(std::string _PDFname) {
 PDFname=_PDFname;
}
void SPXPDF::SetNumPDFMembers(int _n_PDFMembers) {
 n_PDFMembers=_n_PDFMembers;
}
void SPXPDF::SetFillStyleCode(int _fillStyleCode) {
 fillStyleCode=_fillStyleCode;
}
void SPXPDF::SetFillColorCode(int _fillColorCode) {
 fillColorCode=_fillColorCode;
}
void SPXPDF::SetPDFBandType(std::string _PDFBandType) {
 PDFBandType=_PDFBandType;
}
void SPXPDF::SetPDFErrorType(std::string _PDFErrorType) {
 PDFErrorType=_PDFErrorType;
}
//void SPXPDF::SetPDFErrorSize(string _PDFErrorSize) {
// PDFErrorSize=_PDFErrorSize;

void SPXPDF::SetDoPDFBand(bool _doit) {
 do_PDFBand = _doit;
}
void SPXPDF::SetDoAlphaS(bool _doit) {
 do_AlphaS = _doit;
}

void SPXPDF::SetDoScale(bool _doit) {
 do_Scale = _doit;
}

void SPXPDF::SetDoTotError(bool _doit) {
 do_Total = _doit;
}

void SPXPDF::SetScales(std::vector<double> aRenScales,std::vector<double> aFacScales) {

 for (int i=0; i<aRenScales.size(); i++)
  RenScales.push_back(aRenScales[i]);
 for (int i=0; i<aFacScales.size(); i++)
  FacScales.push_back(aFacScales[i]);
 return;
};

void SPXPDF::SetAlphaSmemberNumDown(int _memberNum) {
 AlphaSmemberNumDown=_memberNum;
}
void SPXPDF::SetAlphaSmemberNumUp(int _memberNum) {
 AlphaSmemberNumUp=_memberNum;
}
void SPXPDF::SetAlphaSPDFSetNameDown(std::string _name) {
 AlphaSPDFSetNameDown= _name;
}
void SPXPDF::SetAlphaSPDFSetNameUp(std::string _name) {
 AlphaSPDFSetNameUp= _name;
}

void SPXPDF::CleanUpSPXPDF() {
  if (debug) std::cout<<cn<<" CleanUpSPXPDF: Starting to clean up..."<<std::endl;

 if (h_errors_PDF.size()>0) {
  for (int i=0; i<h_errors_PDF.size(); ++i) {
   delete h_errors_PDF.at(i);
  }
 }

 if (h_errors_AlphaS.size()>0) {
  for (int i=0; i<h_errors_AlphaS.size(); ++i) {
   delete h_errors_AlphaS.at(i);
  }
 }

 if (h_errors_Scale.size()>0) {
  for (int i=0; i<h_errors_Scale.size(); ++i) {
   delete h_errors_Scale.at(i);
  }
 }

 if (debug) std::cout<<cn<<"CleanUpSPXPDF: Finished clean up!"<<std::endl;
}


TH1D * SPXPDF::FillPdfHisto(){
 std::string mn = "FillPdfHisto: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (!hpdf) {
  throw SPXParseException(cn+mn+"WARNING: FillPdfHistos histo not found ");
 }
 //const int nstep=1000;
 const double xmin=hpdf->GetBinCenter(1);
 const int    nbin=hpdf->GetNbinsX();
 const double xmax=hpdf->GetBinWidth(nbin)/2.+hpdf->GetBinCenter(nbin);
 const double binw=hpdf->GetBinWidth(1);

 if (debug) std::cout<<cn<<"FillPdfHisto: xmin= "<<xmin<<" xmax= "<<xmax<<" binw= "<<binw<<std::endl;
 if (debug) std::cout<<cn<<"FillPdfHisto: Q2= "<<Q2<<std::endl;

 double xfl[13];
 for (int i=0; i<nbin; i++){
  double x=xmin+i*(xmax-xmin)/nbin;
  double Q=sqrt(Q2);
  getPDF(x, Q,xfl);

  int ibin=hpdf->FindBin(x);
  if (debug)
   std::cout<<cn<<"FillPdfHisto: ibin= "<<ibin<<" Q2= "<<Q2<<" x= "<<x<<" xfl["<<ifl<<"]= "<<xfl[6+ifl]<<std::endl;

  hpdf->SetBinContent(ibin,x,xfl[6+ifl]);
 }

 return hpdf;
};

void SPXPDF::PrintMap(BandMap_T &m) {
 std::string mn = "PrintMap: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (debug) std::cout<<cn<<mn<<" Map size= "<<Mapallbands.size()<<std::endl;

 for(BandMap_T::const_iterator it = m.begin(); it != m.end(); ++it) {
  std::cout <<cn<<mn<< " " << std::endl;
  std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " graph name " << it->second->GetName() << std::endl;
  it->second->Print("all");
 }
 return;
 if (debug) std::cout<<cn<<mn<<" ending "<<std::endl;
}

TGraphAsymmErrors * SPXPDF::GetBand(int i){ 
 if (i>Mapallbands.size()){
  std::ostringstream oss;
  oss << cn <<"GetBands:"<<"ERROR i= "<<i<<" is too large Number of bands= "<< Mapallbands.size();
  throw SPXParseException(oss.str());
 }
 int j=0.;
 for(BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  //if (debug) std::cout<<cn<<"GetBands: i= "<<i<<" j= "<<j<<" bandname= "<<it->second->GetName()<<std::endl;
  if (j==i) {
    return it->second;
  } else j++;
 }
 return 0;
};

std::string SPXPDF::GetBandType(int i){ 
 std::string mn = "GetBandType: ";
 //if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (i>Mapallbands.size()){
  std::ostringstream oss;
  oss << cn <<"GetBands:"<<"ERROR i= "<<i<<" is too large Number of bands= "<< Mapallbands.size();
  throw SPXParseException(oss.str());
 }
 int j=0.;
 for (BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  //if (debug) std::cout<<cn<<"GetBands: i= "<<i<<" j= "<<j<<" bandname= "<<it->second->GetName()<<std::endl;
  if (j==i) {
    return it->first;
  } else j++;
 }
 std::string empty="";
 return empty;
};

void SPXPDF::ApplyBandCorrection(TGraphAsymmErrors *gcorr, std::string corrLabel, bool includeinband){
 std::string mn = "ApplyBandCorrections: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (!gcorr) {
  std::ostringstream oss;
  oss << cn << mn << "Correction graph not found ";
  throw SPXParseException(oss.str());
 }

 if (debug) { 
  std::cout<<cn<<mn<<"Apply "<<corrLabel.c_str()<<" correction from graph: "<<gcorr->GetName()<<std::endl;
  gcorr->Print();
 }
 
 bool dividebybinwidth=true;

 //if (debug) std::cout<<cn<<mn<<" map size= "<<Mapallbands.size()<<std::endl;

 // store last band for grid correction uncertainty
 TGraphAsymmErrors *gband2=0;

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can only show grid corrections, if specified "<<std::endl;
  // throw SPXParseException(cn+mn+"Map is empty");
 } 

 for (BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  std::cout <<cn<<mn<< " " << std::endl;
  if (debug) std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " " << it->second->GetName() << std::endl;

  TGraphAsymmErrors *gband=it->second;
  if (!gband) {
   std::ostringstream oss;
   oss << cn << mn << "Graph in band not found !";
   throw SPXParseException(oss.str());
  }

  if (debug) {
   std::cout <<cn<<mn<< "Before correction Print gband "<< gband->GetName()<< std::endl;
   gband->Print("all");
  } 

  if (gband->GetN()!=gcorr->GetN()) {
   if (debug) std::cout <<cn<<mn<< "Bins not equal. Need to match bins " 
			<< gband->GetName()<<" nbins= "<<gband->GetN()<<" "
			<< gcorr->GetName()<<" nbins= "<<gcorr->GetN()<<" "<< std::endl;

   SPXGraphUtilities::MatchBinning(gband, gcorr, dividebybinwidth);
  }

  if (gband->GetN()!=gcorr->GetN()) {
   std::cout<<cn<<mn<<"Was not able to match graphs ! "<<std::endl;
   std::ostringstream oss;
   oss << cn << mn << "Graph "<<gband->GetName()<<" has different nbin= "<<gband->GetN()<<" than graph"<<gcorr->GetName()<<" nbin= "<<gcorr->GetN();
   throw SPXParseException(oss.str());
  }

  if (debug) {
   std::cout <<cn<<mn<< "After MatchBinning and before multiply: gcorr: " << gcorr->GetName()<< std::endl;
   gcorr->Print();
  }

  SPXGraphUtilities::Multiply(gband,gcorr,1);

  //if (debug) { 
  // std::cout<<cn<<mn<<"After multiply: gcorr: " << gcorr->GetName()<< std::endl;
  // gcorr->Print();
  //}

  TString newname=gband->GetName(); 
  if (debug) std::cout <<cn<<mn<<"Alter multiplication gband name: " << gband->GetName()<< std::endl;
  newname+="_"+corrLabel;
  gband->SetName(newname);
  gband2=(TGraphAsymmErrors*)gband->Clone();;
 }

 if (debug) {
  std::cout <<cn<<mn<<"Before correction Print hpdfdefault: "<< hpdfdefault->GetName()<< std::endl;
  hpdfdefault->Print("all");
 }

 // use only the correction, not the uncertainty in ratio
 TString hname=hpdfdefault->GetName();
 hname+="_"+corrLabel;
 TGraphAsymmErrors* gcorr2=(TGraphAsymmErrors*)gcorr->Clone(TString("g"+hname));;
 SPXGraphUtilities::ClearYErrors(gcorr2);
 TH1D*  hcorr=SPXGraphUtilities::GraphToHistogram(gcorr2);

 if (debug) {
  std::cout <<cn<<mn<<"Print hcorr: "<< hcorr->GetName()<< std::endl;
  hcorr->Print("all");
 }

  //if (hpdfdefault->GetNbinsX()==hcorr->GetNbinsX()){
  //if (debug)  std::cout <<cn<<mn<<" Multiply "<< hpdfdefault->GetName()<<" with "<<hcorr->GetName()<<std::endl;
  //hpdfdefault->Multiply(hcorr);
  //} else {
  // if (debug) {
  //  std::cout <<cn<<mn<<hpdfdefault->GetName()<<" has Nbin= "<< hpdfdefault->GetNbinsX()<<" "<<
  //                       hcorr->GetName()      <<" has Nbin= "<< hcorr->GetNbinsX()<<std::endl;
  //std::cout <<cn<<mn<<"recalculate "<< hpdfdefault->GetName()<<std::endl;
  //}
  //if (!gband2) {
  // std::ostringstream oss;
  // oss<<cn<<mn<<"ERROR Band not found May be turned off in steering ? --> can not recalculate Histograms ";
  // throw SPXParseException(oss.str());
  //}
  // need to recalculate hpdfdefault
 if (debug) std::cout <<cn<<mn<<"Call MatchandMultiply "<< hpdfdefault->GetName()<< std::endl;
 TH1D* htmp=SPXGraphUtilities::MatchandMultiply(hcorr, hpdfdefault,dividebybinwidth);
 hpdfdefault=htmp;
 //}
 hpdfdefault->SetName(hname);

 if (debug) {
  std::cout <<cn<<mn<< "After Correction Print hpdfdefault: "<< hpdfdefault->GetName()<< std::endl;
  hpdfdefault->Print("all");

  std::cout <<cn<<mn<< "After Correction Print hcorr: "<< hcorr->GetName()<< std::endl;
  hcorr->Print("all");
 }

 //
 // update also PDF, scale and alphas histograms
 //

 for (int ipdf=0; ipdf<h_errors_PDF.size(); ipdf++) {
  TString hname=h_errors_PDF.at(ipdf)->GetName();
  hname+="_"+corrLabel;

  TH1D* htmp=SPXGraphUtilities::MatchandMultiply(hcorr,h_errors_PDF.at(ipdf),false);
  if (debug) {
   std::cout <<cn<<mn<< "After MatchandMultiply Print hcorr: "<< hcorr->GetName()<< std::endl;
   hcorr->Print("all");
  }
  h_errors_PDF.at(ipdf)=htmp;
  h_errors_PDF.at(ipdf)->SetName(hname);

  if (debug) {
    std::cout <<cn<<mn<< "After MatchandMultiply Print h_errors_PDF.at("<<ipdf<<"): "<< h_errors_PDF.at(ipdf)->GetName()<< std::endl;
   h_errors_PDF.at(ipdf)->Print("all");
  }
 }

 for (int iscale=0; iscale<h_errors_Scale.size(); iscale++) {
  TString hname=h_errors_Scale.at(iscale)->GetName();
  hname+="_"+corrLabel;

  TH1D* htmp=SPXGraphUtilities::MatchandMultiply(hcorr,h_errors_Scale.at(iscale),false);
  h_errors_Scale.at(iscale)=htmp;
  h_errors_Scale.at(iscale)->SetName(hname);

 }
 // read alphas uncertainty components
 for (int ialphas=0; ialphas<h_errors_AlphaS.size(); ialphas++) {
  TString hname=h_errors_AlphaS.at(ialphas)->GetName();
  hname+="_"+corrLabel;

  TH1D* htmp=SPXGraphUtilities::MatchandMultiply(hcorr,h_errors_AlphaS.at(ialphas),false);
  h_errors_AlphaS.at(ialphas)=htmp;
  h_errors_AlphaS.at(ialphas)->SetName(hname);
 }

 // read beam uncertainty components
 for (int ibeam=0; ibeam<h_errors_BeamUncertainty.size(); ibeam++) {
  TString hname=h_errors_BeamUncertainty.at(ibeam)->GetName();
  hname+="_"+corrLabel;

  TH1D* htmp=SPXGraphUtilities::MatchandMultiply(hcorr,h_errors_BeamUncertainty.at(ibeam),false);
  h_errors_BeamUncertainty.at(ibeam)=htmp;
  h_errors_BeamUncertainty.at(ibeam)->SetName(hname);
 }



 if (Mapallbands.empty()) {
  gband2=SPXGraphUtilities::TH1TOTGraphAsymm(hpdfdefault);
 }

 if (!gband2) {
  std::ostringstream oss;
  oss << cn << mn << "ERROR gband2 not found " << std::endl;
  throw SPXParseException(oss.str());
 }

 gband2->SetName(TString("_corrections_")+TString(corrLabel));
 
 int nbin=gband2->GetN();
 if (debug) std::cout <<cn<<mn<< "nbin= "<<nbin<< std::endl;

 if (gband2->GetN()!=gcorr->GetN()) {
  std::ostringstream oss;
  oss << cn << mn << "Band n= "<<gband2->GetN()<<" and correction graph n= "<<gcorr->GetN()<<" do not have the same number of bins ";
  throw SPXParseException(oss.str());
 }

 // calculate uncertainty band with same y values as others
 // take relative uncertainties
 //
 for (int ibin=0; ibin<nbin; ibin++) {

  double ycorr=0., xcorr=0.; 
  gcorr->GetPoint(ibin,xcorr,ycorr);

  double exh=gcorr->GetErrorXhigh(ibin);
  double exl=gcorr->GetErrorXlow(ibin);
  double eyh=gcorr->GetErrorYhigh(ibin);
  double eyl=gcorr->GetErrorYlow(ibin);

  //double reyl=(ycorr-eyl)/ycorr;
  //double reyh=(ycorr+eyh)/ycorr;

  double reyl=eyl/ycorr;
  double reyh=eyh/ycorr;

  double x=0., y=0.;
  gband2->GetPoint(ibin,x,y);

  if (debug) std::cout<<cn<<mn<<"ibin= "<<ibin<<" y= "<<y<<" reyl= "<<reyl<<" reyh= "<<reyh
                      << " "<<(ycorr+eyh)/ycorr<<" "<<(ycorr-eyl)/ycorr<<std::endl;

  double eyl2 = reyl*y, eyh2 = reyh*y;
  //double eyl2 = (1-reyl*y, eyh2 = (1+reyh)*y;
  //double eyl2 = ycorr-eyl, eyh2 = eyh;

  gband2->SetPoint(ibin,x,y);
  gband2->SetPointError(ibin,exl,exh,eyl2,eyh2);

 }
 
 // insert graph to band
 if (debug) {
  std::cout <<cn<<mn<< "Put corrLabel "<<corrLabel.c_str()<<" gband2= "<<gband2->GetName()<<" to map !"<< std::endl;   
 }

 if (Mapallbands.count(corrLabel)==0) {
  if (includeinband) {
   if (debug) {
    std::cout <<cn<<mn<< "Correction band uncertainty as it goes to map["<<corrLabel<<"]: gband2= "<<gband2->GetName()<< std::endl;   
    gband2->Print("all");
   }
   Mapallbands[corrLabel]=gband2;
   if (debug) std::cout<<cn<<mn<< "Correction "<<gband2->GetName()<<" included in Map "<< std::endl;   
  } else {
   if (debug) std::cout<<cn<<mn<< "Correction "<<gband2->GetName()<<" Not requested to include in Map "<< std::endl;
  }
 } else {
  std::ostringstream oss;
  oss << cn << mn << "Band correction apply twice corrections= "<<corrLabel;
  throw SPXParseException(oss.str());
 }

 if (debug) {
  std::cout <<cn<<mn<< "Print new Map: Mapallbands"<< std::endl;
  PrintMap(Mapallbands);
 }

 //correctedgrid=true;

 return;

}

bool SPXPDF::BandsHaveDifferentProperties(){
 std::string mn = "BandsHaveDifferentProperties: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can only show grid corrections, if specified "<<std::endl;
  // throw SPXParseException(cn+mn+"Map is empty");
 } 

 // Look first, if properties of bands are different
 int old_fill_style=-999, old_fill_color=-999;
 int old_marker_style=-999, old_marker_color=-999;
 bool bandsdifferent=false;

 //debug=true;

 int iband=0;
 for(BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  if (debug) std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " " << it->second->GetName() << std::endl;

  iband++;
  TGraphAsymmErrors *gband=it->second;
  if (!gband) {
   std::ostringstream oss;
   oss << cn << mn << "Graph in band not found !";
   throw SPXParseException(oss.str());
  }

  if (iband==1) {
   old_fill_style=gband->GetFillStyle();
   old_fill_color=gband->GetFillColor();
   old_marker_style=gband->GetMarkerStyle();
   old_marker_color=gband->GetMarkerColor();
  } else {

   if (old_fill_style  !=gband->GetFillStyle())   bandsdifferent=true;
   if (old_fill_color  !=gband->GetFillColor())   bandsdifferent=true;
   if (old_marker_style!=gband->GetMarkerStyle()) bandsdifferent=true;
   if (old_marker_color!=gband->GetMarkerColor()) bandsdifferent=true;           
  }


  if (debug) {
    std::cout <<cn<<mn<< "old_fill_style= "   <<old_fill_style  <<" gband= "<<gband->GetFillStyle()  << std::endl;
    std::cout <<cn<<mn<< "old_fill_color= "   <<old_fill_color  <<" gband= "<<gband->GetFillColor()  << std::endl;
    std::cout <<cn<<mn<< "old_marker_style= " <<old_marker_style<<" gband= "<<gband->GetMarkerStyle()<< std::endl;
    std::cout <<cn<<mn<< "old_marker_color= " <<old_marker_color<<" gband= "<<gband->GetMarkerColor()<< std::endl;

    if (bandsdifferent) std::cout <<cn<<mn<<"iband= "<<iband<< " TRUE" << std::endl;
    else                std::cout <<cn<<mn<<"iband= "<<iband<< " FALSE" << std::endl;

  }
 }

 if (debug) {
  if (bandsdifferent) std::cout <<cn<<mn<< " TRUE" << std::endl;
  else                std::cout <<cn<<mn<< " FALSE" << std::endl;
 }
 return bandsdifferent;

};


bool SPXPDF::BandsHaveDifferentMarker(){
 std::string mn = "BandsHaveDifferentMarker: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can only show grid corrections, if specified "<<std::endl;
  // throw SPXParseException(cn+mn+"Map is empty");
 } 

 // Look first, if properties of bands are different
 int old_marker_style=-999, old_marker_color=-999;
 bool bandsdifferent=false;

 int iband=0;
 for(BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
 
  if (debug) std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " " << it->second->GetName() << std::endl;

  iband++;
  TGraphAsymmErrors *gband=it->second;
  if (!gband) {
   std::ostringstream oss;
   oss << cn << mn << "Graph in band not found !";
   throw SPXParseException(oss.str());
  }

  if (iband==1) {
   old_marker_style=gband->GetMarkerStyle();
   old_marker_color=gband->GetMarkerColor();
  } else {

   if (old_marker_style!=gband->GetMarkerStyle()) bandsdifferent=true;
   if (old_marker_color!=gband->GetMarkerColor()) bandsdifferent=true;           
  }
 }

 if (debug) {
  if (bandsdifferent) std::cout <<cn<<mn<< " TRUE" << std::endl;
  else               std::cout <<cn<<mn<< " FALSE" << std::endl;
 }
 return bandsdifferent;

};

bool SPXPDF::BandsHaveDifferentFillStyle(){
 std::string mn = "BandsHaveDifferentFillStyle: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can only show grid corrections, if specified "<<std::endl;
  // throw SPXParseException(cn+mn+"Map is empty");
 } 

 // Look first, if properties of bands are different
 int old_fill_style=-999, old_fill_color=-999;
 bool bandsdifferent=false;

 int iband=0;
 for(BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  if (debug) std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " " << it->second->GetName() << std::endl;

  iband++;
  TGraphAsymmErrors *gband=it->second;
  if (!gband) {
   std::ostringstream oss;
   oss << cn << mn << "Graph in band not found !";
   throw SPXParseException(oss.str());
  }

  if (iband==1) {
   old_fill_style=gband->GetFillStyle();
   old_fill_color=gband->GetFillColor();
  } else {

   if (old_fill_style  !=gband->GetFillStyle())   bandsdifferent=true;
   if (old_fill_color  !=gband->GetFillColor())   bandsdifferent=true;
  }
 }

 if (debug) {
  if (bandsdifferent) std::cout <<cn<<mn<< " TRUE" << std::endl;
  else               std::cout <<cn<<mn<< " FALSE" << std::endl;
 }
 return bandsdifferent;

};


bool SPXPDF::HasDetailedBands(){
 std::string mn = "HasDetailedBands: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can only show grid corrections, if specified "<<std::endl;
  // throw SPXParseException(cn+mn+"Map is empty");
 } 
 // 
 bool detailedband=false;

 for(BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  if (debug) std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " " << it->second->GetName() << std::endl;
  TGraphAsymmErrors *gband=it->second;
  if (!gband) {
   std::ostringstream oss;
   oss << cn << mn << "Graph in band not found !";
   throw SPXParseException(oss.str());
  }
  TString gname=gband->GetName();
  if (gname.Contains("_total_")) detailedband=true;
  if (gname.Contains("_scale_")) detailedband=true;
  if (gname.Contains("_pdf_"))   detailedband=true;
  if (gname.Contains("_alphas_"))detailedband=true;
 }

 if (debug) {
  if (detailedband)  std::cout <<cn<<mn<< " TRUE" << std::endl;
  else               std::cout <<cn<<mn<< " FALSE" << std::endl;
 }
 return detailedband;
};


bool SPXPDF::HasBandofType(std::string bandtype){
 std::string mn = "HasBandofType: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (Mapallbands.empty()) {
  std::cout<<cn<<mn<<"WARNING: Band map is empty; can only show grid corrections, if specified "<<std::endl;
 } 
 // 
 bool hasband=false;

 for (BandMap_T::const_iterator it = Mapallbands.begin(); it != Mapallbands.end(); ++it) {
  if (debug) std::cout <<cn<<mn<< "map["<<it->first<<"]="<< " " << it->second->GetName() << std::endl;
  TGraphAsymmErrors *gband=it->second;
  if (!gband) {
   std::ostringstream oss;
   oss << cn << mn << "Graph in band not found !";
   throw SPXParseException(oss.str());
  }
  TString gname=gband->GetName();
  if (gname.Contains(bandtype.c_str())) hasband=true;
 }

 if (debug) {
  if (hasband)  std::cout <<cn<<mn<< " TRUE" << std::endl;
  else          std::cout <<cn<<mn<< " FALSE" << std::endl;
 }
 return hasband;
};

TH1D* SPXPDF::GetIndividualPDFComponent(int ipdf){
 std::string mn = "GetIndividualPDFComponent: ";

 if (debug) std::cout<<cn<<mn<<"Return PDF id= "<<ipdf<<std::endl;

 TH1D *h1=0;
 
 if (h_errors_PDF.size()==0) {
   if (debug) std::cout<<cn<<mn<<"h_errors_PDF.size()==0 return 0 "<<std::endl;
   return h1;
  }

 if (ipdf>h_errors_PDF.size()) {
  std::cout<<cn<<mn<<"Something is wrong PDF id= "<<ipdf<<" but size= "<<h_errors_PDF.size()<<std::endl;
  return h1;
 }

 return h_errors_PDF.at(ipdf);

}

void SPXPDF::SetIndividualPDFComponent(int ipdf, TH1D *h){ 
 std::string mn = "GetIndividuallPDFComponent: ";
 if (ipdf>h_errors_PDF.size()) {
  std::ostringstream oss;
  oss<<cn<<mn<<" ipdf= "<<ipdf<<" > maximum= "<<h_errors_PDF.size();
  throw SPXParseException(oss.str());
 }

 if (!h) std::cout<<cn<<mn<<"WARNING histogram not found !"<<std::endl;

 h_errors_PDF.at(ipdf)=h; 

 return;
}

TH1D* SPXPDF::GetIndividualScaleVariation(int iscale){
 std::string mn = "GetIndividualScaleVariation: ";

 if (debug) std::cout<<cn<<mn<<"Return scale variation i= "<<iscale<<std::endl;

 TH1D *h1=0;
 
 if (h_errors_Scale.size()==0) {
   if (debug) std::cout<<cn<<mn<<"h_errors_Scale.size()==0 return 0 "<<std::endl;
   return h1;
  }

 if (iscale>h_errors_Scale.size()) {
  std::cout<<cn<<mn<<"Something is wrong scale i= "<<iscale<<" but size= "<<h_errors_Scale.size()<<std::endl;
  return h1;
 }

 return h_errors_Scale.at(iscale);

}

void SPXPDF::SetIndividualScaleVariation(int iscale, TH1D *h){ 
 std::string mn = "SetIndividualScaleVariation: ";
 if (iscale>h_errors_Scale.size()) {
  std::ostringstream oss;
  oss<<cn<<mn<< "iscale= "<<iscale<<" > maximum= "<<h_errors_Scale.size();
  throw SPXParseException(oss.str());
 }

 if (!h) std::cout<<cn<<mn<<"WARNING histogram not found !"<<std::endl;

 h_errors_Scale.at(iscale)=h; 
 return;
}

TH1D* SPXPDF::GetIndividualAlphaSVariation(int ialphas){
 std::string mn = "GetIndividualAlphaSVariation: ";

 if (debug) std::cout<<cn<<mn<<"Return AlphaS variation i= "<<ialphas<<std::endl;

 TH1D *h1=0;
 
 if (h_errors_AlphaS.size()==0) {
   if (debug) std::cout<<cn<<mn<<"h_errors_AlphaS.size()==0 return 0 "<<std::endl;
   return h1;
  }

 if (ialphas>h_errors_AlphaS.size()) {
  std::cout<<cn<<mn<<"Something is wrong AlphaS id= "<<ialphas<<" but size= "<<h_errors_AlphaS.size()<<std::endl;
  return h1;
 }

 return h_errors_AlphaS.at(ialphas);

}

void SPXPDF::SetIndividualAlphaSVariation(int ialphas, TH1D *h){ 
 std::string mn = "SetIndividualAlphaSVariation: ";
 if (ialphas>h_errors_AlphaS.size()) {
  std::ostringstream oss;
  oss<<cn<<mn<< "ialphas= "<<ialphas<<" > maximum= "<<h_errors_AlphaS.size();
  throw SPXParseException(oss.str());
 }

 if (!h) std::cout<<cn<<mn<<"WARNING histogram not found !"<<std::endl;

 h_errors_AlphaS.at(ialphas)=h; 

 return;
}


void SPXPDF::SetLHAPDFPDFset(std::string pdfname, int id){ 
 std::string mn = "SetLHAPDFPDFset: ";
 
#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
  if (debug) {
   std::cout<<cn<<mn<<"  "<<std::endl;
   std::cout<<cn<<mn<<"Running on LHAPDF6: "<<std::endl;
   std::cout<<cn<<mn<<"Calling mkPDF  "<<std::endl;
   std::cout<<cn<<mn<<"Set PDF "<<pdfname.c_str()<<" id= "<<id<<std::endl;
  }

  mypdf=LHAPDF::mkPDF(pdfname.c_str(),id);
  if (!mypdf) std::cout<<"PDF not found name= "<<pdfname.c_str()<<" member= "<<id<<std::endl;
  //else if (debug) mypdf->print();
  const LHAPDF::PDFSet set(pdfname.c_str());
#else
  if (debug) {
   std::cout<<cn<<mn<<"  "<<std::endl;
   std::cout<<cn<<mn<<"Running on LHAPDF5: "<<std::endl;
   std::cout<<cn<<mn<<"Calling initPDFSet  "<<std::endl;
  }
  LHAPDF::initPDFSet(((std::string) (pdfSetPath+"/"+pdfname+".LHgrid")).c_str(), id);
#endif

 return; 
}


