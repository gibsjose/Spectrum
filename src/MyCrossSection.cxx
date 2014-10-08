//
//   for sherpa
//
using namespace std;
#include "appl_grid/generic_pdf.h"
#include "MyCrossSection.h"
#include "TMatrixTLazy.h" //TODO - move to an appropriate spot or delete
#include "TMatrixT.h" //TODO - move to an appropriate spot or delete
#include "SteeringFile.h"
#include "SteeringUtilities.h"


/******************************************************************
 ** Method Implementations
 ******************************************************************/

//HARDCODED default value for an unused double - how to choose a better number?
const double DEFAULT_DOUBLE = -999;
const string cn = " MyCrossSection::"; //class name, used for print statemants

MyCrossSection::MyCrossSection(char name[100])
{
  debug=false;
  crosssectionname="";
  ratiotitlelabel="";

  xunits="GeV"; //TODO - default, make more generic?
  yunits="fb"; //TODO - default, make more generic?
  
  frameid.clear();
  divideid.clear();
  framepointer.clear();
  vpdfchi2.clear();

  rationames.clear();
  rationames.push_back( "reference" ); // DEFAULT - Set reference as ratio  numerator
  rationames.push_back( "data" );      // DEFAULT - Set data as ratio  denominator
  overlaynames.clear();
  overlaynames.push_back( "reference" ); // DEFAULT - Display reference in overlay
  overlaynames.push_back( "data" );      // DEFAULT - Display data in overlay
  displaynames.clear();
  displaynames.push_back( "overlay" );
  displaynames.push_back( "ratio" );

  xlegend=0.90; //default legend position is the top right of the frame
  ylegend=0.90;

  //default values for overloading ratio & overlay y-axes 
  ymaxoverlay=DEFAULT_DOUBLE;
  yminoverlay=DEFAULT_DOUBLE;
  ymaxratio=DEFAULT_DOUBLE;
  yminratio=DEFAULT_DOUBLE;

  plotchi2=false;
  plotmarker=false;
  plotband=false;
  ploterrorticks=false;
  //staggerpdfpoints=false;
  plotstaggered=false;
  overlayData = true; //always display data by default
  overlayConvolute = false;
  overlayReference = false;
  displayOverlay = true;
  displayRatio = true;

  xerroroff = false;

  pdf_function="";
  subprocesssteername="";

  // <<<<<<<<<<<<<<<<< should be rather a vector for general use
  gridnamebasedir=".";
  gridnamesystbasedir=".";
  gridnamedefaultdir=".";
  //gridnamemassdowndir=".";
  //gridnamemassupdir=".";
  //gridnamebeamdowndir=".";
  //gridnamebeamupdir=".";
  // <<<<<<<<<<<<<<<<<

  datanamedir=".";
  ntupname="";
        
  steername=name;
  if (debug) cout<<" MyCrossSection:: steering file= "<<steername<<endl;
  gridname.clear();
  vardesc.clear();
  dataname.clear();
  //corrname.clear();
  leglabel.clear();
  refhistlinestyle.clear();
  refhistlinecolor.clear();
  markerstyle.clear();
  markercolor.clear();
  mydata.clear();
  do_PDFBand = false;
  do_AlphaS = false;
  do_RenormalizationScale = false;
  do_FactorizationScale = false;
  do_TotError = false;
    
  this->ReadSteering(name);

  if (debug) cout<<" MyCrossSection finished reading name= "<<name<<endl;

  this->Initialize();

  if (debug) this->Print();

  return;

}

