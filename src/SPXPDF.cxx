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
#include "SPXUtilities.h"

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

void getPDF(const double& x, const double& Q, double* xf) {
		evolvepdf_(&x, &Q, xf);
}

string SPXPDF::GetEnv( const string & var ) {
		const char* res= getenv( var.c_str() );

		std::string s = res!=NULL? res:"";
		if (debug) cout<<"s: "<<s<<endl;
		return s;
}

/*
//currently declared in local directories LHAPDF.h file
double alphasPDF(const double& Q) {
		return alphaspdf_(Q);
}
*/

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

	//ReadSteering(steeringFileName);
	ReadPDFSteeringFile(psf);

	hpdf=h1;
	if (!hpdf) cout<<" SPXPDF histograme not found "<<endl;

	ifl=iflpdf;

	Q2=Q2value;

	cout<<" SPXPDF: Get PDF for Q2= "<<Q2<<" ifl= "<<ifl<<endl;
	if (debug) {
		//cout<<" SPXPDF: Get PDF for Q2= "<<Q2<<" ifl= "<<ifl<<endl;
	 //cout<<" SPXPDF Histogram: "<<endl;
	 //hpdf->Print("all");
	}

	Initialize();
}

//SPXPDF::SPXPDF(string _gridName, string _steeringFile, bool do_PDFBand, bool do_AlphaS)
SPXPDF::SPXPDF(SPXPDFSteeringFile *psf, const std::string &_gridName)
{
	if (debug) cout<<" SPXPDF::SPXPDF: Start overloaded constructor"<<endl;

	SetVariablesDefault();   
	gridName=_gridName;

	//ReadSteering(steeringFileName);
	ReadPDFSteeringFile(psf);

	Initialize();

	if (debug)cout<<" SPXPDF::SPXPDF: end constructor"<<std::endl;
}

//This function replaces the ::ReadSteering(), since the PDF Steering file has already been parsed by the
//Top-Level Steering File parser, and has been passed directly to the SPXPDF constructor, and thus this function
void SPXPDF::ReadPDFSteeringFile(SPXPDFSteeringFile *psf) {

	steeringFileName = psf->GetFilename();

 	if (debug) cout<<" SPXPDF::ReadSteering: processing pre-parsed steering file named: "<< steeringFileName << std::endl;

 	pdfSetPath = defaultPDFSetPath;
 	string pdfSetDefaultPath = GetEnv("LHAPATH");

	 if (pdfSetDefaultPath.size()>0) {
		if (pdfSetDefaultPath.find_last_of("/") == pdfSetDefaultPath.size()-1) {
			pdfSetDefaultPath = pdfSetDefaultPath.substr(0,pdfSetDefaultPath.size()-1); //remove trailing slashes if there are any
		}

		pdfSetPath=pdfSetDefaultPath;
		cout<<" SPXPDF: LHAPATH environment variable found, using path: "<<pdfSetPath<<endl;

	 } else {
		cout<<" SPXPDF: LHAPATH environment variable not set, using default: "<<pdfSetPath<<endl;
	 }

	 SPXPDF::debug = psf->GetDebug();

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
	 nLoops = psf->GetNumberOfLoops();
	 fillStyleCode = psf->GetFillStyle();
	 fillColorCode = psf->GetFillColor();
	 markerStyle = psf->GetMarkerStyle();
	 PDFBandType = psf->GetBandType().ToString();
	 includeEIG = psf->GetIncludeEig();
	 includeQUAD = psf->GetIncludeQuad();
	 includeMAX = psf->GetIncludeMax();

	 PDFErrorType = psf->GetErrorType().ToString();
	 { //Parse Error Type...
		 if(psf->GetErrorType().IsAlphaS()) {
		 	do_AlphaS = true;
		 	do_PDFBand = false;
		 } else if(psf->GetErrorType().IsPDFBand()) {
		 	do_AlphaS = false;
		 	do_PDFBand = true;
		 }
	 }

	 PDFErrorSize = psf->GetErrorSize().ToString();

	 if(!psf->GetPDFSetPath().empty()) {
	 	pdfSetPath = psf->GetPDFSetPath();
	 }

	 AlphaSmemberNumDown = psf->GetAlphaSErrorNumberDown();
	 AlphaSmemberNumUp = psf->GetAlphaSErrorNumberUp();
	 AlphaSPDFSetNameDown = psf->GetAlphaSPDFNameDown();
	 AlphaSPDFSetNameUp = psf->GetAlphaSPDFNameUp();
	 AlphaSPDFSetHistNameDown = psf->GetAlphaSPDFHistogramNameDown();
	 AlphaSPDFSetHistNameUp = psf->GetAlphaSPDFHistogramNameUp();
}