void MyCrossSection::Initialize() {
  //
  // Perform any initialization needed before plotting
  //
  string mn = "Initialize:"; //method name, for debug printing

  if (debug) cout<<" MyCrossSection::Initialize()"<<endl;
  cout<<" MyCrossSection::Initialize: Number of grids to produce "<<gridname.size()<<endl;

  //
  // Loop over grid
  //
  for (int  igrid = 0; igrid < gridname.size(); igrid++) {

    MyBand *mybandtmp= new MyBand();
    if (ploterrorticks) mybandtmp->SetErrorTicks(); 
    if (plotband) mybandtmp->SetPlotBand();
    if (plotmarker) {
      mybandtmp->SetPlotMarker();
      if (debug) cout<<" MyCrossSection::Initialize: Plotmarker ON move points "<<endl;
      mybandtmp->MovePDFPoints(); //plot markers are more visible if data points are shifted slightly
    }
    if (plotstaggered) mybandtmp->SetStaggerPDFPoints();
        

    myband.push_back(mybandtmp);


    //read in Data and prepare for it's use
    if (debug) cout<<" MyCrossSection::Initialize: Prapare Data \""<<this->GetDataName(igrid)<<"\""<<endl;
    if (this->GetDataOk(igrid)) {
      MyData *mydatatmp= new MyData;
      if (debug)
	cout<<" MyCrossSection::Initialize: DataName= "<<TString(this->GetDataName(igrid)).Data()
	    <<" DataDir= "<<TString(this->GetDataNameDir(igrid)).Data() <<endl;

      mydatatmp->ReadData(this->GetDataName(igrid),this->GetDataNameDir(igrid));

      mydatatmp->SetMarkerStyle(this->GetMarkerStyle(igrid));
      mydatatmp->SetMarkerColor(this->GetMarkerColor(igrid));
      mydata.push_back(mydatatmp);
    } else {
      if (debug) cout<<" MyCrossSection::Initialize no data found "<<igrid<<endl;
    }

    // Read in Grid data and prepare for it's use
    if (debug) cout<<" MyCrossSection::Initialize: Prepare Grid \""<<this->GetDataName(igrid)<<"\""<<endl;
    if (this->GetGridOk(igrid)) {
      string gridSteeringPath = this->GetGridSteeringPath(igrid);
      //      MyGrid *mygridtmp= new MyGrid(gridname.at(igrid));
      MyGrid *mygridtmp= new MyGrid( gridSteeringPath );
      
      if (debug)
	cout<<" MyCrossSection::Initialize:\n\t GridSteeringName=\""<<gridname.at(igrid)<<"\""
	    <<"\n\t GridSteeringPath=\""<<this->GetGridSteeringPath(igrid)<<"\""
	    <<"\n\t GridFilePath=\""<<mygridtmp->GetGridPath()<<"\""<<endl;

      
      mygrid.push_back(mygridtmp);
    } else {
      if (debug) cout<<" MyCrossSection::Initialize no grid found for igrid="<<igrid<<endl;
    }
    

    //container for mypdf instances per grid type. Each grid can have multiple pdfs
    //std::vector<MyPDF*> pdfStorage; 
    //t_mypdf.push_back(pdfStorage);

    double yfac=1.;
    double xfac=1.;
    bool normtot=false;
    bool divbinwidth=false;
    if (this->GetDataOk(igrid)) {
      //yfac=mydata[igrid]->GetUnitfbFactor(); //old scaling
      //xfac=mydata[igrid]->GetUnitGeVFactor(); //old scaling

      //convert units from data to desired
      yfac = GetUnitScaleY(mydata[igrid]->GetYUnits(), this->GetYUnits()); //new scaling
      xfac = GetUnitScaleX(mydata[igrid]->GetXUnits(), this->GetXUnits()); //new scaling

      
      //mydata[igrid]->Scale(1.,yfac/xfac);
      //mydata[igrid]->Normalis(yfac, xfac); //TODO - scale measurment data now?
      cout<<" MyCrossSection::Initialize: data to this, FACs: igrid:"<<igrid<<", xfac:"<<xfac<<", yfac:"<<yfac<<endl;


      if (debug) cout<<" MyCrossSection::Initialize: yfac= "<<yfac<<" xfac= "<<xfac<<endl;
      normtot=mydata[igrid]->isNormTot();
      if (debug&&normtot) cout<<" MyCrossSection::Initialize: normalise to total cross section "<<endl;
      divbinwidth=mydata[igrid]->DivideByBinWidth();
      if (debug&&divbinwidth) cout<<" MyCrossSection::Initialize: Data is divided by binwidth "<<endl;
    } else
      cout<<" MyCrossSection::Initialize: data is not ok ! "<<endl;

    if (pdfdata.size()==0) {cout<<" MyCrossSection::Initialize: no PDF found ! "<<endl;}

    if (debug) cout<<" MyCrossSection::Initialize: Number of PDF names pdfdata.size()= "<<  pdfdata.size()<<endl;

    int npdf=0;
    //if (pdfdata.size()>0) npdf=pdfdata.at(igrid).size();
    if (pdfdata.size()>0) npdf=pdfdata.size();
 
    if (debug) cout<<" MyCrossSection::Initialize: npdf= "<<npdf<<endl;

    for (int ipdf=0; ipdf<npdf; ipdf++){
      if (debug) cout<<" MyCrossSection::Initialize: load pdf= "<<pdfdata.at(ipdf).c_str()<<endl;

      /*
      MyPDF *newpdf = new MyPDF(GetGridName(igrid), //CHANGED
				pdfdata.at(ipdf),
                                do_PDFBand,
                                do_AlphaS
				);
      */

      MyPDF *newpdf = new MyPDF(mygrid.at(igrid)->GetGridPath(), 
				pdfdata.at(ipdf),
                                do_PDFBand,
                                do_AlphaS
				);
                      
      if (debug) {
	cout<<" MyCrossSection::Initialize: print pdf= "<<ipdf
	    <<" name= "<<pdfdata.at(ipdf).c_str()
	    <<" for grid: "<<mygrid.at(igrid)->GetGridPath()<<endl;

	cout<<" MyCrossSection::Initialize: ipdf= "<<ipdf<<" "<<newpdf->GetPDFtype().c_str()<<endl;
	newpdf->h_PDFBand_results->Print("all");
	
	cout<<" MyCrossSection::Initialize: npoint= "<<newpdf->h_PDFBand_results->GetN()<<endl;
      }

      newpdf->Print(); //display pdf info

      // get myband information for each pdf
      TGraphAsymmErrors* gtmp= (TGraphAsymmErrors*) newpdf->h_PDFBand_results->Clone((TString) (newpdf->GetPDFtype() + "_pdfband"));

      gtmp->SetName(TString(newpdf->GetPDFtype()));
      if (debug) {
	cout<<" MyCrossSection::Initialize: yfac= "<<yfac<<" xfac= "<<xfac<<endl; 
	if (normtot) cout<<" MyCrossSection::Initialize: normtot on "<<endl; 
	if (divbinwidth) cout<<" MyCrossSection::Initialize: divbinwidth "<<endl; 
      }

      
      this->Normalise(gtmp,yfac,xfac,normtot,divbinwidth);
      if (debug) {cout<<" MyCrossSection::Initialize: fill Band gtmp"<<endl; gtmp->Print("all");}


      gtmp->SetMarkerStyle(newpdf->GetMarkerStyle()); //marker style for PDF
      gtmp->SetMarkerColor(newpdf->GetMarkerColor()); //fill color code for PDF
      if(xerroroff == true) {
	for(int i=0; i<gtmp->GetN(); i++) {	
	  //gtmp->SetPointEXlow (i, 0.0);//TEST
	  //gtmp->SetPointEXhigh(i, 0.0);//TEST
	}
      }


      // set my band info for displyaing later
      myband[igrid]->SetPdfBand(gtmp); 
      

      // if data is being forcfully scaled, also scale the convolute as well
      if(mydata[igrid]->GetScalex() != 1.0 || mydata[igrid]->GetScaley() != 1.0 ) {
	
	if(debug) cerr<<" MyCrosSection:Initialize: WARN: Forcefully scaling convolute to data. "<<endl;
	
	//set convolutes's scaling to the same as data's
	myband[igrid]->SetScalex( mydata[igrid]->GetScalex() );
	myband[igrid]->SetScaley( mydata[igrid]->GetScaley() );
      }



      TH1D *hdefault=newpdf->GetPdfdefault();
      if (!hdefault) cout<<" MyCrossSection::Initialize: hdefault not found "<<endl;
      if (debug) {
	cout<<" MyCrossSection::Initialize: print default histogram ipdf= "<<ipdf<<endl; 
	hdefault->Print("all");
      }
      TGraphAsymmErrors* gdefaulttmp=TH1TOTGraphAsymm(hdefault); 
      this->Normalise(gdefaulttmp,yfac,xfac,normtot,divbinwidth);
      TString gname=TString(newpdf->GetPDFtype());
      gname+="_default";
      gdefaulttmp->SetName(gname);
      myband[igrid]->SetPdfDefault(gdefaulttmp);   

      if (debug) {
	cout<<" MyCrossSection::Initialize: print normalised default ipdf= "<<ipdf<<endl; 
	gdefaulttmp->Print("all");
      }

      if (newpdf->GetDoAlphaS() ) {
	if (debug) cout<<" MyCrossSection::Initialize: DoAlphaS "<<endl;

	TGraphAsymmErrors* gtmp= (TGraphAsymmErrors*) newpdf->h_AlphaS_results->Clone((TString) (newpdf->GetPDFtype() + "_alphasband"));

	this->Normalise(gtmp,yfac,xfac,normtot,divbinwidth);
	myband[igrid]->SetAlphasBand(gtmp); 
      } else if (debug) cout<<" MyCrossSection::Initialize: do alphas not set in MyPDF "<<endl;

    }

    cout<<" MyCrossSection::Initialize number of ren scale= "<<renscale.size()<<endl;
    cout<<" MyCrossSection::Initialize number of fac scale= "<<facscale.size()<<endl;

    if (renscale.size()!=facscale.size()) 
      cout<<" MyCrossSection::Initialize Something is wrong #renscale != #facscale "
	  <<" renscale= "<<renscale.size()<<" fac= "<<facscale.size() 
	  <<endl;

    for (int i=0; i<renscale.size(); i++){
      if (debug) cout<<" MyCrossSection::Initialize: scales= "<<renscale[i]<<" facscale= "<<facscale[i]<<endl;

      int pdfset=0;
      //TGraphAsymmErrors *gtmp=newpdfdefault->GetScaleDependence(renscale[i],facscale[i],pdfset);
      //if (!gtmp) cout<<" MyCrossSection::Initialize: gtmp from GetScaleDependence not found ! "<<endl;

      //if (debug) { 
      // gtmp->Print("all"); 
      //}
      cout<<" MyCrossSection::Initialize: nothing implemented "<<endl;
      //myband[igrid]->SetScaleBand(gtmp);   
    }

    std::vector<double> mychi2;

    //TMatrixT  <double> mcorr;
    TMatrixT <double> *mcorr;
    if (mydata[igrid]->CovarianceMatrixOk()){
      if (debug) cout<<" MyCrossSection::Initialize: get correlation matrix igrid= "<<igrid<<endl;
      mcorr= mydata[igrid]->GetCovarianceMatrix();
      if (debug) cout<<" MyCrossSection::Initialize: mcorr done "<<endl;
      if (debug) mcorr->Print();
      if (debug) cout<<" MyCrossSection::Initialize: printed "<<endl;
    } else cout<<" MyCrossSection::Initialize covariance matrix not found igrid= "<<igrid<<endl;

    if (debug) cout<<" MyCrossSection::Initialize: npdf= "<<pdfdata.size()<<endl;
    if (mydata[igrid]->CovarianceMatrixOk()){
      if (debug) {
	cout<<" MyCrossSection::Initialize: before CalcChi2 "<<endl;
	mcorr->Print();
      }

      for (int ipdf=0; ipdf<pdfdata.size(); ipdf++) {
	TGraphAsymmErrors *pdftmp=myband[igrid]->GetPdfBand(ipdf);
	if (!pdftmp) cout<<" MyCrossSection::Initialize: pdftmp not found ! "<<endl;
	double chi2=this->CalcChi2(pdftmp, mydata[igrid]->GetTGraphTotErr(), mcorr);
	if (debug) cout<<" MyCrossSection::Initialize: igrid= "
		       <<igrid<<" ipdf= "<<ipdf<<" chi2= "<<chi2<<endl;
	mychi2.push_back(chi2);
      }
    } else cout<<" MyCrossSection::Initialize:  no correllation matrix found "<<endl;

    vpdfchi2.push_back(mychi2);

    if (debug) cout<<" MyCrossSection::Initialize: finished chi2 calculation "<<endl;


    //COMPUTE DENOMINATOR
    string denomName = "";
    int denomMarkerStyle, denomMarkerColor, denomLineColor, denomLineStyle, denomFillColor=kGray, denomFillStyle, denomErrorBar;
    TGraphAsymmErrors* denomTop, *denomBot;
    
    if(debug) cout<<cn<<mn<<" Getting ratio denominator settings..."<<endl;
    if( IsRatioDenominator("data") ) {
      if(debug) cout<<cn<<mn<<" Denominator is DATA"<<endl;      
      TGraphAsymmErrors* g = mydata[igrid]->GetTGraphTotErr();
      denomName            += mydata[igrid]->GetDataName()+"/"+mydata[igrid]->GetDataName();
      denomMarkerStyle     = g->GetMarkerStyle();
      denomMarkerColor     = g->GetMarkerColor();
      denomLineColor       = g->GetMarkerColor();
      denomLineStyle       = g->GetLineStyle();
      //denomFillColor       = kGray;//mydata[igrid]->GetDataMarkerStyle();
      denomFillStyle       = g->GetFillStyle();
      denomTop             = g;
      denomBot             = g; //same as Top
    } else if ( IsRatioDenominator("reference") ) {
      if(debug) cout<<cn<<mn<<" Denominator is REFERENCE"<<endl;      
      //TGraphAsymmErrors* g = TH1TOTGraphAsymm(GetNormalisedReference(igrid)); //old
      TGraphAsymmErrors* g = GetNormalisedReferenceAsGraph(igrid, false); //new
      g->GetName();
      denomName            += g->GetName()+igrid;
      denomName            +="/";
      denomName            += g->GetName()+igrid;
      denomMarkerStyle     = this->GetMarkerStyle(igrid);
      denomMarkerColor     = this->GetMarkerColor(igrid);
      denomLineColor       = refhistlinecolor[igrid];
      denomLineStyle       = refhistlinestyle[igrid];
      //denomFillColor       = refhistlinecolor[igrid];
      denomFillStyle       = refhistlinecolor[igrid];
      denomTop             = g;
      denomBot             = g; //same as Top
    } else if ( IsRatioDenominator("convolute") ) {
      //TODO - handle multiple pdfs case
      if(debug) cout<<cn<<mn<<" Denominator is CONVOLUTE"<<endl;      
      TGraphAsymmErrors* g = NULL;
      myband[igrid]->ComputePDFBandRatio(g);
      denomBot = myband[igrid]->GetPdfBand(0); //TODO - make for all PDFs
      //currently only used the single denominator for the first pdf (0).
    }
     
    //normal process if denom was data or reference; myband handles the convolute case
    if ( IsRatioDenominator("convolute") == false ) { 
      cout<<cn<<mn<<" Computing denominator... "<<endl;
      ratiodenom.push_back(myTGraphErrorsDivide(denomTop, 
						denomBot, 2)); //TODO - change error bar saving?
      ratiodenom[igrid]->SetName(TString(denomName));
      ratiodenom[igrid]->SetMarkerStyle(denomMarkerStyle);
      ratiodenom[igrid]->SetMarkerColor(denomMarkerColor);
      ratiodenom[igrid]->SetLineColor(denomLineColor);
      ratiodenom[igrid]->SetLineStyle(denomLineStyle);
      ratiodenom[igrid]->SetFillColor(denomFillColor);
      ratiodenom[igrid]->SetFillStyle(denomFillStyle);
      
      if(debug) {
	cout<<cn<<mn<<" Reporting ratio denominator Print:"<<endl;
	ratiodenom[igrid]->Print("all"); //TEST
      }
     }


    // COMPUTE NUMERATOR(S)
    if(debug) cout<<cn<<mn<<" Getting Ratio Numerator settings..."<<endl;
    string name = "";
    if( IsRatioNumerator("data") == true ) {
      if(debug) cout<<cn<<mn<<" DATA is a ratio numerator, computing..."<<endl;

      TGraphAsymmErrors* g = mydata[igrid]->GetTGraphTotErr();
      ratiodata.push_back(myTGraphErrorsDivide(g,
					       denomBot, 2));
      name += g->GetName()+igrid;
      name += "/";
      name += denomBot->GetName()+igrid;
      ratiodata[igrid]->SetName(TString(denomName));
      ratiodata[igrid]->SetMarkerStyle(this->GetMarkerStyle(igrid));//g->GetMarkerStyle());
      ratiodata[igrid]->SetMarkerColor(this->GetMarkerColor(igrid));//g->GetMarkerColor());
      ratiodata[igrid]->SetMarkerSize(g->GetMarkerColor());
      ratiodata[igrid]->SetLineColor(kBlack); //TODO - set color depending on occurances per frame?
      ratiodata[igrid]->SetLineStyle(g->GetLineStyle());
      ratiodata[igrid]->SetFillColor(g->GetFillColor());
      ratiodata[igrid]->SetFillStyle(g->GetFillStyle());    
    
      if(debug) {
	cout<<cn<<mn<<" Reporting ratio numerator DATA Print"<<endl;
	ratiodata[igrid]->Print("all");
      }
    }
    
    if( IsRatioNumerator("reference") == true ) {
      if(debug) cout<<cn<<mn<<" REFERENCE is a ratio numerator, computing..."<<endl;
             
      TH1* href = GetNormalisedReference(igrid);
      //TGraphAsymmErrors* g = TH1TOTGraphAsymm(href); // old
      TGraphAsymmErrors* g = GetNormalisedReferenceAsGraph(igrid, true); // new
      ratioreference.push_back(myTGraphErrorsDivide(g,
						    denomBot, 0)); 
      //error bars were not meaninful here, so set to zero
      name += g->GetName()+igrid;
      name += "/";
      name += denomBot->GetName()+igrid;
      ratioreference[igrid]->SetName(TString(denomName));
      ratioreference[igrid]->SetMarkerStyle(g->GetMarkerStyle());
      ratioreference[igrid]->SetMarkerColor(g->GetMarkerColor());
      ratioreference[igrid]->SetMarkerSize(0);
      ratioreference[igrid]->SetLineColor(href->GetLineColor());//refhistlinestyle[igrid]);
      ratioreference[igrid]->SetLineStyle(href->GetLineStyle());
      ratioreference[igrid]->SetFillColor(g->GetFillColor());
      ratioreference[igrid]->SetFillStyle(g->GetFillStyle());    
    
      if(debug) {
	cout<<cn<<mn<<" Reporting ratio numerator REFERENCE Print"<<endl;
	ratioreference[igrid]->Print("all");
      }
    }
    
    if( IsRatioNumerator("convolute") ){
      if(debug) cout<<cn<<mn<<" CONVOLUTE is a ratio numerator, computing..."<<endl;

      myband[igrid]->ComputePDFBandRatio(denomBot); //TEST - origonal
    }
  } // igrid loop

  if (debug) cout<<" MyCrossSection::Initialize: end of igrid loop"<<endl;



  // grid in frames
  gridinframe.clear();
  //gridinframedata.clear();

  vframe vigrid; 

  for (int iframe=0; iframe<this->GetFrameNumber(); iframe++) {
    vigrid.clear(); 
    for (int igrid=0; igrid<this->GetNGrid(); igrid++) {
      int jframe=this->GetFrameID(igrid);
      if (debug) cout<<" MyCrossSection::Initialize: igrid= "<<igrid
		     <<" iframe= "<<iframe<<" jframe= "<<jframe<<endl;
      if (jframe==iframe) {
	if (debug) cout << " MyCrossSection::Initialize: igrid= " << igrid 
                        << " push back this grid" << endl;
	vigrid.push_back(igrid);
      }
    }

    if (debug) cout << " MyCrossSection::Initialize: iframe= "<<iframe 
		    << " push back this grid vector size= "<<vigrid.size()
		    << endl;
    if (debug) {
      cout<< " MyCrossSection::Initialize: vigrid vector size= "<<vigrid.size()<<endl;
      for (int i=0; i< vigrid.size(); i++) {
	cout<<" MyCrossSection::Initialize: vigrid["<<i<<"]="<<vigrid[i]<<endl;
      }
    }
    gridinframe.push_back(vigrid);

    if (debug) {
      cout << " MyCrossSection::Initialize: print gridinframe.size()= "
	   <<  gridinframe.size()<<endl;
      for (int i=0; i< gridinframe.size(); i++) {
	for (int j=0; j< gridinframe[i].size(); j++) {
	  cout<<" MyCrossSection::Initialize: gridinframe["<<i<<"]["
	      <<j<<"]= "<<gridinframe[i][j]<<endl;
	}
      }
    }

    const double BIG=1.e30;
    double ymin= BIG;
    double ymax=-BIG;
    double xmin= BIG;
    double xmax=-BIG;
    double datascalex=1.2;
    double scaley=1.2;
    double x,y;

    vframedata vigriddata; 
    vigriddata.clear(); 

    if (debug) cout << " MyCrossSection::Initialize: iframe= "
		    <<iframe<<endl;

    if (debug) cout << " MyCrossSection::Initialize: gridinframe["
		    <<iframe<<"].size()= "<<gridinframe[iframe].size()
		    <<endl;


    for (int i=0; i<gridinframe.size(); i++) {
      //if (debug) cout<<" MyCrossSection::Initialize: i= "<<i<<endl;
      int igrid=gridinframe[iframe][i];
      int jframe=this->GetFrameID(igrid);
      if (iframe!=jframe) continue;
      //if (debug) cout<<" MyCrossSection::Initialize: igrid= "<<igrid<<endl;

      if (debug) cout<<" MyCrossSection::Initialize: gridinframe["<<iframe<<"]["
		     <<i<<"]= "<<gridinframe[iframe][i]<<endl;

      if (this->GetDataOk(igrid)) {
	if (debug) cout<<" MyCrossSection::Initialize: igrid= "<<igrid<<" iframe= "<<iframe
		       <<" push "<<mydata[igrid]->GetDataName()<<endl;

	vigriddata.push_back(mydata[igrid]);

	if (mydata[igrid]->GetLogY1()) {scaley=2.0;}
	if (mydata[igrid]->GetLogX())  {datascalex=1.5;}
	//cout<<" datascalex= "<<datascalex<<" scaley= "<<scaley<<endl;
	y=mydata[igrid]->GetMinY();
	if (ymin>y) ymin=y;

	y=mydata[igrid]->GetMaxY();
	if (ymax<y) ymax=y;

	x=mydata[igrid]->GetMinX();
	if (xmin>x) xmin=x;

	x=mydata[igrid]->GetMaxX();
	if (xmax<x) xmax=x;

      } else cout<<" MyCrossSection::Initialize: data is not ok ?? "<<endl;
    } 

    //gridinframedata.push_back(vigriddata);

    if (debug) cout<<" MyCrossSection::Initialize: datascalex= "<<datascalex<<" scaley= "<<scaley<<endl;
    if   (xmin>0) xmin/=datascalex;
    else  xmin*=datascalex;

    if   (xmax>0) xmax*=datascalex;
    else  xmax/=datascalex;

    if   (ymin>0) ymin/=scaley;
    else  ymin*=scaley;

    if   (ymax>0) ymax*=scaley;
    else  ymax/=scaley;

    if (debug)
      cout<<" MyCrossSection::Initialize: iframe= "<<iframe<<" xmin= "
	  <<xmin<<" xmax= "<<xmax<< " ymin= "<<ymin<<" ymax= "<<ymax<<endl;


    xminframe.push_back(xmin);
    xmaxframe.push_back(xmax);
    yminframe.push_back(ymin);
    ymaxframe.push_back(ymax);

  }

  if (debug) cout<<" MyCrossSection::Initialize: finished  "<<endl;
  return;

}

void MyCrossSection::ReadSteeringFile(const std::string &filename) {
	std::string mn = "ReadSteering: ";
	
	//Instantiate a steering file object with the filename
	this->steeringFile = new SteeringFile(filename);
	
	//Create a local reference (for syntactical simplicity henceforth)
	SteeringFile &steeringFile = *(this->steeringFile);
	
	//Parse the Steering File
	try {
		steeringFile.Parse();
	} catch (const std::exception &e) {
		std::cerr << cn << mn << e.what() << std::endl;
		std::cerr << cn << mn << "FATAL: Could not parse the steering file: " << filename << std::endl;
		std::cerr << cn << mn << "\tGracefully exiting..." << std::endl;
		
		//Delete the object's steering file instance and exit
		delete this->steeringFile;
		exit(-1); 
	}
	
	//Print the Steering File
	if(debug) {
		steeringFile.Print();	
	}	
}




