// perform any additional work after constructors but before the object is available for use
void SPXPDF::Initialize()
{
 if (debug) cout<<" SPXPDF::Initialize: Performing Initialization"<<endl;

 //if(steeringFilePath.size()>0) SetSteeringFileNameAndDir(steeringFilePath);
 //else cout<<" SPXPDF::Initialize: ERROR: No steering file was provided!"<<std::endl;

 calc_desc = "SPXPDF-theory_errors";
 if (do_PDFBand)
	calc_desc+="_PDFBand";
 if (do_AlphaS)
	calc_desc+="_AlphaS";
 if ( !do_PDFBand && !do_AlphaS) {
	cout<<" SPXPDF::Initialize: ERORR: All theory uncertainties disabled. Possible steering file error? Check file: "<<steeringFileName<<endl;
	 exit(0); //TEST
 }

 if (debug) cout<<" SPXPDF::Initialize: gridName= "<<gridName.c_str()<<endl; 

 if (gridName.size()==0) {
	//cout<<" SPXPDF no applgrid found gridname= "<<TString(gridName).Data()<<endl;
	applgridok=false;
 } else {
	my_grid = new appl::grid(gridName.c_str());

	if (!my_grid) {
	 if (debug) cout<<" SPXPDF no applgrid found gridname= "<<TString(gridName).Data()<<endl;
	 applgridok=false;
	} else {
		if (debug) cout<<" SPXPDF applgrid ok gridname= "<<TString(gridName).Data()<<endl;
	 applgridok=true;
	}
 }

 if (applgridok) my_grid->trim();

 //nLoops    = 1; //nLoops now set by constructor
 static const int nFlavours = 5;
 h_errors_PDFBand.clear();
 h_errors_AlphaS.clear();

 TH1D* temp_hist=0;

 if (debug) 
	cout<<" SPXPDF::Initialize: Fill PDF errors for PDFType: "
			<<PDFtype<<", PDFName: "<<PDFname<<", from PDFPath: "<<pdfSetPath<<endl;

 default_pdf_set_name = (std::string) (pdfSetPath+"/"+PDFname+".LHgrid");

 cout<<" SPXPDF::Initialize: init PDF set called: "<<default_pdf_set_name.c_str()<<endl;

/*
		h_qqbar = (TH1D*)my_grid->convolute_subproc(6, getPDF, alphasPDF, nLoops);      ///// maybe also subprocess 5?
		h_qqbar->SetName((TString) ("h_qqbar_" + calc_desc));

		TH1D* h_qqbar2 = (TH1D*) my_grid->convolute_subproc(5, getPDF, alphasPDF, nLoops);
		h_qqbar2->SetName((TString) ("h_qqbar_" + calc_desc));
		h_qqbar->Add(h_qqbar2);
		h_qqbar->SetLineColor(fillColorCode);
		h_qqbar->SetMarkerColor(fillColorCode);

		h_gg = (TH1D*) my_grid->convolute_subproc(0, getPDF, alphasPDF, nLoops);
		h_gg->SetName((TString) ("h_gg_" + calc_desc));
		h_gg->SetLineColor(fillColorCode);
		h_gg->SetMarkerColor(fillColorCode);

		h_tot = (TH1D*) my_grid->convolute(getPDF, alphasPDF, nLoops);
		h_tot->SetName((TString) ("h_tot_" + calc_desc));
		h_tot->SetLineColor(fillColorCode);
		h_tot->SetMarkerColor(fillColorCode);

		h_gg_frac = (TH1D*) h_gg->Clone((TString) ("h_gg_frac_" + calc_desc));
		h_gg_frac->Divide(h_tot);

		h_qqbar_frac = (TH1D*) h_qqbar->Clone((TString) ("h_qqbar_frac_" + calc_desc));
		h_qqbar_frac->Divide(h_tot);
*/

 cout<<" SPXPDF::Initialize: checking for do_AlphaS.."<<endl;

 if (do_AlphaS) {
	 cout<<" SPXPDF::Initalize: do_AlphaS is ON"<<endl;
	 
	//check for necessary names before continuing
	if(AlphaSmemberNumDown==DEFAULT) {
	 cout<<" SPXPDF::Initialize: ERROR: 'AlphaSmemberNumDown' not provided in steer file: "<<steeringFileName<<endl;
	 exit(0);
	}

	if (AlphaSmemberNumUp==DEFAULT) {
	 cout<<" SPXPDF::Initialize: ERROR: 'AlphaSmemberNumUp' not provided in steer file: "<<steeringFileName<<endl;
	 exit(0);
	}

	if (AlphaSPDFSetNameUp.compare("")==0) {
	 cout<<" SPXPDF::Initialize: ERROR: 'AlphaSPDFSetNameUp' not provided in steer file: "<<steeringFileName<<endl;
	 exit(0);
	}

	if (AlphaSPDFSetNameDown.compare("")==0) {
	 cout<<" SPXPDF::Initialize: ERROR: 'AlphaSPDFSetNameDown' not provided in steer file: "<<steeringFileName<<endl;
	 exit(0);
	}

	if (AlphaSPDFSetHistNameUp.compare("")==0) {
	 cout<<" SPXPDF::Initialize: ERROR: 'AlphaSPDFSetHistNameUp' not provided in steer file: "<<steeringFileName<<endl;
	 exit(0);
	}

	if(AlphaSPDFSetHistNameDown.compare("")==0) {
	 cout<<" SPXPDF::Initialize: ERROR: 'AlphaSPDFSetHistNameDown' not provided in steer file: "<<steeringFileName<<endl;
	 exit(0);
	}

	cout<<" SPXPDF::Initialize looking up: "<<default_pdf_set_name<<endl;

	//LHAPDF::initPDFSet(default_pdf_set_name.c_str(), 0);
	LHAPDF::initPDFSet(default_pdf_set_name.c_str(), defaultpdfid);

	cout<<" SPXPDF::Initialize finshed looking up: "<<default_pdf_set_name<<endl;

	if (applgridok) {
	 temp_hist= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops);
	 temp_hist->SetName((TString) ("h_xsec_default"));
	} else {
	 if (debug) cout<<" SPXPDF::Initialize histo from PDF not grid ! "<<endl;
	 temp_hist=this->FillPdfHisto();
	 cout<<"  SPXPDF::Initialize  hpdf= "<<hpdf->GetName()<<endl;
	 //temp_hist->SetName((TString) ("h_pdf_default"));
	 temp_hist->SetName(hpdf->GetName());
	}

	if (!temp_hist) cout<<" SPXPDF::Initialize temp_hist not found ! "<<endl;

	if (debug) temp_hist->Print("all");

	h_errors_AlphaS.push_back(temp_hist);

	if (debug) cout<<" SPXPDF::Initialize Setting up alphas down "<<endl;

	LHAPDF::initPDFSet(((std::string) (pdfSetPath+"/"+AlphaSPDFSetNameDown+".LHgrid")).c_str(), AlphaSmemberNumDown);  //// alphaS down

	if (applgridok) {
	 temp_hist= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops);
	 temp_hist->SetName((TString) ("h_xsec_"+AlphaSPDFSetHistNameDown));
	} else {
	 if (debug) cout<<" SPXPDF::Initialize histo from PDF not grid ! "<<endl;
	 temp_hist=this->FillPdfHisto();
	 temp_hist->SetName((TString) ("h_pdf_"+AlphaSPDFSetHistNameDown));
	}

	if (!temp_hist) cout<<" SPXPDF::Initialize temp_hist not found ! "<<endl;

	h_errors_AlphaS.push_back(temp_hist);

	if (debug) cout<<" SPXPDF::Initialize Setting up alphas up "<<endl;

	if (PDFtype.compare("CT10")==0) {
	 LHAPDF::initPDF(7);    /// alphaS up -> should go to steering
	} else {
	 LHAPDF::initPDFSet(((std::string) (pdfSetPath+"/"+AlphaSPDFSetNameUp+".LHgrid")).c_str(), AlphaSmemberNumUp);  //// alphaS up
	}

	if (applgridok) {
	 temp_hist= (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops);
	 temp_hist->SetName((TString) ("h_xsec_"+AlphaSPDFSetHistNameUp));
	} else {
	 if (debug) cout<<" SPXPDF::Initialize histo from PDF not grid ! "<<endl;
	 temp_hist=this->FillPdfHisto();
	 temp_hist->SetName((TString) ("h_pdf_"+AlphaSPDFSetHistNameDown));
	}

	h_errors_AlphaS.push_back(temp_hist);
	h_AlphaS_results=TH1TOTGraphAsymm(temp_hist);

 }

 cout<<" SPXPDF::Initalize: Checking for do_PDFBand..."<<endl;

 // Calculate PDF errors using standard PDF error band
 if (do_PDFBand) {
	if (debug) cout<<" SPXPDF::Initialize: Calc PDF errors"<<endl;
	cout<<" SPXPDF::Initialize: Calc PDF errors for: "<<default_pdf_set_name
			<<" w/ defaultpdfid: "<<defaultpdfid<<endl; //TEST

	// LHAPDF::initPDFSet(default_pdf_set_name.c_str(), 0);
	LHAPDF::initPDFSet(default_pdf_set_name.c_str(), defaultpdfid);

	cout<<" SPXPDF::Initialize: initPDFSet finished for PDFBand"<<endl; //TEST

	// update this to put into steering
	TH1D *hdefault=0;
	for (int pdferri = 0; pdferri < n_PDFMembers; pdferri++) {
	 if (debug) cout<<" SPXPDF::Initialize: pdferri: "<<pdferri<<" of "<<n_PDFMembers
									<<" pdftype="<<PDFtype.c_str()<<endl;                 
 /*
	 if (PDFtype.compare("HERAPDF15NLO")==0){ 
		if (debug) cout<<" SPXPDF::Initialize: initPDFSet HERAPDF pdferri= "<<pdferri<<endl;
		if (pdferri <= 20 ) {
		 LHAPDF::initPDF(pdferri);
		} else if( pdferri == 21 ) { 
		 LHAPDF::initPDFSet(pdfSetPath+"/HERAPDF15NLO_VAR.LHgrid", 0);
		} else if( pdferri > 21 ) {
		 LHAPDF::initPDF(pdferri - 21);
		}
*/
	 if (TString(PDFtype).Contains("HERA") || TString(PDFtype).Contains("ATLAS") ){ 
		if (!includeEIG&&!includeQUAD&&!includeMAX) cout<<" SPXPDF::Initialize: no error band included !! "<<endl;
		if (debug) cout<<" SPXPDF::Initialize: initPDFSet "<<PDFname.c_str()<<" pdferri= "<<pdferri<<endl;
		if (defaultpdfidvar<0) cout<<" SPXPDF::Initialize: No default PDF id found in steering. Check steering for missing 'defaultpdfidvar'. pdferri= "<<pdferri<<endl;
		if (pdferri <= lasteig ) {
		 LHAPDF::initPDF(pdferri);
		} else if( pdferri == lasteig+1 ) { 
			
			//account for PDF set "ATLAS.txt" and "ATLAS3jet" with no error bands?
			if (debug) std::cout<<" SPXPDF::Initialize: A 'PDFnamevar' was "
				<<(PDFnamevar.empty()? "":"NOT")
				<<" found."<<std::endl;

			//Band-aid for accounting for no error bands - This needs to be handled better
			if(PDFnamevar.empty() == false) {
	TString pdfname=TString(pdfSetPath)+"/"+PDFnamevar+".LHgrid";
	LHAPDF::initPDFSet(pdfname.Data(), defaultpdfidvar);
			}
			
		} else if ( pdferri > lasteig+1 ) {
		 if (debug) cout<<" SPXPDF::Initialize: initPDF set= "<<pdferri-lasteig<<endl;
		 LHAPDF::initPDF(pdferri - lasteig);
		} 
	 } else { 
		if (debug) cout<<" SPXPDF::Initialize: init normal PDF set= "<<pdferri<<endl;
		LHAPDF::initPDF(pdferri);
	 }

	 TH1D* temp_hist = 0;
	 if (applgridok) {
		temp_hist = (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops);
	 } else {
		if (debug) cout<<" SPXPDF::Initialize histo from PDF not grid ! "<<endl;
		TH1D *tmp=this->FillPdfHisto();
		if (!tmp) cout<<" SPXPDF::Initialize  tmp histogram not found "<<endl;
		temp_hist=(TH1D*)tmp->Clone(tmp->GetName());
		temp_hist->SetName(tmp->GetName());
	 }

	 if (!temp_hist) cout<<" SPXPDF::Initialize  temp_hist histogram not found "<<endl;
	 if (pdferri==defaultpdfid) hdefault=(TH1D*)temp_hist->Clone(temp_hist->GetName());

	 if (debug) {
		if (pdferri==defaultpdfid)  {
		 cout<<" SPXPDF::Initialize print defaultpdf histo: "<<endl;
		 temp_hist->Print("all");
		} else {
		 cout<<" SPXPDF::Initialize ratio pdferri=  "<<pdferri<<" to default= "<<defaultpdfid<<endl;
		 TString hname=TString("hratio_")+temp_hist->GetName()+TString("pdf_");
		 hname+=pdferri;
		 hname+="_Divided_by_pdf_";
		 hname+=defaultpdfid;
		 cout<<" SPXPDF::Initialize hname=  "<<hname<<endl;
		 if (!hdefault) cout<<" SPXPDF::Initialize  hdefault histogram not found "<<endl;
		 TH1D * hratio=(TH1D*) temp_hist->Clone(hname); 
		 hratio->Divide(hdefault);
		 hratio->Print("all");
		}
	 }

	 //TString this_pdf_err_code = "";
	 //this_pdf_err_code += pdferri;

	 h_errors_PDFBand.push_back(temp_hist);

	 if (defaultpdfid==pdferri) {
		h_PDFBand_results=TH1TOTGraphAsymm(temp_hist);
		h_PDFBand_results->SetName(temp_hist->GetName());

		h_PDFBand_results->SetFillStyle(fillStyleCode);
		h_PDFBand_results->SetMarkerColor(fillColorCode);
		h_PDFBand_results->SetLineColor(fillColorCode);
		h_PDFBand_results->SetFillColor(fillColorCode);
	 }
	}   /// pdf errors loop
 }  /// do_PDFBand

 if (debug) cout<<" SPXPDF::Initialize: End of PDF errors loop"<<endl;
 cout<<" SPXPDF::Initialize: End of PDF errors loop"<<endl; //TEST

 if (debug) cout<<" SPXPDF now calling CalcSystErrors "<<endl;

 this->CalcSystErrors(); 

 if (debug) {
	if (do_AlphaS) {
	 cout<<" SPXPDF h_AlphaS_results "<<endl;
	 h_AlphaS_results->Print("all");
	}
	if (do_PDFBand) {
	 cout<<" SPXPDF::h_PDFBand_results: "<<endl;
	 h_PDFBand_results->Print("all");
	 cout<<" SPXPDF:: default= "<<defaultpdfid<<endl;
	 hpdfdefault->Print("all");
	 /*
	 if (debug) {
		cout<<" SPXPDF::Initialize After syst uncertainty calculation ratio to default "<<endl;
		TGraphAsymmErrors *gdefault=TH1TOTGraphAsymm(temp_hist);
		TGraphAsymmErrors *gratio= MyTGraphErrorsDivide(h_PDFBand_results, gdefault);
		gratio->Print("all");
	 }
	 */
	}
 }
}

TGraphAsymmErrors *SPXPDF::GetScaleDependence(double renscale, double facscale, int pdfset=0){
	
	if (debug)
	 cout<<" SPXPDF::GetScaleDependence renscale= "<<renscale<<" facscale= "<<facscale<<" PDF= "
			 <<default_pdf_set_name.c_str()<<endl;

	LHAPDF::initPDFSet(default_pdf_set_name.c_str(), pdfset);

	if (debug) cout<<" SPXPDF::GetScaleDependence  now get histo: "<<endl;
	
	TH1D* temp_hist = 0;

	 if (applgridok) {
		temp_hist = (TH1D*) my_grid->convolute( getPDF, alphasPDF, nLoops,renscale,facscale);
	 } else {
		cout<<" SPXPDF::GetScaleDependence histo from templates not found ! "<<endl;
		//temp_hist->SetName((TString) ("h_pdf_default"));
	 }

	if (!temp_hist) cout<<" SPXPDF::GetScaleDependence  temp_hist not found "<<endl;

	TGraphAsymmErrors *gtmp=TH1TOTGraphAsymm(temp_hist);
	char rs[100];
	sprintf(rs,"_renscale_%3.1f_facscale_%3.1f",renscale,facscale);
	TString gname=TString(default_pdf_set_name)+TString(rs);
	gname.ReplaceAll("PDFsets/","");
	gtmp->SetName(gname);
	if (debug) {
	 cout<<" SPXPDF::GetScaleDependence got: "<<gtmp->GetName()<<endl;
	 gtmp->Print("all");
	}

	return gtmp;
}

void SPXPDF::CalcSystErrors()
{
 if (debug)
	cout<<" SPXPDF::CalcSystErrors: Start syst error calc for: "<<PDFtype
			<<"\n\tPDFBand: "<<do_PDFBand
			<<" do_AlphaS: "<<do_AlphaS<<endl;

	if (do_PDFBand) CalcPDFBandErrors();
	if (do_AlphaS ) CalcAlphaSErrors();

	if (debug) cout<<" SPXPDF::CalcSystErrors: End syst error calc for: "<<PDFtype<<endl;
}