void MyCrossSection::ReadSteering(char fname[100]) {
  //
  // read steering from file fname
  //
  string mn = "ReadSteering:"; //Method name, for printing

  steername=fname; 
  if (debug) cout<<" MyCrossSection::ReadSteering: steering "<<steername<<endl;

  ifstream infiletmp(steername.c_str(), ios::in);
  //if( debug ) std::cout << " MyCrossSection::ReadSteering: Why am I not here? " << std::endl;
  if(!infiletmp){ // Check open
    cerr << " MyCrossSection::ReadSteering Can't open " << steername <<"\n";
    infiletmp.close();
    exit (1);
  } else {
    if (debug) cout <<" MyCrossSection::ReadSteering: read data file: " << steername << endl;
  }

  int iline=0;
  int nsyst=1;

  char line[1024]; char text[256]; char name[256];  int intVal; double doubleVal;

  while (1) {
    //if (debug) cout << " good: " << infile.good() << " eof: " << infile.eof() << endl;
    if (!infiletmp.good()) break;
    infiletmp.getline(line,sizeof(line),'\n');
    if(line[0] != 'g') continue;
    if (debug) cout << "line " << line << " compare with gridname\n";

    if (strstr(line,"gridname")!=0) {
      if( strstr(line,"gridnamedefaultdir")!=0 ) continue;
      if( strstr(line,"gridnamebasedir")!=0 ) continue;
      if( strstr(line,"gridnamesystbasedir")!=0 ) continue;
      sscanf(line," %s %[^\n] ",text, name);
      if (debug) cout<<" MyCrossSection::ReadSteering: "<<text<<" "<<name<<endl;
      string myname=name;
      if (debug) cout << "Push back " << myname << endl;
      //MyGrid *tmpgrid = new MyGrid(myname);
      gridname.push_back(trim(myname)); //CHANGED
      //gridname.push_back(tmpgrid); //CHANGED

      //MyGrid *test_mygrid = new MyGrid( myname ); //TEST
      //test_mygrid->Print();
      //test_mygrid->GetReference()->Print("all");
      //exit( 0 ); //TEST
    }
  }

  
  if (debug) cout<<" MyCrossSection::ReadSteering: number of grids "<<gridname.size()<<endl;

  // set up options vector, one option for each grid
  for (int igrid=0; igrid<gridname.size(); igrid++){
    if (debug) cout<<" MyCrossSection::ReadSteering: gridname["<<igrid<<"]= "<<gridname[igrid]<<endl;
    markerstyle.push_back(20);
    markercolor.push_back(1);
    //mcscalex.push_back(1.);
    //datascalex.push_back(1.);
    //scaley.push_back(1.);
    frameid.push_back(-1);
    divideid.push_back(-1);
    refhistlinestyle.push_back(1.);
    refhistlinecolor.push_back(1.);
  }

  ifstream infile(steername.c_str(), ios::in);

  int igrid=-1;


  string curLine;
  string optionName;
  string optionValue;
  int w = 20; //arbitrary width units that make formatting nice

  // read all options from text file
  while (infile.good()) {
    getline(infile, curLine);

    curLine = trim(curLine); //remove leading and trailing white space
    int optionSep = curLine.find(' '); //optioname ends after first space

    
    optionName  = curLine.substr(0, optionSep);
    optionValue = trim(curLine.substr(optionSep+1, curLine.size())); //'optionValue' could be broken up further if needed


    
    if (debug) {
      cout<<cn<<mn<<" Read in:>>>>>>>>>>>"
	  <<"\n"<<setw(w)<<"currentLine: \""<<curLine<<"\""
	  <<"\n"<<setw(w)<<" optionName: \""<<optionName<<"\""
	  <<"\n"<<setw(w)<<"optionValue: \""<<optionValue<<"\""
	  <<"\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    }
    

    if(curLine[0] != '%' && !curLine.empty() ) { //ignore lines beginning with comments - could be done better
      
      if ( optionName == "debug" ) {
	debug=true;
	if (debug) cout<<cn<<mn<<" Debug turned on"<<endl;
      } else if ( optionName == "datanamedir" ) {
	datanamedir = optionValue;
      } else if ( optionName == "divideid" ) {
	sscanf( optionValue.c_str(), "%d", &intVal );
	divideid[igrid]=intVal;
      } else if ( optionName == "dataname" ) {
	dataname.push_back(optionValue);
      } else if ( optionName == "frameid" ) {
	sscanf( optionValue.c_str(), "%d", &intVal);
	frameid[igrid]=intVal;
      } else if ( optionName == "facscale" ) {
	if (debug) cout<<cn<<mn<<" OPTION: "<<optionName<<", VALUE: "<<optionValue<<endl;
	std::vector<string> *parsedNames;
	char delimeter = ' ';
	parsedNames = ParseString( optionValue, delimeter);
	
	if (debug) cout<<cn<<mn<<" found '"<<parsedNames->size()<<"' fac scales "<<endl;
	if (debug) cout<<cn<<mn<<" fac for grid: '"<<optionValue<<"'"<<endl;
	
	for (int i=0; i<parsedNames->size(); i++){
	  facscale.push_back(atof( (parsedNames->at(i)).c_str()) );
	}
	
	do_FactorizationScale=true;
      } else if ( optionName == "Gridnamedir" ) {
	gridnamebasedir = optionValue;
      } else if ( optionName == "gridnamebasedir" ) {
	gridnamebasedir = optionValue;
      } else if ( optionName == "gridnamesystbasedir" ) {
	gridnamesystbasedir = optionValue;
      } else if ( optionName == "gridnamedefaultdir" ) {
	gridnamedefaultdir = optionValue;
      } else if ( optionName == "gridname" ) {
	igrid++;
      } else if ( optionName == "leglabel" ) {
	leglabel.push_back(optionValue);
      } else if ( optionName == "markerstyledata" ) {
	sscanf( optionValue.c_str(), "%d ", &intVal);
	if (igrid<0) cerr<<cn<<mn<<" Something wrong ! '"<<intVal<<"'"<<endl;
	markerstyle[igrid]=intVal;
      } else if ( optionName == "markercolordata" ) {
	sscanf( optionValue.c_str(), "%d", &intVal);
	markercolor[igrid]=intVal; 
      } else if ( optionName == "ntupname" ) {
	ntupname = optionValue; 
      } else if ( optionName == "overlaystyle" ) {
	overlaynames.clear(); //remove any defaults
	
	//sscanf(line," %s %[^\n] ",text, name);
	if (debug) cout<<cn<<mn<<" Overlaystyle: "<<optionValue<<endl;
	std::vector<string> *parsedNames;
	//std::string allNames = name;
	char delimeter = ',';
	parsedNames = ParseString(optionValue, delimeter);
	
	cout<<cn<<mn<<" overlay found '"<<parsedNames->size()<<"' to overlay"<<endl;
	for (int iname=0; iname<parsedNames->size(); iname++) {
	  cout<<" \tname: \""<<parsedNames->at(iname)<<"\""<<endl;
	  //  overlaynames.push_back(parsedNames->at(iname));
	}
	overlaynames = *parsedNames;
	
	if( this->validateOverlayStyle(overlaynames) == false ) {
	  cerr<<cn<<mn<<" Invalid overlay stye detected!"<<endl;
	  exit(1);
	} 
      } else if ( optionName == "pdffunction" ) {
	pdf_function = optionValue;
      } else if ( optionName == "plotmarker" ) {
	plotmarker=true;
      } else if ( optionName == "plotband" ) {
	plotband=true;
      } else if ( optionName == "ploterrorticks" ) {
	ploterrorticks=true;
      } else if ( optionName == "plotchi2" ) {
	plotchi2=true;
      } else if ( optionName == "pdfdata" ) {
	//sscanf(line," %s %[^\n] ",text, name);
	if (debug) cout<<cn<<mn<<" pdfdata: "<<optionName<<", pdf(s): "<<optionValue<<endl;
	std::vector<string> *parsedNames;
	//std::string pdfSteeringFileNames = name;
	char delimeter = ',';
	parsedNames = ParseString(optionValue, delimeter);
	
	//for (int iname=0; iname<parsedNames->size(); iname++)
	//  pdfdata.push_back(parsedNames->at(iname));
	
	pdfdata = *parsedNames;
      } else if ( optionName == "plotstaggered" ) {
	plotstaggered=true;
      } else if ( optionName == "ratiotitlelabel" ) {
	ratiotitlelabel = optionValue;
      } else if ( optionName == "reflinestyle" ) {
	sscanf( optionValue.c_str(), "%d", &intVal);
	refhistlinestyle[igrid]=intVal;
      } else if ( optionName == "reflinecolor" ) {
	sscanf( optionValue.c_str(), "%d", &intVal);
	if (debug) cout<<cn<<mn<<" reflinecolor:  "<<intVal<<endl;
	refhistlinecolor[igrid]=intVal;
      } else if ( optionName == "ratiostyle" ) {
	rationames.clear(); //remove any defaults for overlay
	if (debug) cout<<cn<<mn<<" RatioStyle: "<<optionValue<<endl;
	
	std::vector<string> *parsedNames = this->ParseRatioStyle( optionValue );
	rationames = *parsedNames;
	
	if(debug) cout<<cn<<mn<<" Computed ratiostyle to be: "<<GetRatioStyleString()<<endl;
	
      } else if ( optionName == "renscale" ) {
	if (debug) cout<<cn<<mn<<" RENSCALE: "<<optionName<<", value(s): "<<optionValue<<endl;
	
	std::vector<string> *parsedNames; //TODO - clean this up after use?
	char delimeter = ' ';
	parsedNames = ParseString(optionValue, delimeter);
	if (debug) cout<<cn<<mn<<" found '"<<parsedNames->size()<<"' ren scales"<<endl;
	if (debug) cout<<cn<<mn<<" pdfsteering for grid: "<<optionValue<<"'"<<endl;
	for (int i=0; i<parsedNames->size(); i++){
	  renscale.push_back(atof( (parsedNames->at(i)).c_str()) );
	}
	do_RenormalizationScale=true;
      } else if ( optionName == "subprocesssteername" ) {
	subprocesssteername = optionValue;
      } else if ( optionName == "vardesc" ) {
	vardesc.push_back(optionValue);
      } else if ( optionName == "xlegend" ) {
	sscanf( optionValue.c_str(), "%f", &xlegend);
      } else if ( optionName == "xunits" ) {
	xunits = optionValue;
      } else if ( optionName == "xerroroff" ) {
	xerroroff = true;
      } else if ( optionName == "ylegend" ) {
	sscanf( optionValue.c_str(), "%f", &ylegend);
      } else if ( optionName == "ymaxoverlay" ) {
	sscanf( optionValue.c_str(), "%lf", &ymaxoverlay);
      } else if ( optionName == "yminoverlay" ) {
	sscanf( optionValue.c_str(), "%lf", &yminoverlay);
      } else if ( optionName == "ymaxratio" ) {
	sscanf( optionValue.c_str(), "%lf", &ymaxratio);
      } else if ( optionName == "yminratio" ) {
	sscanf( optionValue.c_str(), "%lf", &yminratio);
      } else if ( optionName == "yunits" ) {
	yunits = optionValue;
      } else if ( optionName == "displaystyle" ) {
	displaynames.clear(); //remove any defaults
		
	std::vector<string> *parsedStyles;
	char delimeter = ',';
	parsedStyles = ParseString(optionValue, delimeter);
	
	cout<<cn<<mn<<" displaystyles found '"<<parsedStyles->size()<<"' styles"<<endl;
	for (int iname=0; iname<parsedStyles->size(); iname++) {
	  cout<<" \tstyle: \""<<parsedStyles->at(iname)<<"\""<<endl;
	  //  overlaynames.push_back(parsedNames->at(iname));
	}
	displaynames = *parsedStyles;
	
	if( this->validateDisplayStyle(displaynames) == false ) {
	  cerr<<cn<<mn<<" Invalid display stye detected!"<<endl;
	  exit(1);
	} 
	
	
      } else {	
	cerr<<cn<<mn<<" WARN: Invalid steering option found '"<<line<<"'"<<endl;
	//exit(0); //stop if an invalid option found, it's probably an error!
      } //end if chain
    } //end comment check
  } //end file reading

  
  if(debug) { 
    cout<<cn<<mn<<" Finished reading steering. REPORT:"<<endl;
    Print();
  }

  infile.close(); //cleanup

  return;
};


void MyCrossSection::Print() {
  //
  // Display internal setting states read from steering
  //

  int w=30; //arbitrary spacing size that makes the formatting look pretty

  cout<<" MyCrossSection::Print: >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
      <<"\n"<<setw(w)<<"steering:"            <<setw(w)<<steername
      <<"\n"<<setw(w)<<"subprocesssteername:" <<setw(w)<<subprocesssteername
      <<"\n"<<setw(w)<<"pdf_function:"        <<setw(w)<<pdf_function
    //<<"\n"<<setw(w)<<"ntupdir= "<<ntupdirinput<<" ntupoutput= "<<ntupdiroutput
      <<"\n"<<setw(w)<<"Dir of grids:"        <<setw(w)<<gridnamebasedir
      <<"\n"<<setw(w)<<"Dir of data:"         <<setw(w)<<datanamedir
      <<"\n"<<setw(w)<<"Num of grids:"        <<setw(w)<<gridname.size()
      <<"\n"<<setw(w)<<"Display Style:"       <<setw(w)<<GetDisplayStyleString()
      <<"\n"<<setw(w)<<"Overlay Style:"       <<setw(w)<<GetOverlayStyleString()
      <<"\n"<<setw(w)<<"Ratio Style:"         <<setw(w)<<GetRatioStyleString()<<endl;

  for (int  i = 0; i <   gridname.size(); i++) {
    if (this->GetDataOk(i)) {
      cout<<"\n"<<setw(w)<<"INFO FOR GRID NUM:"<<setw(w)<<i
	  <<"\n"<<setw(w)<<"dataname.size():"  <<setw(w)<<dataname.size()
	//<<"\n"<<setw(w)<<"events.size():"    <<setw(w)<<events.size()
	  <<"\n"<<setw(w)<<"gridname.size():"  <<setw(w)<<gridname.size()
                
	  <<"\n"<<setw(w)<<"grid steering:"<<setw(w)<<gridname[i]
	  <<"\n"<<setw(w)<<"data steering:"<<setw(w)<<dataname[i]
	//<<"\n"<<setw(w)<<"events:"       <<setw(w)<<events[i]
	  <<"\n"<<setw(w)<<"style:"        <<setw(w)<<this->GetMarkerStyle(i)
	  <<"\n"<<setw(w)<<"color:"        <<setw(w)<<this->GetMarkerColor(i)
	  <<"\n"<<setw(w)<<"frameid:"      <<setw(w)<<this->GetFrameID(i)
	  <<"\n"<<setw(w)<<"divideid:"     <<setw(w)<<this->GetDivideID(i)<<endl;
    } else {
      cout<<"\n"<<setw(w)<<"INFO FOR GRID NUM:"<<setw(w)<<i
	  <<"\n"<<setw(w)<<"grid:"             <<setw(w)<<gridname[i]
	  <<"\n"<<setw(w)<<"frameid:"          <<setw(w)<<this->GetFrameID(i)
	  <<"\n"<<setw(w)<<"divideid:"         <<setw(w)<<this->GetDivideID(i)<<endl;
    }
  }
    
  cout<<" MyCrossSection::Print: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
}


bool MyCrossSection::file_exists(const string& s) {
  //
  // Helper to make sure a file exists
  //

  if ( FILE* testfile=fopen(s.c_str(),"r") ) {
    fclose(testfile);
    return true;
  }
  else return false;
}


void MyCrossSection::Normalise(TH1D* h1, double yscale, double xscale=1., bool normtot=false, bool divbinwidth=false)
{
  //
  // reference in ApplGrid is not divided by binwidth, by convention.
  // We must divide it if asked because it is not already divided
  //

  /*
  string mn="Normalise:";

  double x, y, ey;
  double sigtot=0.;

  for (int i=0; i<=h1->GetNbinsX(); i++) {
    y=h1->GetBinContent(i)*yscale;

    double binw = 1.;
    if (divbinwidth) binw = h1->GetBinWidth(i); 

    //sigtot+=y;
    sigtot+=y*binw;
  }

  for (int i=0; i<=h1->GetNbinsX(); i++) {
    
    double binw = 1;
    if(divbinwidth) binw = h1->GetBinWidth(i);

    cout<<cn<<mn<<" divbinwidth ON, binwidth: "<<binw<<endl;

    y =h1->GetBinContent(i)*yscale;
    ey=h1->GetBinError(i)  *yscale;
    x =binw                *xscale;
    if (x!=0) h1->SetBinContent(i,y/x);
    else      h1->SetBinContent(i,0.);
    //cout << "BinWidth: " << h1->GetBinWidth(i)  << ", xscale: " << xscale 
    //     << ", x: " << x << ", bin content: " << h1->GetBinContent(i) << "\n";

    if (x!=0) h1->SetBinError(i,ey/x);
    else      h1->SetBinError(i,0.);
  }

  if (normtot) {
    //if (debug) cout<<" MyCrossSection::Normalise: sigtot= "<<sigtot<<endl;
    
    if(sigtot!=0.) {
      h1->Scale(1. / sigtot);
    } else {
      cerr<<cn<<mn<<" ERROR: Division by zero!! sigtot: "<<sigtot<<endl;
    }
  }
  */

  /*
    for ( int ibin=1 ; ibin<=h->GetNbinsX() ; ibin++ )
    {
    double width = h->GetBinLowEdge(ibin+1) - h->GetBinLowEdge(ibin);
    h->SetBinContent( ibin, h->GetBinContent(ibin)/width );
    }
  */
  //if (debug) std::cout << " MyCrossSection::Normalise: return" << std::endl;
  




  string mn="Normalise:";

  double x, y, ey;
  double sigtot=0.;

  if(debug){
    cout<<cn<<mn<<" Hist ORIGIONAL Print: "<<endl;
    h1->Print("all");
  }


  //for (int i=0; i<=h1->GetNbinsX(); i++) {
  for (int i=1; i<=h1->GetNbinsX(); i++) {
    y=h1->GetBinContent(i)*yscale;

    //double binw = 1.;
    //if (divbinwidth) binw = h1->GetBinWidth(i); 
    double binw = binw = h1->GetBinWidth(i); //<---changed that fixed everything...

    //sigtot+=y;
    sigtot+=y*binw;

    if(debug) cout<<" MyCrossSection::Normalise: Hist calc: "
		  <<"content="<<h1->GetBinContent(i)
		  <<", yscale="<<yscale
		  <<", y="<<y
		  <<", binw="<<binw
		  <<", cumulative sigtot="<<sigtot<<endl;
  }

  if(debug) cout<<"MyCrossSection::Normalise: Hist sigtot final: "<<sigtot<<endl;

  double scal=yscale;
  //for (int i=0; i<=h1->GetNbinsX(); i++) {
  for (int i=1; i<=h1->GetNbinsX(); i++) { //TODO - Need to exclude underflow but include overflow
    //if (normtot) if(sigtot!=0.) scal=yscale/sigtot;
    if (normtot) {
      if(sigtot != 0.0) { 
	scal=yscale/sigtot;
      } else {
	cerr<<" MyCrossSection::Normalise: WARN: Sigtot was ZERO, avoiding division by 0.";
	scal = 0.;
      }
    }

    double binw = 1;
    if(divbinwidth)  binw = h1->GetBinWidth(i);
    cout<<cn<<mn<<" Hist binwidth="<<binw<<" at bin "<<i<<endl;
   
    y =h1->GetBinContent(i)*scal;//*binw;
    ey=h1->GetBinError(i)  *scal;//*binw;
    x =binw                *xscale;//*h1->GetBinWidth(i);

    cout<<"\t DATA:  content: "<<h1->GetBinContent(i)<<", scal: "<<scal<<endl; 
    cout<<"\t DATA:  Numerator: "<<y<<", Denominator: "<<x<<", RES: "<<(y/x)<<endl;
    cout<<"\t ERROR: Numerator: "<<ey<<", Denominator: "<<x<<", RES: "<<(ey/x)<<endl;
    if (x!=0) h1->SetBinContent(i,y/x);
    else      h1->SetBinContent(i,0.);
    
    if (x!=0) h1->SetBinError(i,ey/x);
    else      h1->SetBinError(i,0.);
  }
  /*
  if (normtot) {
    if(sigtot!=0.) {
      h1->Scale(1. / sigtot);
    } else cerr<<cn<<mn<<" ERROR: Division by zero!! sigtot: "<<sigtot<<endl;
  }
  */

  if(debug){
    cout<<cn<<mn<<" Hist NORMALISED Print: "<<endl;
    h1->Print("all");
  }

  return;
}


void MyCrossSection::Normalise(TGraphAsymmErrors* g1, double yscale, double xscale=1., bool normtot=false, bool divbinwidth=true) {
  //
  // ApplGrid convolute is already divided by binwidth, by convention
  // We must NOT divide graph if divbinwidth is on because it is already divided
  //

  string mn="Normalise:";
  // convolution is divide by binwidth
  Double_t x, y, ey;
  Double_t sigtot=0.;

  Double_t* X1 = g1->GetX();
  Double_t* Y1 = g1->GetY();
  Double_t* EXhigh1 = g1->GetEXhigh();
  Double_t* EXlow1  = g1->GetEXlow();
  Double_t* EYhigh1 = g1->GetEYhigh();
  Double_t* EYlow1  = g1->GetEYlow();

  if (!g1) cout<<" MyCrossSection::Normalise graph not found ! "<<endl;
  int Nbin=g1->GetN();
  //cout<<" MyCrossSection::Normalize: n= "<<Nbin<<endl;

  for (Int_t i=0; i<Nbin; i++) {
    y=Y1[i]*yscale;
    double binw=EXhigh1[i]+EXlow1[i];
    //cout<<" binw= "<<binw<<endl;
    if (divbinwidth) sigtot+=y*binw;
    else             sigtot+=y;
    sigtot+=y; //TODO - should this be done twice here?


    if(debug) cout<<"MyCrossSection::Normalise: Graph sigtot="<<sigtot
		  <<", content="<<Y1[i]
		  <<", yscale="<<yscale
		  <<", binw="<<binw<<endl; 
  }

  if(debug) cout<<"MyCrossSection::Normalise: Graph sigtot="<<sigtot<<endl;

  double scal=yscale;
  for (Int_t i=0; i<Nbin; i++) {
    if (normtot) {
      if(sigtot != 0.0) { 
	scal=yscale/sigtot;
      } else {
	cerr<<"MyCrossSection::Normalise: WARN: Sigtot was ZERO, avoiding division by 0.";
	scal = 0.;
      }
    }

    //double binw=1.;
    //if (divbinwidth) 
    double binw=EXhigh1[i]+EXlow1[i];
    //if (debug) cout<<" MyCrossSection::Normalise: i= "<<i<<" scal= "<<scal<<endl;
    cout<<cn<<mn<<" Graph binwidth="<<binw<<" at bin "<<i<<endl;


    double y =Y1[i]*scal*binw/xscale;
    double yl=EYlow1[i] *scal*binw/xscale;
    double yh=EYhigh1[i]*scal*binw/xscale;
    if (divbinwidth) {
      //binw*=xscale;
      yl/=binw;  
      yh/=binw;
      y /=binw;
    }
    g1->SetPoint(i, X1[i],y);
    g1->SetPointError(i,EXlow1[i],EXhigh1[i],yl,yh);
  }

  if(debug){
    cout<<cn<<mn<<" Normalised Graph Print: "<<endl;
    g1->Print("all");
  }

  //if (debug) std::cout << " MyCrossSection::Normalise: return" << std::endl;
  return;
}

/*
TH1D *MyCrossSection::GetReference(int igrid) {
  //
  // get reference histogram from original grid
  // 

  string fname=this->GetGridName(igrid);
  if (debug) cout<<" MyCrossSection::GetReference: Grid Name "<<fname.c_str()<<endl;
  appl::grid *tmpgrid = 0;
  if (this->file_exists(fname)) {
    //if (debug) cout<<" MyCrossSection::GetReference: grid file "<<fname<<" exist ! "<<endl;
    //isBooked.push_back(false);
    if (debug) cout<<" MyCrossSection::GetReference: open "<<fname<<endl;
    tmpgrid = new appl::grid(fname);
    //if (debug) cout << " MyCrossSection::GetReference: grid CMS energy " 
    //                << tmpgrid->getCMSScale() << endl;
    //mygrid.push_back(tmpgrid);
    //if (debug) cout << " MyCrossSection::GetReference: tmpgrid pushed" << endl;
    //events.push_back(int(mygrid[igrid]->run()));
    //if (debug) cout << " MyCrossSection::GetReference: events pushed" << endl;
  } else {
    cout<<" MyCrossSection::GetReference:  file not found "<<fname.c_str()<<endl;
  }

  if (!tmpgrid) 
    cout<<" MyCrossSection::GetReference: tmpgrid not found ! "<<endl;

  TH1D *href=(TH1D*)tmpgrid->getReference();
  if (!href) {
    cout<<" MyCrossSection::GetReference: reference histo not found igrid= "<<igrid<<endl;
    cout<<" MyCrossSection::GetReference: grid name=" << this->GetGridName(igrid) << endl;
    exit(0);
  }
  //if (debug) cout <<" MyCrossSection::GetReference Got reference" <<endl;
  int ntot=tmpgrid->run();
  //if (debug) cout<<" MyCrossSection::GetReference ntot= "<<ntot << endl;
  href->Scale(1./ntot);

  return href;
}
*/



TH1D *MyCrossSection::GetReference(int igrid) {
  //
  // get reference histogram from original grid
  // 

  TH1D *href=(TH1D*)mygrid.at(igrid)->GetReference();

  if (!href) {
    cout<<" MyCrossSection::GetReference: reference histo not found igrid= "<<igrid<<endl;
    cout<<" MyCrossSection::GetReference: grid name=" << mygrid.at(igrid)->GetGridPath() << endl;
    exit(0); 
  }
  
  return href;
}


void MyCrossSection::Draw(int igrid) {
  //
  // Draw data for a grid by index
  //

  if (debug) cout << " MyCrossSection::Draw igrid " << igrid << endl;
  mydata[igrid]->DrawData();
  if (debug) mydata[igrid]->Print();
  return;
}


TGraphAsymmErrors *MyCrossSection::GetPDFRatio(int iset1, int iset2, int itype, int igrid) {
  //
  // Compute and return the ratio of two graphs 
  //
 
  //MyPDF * mypdf=t_mypdf[igrid][itype];
  TGraphAsymmErrors * mypdf=myband.at(igrid)->GetPdfBand(itype);
 
  //TString ratio_to_ref_name = (TString) gpdfband[igrid][itype]->GetName() + "_set";
  TString ratio_to_ref_name = (TString) mypdf->GetName() + "_set";
  ratio_to_ref_name +=iset1;

  if (igrid>=this->GetNGrid()){
    cout<<" MyCrossSection::GetPDFRatio igrid== "<< igrid<<endl;
    exit (0);
  }

  // if (itype>=this->GetNPDF()){ // not correct for two dim vector
  if (itype>=this->GetNPDF(igrid)){ // not correct for two dim vector
    cout<<" MyCrossSection::GetPDFRatio  "<<" itype= "<<itype<<endl;
    exit (0);
  } 


  int numPDF= this->GetNPDF(igrid);
  if (iset1>=numPDF||iset2>=numPDF||iset1<0||iset2<0){
    cout<<" MyCrossSection::GetPDFRatio iset too large iset2= "<< iset2<<" iset1= "<<iset1<<endl;
    exit (0);
  }

  if (iset2!=0) {
    ratio_to_ref_name +=TString("/set");
    ratio_to_ref_name +=iset2;
  }
  if (debug) cout<<" MyCrossSection::GetPDFRatio: "<< ratio_to_ref_name.Data()<<endl;


  TGraphAsymmErrors* gpdfratio=myTGraphErrorsDivide(myband.at(igrid)->GetPdfBand(iset1),
						    myband.at(igrid)->GetPdfBand(iset2),2);
  gpdfratio->SetLineColor(markercolor[igrid]);
  gpdfratio->SetMarkerColor(markercolor[igrid]);
  gpdfratio->SetName(ratio_to_ref_name);
  
  return gpdfratio;
}


TGraphAsymmErrors *MyCrossSection::GetPDFTypeRatio(int iset1, int iset2, int igrid) {
  //
  //
  //

  TGraphAsymmErrors * mypdf=myband.at(igrid)->GetPdfBand(iset2);
  //TString ratio_to_ref_name = (TString) gpdfband[igrid][iset2]->GetName() + "_set";
  TString ratio_to_ref_name = (TString) mypdf->GetName() + "_set";
  ratio_to_ref_name +=iset1;

  int numPDF= this->GetNPDF(igrid);
  if (iset1>=numPDF||iset2>=numPDF||iset1<0||iset2<0){
    cout<<" MyCrossSection::GetPDFTypeRatio iset too large iset2= "<< iset2<<" iset1= "<<iset1<<endl;
    exit (0);
  }

  if (iset2!=0) {
    ratio_to_ref_name +=TString("/set");
    ratio_to_ref_name +=iset2;
  }
  if (debug) cout<<" MyCrossSection::GetPDFTypeRatio: "<< ratio_to_ref_name.Data()<<endl;


  TGraphAsymmErrors* gpdfratio=myTGraphErrorsDivide(myband.at(igrid)->GetPdfBand(iset1),
						    myband.at(igrid)->GetPdfBand(iset2),2);
  gpdfratio->SetLineColor(markercolor[igrid]);
  gpdfratio->SetMarkerColor(markercolor[igrid]);
  gpdfratio->SetName(ratio_to_ref_name);
 
  return gpdfratio;
}


void MyCrossSection::DrawinFrame(int iframe) {
  //
  // Draw data overlay and ratio for either data and grid reference or PDFs
  //

  MyFrame *myframe= new MyFrame(600,600,displayOverlay,displayRatio);
  framepointer.push_back(myframe);

  double x,y;
  TString name="";

  int iplotdata=0;

  //int jframe=this->GetFrameID(iframe);
  if (debug) cout<<" MyCrossSection::DrawInFrame: gridinframe["<<iframe<<"].size()= "
		 <<gridinframe[iframe].size()<<endl;

  for (int i=0; i<gridinframe[iframe].size(); i++) {
    int igrid=gridinframe[iframe][i];
    int jframe=this->GetFrameID(igrid);
    if (iframe!=jframe) continue;
    if (debug) cout<<" MyCrossSection::DrawInFrame: i= "<<i
		   <<" igrid= "<<igrid<<" iframe= "<<iframe<<endl;

    //if (gridinframedata[iframe][igrid]>0){
    if (mydata[igrid]>0){

      //name=gridinframedata[iframe][igrid]->GetDataName();
      //if (debug) cout<<" MyCrossSection::DrawInFrame: gridinframedata["<<iframe<<"]["<<igrid<<"]="
      //          <<name.Data()<<endl;

      //if (debug) cout<<" MyCrossSection::DrawInFrame: before igrid iframe "<<endl;
      //cout<<" name= "<<gridinframedata[igrid][iframe]->GetDataName()<<endl;

      //if (debug) cout<<" MyCrossSection::DrawInFrame: before iframe igrid "<<endl;
      //cout<<" MyCrossSection::DrawInFrame:  name= "<<gridinframedata[iframe][igrid]->GetDataName()<<endl;

      //if (debug) cout<<" MyCrossSection::DrawInFrame: before print "<<endl;
      //gridinframedata[iframe][igrid]->Print();
      //if (debug) cout<<" MyCrossSection::DrawInFrame: after print "<<endl;
      //if (gridinframedata[iframe][igrid]->GetLogY1()) {
      if (mydata[igrid]->GetLogY1()) {
	//if (debug) cout<<" MyCrossSection::DrawInFrame: before setting myframe LogY myframe="<<myframe<<endl;
	myframe->SetLogY1();
      }
      //if (gridinframedata[iframe][igrid]->GetLogX ()) {
      if (mydata[igrid]->GetLogX ()) {
	//if (debug) cout<<" MyCrossSection::DrawInFrame: before setting myframe log X myframe= "<<myframe<<endl;
	myframe->SetLogX();
      }
    }
  }    /// loop over grid frames

  if (!myframe) cout<<" MyCrossSection::DrawinFrame: myframe not found ! "<<endl;
  if (gridinframe[iframe].size()<1) 
    cout<<" MyCrossSection::DrawInFrame: gridinframe[iframe].size()  too small ! "<<endl;

  //if (debug) cout<<" MyCrossSection::DrawInFrame: before setting igrid= "<<igrid<<" iframe= "<<iframe<<endl;
  int igrid=gridinframe[iframe][0];
  if (debug) cout<<" MyCrossSection::DrawInFrame: plotting igrid= "<<igrid<<" in iframe= "<<iframe<<endl;

  TString titx="", tity="";
  if (this->GetDataOk(igrid)) {
    //titx=gridinframedata[iframe][igrid]->GetTitleX();
    //tity=gridinframedata[iframe][igrid]->GetTitleY();
    //name=gridinframedata[iframe][igrid]->GetDataName();
    titx=mydata[igrid]->GetTitleX();
    tity=mydata[igrid]->GetTitleY();
    name=mydata[igrid]->GetDataName();
    name+=iframe;
  } else {
    titx=mygrid[igrid]->GetGridPath(); //CHANGED
    //titx=gridname.at(igrid)->GetGridName();
    //tity=this->GetGridName(igrid);
    name+=mygrid[igrid]->GetGridName();//this->GetGridName(igrid); //CHANGED
    //name+=gridname.at(igrid)->GetGridName();
    name+=iframe;
  }

  myframe->SetFrameName(name);
  myframe->SetSubPad2TitleX(titx);
  myframe->SetSubPad1TitleY(tity);

  double xmin=  xminframe[iframe];
  double xmax=  xmaxframe[iframe];
  double ymin=  yminframe[iframe];
  double ymax=  ymaxframe[iframe];

  if (debug)
    cout<<" MyCrossSection::DrawInFrame: frame= "<<iframe<<" xmin= "
	<<xmin<<" xmax= "<<xmax<< " ymin= "<<ymin<<" ymax= "<<ymax<<endl;


    
  //allow for fixed Y-axis min/max for overlay pad if steering file provided
  if( yminoverlay != DEFAULT_DOUBLE ) ymin = yminoverlay;
  if( ymaxoverlay != DEFAULT_DOUBLE ) ymax = ymaxoverlay;
  
  //ensure user provided appropreate bounds (ymax must be > ymin)
  if( ymin > ymax) {
    cout<<" MyCrossSection::DrawinFrame: ERROR: Invalid overlay max and minimums set!"
	<<" ymin = '"<<ymin<<"'"
	<<" ymax = '"<<ymax<<"'"<<endl;
    exit(0); //Do no further drawing if the steering file was set incorrectly
  }


  //set appropreate xmin/max and ymin/max based on data
  myframe->SetXmin( xmin);
  myframe->SetXmax( xmax);
  myframe->SetYmin1(ymin);
  myframe->SetYmax1(ymax);
  myframe->SetYmin2(1.1);
  myframe->SetYmax2(0.9);
  //myframe->DrawFrame(); //OLD
  myframe->DrawFrame2();  //NEW
    

  



  
  //TODO - new legend implimentation - START
  // TLegend *leg = BuildLegend();
  //TODO - new legend implimentation - STOP




  if (debug)
    cout<<" MyCrossSection::DrawinFrame: number of grids= "<<gridinframe[iframe].size()
	<<" for frame= "<<iframe<<endl;
 
  // const int npdf=gpdfband[igrid].size();
  //const int npdf=myband[igrid]->GetNPDF();
  const int npdf=pdfdata.size();
  if (npdf<1) {
    cout<<" MyCrossSection::DrawinFrame: No PDF information found ...plotting reference"<<endl;
  }

  const double BIG=1.36;
  double Ymin=BIG, Ymax=-BIG, Xmin=BIG, Xmax=-BIG;

  
  //if(displayOverlay == true) { //TEST

  myframe->GetSubPad1()->cd();

  /*
  //compute an optimal position for the legend for this plot
  y=ylegend;
  int namesize=0;
  if (npdf<1) {
    namesize=5; //why choose 5 as default?
    
    TString mylabel=mydata[igrid]->GetLabel();
    int mysize=TString(mylabel).Sizeof(); 
    if (mysize>namesize) namesize=mysize;
    //y-=0.1;
  } else {
    //y-=0.07*npdf; //origional
    for (int ipdf=0; ipdf<npdf; ipdf++) {
      if (debug) cout<<" MyCrossSection::DrawinFrame: label= "
		     <<this->GetPdfName(ipdf).Data()<<endl;
     
      int mysize=this->GetPdfName(ipdf).Sizeof(); 
      if (mysize>namesize) namesize=mysize;
    }
    if (plotchi2) namesize+=10;
  }

  if (debug) cout<<" MyCrossSection::DrawinFrame: xlegend= "<<xlegend
		 <<" ylegend= "<<ylegend<<endl;

  //origional
  //  double x1=xlegend, x2=0.95;
  //  double y1=y, y2=ylegend;
  //  x2+=namesize*0.02;
 

  // place legend based on the YTop and XRight position and set xLeft and YBottom by:
  //  XLeft = the length of the largest name in the legend 
  //  YBottom = the number of entries in the legend
  double x1 = xlegend-(namesize*0.02), x2=xlegend;
  double y1 = 0, y2=ylegend;
  
  
  // ydimention is based of what elements are being put in legend.
  if(npdf<1) {
    // size based on no PDFs meaning legend is full of just grid data
    y1 = ylegend-0.2;
  } else {
    // size based on number of pdfs
    y1 = ylegend-(npdf*0.11);
  }
  
  
  if (debug) cout<<" MyCrossSection::DrawinFrame: namesize="<<namesize<<", npdf="<<npdf
		 <<", x1="<<x1<<", x2="<<x2<<", y1="<<y1<<", y2="<<x2<<std::endl;

  */

  // new way of choosing Legend size
  double x1=0., y1=0., x2=0., y2=0.;
  ComputeLegendBounds(iframe, &x1, &y1, &x2, &y2);


  // prepare legend for this Plot
  TLegend *leg = new TLegend(x1,y1,x2,y2);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetMargin(0.1);
  //leg->SetTextSize(size);
  

  //const double BIG=1.36;
  //double Ymin=BIG, Ymax=-BIG, Xmin=BIG, Xmax=-BIG;
  int iframeRepeatCheck = -1; // turn on or off certain features if they are being done in the same frame twice

  for (int i=0; i<(int)gridinframe[iframe].size(); i++) {
    int igrid=gridinframe[iframe][i];
    int jframe=this->GetFrameID(igrid);
    if (iframe!=jframe) {
      //cout<<"TEST: NOT EQUAL: iframe:"<<iframe<<", jframe:"<<jframe<<endl; //TODO - check does this do anything?
      continue;
    } else {
      //cout<<"TEST: EQUAL, SKIPPING: iframe:"<<iframe<<", jframe:"<<jframe<<endl;
    }

    if (debug) cout<<" MyCrossSection::DrawinFrame: for i= " << i << " igrid= "<<igrid<<endl;
    TH1D* href=0;
    //cout << "Is data ok? " << this->GetDataOk(igrid) << endl;


    //plot the overlay portion of graph
    if(overlayReference) {
      /*
      //old
      href=this->GetNormalisedReference(igrid);
      if (!href) cout<<" MyCrossSection::DrawinFrame: reference not found ! "<<endl;
      
      //no PDF, plot reference histogram for overlay
      href->Draw("same,hist"); //no PDF overlay draw
      if (debug) {
	cout<<" MyCrossSection::DrawinFrame: Print reference histogram TEST"<<endl;
	href->Print("all");
      }
      //old
      */
      
      //new
      TGraphAsymmErrors* gref=this->GetNormalisedReferenceAsGraph(igrid, false);
      if (!gref) cout<<" MyCrossSection::DrawinFrame: reference not found ! "<<endl;
      
      gref->SetLineColor(refhistlinecolor[igrid]);
      gref->SetLineStyle(refhistlinestyle[igrid]);
      
      
      //no PDF, plot reference histogram for overlay
      gref->Draw("P,same"); //no PDF overlay draw
      if (debug) {
	cout<<" MyCrossSection::DrawinFrame: Print reference histogram: "<<endl;
	gref->Print("all");
      }
      //new
      

    }
    if(overlayConvolute) {
      if (debug) cout<<" MyCrossSection::DrawinFrame: draw band igrid= "<<igrid<<endl;
      
      //draw each PDF band on top pad of plot for overlay
      if(npdf>=1) myband.at(igrid)->DrawPDFBand();
      else        cerr<<"MyCrossSection::DrawinFrame: ERROR: No PDFs listed so none overlayed."<<endl;
    }
    


    // Draw data to the frame and add data name to the legend
    if (this->GetDataOk(igrid)) {
      if (!mydata[igrid]) cout<<" MyCrossSection::DrawinFrame: mydata["<<igrid<<"] not found "<<endl;
      else if (debug)     cout<<" MyCrossSection::DrawinFrame: mydata["<<igrid<<"]  found "<<endl;

            
   
      // Only plot data once (ignore repeats) unless repeated data is on a new frame
      if ( DoubleDataSetName(igrid) == false || iframeRepeatCheck != iframe ) { 
	//repeated data was not found on that frame
	iframeRepeatCheck = iframe;
	
	if (debug) cout<<" MyCrossSection::DrawinFrame: plot data "<<dataname[igrid]<<endl;
       	// any scaling of data is already done in the MyData class from it's own steering
	mydata[igrid]->DrawData(); // plot actual data

	
	TString mylabel=mydata[igrid]->GetLabel();	
	if (debug) cout<<" MyCrossSection::DrawinFrame: mylabel= "<<mylabel.Data()<<endl;
	
	
	// label sqrts before data on legend if MyData Steering requested it
	//@TODO Move to MyCrossSection class and to top-level steering file
	if(mydata[igrid]->GetPlotSqrts())  {
	  TString sqrtslabel = "";
	  double sqrtsval = mydata[igrid]->GetSQRTS() * GetUnitScaleX(mydata[igrid]->GetXUnits(), this->GetXUnits()); //new
	  TString sqrtsunits = mydata[igrid]->GetXUnits();

	  sqrtslabel.Form("#sqrt{s}= %.f %s", 
			  double(sqrtsval), 
			  sqrtsunits.Data() ); //TODO - too hardcoded? make generic
	  leg->AddEntry((TObject*)0, sqrtslabel, "");
	}
	
	
	//add the data label to the legend, different if only ratio is showing
	if(displayRatio && !displayOverlay) {
	  mydata[igrid]->GetTGraphTotErr()->SetFillColor(kGray);
	  leg->AddEntry(mydata[igrid]->GetTGraphTotErr(),mylabel,"f"); //TODO - make more generic?
	} else {
	  leg->AddEntry(mydata[igrid]->GetTGraphTotErr(),mylabel,"ep");
	}

      }
    } else { //Data not OK
      href=this->GetReference(igrid);
      href->SetLineColor(refhistlinecolor[igrid]);
      //TGraphAsymmErrors * gref = GetNormalisedReferenceAsGraph(igrid,false);
      if (debug) cout<<" MyCrossSection::DrawinFrame: Data not ok igrid= "<<igrid<<endl;
      if (debug) cout<<" MyCrossSection::DrawinFrame: now draw reference igrid= "<<igrid<<endl;
      if (!href){cout<<" MyCrossSection::DrawinFrame: hreference not found "<<endl; exit;}
      href->Draw("hist,same");
    }
    

    if (debug) cout<<" MyCrossSection::DrawinFrame: npdf= "<<npdf<<endl;    
    if (debug) cout<<" MyCrossSection::DrawinFrame: vpdfchi2.size()= "<< vpdfchi2.size()<<endl;
    if (debug) cout<<" MyCrossSection::DrawinFrame: vpdfchi2.at("<<igrid<<").size()= "<< vpdfchi2.at(igrid).size()<<endl;


    // when no pdfs, provide reference histogram info after each data name in legend
    //if (npdf<1) { 
    if(overlayReference) {
      cout<<" MyCrossSection::DrawinFrame: No PDF's requested: Adding references to legend."<<endl;
      
      // create a nice legend name for reference histos
      TString curLegLable = "";
      if (leglabel.size()>0) curLegLable = leglabel[igrid].c_str();
      else                   curLegLable = "reference";
      
      if (debug) cout<<" MyCrossSection::DrawinFrame: Added '"<<curLegLable<<"' to legend."<<endl;	
      TGraphAsymmErrors *gref = GetNormalisedReferenceAsGraph(igrid,false);
      leg->AddEntry(gref, curLegLable, "l");
      //leg->AddEntry(href, curLegLable, "l");
    }	    
    

    //All data names should be added to the legend before PDF names
    if(this->GetFrameID(igrid) != this->GetFrameID(igrid+1) ) {
      for (int ipdf=0; ipdf<npdf && overlayConvolute; ipdf++) {
	
	//retrieve a PDF name
	TString pdfname="";
	pdfname+= this->GetPdfName(ipdf);
	
	
	if (plotchi2) {
	  //pdfname+=" #C^2 ";
	  pdfname+=" Chi2= ";
	  char tmp[30];
	  sprintf(tmp,"%5.2f",vpdfchi2.at(igrid).at(ipdf));
	  pdfname+=TString(tmp);
	}
	if (debug) cout<<" MyCrossSection::DrawinFrame: pdfname= "<<pdfname.Data()<<endl;
	
	
	//Before adding PDF titles to legend, create a title for the PDFs within the legend
	if (ipdf==0 ) { 
	  TString pdfDataTitle="NLO QCD with:"; //"subtitle" for the following PDFs on the legend
	  leg->AddEntry((TObject*)0, pdfDataTitle.Data(), "");
	}
	
	//Add PDF titles to legend
	if (plotmarker) leg->AddEntry(myband.at(igrid)->GetPdfBand(ipdf),pdfname,"p");
	else            leg->AddEntry(myband.at(igrid)->GetPdfBand(ipdf),pdfname,"f");	
      }      
    }
    

    // Draw the filled legend always in Overlay section if overlay is being displayed
    if(displayOverlay) leg->Draw();
    
    if (debug) cout<<" MyCrossSection::DrawinFrame: legend prepared "<<endl;
  }
  //} //TEST

  // RATIO DRAWING
  myframe->GetSubPad2()->cd();
  myframe->SetSubPad2TitleOffsetX(0.8);

  // ratio label is the ratiostyle by default
  TString titlename = GetRatioStyleString();
 
  // user can overload default ratiotitle
  if (ratiotitlelabel.size()>0) titlename=ratiotitlelabel; 


  if (!this->GetDataOk(igrid)) titlename="";
  myframe->GetYAxis2()->SetTitle(titlename.Data());
  if (debug) cout<<" MyCrossSection::DrawinFrame: npdf= "<<npdf<<endl;



  
  

  // determine ratio view size
  for (int i=0; i<(int)gridinframe[iframe].size(); i++) {
    int igrid=gridinframe[iframe][i];
    int jframe=this->GetFrameID(igrid);
    if (iframe!=jframe) continue;

    if (debug) cout<<" MyCrossSection::DrawinFrame: gridinframe["<<iframe<<"]["<<i<<"] = " 
		   << gridinframe[iframe][i] 
		   << " Draw reference for igrid= "<<igrid<<endl;
  

    ComputeRatioRange(&Ymin, &Ymax, igrid);
    
    /*
    //COMPUTE RATIO WINDOW
    // check denominator window size
    if( IsRatioDenominator("data") == true ) {
      cout<<"TEST1"<<endl;
      ratiodenom[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
    } else if (IsRatioDenominator("reference") == true) {
      cout<<"TEST2"<<endl;      
      ratiodenom[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
    } else if (IsRatioDenominator("theory") == true) {
      //denom = mydata[igrid]->->GetTGraphTotErr();
      cout<<"TEST3"<<endl;      
      ymin=myband[igrid]->GetYmin();
      ymax=myband[igrid]->GetYmax();
    }
    
    if (ymin<Ymin) Ymin=ymin;
    if (ymax>Ymax) Ymax=ymax;


    //check numerator(s) window size
    if( IsRatioNumerator("data") == true ) {
      ratiodata[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
      if (ymin<Ymin) Ymin=ymin;
      if (ymax>Ymax) Ymax=ymax;
    }

    if( IsRatioNumerator("reference") == true ) {
      ratioreference[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
      if (ymin<Ymin) Ymin=ymin;
      if (ymax>Ymax) Ymax=ymax; 
    }
    
    if( IsRatioNumerator("convolute") == true ) {
      ymin=myband[igrid]->GetYmin();
      ymax=myband[igrid]->GetYmax();
      if (ymin<Ymin) Ymin=ymin;
      if (ymax>Ymax) Ymax=ymax;
    }
    */
    
    if (debug) cout<<" MyCrossSection::DrawinFrame: Ymin= "<<Ymin<<" Ymax= "<<Ymax<<endl;
  }

    


  // user can explicitly fix Y-axis min/max for ratio pad if steering file provided them
  if( yminratio != DEFAULT_DOUBLE ) Ymin = yminratio;
  if( ymaxratio != DEFAULT_DOUBLE ) Ymax = ymaxratio;
  
  //ensure ratio window size is an appropreate bounds (ymax > ymin)
  if( Ymin > Ymax ) { 
    cerr<<" MyCrossSection::DrawinFrame: ERROR: Invalid ratio max and minimums set!"
	<<" Ymin = '"<<Ymin<<"'"
	<<" YmX = '"<<Ymax<<"'"<<endl;
    exit(0); //Do no further drawing if the steering file was set incorrectly
  }
 

  //Set computed min and max on Yaxis with small buffer on top and bottom for visibility
  if (debug) cout<<" MyCrossSection::DrawinFrame: Ymin= "<<Ymin<<" Ymax= "<<Ymax<<endl;
  
  double scaleMin = 0.9, scaleMax = 1.1;
  if( !displayOverlay && displayRatio ) scaleMax = 1.3; //higher buffer in max if only a ratio

  myframe->GetYAxis2()->SetRangeUser(Ymin*scaleMin,Ymax*scaleMax); 



  // Draw ratio onto plots
  // draw data ratio (background bands) before pdf ratio (relative to bands)

  //FIRST - draw data ratio to itself in background    
  for (int i=0; i<(int)gridinframe[iframe].size(); i++) {
    int igrid=gridinframe[iframe][i];
    if (debug) std::cout<<" MyCrossSection::DrawinFrame: Drawing ratiodata for '"<<igrid<<"'"<<std::endl;          
    if(IsRatioDenominator("convolute") == true)   {
      cout<<" CONVOLUTE DENOM PLOT"<<endl;
      myband[igrid]->DrawPDFBandRatio();      
    } else {
      cout<<" REGULAR DENOM PLOT"<<endl;
      ratiodenom[igrid]->Draw("E2,same");
    }
  } 

  //SECOND - Draw the reference or pdf ratio over the data ratio in forground
  for (int i=0; i<(int)gridinframe[iframe].size(); i++) {
    int igrid=gridinframe[iframe][i];
    if (debug) cout<<" MyCrossSection::DrawinFrame: DrawPDFBandRatio igrid= "<<igrid<<endl;
    
    if( IsRatioNumerator("data")      == true ) {
      cout<<" RATIO DATA PLOT"<<endl;
      ratiodata[igrid]->Draw("p,same");
    }
    if( IsRatioNumerator("reference") == true ) {
      cout<<" RATIO REFERENCE PLOT"<<endl;
      ratioreference[igrid]->Draw("P,same");
    }
    if( IsRatioNumerator("convolute")    == true ) {
      cout<<" RATIO CONVOLUTE PLOT"<<endl;
      myband[igrid]->DrawPDFBandRatio();
    }
  }
  // finished drawing ratio (conv/data)
  
  // only display legend in ratio section if The overlay is not being shown
  if(displayRatio && !displayOverlay) leg->Draw();

  //Always plot a line at 1, where the ratios are relative to
  TLine *myline= new TLine(myframe->GetXmin(),1.,myframe->GetXmax(),1.);
  myline->Draw(); 

  myframe->GetSubPad1()->RedrawAxis();
  myframe->GetSubPad2()->RedrawAxis();

  if (debug) cout << " MyCrossSection::DrawInFrame: End of DrawInFrame" << endl;
  return;
}

void MyCrossSection::DrawReference(int igrid) {
  //
  // Draw reference histograms for grid with index igrid
  //
  TH1D *href=this->GetReference(igrid);
  if (!href) cout<<" MyCrossSection::DrawReference: reference histo not found igrid="<<igrid<<endl;
  //href->Print("all");
  href->SetLineStyle(refhistlinestyle[igrid]);
  href->SetLineColor(refhistlinecolor[igrid]);
  href->Draw("same");
  return;
}

TGraphAsymmErrors* MyCrossSection::GetNormalisedReferenceAsGraph(int igrid, bool ebars) {
  //
  // Get reference histograms for grid with index igrid
  // and normalise according to datascalex and scaley and return as a TGraph
  // 
  string mn = "GetNormalisedReferenceAsGraph:";

  TH1D *href=this->GetReference(igrid);
  if (!href) {
    cout<<cn<<mn<<" Reference histo not found igrid="<<igrid<<endl;
    return 0;
  }

  href->SetLineStyle(refhistlinestyle[igrid]);
  href->SetLineColor(refhistlinecolor[igrid]);


  

  // Divide reference by bin width before doing anything else, because the Convolute is always
  double binw, y, ey;
  for (int i=0; i<=href->GetNbinsX(); i++) {
    if(mygrid[igrid]->GetDividedByBinWidth() == false) {
      cout<<cn<<mn<<" Reference histo not divided by bin width, so we do that now."<<endl;

      binw = href->GetBinWidth(i);
      y    = href->GetBinContent(i);
      ey   = href->GetBinError(i);
      if (binw!=0) href->SetBinContent(i, y/binw);
      else         href->SetBinContent(i, 0.);
      
      if(ebars) {
	if (binw!=0) href->SetBinError(i, ey/binw);
	else         href->SetBinError(i, 0.);
      } else {
	href->SetBinError(i, 0.); //turn error bars off
      }
    }
    cout<<cn<<mn<<" Reference histo is reported to be divided by bin width, so we do nothing."<<endl;

    if(ebars == false)  href->SetBinError(i, 0.); //TODO - make more generic? - turn error bars off
  }

  /*
  // normalise by number of entries, if asked to
  if(mygrid[igrid]->GetNormTot() == true) {
    cout<<"TEST: grid ntot is ON!"<<endl;
    cout<<"TEST: num of entries: "<<href->GetEntries()<<endl;
    cout<<"TEST: BEFORE"<<endl;
    href->Print("all");

    cout<<"TEST: AFTER"<<endl;
    //href->Scale( 1.0 /  href->GetEntries() );
    //href->Scale( 1.0 /  2000 );
    href->Print("all");
  }
  */

  cout<<cn<<mn<<" Reference is divided by binwidth? "
      <<(mygrid[igrid]->GetDividedByBinWidth()? "Y":"N");


  TGraphAsymmErrors *gref = TH1TOTGraphAsymm(href);

  if (this->GetDataOk(igrid)) {
    //convert units from data to desire
    double xscale = GetUnitScaleX(mygrid[igrid]->GetXUnits(), this->GetXUnits()); //new scaling
    double yscale = GetUnitScaleY(mygrid[igrid]->GetYUnits(), this->GetYUnits()); //new scaling
    
    if (debug) cout<<cn<<mn<<" xscale:"<<xscale<<", yscale:"<<yscale<<endl;

    bool normtot=mydata[igrid]->isNormTot();
    bool divbinwidth=mydata[igrid]->DivideByBinWidth();
    

    //  change normtot and divbinwidth accordingly for reference hist to be same as data
    //normtot = true; //TEST
    this->Normalise(gref,yscale,xscale,normtot, divbinwidth);    
  }


  if(mydata[igrid]->GetScalex() != 1.0 || mydata[igrid]->GetScaley() != 1.0 ) {
      if(debug) cerr<<cn<<mn<<" WARN: Forcefully scaling reference histo. "
		    <<"\n\txscale: "<<mydata[igrid]->GetScalex()
		    <<"\n\tyscale: "<<mydata[igrid]->GetScaley()<<endl;
      
      gref->Print("all");
      //set reference's artificial scaling to the same as data's
      ScaleGraph( gref, 1.0, mydata[igrid]->GetScaley() );     
      gref->Print("all");
  }

  gref->SetLineStyle(refhistlinestyle[igrid]);
  gref->SetLineColor(refhistlinecolor[igrid]);
  gref->SetMarkerSize(0);
  
  return gref;
}


TH1D* MyCrossSection::GetNormalisedReference(int igrid) {
  //
  // Get reference histograms for grid with index igrid
  // and normalise according to datascalex and scaley
  //
  string mn = "GetNormalisedReference:";

  TH1D *href=this->GetReference(igrid);
  if (!href) {
    cout<<" MyCrossSection::GetNormalisedReference: reference histo not found igrid="<<igrid<<endl;
    return 0;
  }

  href->SetLineStyle(refhistlinestyle[igrid]);
  href->SetLineColor(refhistlinecolor[igrid]);

  if (this->GetDataOk(igrid)) {
    //convert units from data to desire
    double xscale = GetUnitScaleX(mygrid[igrid]->GetXUnits(), this->GetXUnits()); //new scaling
    double yscale = GetUnitScaleY(mygrid[igrid]->GetYUnits(), this->GetYUnits()); //new scaling
    
    if (debug) cout<<cn<<mn<<" xscale:"<<xscale<<", yscale:"<<yscale<<endl;

    //   if(mydata[igrid]->GetScalex() != 1.0) xscale += mydata[igrid]->GetScalex();
    //   if(mydata[igrid]->GetScaley() != 1.0) yscale += mydata[igrid]->GetScaley();

    if (debug) cout<<cn<<mn<<" After checking data: xscale:"<<xscale<<", yscale:"<<yscale<<endl;

    bool normtot=mydata[igrid]->isNormTot();
    bool divbinwidth=mydata[igrid]->DivideByBinWidth();
    
    //TODO - check here if data and reference have different normtot and divbinwidth
    //  change normtot and divbinwidth accordingly for reference hist to be same as data

    this->Normalise(href,yscale,xscale,normtot, divbinwidth);

    
    if(mydata[igrid]->GetScalex() != 1.0 || mydata[igrid]->GetScaley() != 1.0 ) {
      if(debug) cerr<<cn<<mn<<" WARN: Forcefully scaling reference histo. "
		    <<"\n\txscale: "<<mydata[igrid]->GetScalex()
		    <<"\n\tyscale: "<<mydata[igrid]->GetScaley()<<endl;
      
      href->Print("all");
      //set convolutes's artificial scaling to the same as data's
      href->Scale( mydata[igrid]->GetScaley() ); //TODO - include checking for scalex?      
      href->Print("all");
    }
  }

  

  
  if(debug) cout<<cn<<mn<<" Returning normalised reference histogram"<<endl;
  
  return href;
}

/*
  TGraphAsymmErrors* MyCrossSection::GetReferenceRatio(int igrid) {
  //
  // Get reference histograms for grid with index igrid
  // and divide data associated to grid
  //
  TH1D *href=this->GetReference(igrid);
  TGraphAsymmErrors* gref=TH1TOTGraphAsymm(href);
  //gref->Print();
  TGraphAsymmErrors* ratio=myTGraphErrorsDivide(mydata[igrid]->GetTGraphTotErr(),gref);
  return ratio;
  };
*/

TGraphAsymmErrors* MyCrossSection::TH1TOTGraphAsymm(TH1 *h1) {
  //
  // convert the histogram h1 into a graph
  //
  const char *name="MyCrossSection::TH1TOTGraphAsymm";
  if (!h1) cout <<name<< " histogram not found !" << endl;
  //else h1->Print();
  //cout<<name<<" n= "<<h1->GetNbinsX()<<endl;
  TGraphAsymmErrors* g1= new TGraphAsymmErrors();
  if (!g1) cout<<name<<" graph g1 not created "<<endl;
  //g1->Print();
  g1->SetName(h1->GetName());

  Double_t x, y, ex, ey;
  for (Int_t i=0; i<h1->GetNbinsX(); i++) {
    y=h1->GetBinContent(i+1);
    ey=h1->GetBinError(i+1);
    x=h1->GetBinCenter(i+1);
    ex=h1->GetBinWidth(i+1)/2.;

    //cout << i<<" x,y = " << x << " " << y << " ex,ey = " << ex << " " << ey << endl;

    g1->SetPoint(i,x,y);
    g1->SetPointError(i,ex,ex,ey,ey);
  }

  //g1->Print();
  //cout<<name<<" return "<<endl;
  return g1;
}

TGraphAsymmErrors* MyCrossSection::myTGraphErrorsDivide(TGraphAsymmErrors* g1,TGraphAsymmErrors* g2, Int_t noerr) {
  // Divide two TGraphAsymmErrors: new=g1/g2
  //
  // noerr=0: put all errors to zero
  //       1: add errors from two graph quadrically
  //       2: set errors from graph 2 to zero
  //       3: set errors from graph 1 to zero

  const bool debug=false;
  const char *name="**myTGraphErrorsDivide:";

  if (!g1) cout << name<<" MyCrossSection::myTGraphErrorsDivide: g1 does not exist ! " << endl;
  if (!g2) cout << name<<" MyCrossSection::myTGraphErrorsDivide: g2 does not exist ! " << endl;

  Int_t n1=g1->GetN();
  Int_t n2=g2->GetN();

  if (n1!=n2) {
    printf("%s: vector do not have same number of entries !  \n",name);
    cout <<name<< " g1: " << g1->GetName() << " n1= " << n1 << endl;
    cout <<name<< " g2: " << g2->GetName() << " n2= " << n2 << endl;
  }

  TGraphAsymmErrors* g3= new TGraphAsymmErrors();
  if (!g3) cout<<name<<" problem to make new vector ! " << endl;
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
	//cout <<name<<" x1 and x2 not the same x1= "<<x1<<" x2= "<<x2<<endl;
      } else { // do something only if x1=x2
	matchcount++;
	//cout <<name<<" x1 and x2 match x1= "<<x1<<" x2= "<<x2<<endl;
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

	if (debug)
	  printf("%s: %d %d dy1=%f %f dy2=%f %f sqrt= %f %f \n",name,i1,i2,dy1l,dy1h,dy2l,dy2h,
		 sqrt(dy1l*dy1l+dy2l*dy2l),sqrt(dy1h*dy1h+dy2h*dy2h));

	if (y2!=0.) g3->SetPoint(iv, x1,y1/y2);
	else        g3->SetPoint(iv, x1,y2);
	Double_t el=0.;
	Double_t eh=0.;

	if (noerr==2) {
	  dy2l=0.;
	  dy2h=0.;
	}

	if (noerr==3) {
	  dy1l=0.;
	  dy1h=0.;
	}

	if (y1!=0. && y2!=0.) el=sqrt(dy1l*dy1l+dy2l*dy2l)*(y1/y2);
	if (y1!=0. && y2!=0.) eh=sqrt(dy1h*dy1h+dy2h*dy2h)*(y1/y2);


	if (debug) printf("dx1h=%f  dx1l=%f  el=%f  eh=%f \n",dx1h,dx1l,el,eh);
	if (noerr==0) {
	  g3->SetPointError(iv,dx1l,dx1h,0,0);
	} else {
	  g3->SetPointError(iv,dx1l,dx1h,el,eh);
	}

	iv++;
      }
    }
    //if (matchcount>1) {cout<<name<<" too many x-points matched ! "<<endl; exit (1);}
    if (matchcount>1) {
      cout<<name<<" too many x-points matched ! "<<endl;
    }
  }
  return g3;
}