void SPXPDF::CalcPDFBandErrors()
{
 // needs some more checking for uncertainty bands
 // calculate uncertainty bands
 //
 // to be done: write generic methods
 // Hessian: symetric, asymetric, RMS etc
 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: Start calc of PDFBandErrors for: "<<PDFtype<<endl;
 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: defaultpdf= "<<defaultpdfid<<endl;

 TString defname=PDFtype+"_default_set= ";
 defname+=defaultpdfid; 
 hpdfdefault=(TH1D*)h_errors_PDFBand.at(defaultpdfid)->Clone(defname);

 if (debug) {
	cout<<" SPXPDF::CalcPDFBandErrors: defaultpdf: "<<endl;
	hpdfdefault->Print("all");
 }

 for (int bi = 1; bi <= h_errors_PDFBand.at(0)->GetNbinsX(); bi++) { // loop over bins
	if (debug) cout<<" SPXPDF::CalcPDFBandErrors: bin= "<<bi<<endl;

	double this_err_up         = 0.;
	double this_err_down       = 0.;
	double central_val         = 0.;    // needed for MSTW2008nlo and HERAPDF
	double average             = 0.;    // needed for NNPDF
	double extreme_pos_diff    = 0.;    // needed for HERAPDF
	double extreme_neg_diff    = 0.;    // needed for HERAPDF
	double diff_central        = 0.;    // needed for HERAPDF
	double mod_val             = 0.;    // needed for MSTW2008nlo
 
	if (TString(PDFtype).Contains("NNPDF") ) {
	 for (int pdferri = 0; pdferri < (int) h_errors_PDFBand.size(); pdferri++) {
		average += h_errors_PDFBand.at(pdferri)->GetBinContent(bi);
	 }
	 average /= h_errors_PDFBand.size()-1;
	 hpdfdefault->SetBinContent(bi,average);

	 for (int pdferri = 1; pdferri < (int) h_errors_PDFBand.size(); pdferri++)  {
		this_err_up += pow(h_errors_PDFBand.at(pdferri)->GetBinContent(bi)-average, 2.);
	 }
	 this_err_up = TMath::Sqrt(this_err_up / (h_errors_PDFBand.size()-1));
	 // error with respect to average
	 this_err_down = this_err_up;
	 if (PDFErrorSize.compare("90Percent")==0 ) {
		this_err_up *= 1.645;
		this_err_down *= 1.645;
	 }
	} else if (PDFtype.compare("CT10")==0 ) {
	 for (int pdferri = 1; pdferri < (int) h_errors_PDFBand.size()-1; pdferri += 2) {
		this_err_up += pow( h_errors_PDFBand.at(pdferri)->GetBinContent(bi) - h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi), 2.);
	 }

	 // here errors are symmetrized
	 this_err_up = 0.5*TMath::Sqrt(this_err_up);
	 if (debug)
		cout<<" SPXPDF::CalcPDFBandErrors: CT10  this_err_up= "<<this_err_up<<" PDFErrorSize= "<<PDFErrorSize.c_str()<<endl;
	 if (PDFErrorSize.compare("OneSigma")==0) {
		if (debug) cout<<" SPXPDF::CalcPDFBandErrors: put to one sigma "<<endl;
		this_err_up /= 1.645;
	 }
	 this_err_down = this_err_up;

	} else if (PDFtype.compare("CTEQ61")==0 ) {
	 for (int pdferri = 1; pdferri < (int) h_errors_PDFBand.size()-1; pdferri += 2) {
		this_err_up += pow( h_errors_PDFBand.at(pdferri)->GetBinContent(bi) - h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi), 2.);
		
		if (debug) cout<<" SPXPDF::CalcPDFBandErrors: CT10 pdferri= "<<pdferri
									 <<"  i= "<<h_errors_PDFBand.at(pdferri)->GetBinContent(bi)
									 <<" i+1= "<<h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi)
									 <<" this_err_up= "<<this_err_up<<endl;
	 
	 }
	 // here errors are symmetrized
	 this_err_up = 0.5*TMath::Sqrt(this_err_up);
	 if (PDFErrorSize.compare("OneSigma")==0) this_err_up /= 1.645;
	 this_err_down = this_err_up;

	} else if (PDFtype.compare("MSTW2008nlo")==0 ) {
	 central_val = h_errors_PDFBand.at(defaultpdfid)->GetBinContent(bi);
	 for (int pdferri = 1; pdferri < (int) h_errors_PDFBand.size(); pdferri ++) {
		mod_val = h_errors_PDFBand.at(pdferri)->GetBinContent(bi);
		if (mod_val > central_val ) this_err_up += pow(mod_val-central_val, 2.);
		else this_err_down += pow(central_val - mod_val, 2.);
	 }
	 this_err_down = TMath::Sqrt(this_err_down);
	 this_err_up   = TMath::Sqrt(this_err_up);
	 // needs checking assumes that MRST comes 90%
	 if (PDFErrorSize.compare("OneSigma")==0 ) {
		this_err_up  /= 1.645;
		this_err_down/= 1.645;
	 }

	} else if (TString(PDFtype).Contains("HERA") || TString(PDFtype).Contains("ATLAS")) {

	 if (debug) {
		cout<<" SPXPDF::CalcPDFBandErrors: HERA/ATLAS type PDF "<<endl;
		cout<<" SPXPDF::CalcPDFBandErrors: PDFname= "<<PDFname.c_str()<<" PDFnamevar= "<<PDFnamevar.c_str()<<endl;
		cout<<" SPXPDF::CalcPDFBandErrors: firsteig= "<<firsteig<<" lasteig= "<<lasteig<<endl;
		cout<<" SPXPDF::CalcPDFBandErrors: firstquadvar= "<<firstquadvar<<" lastquadvar= "<<lastquadvar<<endl;
		cout<<" SPXPDF::CalcPDFBandErrors: firstmaxvar= "<<firstmaxvar<<" lastmaxvar= "<<lastmaxvar<<endl;

		cout<<" SPXPDF::CalcPDFBandErrors: defaultpdfid= "<<defaultpdfid<<" defaultpdfidvar= "<<defaultpdfidvar<<endl;
	 }

	 central_val = h_errors_PDFBand.at(defaultpdfid)->GetBinContent(bi);

	 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: default sample id= "<<defaultpdfid<<" "<<" central_val= "<<central_val<<endl;
	 //for (int pdferri = 1; pdferri < (int) h_errors_PDFBand.size(); pdferri += 2) {   //// experimental errors
	 if (includeEIG) {
		for (int pdferri = firsteig; pdferri <=lasteig; pdferri += 2) {   //// experimental errors
		 this_err_up += pow( 0.5*(h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi) 
														- h_errors_PDFBand.at(pdferri)  ->GetBinContent(bi)), 2.);

		 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: "<<PDFtype.c_str()
										<<" pdferri= "<<pdferri<<" , "<<pdferri+1<<" EIG "
										<<" this_err_up= "<<this_err_up
										<<" diff/nom= "<<(h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi)-h_errors_PDFBand.at(pdferri)  ->GetBinContent(bi))/central_val
										<<" (var+1)/nom= "<<h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi)/central_val
										<<" (var)/nom= "  <<h_errors_PDFBand.at(pdferri)  ->GetBinContent(bi)/central_val
										<<endl;
		}
	 }

	 this_err_down = this_err_up;

	 int firstvar=lasteig+1; // uncertainties start at last eigenvector previous set + default of variation samle

	 central_val = h_errors_PDFBand.at(firstvar+defaultpdfidvar)->GetBinContent(bi);
	 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: variation sample id= "<<firstvar+defaultpdfidvar<<" "<<" central_val= "<<central_val<<endl;
	 if (includeQUAD) {
		for (int pdferri = firstvar+firstquadvar; pdferri < firstvar+lastquadvar; pdferri++) {// parameterisation errors
		 if (h_errors_PDFBand.at(pdferri)->GetBinContent(bi) > central_val ) {
			this_err_up  += pow( h_errors_PDFBand.at(pdferri)->GetBinContent(bi) - central_val, 2.);
			this_err_down+= 0.;
		 } else  {
			this_err_up  += 0.;
			this_err_down+= pow( central_val - h_errors_PDFBand.at(pdferri)->GetBinContent(bi), 2.);
		 }
		 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: "<<PDFtype.c_str()
										<<" pdferri= "<<pdferri<<" , "<<pdferri+1<<" parameterisation "
										<<" this_err_up= "<<this_err_up
										<<" this_err_down= "<<this_err_down
										<<" (var)/nom= "  <<h_errors_PDFBand.at(pdferri)  ->GetBinContent(bi)/central_val
										<<endl;
		}
	 }

	 if (includeMAX) {
		for (int pdferri = firstvar+firstmaxvar; pdferri < firstvar+lastmaxvar; pdferri++) { // model errors
		 diff_central = h_errors_PDFBand.at(pdferri)->GetBinContent(bi) - central_val;
		 //if (debug) cout<<" SPXPDF::CalcPDFBandErrors: diff_central= "<<diff_central
		 //               <<" var/nom= "<<h_errors_PDFBand.at(pdferri)->GetBinContent(bi)/central_val
		 //               <<endl;
		 if (diff_central > 0 && diff_central > extreme_pos_diff ) extreme_pos_diff = diff_central;
		 if (diff_central < 0 && diff_central < extreme_neg_diff ) extreme_neg_diff = diff_central;

		 if (debug) cout<<" SPXPDF::CalcPDFBandErrors: "<<PDFtype.c_str()<<" pdferri= "
										<<pdferri<<" , "<<pdferri+1<<" model "
										<<" extreme_pos_diff= "<<extreme_pos_diff
										<<" extreme_neg_diff= "<<extreme_neg_diff
										<<endl;
		}
		if (extreme_pos_diff > 0. ) this_err_up   += pow(extreme_pos_diff, 2.);
		if (extreme_neg_diff < 0. ) this_err_down += pow(extreme_neg_diff, 2.);
	 }

	 this_err_up   = TMath::Sqrt(this_err_up);
	 this_err_down = TMath::Sqrt(this_err_down);

	 if (PDFErrorSize.compare("90Percent")==0 ) {
		this_err_up   *= 1.645;
		this_err_down *= 1.645;
	 }

 /*
	} else if (PDFtype.compare("HERAPDF15NLO")==0) {
	 central_val = h_errors_PDFBand.at(0)->GetBinContent(bi);
	 for (int pdferri = 1; pdferri < 20; pdferri += 2) {   //// experimental errors
		this_err_up += pow( 0.5*(h_errors_PDFBand.at(pdferri+1)->GetBinContent(bi) 
													 - h_errors_PDFBand.at(pdferri)  ->GetBinContent(bi)), 2.);
	 }

	 this_err_down = this_err_up;
	 for (int pdferri = 21; pdferri < 29; pdferri++) {   /// model errors
		if (h_errors_PDFBand.at(pdferri)->GetBinContent(bi) > central_val ) {
		 this_err_up  += pow( h_errors_PDFBand.at(pdferri)->GetBinContent(bi) - central_val, 2.);
		 this_err_down += 0.;
		} else  {
		 this_err_up  += 0.;
		 this_err_down += pow( central_val - h_errors_PDFBand.at(pdferri)->GetBinContent(bi), 2.);
		}
	 }

	 for (int pdferri = 29; pdferri < 33; pdferri++) {     //// parameterization errors
		diff_central = h_errors_PDFBand.at(pdferri)->GetBinContent(bi) - central_val;
		if (diff_central > 0 && diff_central > extreme_pos_diff ) extreme_pos_diff = diff_central;
		if (diff_central < 0 && diff_central < extreme_neg_diff ) extreme_neg_diff = diff_central;
	 }
	 if (extreme_pos_diff > 0. ) this_err_up   += pow(extreme_pos_diff, 2.);
	 if (extreme_neg_diff < 0. ) this_err_down += pow(extreme_neg_diff, 2.);
	 this_err_up   = TMath::Sqrt(this_err_up);
	 this_err_down = TMath::Sqrt(this_err_down);

	 if (PDFErrorSize.compare("90Percent")==0 ) {
		this_err_up *= 1.645;
		this_err_down *= 1.645;
	 }
*/

	} else {
		cout<<" SPXPDF::CalcPDFBandErrors: unsupported pdfCode encountered."<<PDFtype.c_str()<<endl;
	 exit(0); //TEST
	}
		 
	if (debug) cout<<" SPXPDF::CalcPDFBandErrors: this_err_up= "  <<this_err_up  <<endl;
	if (debug) cout<<" SPXPDF::CalcPDFBandErrors: this_err_down= "<<this_err_down<<endl;

	if (TString(PDFtype).Contains("NNPDF") ) { // better here Contains NNPDF
	 double x_val, y_val;
	 h_PDFBand_results->GetPoint(bi-1, x_val, y_val);
	 h_PDFBand_results->SetPoint(bi-1, x_val,average);
	}

	h_PDFBand_results->SetPointEYhigh(bi-1, this_err_up);
	h_PDFBand_results->SetPointEYlow (bi-1, this_err_down);

	if (debug) {
	 double x_val;
	 double y_val;
	 h_PDFBand_results->GetPoint(bi-1, x_val, y_val);
	 //cout<<" SPXPDF::CalcPDFBandErrors: y_val= "<<y_val<<endl;
	 //cout<<" SPXPDF::CalcPDFBandErrors: x_val= "<<x_val<<endl;
	 if (y_val!=0) {
		cout<<" SPXPDF::CalcPDFBandErrors: bin= "<<bi<<" (y_val+this_err_up)/y_val= "  <<(y_val+this_err_up)/y_val
																								<<" (y_val-this_err_down)/y_val= "<<(y_val-this_err_down)/y_val<<endl;
	 }
	}
 }  /// loop over bins

 if(debug) cout<<" SPXPDF::CalcPDFBandErrors: End cal of PDFBandErrors for: "<<PDFtype<<std::endl;
}

void SPXPDF::CalcAlphaSErrors()
{
 if (debug) cout<<" SPXPDF::CalcAlphaSErrors: Starting calc of PDFAlphaSErrors for: "<<PDFtype<<std::endl;

 //assert(h_errors_AlphaS.size() == 3);
 double this_default_val = 0.;
 double this_err_down = 0.;
 double this_err_up = 0.;
 double error = 0.;

 if (!do_AlphaS) {
	cout<<" SPXPDF::CalcAlphaSErrors: called but do_AlphaS off ! "<<endl;
	return;
 }

//if (debug) cout<<" Size h_errors_AlphaS= "<<h_errors_AlphaS.size()<<endl;

 if (h_errors_AlphaS.size()==0)
	cout<<" SPXPDF::CalcAlphaSErrors: h_errors_AlphaS not filled "<<endl;
 if (!h_errors_AlphaS[0])
	cout<<" SPXPDF::CalcAlphaSErrors: h_errors_AlphaS[0] not found ! "<<endl;

 for (int bi = 1; bi <= h_errors_AlphaS.at(0)->GetNbinsX(); bi++) {
	//if (debug) cout<<" SPXPDF::CalcAlphaSErrors: bin= "<<bi<<endl;
	this_default_val = h_errors_AlphaS.at(0)->GetBinContent(bi);
	this_err_down = h_errors_AlphaS.at(1)->GetBinContent(bi);
	this_err_up = h_errors_AlphaS.at(2)->GetBinContent(bi);
	if (debug) cout<<" SPXPDF::CalcAlphaSErrors: bin = "<<bi
													 <<", default val = "<<this_default_val<<" +"<<this_err_up<<" -"<<this_err_down<<std::endl;

	error = 0.5*fabs(this_err_up-this_err_down);
	if (PDFErrorSize.compare("90Percent")==0 ) error *= 1.645;
	double init_x_val;
	double init_y_val;
	h_AlphaS_results->GetPoint(bi-1, init_x_val, init_y_val);
	h_AlphaS_results->SetPoint(bi-1, init_x_val, this_default_val);
	h_AlphaS_results->SetPointEYhigh(bi-1, error);
	h_AlphaS_results->SetPointEYlow(bi-1, error);

 } /// bi

 if (debug) cout<<" SPXPDF::CalcAlphaSErrors: End calc of PDFAlphaSErrors for: "<<PDFtype<<std::endl;
}


void SPXPDF::GetRatioToTH1(TH1D* href)
{
 if (debug) cout<<" SPXPDF::GetRatioToTH1: start"<<endl;

 TGraphAsymmErrors* tgraph_href = TH1TOTGraphAsymm(href);
 TString ratio_to_ref_name = (TString) h_PDFBand_results->GetName() + "_ratio_to_ref";
 h_PDFBand_results_ratio_to_ref = MyTGraphErrorsDivide(h_PDFBand_results, tgraph_href);
 h_PDFBand_results_ratio_to_ref->SetName(ratio_to_ref_name);
 h_PDFBand_results_ratio_to_ref->SetFillColor(fillColorCode);
 h_PDFBand_results_ratio_to_ref->SetFillStyle(fillStyleCode);

 ratio_to_ref_name = (TString) h_AlphaS_results->GetName() + "_ratio_to_ref";
 h_AlphaS_results_ratio_to_ref = MyTGraphErrorsDivide(h_AlphaS_results, tgraph_href);
 h_AlphaS_results_ratio_to_ref->SetName(ratio_to_ref_name);
 h_AlphaS_results_ratio_to_ref->SetFillColor(fillColorCode);
 h_AlphaS_results_ratio_to_ref->SetFillStyle(fillStyleCode);

 if (debug) cout<<" SPXPDF::GetRatioToTH1: End"<<endl;
}

void SPXPDF::DrawPDFBand(){
 
 TH1D* hpdf=this->GetPdfdefault();
 hpdf->Draw("same");

	//h_PDFBand_results->Print("all");
 h_PDFBand_results->Draw("E2,same");

 if (!hpdf) cout<<" testSPXPDF histo not found "<<endl;
 hpdf->Draw("same");

 return;

};

void SPXPDF::DrawPDFRatio(int iset1, int iset2){

 TH1D* hratio=this->GetPDFRatio(iset1, iset2);
 hratio->Draw();

}

double SPXPDF::GetMaximum(int iset){

	return h_errors_PDFBand[iset]->GetMaximum();
}

TH1D* SPXPDF::GetPDFRatio(int iset1, int iset2)
{

 if (debug) 
	cout<<" SPXPDF::GetPDFRatio iset2= "<< iset2<<" iset1= "<<iset1
			<<" n_PDFMembers= "<<n_PDFMembers<<endl;
 
 TString ratio_to_ref_name = (TString) h_PDFBand_results->GetName() + " Set=";
 ratio_to_ref_name +=iset1;
 ratio_to_ref_name +=" / Set=";
 ratio_to_ref_name +=iset2;

 if (iset1>=n_PDFMembers||iset2>=n_PDFMembers||iset1<0||iset2<0){
	cout<<" SPXPDF::GetPDFRatio iset too large iset2= "<< iset2<<" iset1= "<<iset1<<endl;
	exit (0);
 }

 if (iset2!=0) {
	ratio_to_ref_name +=TString("/set");
	ratio_to_ref_name +=iset2;
 }
 if (debug) cout<<" SPXPDF::GetPDFRatio: "<< ratio_to_ref_name.Data()<<endl;

 TH1D *hratio=(TH1D*) h_errors_PDFBand[iset1]->Clone( ratio_to_ref_name);
 TH1D *htmp=(TH1D*) h_errors_PDFBand[iset2]->Clone("htmp");
 hratio->Divide(htmp);

 hratio->SetLineColor(fillColorCode);
 
 return hratio;
}

double SPXPDF::GetPDFWeight(int iset1, double x1, double x2){
 double w=1.;

 TH1D* hpdf=this->GetPdfdefault();
 double hpdfx1=hpdf->GetBinContent(hpdf->FindBin(x1));
 double hpdfx2=hpdf->GetBinContent(hpdf->FindBin(x2));

 TH1D *htmp=h_errors_PDFBand[iset1];
 double htmpx1=htmp->GetBinContent(htmp->FindBin(x1));
 double htmpx2=htmp->GetBinContent(htmp->FindBin(x2));

 // x1,x2,scale,id1,id2,pdf1,pdf2 0.630916 0.734315 3967.7 21 21 0.000859055 0.000121401
 cout<<" PDF= "<<this->GetPDFName()<<" iset1= "<<iset1<<" ifl= "<<ifl<<endl;
 cout<<" x1= "<<x1<<" x2= "<<x2<<" Q2= "<<Q2<<" pdf1= "<<htmpx1<<" pdf2= "<<htmpx2<<endl;

 w=htmpx1*htmpx2;
 w/=hpdfx1*hpdfx2;

 cout<<" GetPDFWeight w= "<<w<<endl;

 return w;
};