int MyCrossSection::GetFrameNumber() {
  //
  // count frames/canvases that are given in the steering files
  //
  int nframe=0;
  vector<int> frameid2;
  frameid2.clear();
  if (debug) cout<<" MyCrossSection::GetFrameNumber: frameid.size()= "
		 <<frameid.size()<<endl;
  for (int i=0; i<frameid.size(); i++) {
    int iframe=frameid[i];
    bool found=false;
    for (int i2=0; i2<frameid2.size(); i2++) {
      int iframe2=frameid2[i2];
      if (iframe==iframe2) {
	found=true;
      };
    };
    if (!found) frameid2.push_back(iframe);
    if (debug) cout<<" MyCrossSection::GetFrameNumber: iframe= "
		   <<iframe<<" found= "<<found<<endl;
  };
  if (debug) cout<<" MyCrossSection::GetFrameNumber: Number of frames= "<<frameid2.size()<<endl;
  return frameid2.size();
};



void MyCrossSection::split_string(string str, vector<string>& split_results, string delimiters) {
  //
  // Split string by a delimeter
  //

  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos) {
      // Found a token, add it to the vector.
      split_results.push_back(str.substr(lastPos, pos - lastPos));
 
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);

      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

vector<string>* MyCrossSection::ParseString(string rawData, char delimeter) {
  //
  // retrieve each element of a string by a delimeter
  //

  stringstream lineStream(rawData);
  string cell;
  vector<string> *parsedDataVec;
  parsedDataVec = new vector<string>();
  parsedDataVec->clear();

  if (debug) cout<<" MyCrossSection::ParseString: Start parsing data: '"<<rawData<<"'; Delimeterized by: "<<delimeter<<endl;

  while(getline(lineStream,cell,delimeter)) {
    if (debug) cout<<" MyCrossSection::ParseString: found: "<<cell<<endl;
    cell.erase( remove(cell.begin(), cell.end(), ' '), cell.end() ); //remove any whitespace
    parsedDataVec->push_back(cell);
  }
    
  if (debug) cout<<" MyCrossSection::ParseString: End found "<<parsedDataVec->size()<<" parts."<<endl;
    
  if(parsedDataVec->size()==0) parsedDataVec->push_back(rawData);    
    
  return parsedDataVec;
}