//convert the histogram h1 into a graph
TGraphAsymmErrors* SPXPDF::TH1TOTGraphAsymm(TH1 *h1)
{
 //if(debug) cout<<" SPXPDF::TH1TOTGraphAsymm: start"<<std::endl;

	if (!h1) {
	 cout<<" SPXPDF::TH1TOTGraphAsymm: histogram not found !"<<endl;
	 exit(0); //No histogram provided, exit
	}

	TGraphAsymmErrors* g1= new TGraphAsymmErrors();

	double x, y, ex, ey;
	for (Int_t i=0; i<h1->GetNbinsX(); i++) {
		y=h1->GetBinContent(i+1);
	 ey=h1->GetBinError(i+1);
		x=h1->GetBinCenter(i+1);
	 ex=h1->GetBinWidth(i+1)/2.;

 //if(debug) cout<<i<<" x,y = "<<x<<" "<<y<<" ex,ey = "<<ex<<" "<<ey<<std::endl;

	 g1->SetPoint(i,x,y);
	 g1->SetPointError(i,ex,ex,ey,ey);
	}

 //if(debug) cout<<" SPXPDF::TH1TOTGraphAsymm: End"<<std::endl;
 return g1;
}


// Divide two TGraphAsymmErrors: new=g1/g2
//
// noerr=0: put all errors to zero
//       1: add errors from two graph quadrically
//       2: set errors from graph 2 to zero
TGraphAsymmErrors* SPXPDF::MyTGraphErrorsDivide(TGraphAsymmErrors* g1,TGraphAsymmErrors* g2, Int_t noerr) {
 if (debug) cout<<" SPXPDF::MyTGraphErrorsDivide: started"<<std::endl;
 if (!g1) cout<<" SPXPDF::MyTGraphErrorsDivide: g1 does not exist ! "<<std::endl;
 if (!g2) cout<<" SPXPDF::MyTGraphErrorsDivide: g2 does not exist ! "<<std::endl;

 Int_t n1=g1->GetN();
 Int_t n2=g2->GetN();

 if (n1!=n2) {
	cout<<" SPXPDF::MyTGraphErrorsDivide: vector do not have the same number of entries!"
			<<"\n\tg1: "<<g1->GetName()<<" n1= "<<n1
			<<"\n\tg2: "<<g2->GetName()<<" n2= "<<n2<<endl;
	}

	TGraphAsymmErrors* g3= new TGraphAsymmErrors();
	if (!g3) cout<<" SPXPDF::MyTGraphErrorsDivide: problem to make new vector ! "<<std::endl;
	g3->SetName       (g1->GetName());
	g3->SetMarkerStyle(g1->GetMarkerStyle());
	g3->SetMarkerColor(g1->GetMarkerColor());
	g3->SetLineColor  (g1->GetLineColor());

	Double_t   x1=0.,   y1=0., x2=0., y2=0.;
	Double_t dx1h=0., dx1l=0.;
	Double_t dy1h=0., dy1l=0.;
	Double_t dy2h=0., dy2l=0.;

	Double_t* EXhigh1 = g1->GetEXhigh();
	Double_t* EXlow1 =  g1->GetEXlow();
	Double_t* EYhigh1 = g1->GetEYhigh();
	Double_t* EYlow1 =  g1->GetEYlow();

	Double_t* EXhigh2 = g2->GetEXhigh();
	Double_t* EXlow2 =  g2->GetEXlow();
	Double_t* EYhigh2 = g2->GetEYhigh();
	Double_t* EYlow2 =  g2->GetEYlow();

	Int_t iv=0;
	for (Int_t i1=0; i1<n1; i1++) {  //loop over point of graph1
	 Int_t matchcount=0;
	 for (Int_t i2=0; i2<n2; i2++) {//loop over point of graph2
		g1->GetPoint(i1,x1,y1);
		g2->GetPoint(i2,x2,y2);
		Double_t emean=(EXhigh1[i1]+EXhigh2[i2]+EXlow1[i1]+EXlow2[i2])/4.;
		if (fabs(x1-x2)>emean) {
		 //cout<<" SPXPDF::MyTGraphErrorsDivide: x1 and x2 not the same x1= "<<x1<<" x2= "<<x2<<std::endl;
		} else { // do something only if x1=x2
		 matchcount++;
		 //cout<<" SPXPDF::MyTGraphErrorsDivide: x1 and x2 match x1= "<<x1<<" x2= "<<x2<<std::endl;
		 dx1h  = EXhigh1[i1];
		 dx1l  = EXlow1[i1];
		 if (y1!=0.) dy1h  = EYhigh1[i1]/y1;
		 else        dy1h  = 0.;
		 if (y2!=0.) dy2h  = EYhigh2[i2]/y2;
		 else        dy2h  = 0.;
		 if (y1!=0.) dy1l  = EYlow1 [i1]/y1;
		 else        dy1l  = 0.;
		 if (y2!=0.) dy2l  = EYlow2 [i2]/y2;
		 else        dy2l  = 0.;

		 if (debug) {
			cout<<"SPXPDF::MyTGraphErrorsDivide: "
					<<"\n\ti1: "    <<i1<<", i2: "<<i2
					<<"\n\tdy1l: "  <<dy1l<<", dy1h: "<<dy1h
					<<"\n\tdy2l: "  <<dy2l<<", dy2h "<<dy2h
					<<"\n\tsqrt: " <<sqrt(dy1l*dy1l+dy2l*dy2l)<<", "<<sqrt(dy1h*dy1h+dy2h*dy2h)<<endl;
			}

			if (y2!=0.) g3->SetPoint(iv, x1,y1/y2);
			else        g3->SetPoint(iv, x1,y2);
			Double_t el=0.;
			Double_t eh=0.;

			if (noerr==2) {
			 dy2l=0.;
			 dy2h=0.;
			}
			if (y1!=0. && y2!=0.) el=sqrt(dy1l*dy1l+dy2l*dy2l)*(y1/y2);
			if (y1!=0. && y2!=0.) eh=sqrt(dy1h*dy1h+dy2h*dy2h)*(y1/y2);


			if (debug) cout<<"\tdx1h: "<<dx1h<<", dx1l: "<<dx1l<<", el: "<<el<<", eh: "<<eh<<std::endl;

			if (noerr==0)   g3->SetPointError(iv,dx1l,dx1h,0,0);
			else            g3->SetPointError(iv,dx1l,dx1h,el,eh);
			iv++;
		 }
		}
		if (matchcount>1) {
		 cout<<" SPXPDF::MyTGraphErrorsDivide: too many x-points matched ! "<<endl;
		 exit (1);
		}
	 }

	 if (debug) cout<<" SPXPDF::MyTGraphErrorsDivide: End"<<std::endl;
	 return g3;
}