vector<string>* MyCrossSection::ParseRatioStyle(string rawdata) {
  //
  // retrive all data from a string representing what to display for ratio.
  //  one or many strings left of the "/" are numerator(s)
  //  the SINGLE string after "/" is te denominator 
  //  PROPER SYNTAX: numerator/denominator
  //
  string mn = "ParseRatioStyle:";


  // user can provide multiple numerators, so parse by numerator delimter, a comma, first
  char delim = ','; 
  std::vector<std::string> *ratioNames = ParseString(rawdata, delim);

  if(ratioNames->size() == 0 ) {
    cerr<<cn<<mn<<" ERROR: Ratio Style syntax problem! "
	<<"\n\t Should be of the form \"ratiostyle numerator/denominator\" " 
	<<"but found \"ratiostyle "<<rawdata<<"\""<<endl;
    exit(0); // a particular ratio style was requested, but invalid
  }
  
  if(debug) {
    cout<<cn<<mn<<" Found "<<ratioNames->size()<<" element(s): "<<endl;
    for(int i=0; i<ratioNames->size(); ++i) cout<<"\t"<<ratioNames->at(i)<<endl;
  }


  //last element of comma parsed must contain the last numerator and denominator
  string denomDelim    = "/";
  string lastElem      = ratioNames->back(); //last element needs denominator extraction
  int    denomIndex    = lastElem.find( denomDelim );  
  string lastNumerator = "";
  string denominator   = "";

  //ensure a denominator exists
  // -1        -- does not exist
  //  0        -- the slash was the first charcter, syntax error
  //  size()-1 -- the last was the last character, synatx error
  if( denomIndex != -1 && denomIndex != 0 && denomIndex != lastElem.size()-1 ) {
    lastNumerator = lastElem.substr( 0, denomIndex );
    denominator   = lastElem.substr( denomIndex+1, lastElem.size() );
  } else {
    cerr<<cn<<mn<<" ERROR: Ratio Style synatx problem!"
	<<"\n\tShould be of the form \"ratiostyle numerator/denominator\"" 
	<<"\n\tbut found \"ratiostyle "<<rawdata<<"\""<<endl;
    exit(0); //a particular ratio style was requested, but no denominator
  }

  // update ratio's requested by user with last numerator and denominator
  ratioNames->pop_back(); //kill the last element, which has now been parsed
  ratioNames->push_back(lastNumerator);
  ratioNames->push_back(denominator); //denominator is the LAST element in ratio's provided
    


  if(debug) {
    cout<<cn<<mn<<" Parsed Ratio Style: \n\tNUMERATOR = ";
    for(int i=0; i<ratioNames->size()-1; ++i) 
      cout<<ratioNames->at(i)<<(i<(ratioNames->size()-2)? ", ":"");
    cout<<"\n\tDENOMINATOR = "<<ratioNames->back()<<endl;
  }

  if ( validateRatioStyle(*ratioNames) == false ) { 
    cerr<<cn<<mn<<" Ratio validation failed! - update steering file."<<endl; 
    exit(0);
  }
  
  return ratioNames;
}