//read the provided steering file and set internal variables depending on what is read
/*
void SPXPDF::ReadSteering(const string _fileName)
{
 string fName="";
 if (_fileName.size()>0)
	fName=_fileName;
 //else
 //    fName=steeringFilePath;

 if (debug) cout<<" SPXPDF::ReadSteering: reading steering file named: "<<fName.c_str()<<std::endl;

 //Open the file for reading if it can be read/found
 ifstream infile(fName.c_str(), ios::in);

 if(!infile) {
	cerr<<" SPXPDF::ReadSteering: WARNING: Can't open "<<fName<<endl;
	infile.close();
	exit (1); // failed to read steering, exit
 } else {
	if (debug) cout<<" SPXPDF::ReadSteering: Steering file named successfuly opened."<<std::endl;
 }


 pdfSetPath = defaultPDFSetPath;
 string pdfSetDefaultPath = GetEnv("LHAPATH");

 if (pdfSetDefaultPath.size()>0) {
	if (pdfSetDefaultPath.find_last_of("/") == pdfSetDefaultPath.size()-1)
	pdfSetDefaultPath = pdfSetDefaultPath.substr(0,pdfSetDefaultPath.size()-1); //remove trailing slashes if there are any
	pdfSetPath=pdfSetDefaultPath;
	cout<<" SPXPDF: LHAPATH environment variable found, using path: "<<pdfSetPath<<endl;
 } else {
	cout<<" SPXPDF: LHAPATH environment variable not set, using default: "<<pdfSetPath<<endl;
 }

 string line;
 string optionName;
 string text;

 //load in all valid options

 int w=20; //arbitrary width number for printing nicely formatted debug statements

 //read and set all options and data
 while (infile.good()) {
	getline(infile, line);

	optionName=line.substr(0, line.find(' '));
	text=line.substr(line.find(' ')+1,line.size()); //'text' could be broken up further if needed

	if (debug) {
	 cout<<"\n SPXPDF::ReadSteering: Read in:<<<<<<<<<<<<<<<<<<<"
			 <<"\n"<<setw(w)<<"line:"<<line
			 <<"\n"<<setw(w)<<"optionName:"<<optionName
			 <<"\n"<<setw(w)<<"text:"<<text
			 <<"\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
	}

	if (line[0] != '%') { //ignore comments
	 if (optionName.compare("debug")==0) {
		debug=true;
	 } else if (optionName.compare("PDFtype")==0) {
		PDFtype=text;
	 } else if (optionName.compare("PDFname")==0) {
		PDFname=text;
	 } else if (optionName.compare("PDFnamevar")==0) {
		PDFnamevar=text;
	 } else if (optionName.compare("numPDFMembers")==0) {
		sscanf(text.c_str(), "%d", &n_PDFMembers);
	 } else if (optionName.compare("defaultpdfid")==0) {
		sscanf(text.c_str(), "%d", &defaultpdfid);
	 } else if (optionName.compare("defaultpdfidvar")==0) {
		sscanf(text.c_str(), "%d", &defaultpdfidvar);
	 } else if (optionName.compare("firsteig")==0) {
		sscanf(text.c_str(), "%d", &firsteig);
	 } else if (optionName.compare("lasteig")==0) {
		sscanf(text.c_str(), "%d", &lasteig);
	 } else if (optionName.compare("firstquadvar")==0) {
		sscanf(text.c_str(), "%d", &firstquadvar);
	 } else if (optionName.compare("lastquadvar")==0) {
		sscanf(text.c_str(), "%d", &lastquadvar);
	 } else if (optionName.compare("firstmaxvar")==0) {
		sscanf(text.c_str(), "%d", &firstmaxvar);
	 } else if (optionName.compare("lastmaxvar")==0) {
		sscanf(text.c_str(), "%d", &lastmaxvar);
	 } else if (optionName.compare("nloops")==0) {
		sscanf(text.c_str(), "%d", &nLoops);
	 } else if (optionName.compare("fillStyleCode")==0) {
		sscanf(text.c_str(), "%d", &fillStyleCode);
	 } else if (optionName.compare("fillColorCode")==0) {
		sscanf(text.c_str(), "%d", &fillColorCode);
	 } else if (optionName.compare("markerStyle")==0) {
		sscanf(text.c_str(), "%d", &markerStyle);
	 } else if (optionName.compare("PDFBandType")==0) {
		PDFBandType=text;
	 } else if (optionName.compare("includeEIG")==0) {
		includeEIG=true;   
	 } else if (optionName.compare("includeQUAD")==0) {
		includeQUAD=true;   
	 } else if (optionName.compare("includeMAX")==0) {
		includeMAX=true;   
	 } else if (optionName.compare("PDFErrorType")==0) {
		PDFErrorType=text;
		//possible error types: PDFBand, AlphaS, RenormalizationScale, FactorizationScale, TotError
		if (PDFErrorType.compare("PDFBand")==0) {
		 do_PDFBand=true;
		} else if(PDFErrorType.compare("AlphaS")==0) {
		 do_AlphaS=true;
		}
	 } else if (optionName.compare("PDFErrorSize")==0) {
		PDFErrorSize=text;
	 } else if (optionName.compare("pdfSetPath")==0) {
		pdfSetPath=text;
		if (pdfSetPath.find_last_of("/") == pdfSetPath.size()-1)
		pdfSetPath = pdfSetPath.substr(0,pdfSetPath.size()-1); //remove trailing slashes if there are any
		cout<<" SPXPDF::ReadSteering: new PDF set path found! Now path is: "<<pdfSetPath<<std::endl;
	 } else if (optionName.compare("AlphaSmemberNumDown")==0) {
		sscanf(text.c_str(), "%d", &AlphaSmemberNumDown);
	 } else if (optionName.compare("AlphaSmemberNumUp")==0) {
		sscanf(text.c_str(), "%d", &AlphaSmemberNumUp);
	 } else if (optionName.compare("AlphaSPDFSetNameDown")==0) {
		AlphaSPDFSetNameDown = text;
	 } else if (optionName.compare("AlphaSPDFSetNameUp")==0) {
		AlphaSPDFSetNameUp = text;
	 } else if (optionName.compare("AlphaSPDFSetHistNameDown")==0) {
		AlphaSPDFSetHistNameDown = text;
	 } else if (optionName.compare("AlphaSPDFSetHistNameUp")==0) {
		AlphaSPDFSetHistNameUp = text;
	 }
	}
 }
}
*/

//Print all relevant internal variable values
void SPXPDF::Print()
{
 int w=30;               //arbitrary size that makes the formatting look pretty
 string empty="<empty>"; //print this if no input has been provided for that variable
 string ON="ON";         //bool true
 string OFF="OFF";       //bool false

 cout<<" SPXPDF::Print: >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
		 <<"\n"<<setw(w)<<"debug:"                <<setw(w)<<(debug? "ON":"OFF")
			//             <<"\n"<<setw(w)<<"steeringFilePath:"     <<setw(w)<<(steeringFilePath.size()>0? steeringFilePath:empty)
			//             <<"\n"<<setw(w)<<"steeringFileDir:"      <<setw(w)<<(steeringFileDir.size()>0? steeringFileDir:empty)
		 <<"\n"<<setw(w)<<"steeringFileName:"     <<setw(w)<<(steeringFileName.size()>0? steeringFileName:empty)
		 <<"\n"<<setw(w)<<"gridName:"             <<setw(w)<<(gridName.size()>0? gridName:empty)
		 <<"\n"
		 <<"\n"<<setw(w)<<"PDFtype:"              <<setw(w)<<(PDFtype.size()>0? PDFtype:empty)
		 <<"\n"<<setw(w)<<"PDFname:"              <<setw(w)<<(PDFname.size()>0? PDFname:empty)
		 <<"\n"<<setw(w)<<"PDFnamevar:"           <<setw(w)<<(PDFnamevar.size()>0? PDFname:empty)
		 <<"\n"<<setw(w)<<"numPDFMembers:"        <<setw(w)<<(n_PDFMembers!=DEFAULT? patch::to_string(n_PDFMembers):empty)
		 <<"\n"<<setw(w)<<"nLoops: "              <<setw(w)<<(nLoops!=DEFAULT? patch::to_string(nLoops):empty)
		 <<"\n"<<setw(w)<<"fillStyleCode:"        <<setw(w)<<(fillStyleCode!=DEFAULT? patch::to_string(fillStyleCode):empty)
		 <<"\n"<<setw(w)<<"fillColorCode:"        <<setw(w)<<(fillColorCode!=DEFAULT? patch::to_string(fillColorCode):empty)
		 <<"\n"<<setw(w)<<"PDFBandType:"          <<setw(w)<<(PDFBandType.size()>0? PDFBandType:empty)
		 <<"\n"<<setw(w)<<"first Eigenvector: "   <<setw(w)<<(firsteig!=DEFAULT? patch::to_string(firsteig):empty)
		 <<"\n"<<setw(w)<<"last Eigenvector:   "  <<setw(w)<<(lasteig!=DEFAULT?  patch::to_string(lasteig):empty)
		 <<"\n"<<setw(w)<<"first component added in quadrature"  <<setw(w)<<(firstquadvar!=DEFAULT?patch::to_string(firstquadvar):empty)
		 <<"\n"<<setw(w)<<"last component added in quadrature"  <<setw(w)<<(lastquadvar!=DEFAULT?patch::to_string(lastquadvar):empty)
		 <<"\n"<<setw(w)<<"first component to find maximum"  <<setw(w)<<(firstmaxvar!=DEFAULT?patch::to_string(firstmaxvar):empty)
		 <<"\n"<<setw(w)<<"last component to find maximum"  <<setw(w)<<(lastmaxvar!=DEFAULT?patch::to_string(lastmaxvar):empty)
		 //<<"\n"<<setw(w)<<"**PDF ERROR TYPE(s) ACTIVE:"
		 <<"\n"<<setw(w)<<"PDFBand:"              <<setw(w)<<(do_PDFBand? ON:OFF)
		 <<"\n"<<setw(w)<<"AlphaS:"               <<setw(w)<<(do_AlphaS? ON:OFF)
		 <<"\n"<<setw(w)<<"includeEIG:"           <<setw(w)<<(includeEIG? ON:OFF)
		 <<"\n"<<setw(w)<<"includeQUAD:"          <<setw(w)<<(includeQUAD? ON:OFF)
		 <<"\n"<<setw(w)<<"includeMAX:"           <<setw(w)<<(includeMAX? ON:OFF)
		 <<"\n"<<setw(w)<<"PDFErrorSize:"         <<setw(w)<<(PDFErrorSize.size()>0? PDFErrorSize:empty)
		 <<"\n SPXPDF::Print:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"<<endl;
}