double MyCrossSection::CalcChi2(TGraphAsymmErrors *g_theory, TGraphAsymmErrors *g_data, TMatrixT<double> *data_cov_matrix) {
  // 
  // Calculate the Chi2 
  //

  if (debug) cout<<" MyCrossSection::CalcChi2:         "<<endl;
  //
  /// Fill in the theory covariance matrix
  TMatrixT<double> theory_cov_matrix(g_theory->GetN(), g_theory->GetN());
  for (int pi = 0; pi < g_theory->GetN(); pi++) {
    for (int pi2 = 0; pi2 < g_theory->GetN(); pi2++) {
      if ( pi != pi2 ) theory_cov_matrix(pi, pi2) = 0;
      if ( pi == pi2 ) {
	// is this reasonable ?
	double theory_uncertainty = 0.5*(g_theory->GetErrorYhigh(pi) + g_theory->GetErrorYlow(pi));
	theory_cov_matrix(pi, pi2) = theory_uncertainty*theory_uncertainty;
      }
    }
  }

  if (debug) {
    cout<<" MyCrossSection::CalcChi2: theory uncertainty matrix done "<<endl;
    theory_cov_matrix.Print();
  }

  if (debug) {
    cout<<" MyCrossSection::CalcChi2: At Start, dump contents of data cov matrix: \n "<<endl;
    data_cov_matrix->Print();
    cout<<endl;
  }

  TMatrixT<double> tot_cov_matrix = theory_cov_matrix + *data_cov_matrix;
  TMatrixT<double> invertex_cov_matrix = tot_cov_matrix.Invert(); //includes theory errors in diagonal elements

  if (debug) {

    cout<<"\n MyCrossSection::CalcChi2: After adding theory, dump contents of cov matrix: \n "<<endl;
    tot_cov_matrix.Print();

    cout<<endl;
    cout<<"\n MyCrossSection::CalcChi2: After inversion, dump contents of cov matrix: \n"<<endl;
    invertex_cov_matrix.Print();

  }

  // Loop over bins and determine data-theory matrices
  TMatrixT<double> row_data_minus_theory(1, g_theory->GetN());
  TMatrixT<double> col_data_minus_theory(g_theory->GetN(), 1);

  for (int pi = 0; pi < g_theory->GetN(); pi++) {
    Double_t data_val;
    Double_t theory_val;
    Double_t x_val;

    g_theory->GetPoint(pi, x_val, theory_val);
    g_data->GetPoint(pi, x_val, data_val);
    row_data_minus_theory(0,pi) = data_val - theory_val;
    col_data_minus_theory(pi,0) = data_val - theory_val;

    //if (debug) cout<<" MyCrossSection::CalcChi2: At "<<x_val<<" data = "<<data_val
    //              <<" theory = "<<theory_val<<", content = "<<row_data_minus_theory(0,pi)<<endl;
  }  // pi

  TMatrixT<double> cov_times_col = invertex_cov_matrix*col_data_minus_theory;
  //assert( cov_times_col.GetNrows() == g_theory->GetN());
  //assert( cov_times_col.GetNcols() == 1);
  if (debug) {
    cout<<"\n MyCrossSection::CalcChi2: After first multiplication matrix is: \n"<<endl;
    cov_times_col.Print();
    /*
      for (int pi = 0; pi < g_theory->GetN(); pi++) {
      cout<<cov_times_col(pi, 0)<<"\t";
      }
      cout<<"\n";
    */
  }
 
  TMatrixT<double> result = row_data_minus_theory*cov_times_col;
  //assert( result.GetNrows() == 1);
  //assert( result.GetNcols() == 1);

  double chi2 = result(0,0);
  if (debug) cout<<" MyCrossSection::CalcChi2: End chi2 = "<<chi2<<endl;

  return chi2;
}