//THIS FUNCTION HAS BEEN MOVED TO SPXFileUtilities::FileExists()
//always check for file existence before usage
/*
bool SPXPDF::FileExists(const string _fileName)
{
 bool exists;

 if ( FILE* file=fopen(_fileName.c_str(),"r") ) {
	fclose(file);
	exists = true;
 } else exists = false;

 if (debug&&exists) cout<<"file: '"<<_fileName<<"' exist "<<exists<<std::endl;
 if (debug&&!exists) cout<<"file: '"<<_fileName<<"' not found "<<exists<<std::endl;
 return exists;
}
*/

//default values for variables to avoid crashes and check for proper setup before doing anything
void SPXPDF::SetVariablesDefault()
{
 if (debug) cout<<" SPXPDF::setVariablesDefault: Start default values being set."<<std::endl;
 string defaultString="";
 //int defaultInt=-1;

 //debug=false;
 //steeringFileDir=defaultString;
 steeringFileName=defaultString;
 PDFtype=defaultString;
 PDFname=defaultString;
 PDFnamevar=defaultString;
 n_PDFMembers=DEFAULT;
 fillStyleCode=DEFAULT;
 fillColorCode=DEFAULT;
 PDFBandType=defaultString;
 PDFErrorType=defaultString;
 PDFErrorSize="OneSigma";

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

 do_PDFBand=false;
 do_AlphaS=false;

 AlphaSmemberNumDown=DEFAULT;
 AlphaSmemberNumUp=DEFAULT;
 AlphaSPDFSetNameDown=defaultString;
 AlphaSPDFSetNameUp=defaultString;
 AlphaSPDFSetHistNameDown=defaultString;
 AlphaSPDFSetHistNameUp=defaultString;

 if (debug) cout<<" SPXPDF::setVariablesDefault: End default values are set."<<std::endl;
}

/*
//determine the steering file name and directory from the provided path
void SPXPDF::SetSteeringFileNameAndDir(const string _path)
{
		if(debug) cout<<" SPXPDF::setSteeringFileNameAndDir: Start extracting file name and directory name from path: "<<_path<<std::endl;
		int pathLength=_path.length();

		if(pathLength>0) {
				int found=_path.find_last_of("/");
				if(found==-1) {
						steeringFileDir="<current dir>"; //if there are no slashes, then the file is in the current directory
						steeringFileName=_path;
				}
				else {
						steeringFileDir =_path.substr(0,found);
						steeringFileName=_path.substr(found+1);
				}

				if(debug) {
						cout<<" SPXPDF::setSteeringFileNameAndPath:"
										 <<" \n\tSplitting: '"<<_path<<"'"
										 <<" \n\tpath: '"<<_path.substr(0,found)<<"'"
										 <<" \n\tfile: '"<<_path.substr(found+1)<<"'"<<std::endl;
				}
		}
		else {
				cout<<" SPXPDF::setSteeringFileNameAndDir: Steering file path '"<<_path<<"' is invalid."<<std::endl;
		}
}
*/

//mutator methods
void SPXPDF::SetDebug(bool _debug) {
 debug=_debug;
}
void SPXPDF::SetGridName(string _gridName) {
 gridName=_gridName;
}
/*
void SPXPDF::SetSteeringFilePath(string _steeringFilePath) {
		steeringFilePath=_steeringFilePath;
		//udate the Dir location and file name of the steering file if the path is changed
		SetSteeringFileNameAndDir(steeringFilePath);
}
void SPXPDF::SetSteeringFileDir(string _steeringFileDir) {
		steeringFileDir=_steeringFileDir;
}
*/
void SPXPDF::SetSteeringFileName(string _steeringFileName) {
 steeringFileName=_steeringFileName;
}
void SPXPDF::SetPDFtype(string _PDFtype) {
 PDFtype=_PDFtype;
}
void SPXPDF::SetPDFname(string _PDFname) {
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
void SPXPDF::SetPDFBandType(string _PDFBandType) {
 PDFBandType=_PDFBandType;
}
void SPXPDF::SetPDFErrorType(string _PDFErrorType) {
 PDFErrorType=_PDFErrorType;
}
void SPXPDF::SetPDFErrorSize(string _PDFErrorSize) {
 PDFErrorSize=_PDFErrorSize;
}

void SPXPDF::SetDoPDFBand(bool _doit) {
 do_PDFBand = _doit;
}
void SPXPDF::SetDoAplphaS(bool _doit) {
 do_AlphaS = _doit;
}

void SPXPDF::SetAlphaSmemberNumDown(int _memberNum) {
 AlphaSmemberNumDown=_memberNum;
}
void SPXPDF::SetAlphaSmemberNumUp(int _memberNum) {
 AlphaSmemberNumUp=_memberNum;
}
void SPXPDF::SetAlphaSPDFSetNameDown(string _name) {
 AlphaSPDFSetNameDown= _name;
}
void SPXPDF::SetAlphaSPDFSetNameUp(string _name) {
 AlphaSPDFSetNameUp= _name;
}
void SPXPDF::SetAlphaSPDFSetHistNameDown(string _name) {
 AlphaSPDFSetHistNameDown= _name;
}
void SPXPDF::SetAlphaSPDFSetHistNameUp(string _name) {
 AlphaSPDFSetHistNameUp= _name;
}

void SPXPDF::CleanUpSPXPDF() {
 if (debug) cout<<"SPXPDF::CleanUpSPXPDF: Starting to clean up..."<<std::endl;

 if (h_errors_PDFBand.size()>0) {
	for (int i=0; i<h_errors_PDFBand.size(); ++i) {
	 delete h_errors_PDFBand.at(i);
	}
 }


 //    if(h_errors.size()>0) {
 //    for(int i=0; i<h_errors.size(); ++i) {
 //        delete h_errors.at(i);
 //    }
 // }

 if (h_errors_AlphaS.size()>0) {
	for (int i=0; i<h_errors_AlphaS.size(); ++i) {
	 delete h_errors_AlphaS.at(i);
	 }
	}

	if (h_errors_AlphaS.size()>0) {
	 for (int i=0; i<h_errors_AlphaS.size(); ++i) {
		delete h_errors_AlphaS.at(i);
	 }
	}

	if (debug) cout<<"SPXPDF::CleanUpSPXPDF: Finished clean up!"<<std::endl;
}



TH1D * SPXPDF::FillPdfHisto(){

	if (!hpdf) cout<<" SPXPDF::FillPdfHistos histo not found "<<endl;
	
	//const int nstep=1000;
	const double xmin=hpdf->GetBinCenter(1);
	const int    nbin=hpdf->GetNbinsX();
	const double xmax=hpdf->GetBinWidth(nbin)/2.+hpdf->GetBinCenter(nbin);
	const double binw=hpdf->GetBinWidth(1);
 
	if (debug) cout<<" SPXPDF::FillPdfHisto: xmin= "<<xmin<<" xmax= "<<xmax<<" binw= "<<binw<<endl;
	if (debug) cout<<" SPXPDF::FillPdfHisto: Q2= "<<Q2<<endl;

	double xfl[13];
	for (int i=0; i<nbin; i++){
	 double x=xmin+i*(xmax-xmin)/nbin;
	 double Q=sqrt(Q2);
	 getPDF(x, Q,xfl);

	 int ibin=hpdf->FindBin(x);
	 if (debug) 
		cout<<" SPXPDF::FillPdfHisto: ibin= "<<ibin<<" Q2= "<<Q2<<" x= "<<x<<" xfl["<<ifl<<"]= "<<xfl[6+ifl]<<endl;

	 hpdf->SetBinContent(ibin,x,xfl[6+ifl]);
	}
	
	/*
	if (debug) {
	 cout<<" SPXPDF::FillPdfHisto print histo: "<<endl;
	 hpdf->Print("all");
	}
	*/
	return hpdf;
};