bool MyCrossSection::DoubleDataSetName(int idata) {
  //
  // check id data-set name appears twice in dataname vector
  //

  bool doubledataset=false;
  if (idata>dataname.size())
    cout<<" MyCrossSection::DoubleDataSetName: number of data sets too small "
	<<dataname.size()<<" idata= "<<idata<<endl;
  
  if (debug) cout<<" MyCrossSection::DoubleDataSetName: "
		 <<" dataname["<<idata<<"]= "<<dataname[idata]
		 <<" compared to... "<<endl;
  
  //check each data name for duplicates
  for (int jdata=0; jdata<dataname.size(); jdata++) {
    if (jdata<idata) { //only check future data to be displayed
      if(jdata!=idata) { //don't check youself
	if (debug) cout<<" MyCrossSection::DoubleDataSetName: "
		       <<" dataname["<<jdata<<"]= "<<dataname[jdata]<<endl;
	
	if (dataname[idata].compare(dataname[jdata])== 0){
	  if (debug) cout<<" MyCrossSection::DoubleDataSetName: Duplicate data found."<<endl;
	  
	  doubledataset=true;
	}
      }
    }
  }
  
  if (debug) cout<<" MyCrossSection::DoubleDataSetname: found duplicate="
		 <<(doubledataset? "yes":"no")<<endl;

  return doubledataset;
}


void MyCrossSection::ScaleGraph(TGraphAsymmErrors *g1, double scalex, double scaley) {
  //
  // Scale a a graph 'g1' by some x and y scalers
  //

  cout<<" MyCrossSection::ScaleGraph: WARN: Artifical scaling of graph data by: "
      <<"\n\tscalex: "<<scalex
      <<"\n\tscaley: "<<scaley<<endl;

  Double_t* X1 = g1->GetX();
  Double_t* Y1 = g1->GetY();
  Double_t* EXhigh1 = g1->GetEXhigh();
  Double_t* EXlow1  = g1->GetEXlow();
  Double_t* EYhigh1 = g1->GetEYhigh();
  Double_t* EYlow1  = g1->GetEYlow();
  
  for (Int_t i=0; i<g1->GetN(); i++) {
    g1->SetPoint(i, X1[i]*scalex, Y1[i]*scaley);
    g1->SetPointError(i, EXlow1[i]*scalex, EXhigh1[i]*scalex,
                      EYlow1[i]*scaley, EYhigh1[i]*scaley);
  }

  return;
};

double MyCrossSection::GetUnitScaleX(std::string fromUnits, std::string toUnits) {
  std:: string mn = "GetUnitScaleX:"; //Name of method, for printing
  int n = 1; //scale value to return based on fromUnits and toUnits

  cout<<cn<<mn<<" Converting '"<<fromUnits<<"' to '"<<toUnits<<"'"<<endl;
  
  // Possible units to convert from and to
  std::string MEV = "MEV", GEV = "GEV", TEV = "TEV";
  
  // Shift fromUnits and toUnits to be in same case as above
  std::string from = stringToUpper(fromUnits), to = stringToUpper(toUnits);
  

  // Compute scaling
  // scale down to electron volts
  if      (from == MEV) n = -6; // n = -6; 
  else if (from == GEV) n = -9;
  else if (from == TEV) n = -12;
  else    cerr<<cn<<mn<<" ERROR: Unexpected units from '"<<from<<"'"<<endl;

  // scale up from electron volts to desired units
  if      (to == MEV) n += 6; //n += 6;
  else if (to == GEV) n += 9;
  else if (to == TEV) n += 12;
  else    cerr<<cn<<mn<<" ERROR: Unexpected units to '"<<to<<"'"<<endl;


  if(debug) cout<<cn<<mn<<" Will scale by 1e"<<n<<endl;
  
  return pow(10, n);
}


double MyCrossSection::GetUnitScaleY(std::string fromUnits, std::string toUnits) {
  std:: string mn = "GetUnitScaleY:"; //Name of method, for printing
  double n = 1.0; //scale value to return based on fromUnits and toUnits

  cout<<cn<<mn<<" Converting '"<<fromUnits<<"' to '"<<toUnits<<"'"<<endl;

  // Possible units to convert from and to
  std::string FB = "FB", PB = "PB";
    
  // Shift fromUnits and toUnits to be in same case as above
  std::string from = stringToUpper(fromUnits), to = stringToUpper(toUnits);
  

  // Compute scaling
  // scale down to barns
  if      (from == FB) n = -12;
  else if (from == PB) n = -15;
  else    cerr<<cn<<mn<<" ERROR: Unexpected units from '"<<from<<"'"<<endl;

  // scale up from barns to desired units
  if      (to == FB) n += 12;
  else if (to == PB) n += 15;
  else    cerr<<cn<<mn<<" ERROR: Unexpected units to '"<<to<<"'"<<endl;
  

  if(debug) cout<<cn<<mn<<" Will scale by 1e"<<n<<endl;

  return pow(10, n);
}

string MyCrossSection::stringToUpper(std::string s) {
  string upperString = s;
  for (int i = 0; i < s.length(); i++) upperString[i]=toupper(s[i]);

  return upperString;
}


bool MyCrossSection::validateDisplayStyle(std::vector<std::string > styles) {
  string mn = "validateDisplayStyle:";
  bool valid = true;

  //Only these are valid frame style options
  string OVERLAY = "overlay", RATIO = "ratio";
  bool overlayFlag = false, ratioFlag = false;
  
  
  if( styles.size() > 2 || styles.size() <= 0 ) valid = false; //incorrect num of styles
  else {
    for(int i = 0; i<styles.size() && valid == true; ++i) {
      string curStyle = styles.at(i);

      if      (curStyle == OVERLAY) overlayFlag = true;
      else if (curStyle == RATIO)   ratioFlag   = true;
      else {
	valid = false;
	cerr<<cn<<mn<<" ERROR: framestyle of '"<<curStyle<<"' at pos "<<i<<" is not valid"<<endl;
      }
    } //end for
  } //end else


  if(valid == false) { 
    cerr<<cn<<mn<<" ERROR: Invalid framestyle found!"
	<<"\n\t Should in form 'framestyle overlay,ratio'"<<endl;
  } else {
    //set global flags based on user overlay requests
    displayOverlay = overlayFlag;
    displayRatio   = ratioFlag;

    cout<<cn<<mn<<"  displayOverlay:"<<(displayOverlay?"y":"n")
	<<", displayRatio:"<<(displayRatio? "y":"n")<<endl;
  }  

  return valid;
}


bool MyCrossSection::validateOverlayStyle(std::vector<std::string > names) {
  string mn = "validateOverlayStyle: "; //method name, for printing
  bool valid = true;

  //Only these are valid overlay style options
  string DATA = "data", CONVOLUTE = "convolute", REFERENCE = "reference";
  bool dataFlag = false, convoluteFlag = false, referenceFlag = false;

  if( names.size() >= 4 || names.size() <= 0 ) valid = false; //incorrect num of names
  else {
    for(int i = 0; i<names.size() && valid; ++i) {
      string curName = names.at(i);

      if(curName != DATA && curName != CONVOLUTE && curName != REFERENCE) {
	valid = false;
	cerr<<cn<<mn<<" Overlay name '"<<curName<<"' at position "<<i<<" is not valid"<<endl;
      } else {

	if      (curName == DATA)      dataFlag      = true;
	else if (curName == CONVOLUTE) convoluteFlag    = true;
	else if (curName == REFERENCE) referenceFlag = true;	
      }
    } //end for
  } //end else


  if(valid == false) { 
    cerr<<cn<<mn<<" ERROR: Invalid overlaystyle found!"
	<<"\n\t Should in form 'overlaystyle data,reference,convolute'"<<endl;
  }

  //set global flags based on user overlay requests
  overlayData      = dataFlag;
  overlayConvolute = convoluteFlag;
  overlayReference = referenceFlag;
  
  return valid;
}


bool MyCrossSection::validateRatioStyle(std::vector<std::string > names) {
  string mn = "validateRatioStyle: "; //method name, for printing
  bool valid = true;

  //Only these are valid overlay style options
  string DATA = "data", CONVOLUTE = "convolute", REFERENCE = "reference";


  if( names.size() > 4 || names.size() <= 0 ) valid = false; //incorrect num of names
  else {
    for(int i = 0; i<names.size() && valid; ++i) {
      string curName = names.at(i);

      if(curName != DATA && curName != CONVOLUTE && curName != REFERENCE) { //valid names
	valid = false;
	cerr<<cn<<mn<<" Ratio name '"<<curName<<"' at position "<<i<<" is not valid"<<endl;
      } 
    } //end for
  } //end else


  if(valid == false) { 
    cerr<<cn<<mn<<" ERROR: Invalid ratiostyle names found!"
	<<"\n\tShould only include 1 or more of the following: \"data, convolute, reference\""
	<<"\n\tBut instead found: \"";
    for(int i=0;i<names.size();++i) cerr<<names[i]<<((i<names.size()-1)? ", ":"");
    cerr<<endl; //foramtting
  }
  
  return valid;
}


std::string MyCrossSection::trim(std::string s) {
  // 
  // Helper to remove leading and trailing white space from steering file reading
  //  credit to: http://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html
  //
  std::string reducedS = s;

  size_t p = reducedS.find_first_not_of(" \t");
  reducedS.erase(0, p);
  
  p = reducedS.find_last_not_of(" \t");
  if (string::npos != p) reducedS.erase(p+1);

  return reducedS;
}


void MyCrossSection::ComputeRatioRange(double *_Ymin, double *_Ymax, int igrid) {
  //
  // Determines the max and min of the ratio range based on all possible ratio styles
  //
  string mn="ComputeRatioRange:";

  const double BIG=1.e30; //arbitrary size that changes when computing size
  double xmin=BIG, xmax=-BIG, ymin=BIG, ymax=-BIG;
  double Ymin=BIG, Ymax=-BIG;
  
  

  //COMPUTE RATIO WINDOW
  // check denominator window size needed
  if( IsRatioDenominator("data") == true ) {
    ratiodenom[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
  } else if (IsRatioDenominator("reference") == true) {
    ratiodenom[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
  } else if (IsRatioDenominator("convolute") == true) {
    myband[igrid]->ComputePDFBandRatioRange();
    ymin=myband[igrid]->GetYmin();
    ymax=myband[igrid]->GetYmax();
  }

  if (ymin<Ymin) Ymin=ymin;
  if (ymax>Ymax) Ymax=ymax;


  // check numerator(s) window size needed
  if( IsRatioNumerator("data") == true ) {
    ratiodata[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
    if (ymin<Ymin) Ymin=ymin;
    if (ymax>Ymax) Ymax=ymax;
  }

  if( IsRatioNumerator("reference") == true ) {
    ratioreference[igrid]->ComputeRange(xmin,ymin,xmax,ymax);
    if (ymin<Ymin) Ymin=ymin;
    if (ymax>Ymax) Ymax=ymax;
  }

  if( IsRatioNumerator("convolute") == true ) {
    myband[igrid]->ComputePDFBandRatioRange();
    ymin=myband[igrid]->GetYmin();
    ymax=myband[igrid]->GetYmax();
    if (ymin<Ymin) Ymin=ymin;
    if (ymax>Ymax) Ymax=ymax;
  }

  
  *_Ymin = Ymin;
  *_Ymax = Ymax;

  if (debug) cout<<cn<<mn<<" Ratio Ymin= "<<Ymin<<" Ymax= "<<Ymax<<endl;
  
  return;
}

/*
TLegend *MyCrossSection::BuildLegend(int iframe) {
  //
  //
  //
  string mn = "BuildLegend:"; //method name for debugging purposes

  if (debug)
    cout<<cn<<mn<<" number of grids= "<<gridinframe[iframe].size()<<" for frame= "<<iframe<<endl;
 
  // const int npdf=gpdfband[igrid].size();
  //const int npdf=myband[igrid]->GetNPDF();
  const int npdf=pdfdata.size();
  if (npdf<1) cout<<" MyCrossSection::DrawinFrame: No PDF information found"<<endl;


  //compute an optimal position for the legend for this plot
  y=ylegend;
  int namesize=0;
  if (npdf<1) {
    namesize=5; //why choose 5 as default?
    
    TString mylabel=mydata[igrid]->GetLabel();
    int mysize=TString(mylabel).Sizeof(); 
    if (mysize>namesize) namesize=mysize;
    //y-=0.1;
  } else {
    //y-=0.07*npdf; //origional
    for (int ipdf=0; ipdf<npdf; ipdf++) {
      if (debug) cout<<" MyCrossSection::DrawinFrame: label= "
		     <<this->GetPdfName(ipdf).Data()<<endl;
     
      int mysize=this->GetPdfName(ipdf).Sizeof(); 
      if (mysize>namesize) namesize=mysize;
    }
    if (plotchi2) namesize+=10;
  }

  if (debug) cout<<" MyCrossSection::DrawinFrame: xlegend= "<<xlegend
		 <<" ylegend= "<<ylegend<<endl;

  //origional
  //  double x1=xlegend, x2=0.95;
  //  double y1=y, y2=ylegend;
  //  x2+=namesize*0.02;
 

  // place legend based on the YTop and XRight position and set xLeft and YBottom by:
  //  XLeft = the length of the largest name in the legend 
  //  YBottom = the number of entries in the legend
  double x1 = xlegend-(namesize*0.02), x2=xlegend;
  double y1 = 0, y2=ylegend;
  
  
  // ydimention is based of what elements are being put in legend.
  if(npdf<1) {
    // size based on no PDFs meaning legend is full of just grid data
    y1 = ylegend-0.2;
  } else {
    // size based on number of pdfs
    y1 = ylegend-(npdf*0.11);
  }
  
  
  if (debug) cout<<" MyCrossSection::DrawinFrame: namesize="<<namesize<<", npdf="<<npdf
		 <<", x1="<<x1<<", x2="<<x2<<", y1="<<y1<<", y2="<<x2<<std::endl;


  // prepare legend for this Plot
  TLegend *leg = new TLegend(x1,y1,x2,y2);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetMargin(0.1);
  

  const double BIG=1.36;
  double Ymin=BIG, Ymax=-BIG, Xmin=BIG, Xmax=-BIG;
  int iframeRepeatCheck = -1; // turn on or off certain features if they are being done in the same frame twice

  for (int i=0; i<(int)gridinframe[iframe].size(); i++) {
    int igrid=gridinframe[iframe][i];
    int jframe=this->GetFrameID(igrid);
    if (iframe!=jframe) {
      //cout<<"TEST: NOT EQUAL: iframe:"<<iframe<<", jframe:"<<jframe<<endl; //TODO - check does this do anything?
      continue;
    } else {
      //cout<<"TEST: EQUAL, SKIPPING: iframe:"<<iframe<<", jframe:"<<jframe<<endl;
    }

    if (debug) cout<<" MyCrossSection::DrawinFrame: for i= " << i << " igrid= "<<igrid<<endl;
    TH1D* href=0;
    //cout << "Is data ok? " << this->GetDataOk(igrid) << endl;


    //plot the overlay portion of graph
    if(overlaydata) {
      if (this->GetDataOk(igrid)) {
	if (!mydata[igrid]) cout<<" MyCrossSection::DrawinFrame: mydata["<<igrid<<"] not found "<<endl;
	else if (debug)     cout<<" MyCrossSection::DrawinFrame: mydata["<<igrid<<"]  found "<<endl;
	
	
	// Only plot data once (ignore repeats) unless repeated data is on a new frame
	if ( DoubleDataSetName(igrid) == false || iframeRepeatCheck != iframe ) { 
	  //repeated data was not found on that frame
	  iframeRepeatCheck = iframe;
	  
	  
	  TString mylabel=mydata[igrid]->GetLabel();	
	  if (debug) cout<<" MyCrossSection::DrawinFrame: mylabel= "<<mylabel.Data()<<endl;
	  
	  // label sqrts before data on legend if MyData Steering requested it
	  if(mydata[igrid]->GetPlotSqrts())  {
	    TString sqrtslabel = "";
	    double sqrtsval = mydata[igrid]->GetSQRTS() * GetUnitScaleX(mydata[igrid]->GetXUnits(), this->GetXUnits()); //new
	    TString sqrtsunits = mydata[igrid]->GetXUnits();
	    
	    sqrtslabel.Form("#sqrt{s}= %.2f %s", 
			    double(sqrtsval), 
			    sqrtsunits.Data() ); //TODO - too hardcoded? make generic
	    leg->AddEntry((TObject*)0, sqrtslabel, "");
	  }
	  
	  //add the data label to the legend
	  leg->AddEntry(mydata[igrid]->GetTGraphTotErr(),mylabel,"ep");	  
	}
      }  
    }
    
    
    if(overlayReference) {
      TString curLegLable = "";
      if (leglabel.size()>0) curLegLable = leglabel[igrid].c_str();
      else                   curLegLable = "reference";
      
      if (debug) cout<<" MyCrossSection::DrawinFrame: Added '"<<curLegLable<<"' to legend."<<endl;	
      TGraphAsymmErrors *gref = GetNormalisedReferenceAsGraph(igrid,false);
      leg->AddEntry(gref, curLegLable, "l");
    }


    if(overlayConvolute) {
      
      if(this->GetFrameID(igrid) != this->GetFrameID(igrid+1) ) {
	
	//Before adding PDF titles to legend, create a title for the PDFs within the legend
	TString pdfDataTitle="NLO QCD with:"; //"subtitle" for the following PDFs on the legend
	leg->AddEntry((TObject*)0, pdfDataTitle.Data(), "");
	
	for (int ipdf=0; ipdf<npdf && overlayConvolute; ipdf++) {
	  
	  //retrieve a PDF name
	  TString pdfname="";
	  pdfname+= this->GetPdfName(ipdf);
	  
	  
	  if (plotchi2) {
	    //pdfname+=" #C^2 ";
	    pdfname+=" Chi2= ";
	    char tmp[30];
	    sprintf(tmp,"%5.2f",vpdfchi2.at(igrid).at(ipdf));
	    pdfname+=TString(tmp);
	  }
	  if (debug) cout<<" MyCrossSection::DrawinFrame: pdfname= "<<pdfname.Data()<<endl;
	  
	  
	  //Add PDF titles to legend
	  if (plotmarker) leg->AddEntry(myband.at(igrid)->GetPdfBand(ipdf),pdfname,"p");
	  else            leg->AddEntry(myband.at(igrid)->GetPdfBand(ipdf),pdfname,"f");	
	}      
      }
    }
    

    // Draw the correctly filled legend
    leg->Draw();
    
    if (debug) cout<<" MyCrossSection::DrawinFrame: legend prepared "<<endl;
  }
  
  
  return contents;
}
*/

void MyCrossSection::ComputeLegendBounds(int iframe, double *xl, double *yl, double *xh, double *yh) {
  string mn = "ComputeLegendBounds:";

  double x1=0, y1=0, x2=xlegend, y2=ylegend, textSize=0.15;
  int numEntries = 0;


  cout<<cn<<mn<<" Computing bounds...";
  
  //find number of entries
  if(overlayReference || IsRatioNumerator("reference"))
    numEntries += gridinframe[iframe].size();
  
  if(overlayData || IsRatioNumerator("data"))
    numEntries += gridinframe[iframe].size();
      
  if(pdfdata.size()>=1)
    numEntries += (pdfdata.size()+1); //+1 for the "with NLO QCD" label
  //TODO - handle when plotsqrts is on

  
  //number of entries dictates size of legend
  x1 = xlegend-0.2; //fixed x size, always .2 to the left of x2
  y1 = y2-(0.06*numEntries); //y-low chosen by number of entries
  if (displayOverlay && !displayRatio)      y1 += ((y2-y1)/2); //Overlay is larger, scale more
  else if (!displayOverlay && displayRatio) y1 += ((y2-y1)/3); //Ratio is smaller, scale less


  cout<<cn<<mn<<" Legend Bounds are: x1:"<<x1
      <<", y1:"<<y1
      <<", x2:"<<x2
      <<", y2:"<<y2
      <<", entries:"<<numEntries<<endl;
  
  *xl = x1; *yl = y1; *xh = x2; *yh = y2;
  
  return;
}


