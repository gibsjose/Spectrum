//
//   gridwrap.cxx        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: gridwrap.cxx, v   Sat May  3 10:15:04 BST 2008 sutt


#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <iomanip>
using std::setprecision;
using std::setw;

#include <string>
using std::string;

#include <stdlib.h> // exit()
#include <sys/time.h> 


#include "TFile.h"
#include "TH1D.h"
#include "TMatrixT.h"
#include "TVectorT.h"

using std::vector;

#include "mcfm_grid.h"
#include "appl_grid/appl_pdf.h"

using appl::grid;
using appl::igrid;
// >>>>>
// was commented out
//using appl::mcfm_grid;
//using appl::igrid;
//
//#include "appl_grid/appl_pdf.h"
// <<<<<<<<<

extern "C" { 
  
  extern struct {
    int nproc;
  } nproc_;
  
  extern struct  {
    double sqrts;
  } energy_;
  
  
  extern struct {
    bool creategrid;
    int nSubProcess;
  } grid_;

}



bool file_exists(const string& s) {   
  //  FILE* testfile=fopen(s.c_str(),"r");

  if ( FILE* testfile=fopen(s.c_str(),"r") ) { 
    fclose(testfile);
    return true;
  }
  else return false;
}


static const int mxpart = 12;        // mcfm parameter : max number of partons in event record. defined in Inc/constants.f

static const int _Ngrids = 14;
static const int  Ngrids = _Ngrids;
appl::mcfm_grid* mygrid[_Ngrids];
static const char* gridFiles[_Ngrids] = 
  {
    "_eta3.root",
    "_ptmin.root",
    "_ptmax.root",
    "_eta34.root",
    "_pt34.root",
    "_ptatlas.root",   
    "_ptttatlas.root",
    "_ptttatlas5fb.root",
    "_mttatlas.root",
    "_yttatlas.root",
    "_ptcms.root",
    "_ptttcms.root",
    "_mttcms.root",
    "_yttcms.root"
  };

static double Observable[_Ngrids] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  };   // observable array
//int nObsBins[_Ngrids] = {100, 50, 100, 50}; // eta4, pt4 cental eta-bin, pt4 forward eta-bin
//int nObsBins[_Ngrids] = {40, 30, 40, 30}; // eta4, pt4 cental eta-bin, pt4 forward eta-bin
int nObsBins[_Ngrids] = {23, 34, 34, 23, 34, 
                         7, 3, 4, 5, 3,   // atlas pt, ptnew, ptt, mtt, ytt 
                         7, 6, 7, 10}; //

// fine binning 50
static const double eta[] = 
  {
   0.0,  0.2,  0.4,  0.6,  0.8 , 1.0,  1.2,  1.4,  1.6,  1.8,  2.0,  
   2.2,  2.4,  2.6,  2.8 , 3.0,  3.2,  3.4,  3.6,  3.8,  4.0,  4.2,  
   4.4,  4.6,  4.8 , 5.1
};

// fine binning 34
static const double pt[] = 
  {
   0 ,  25 , 50 ,  75 , 100 , 125 , 150 , 175 , 200 , 250 , 
 300 , 400 ,500 , 600 , 700 , 800 , 900 , 1000 ,1100 ,1200 ,
1300 ,1400 ,1450 ,1500 ,1600 ,1700 ,1900, 2100 ,2300 ,2500, 
2800, 3000, 3200, 3400, 3500
};

// new analysis
static const double ptatlas[] = {
   0., 50., 100., 150., 200., 250., 350. , 800. 
  };

// ATLAS binning of new results
static const double ptttatlas5fb[] = {
  0.,  40., 170., 340.,1000.
};

// ATLAS 2012 paper
static const double ptttatlas[] = {
  0.,  40., 170.,1100.
};

static const double mttatlas[] = {
250., 450., 550., 700., 950., 2700.
};
//
//static const double yttatlas[] = {
// -2.5 ,  -1.0 , -0.5 , 0., 0.5, 1., 2.5
//};
static const double yttatlas[] = {
  0., 0.5, 1.0 , 2.5
};
// CMS 2012 paper
static const double ptcms[] =  {
   0., 60., 100., 150., 200., 260., 320. , 400. 
  };

static const double ptttcms[] = {
  0.,  20., 45., 75., 120., 190., 300.
};

static const double yttcms[] = {
  -2.5 ,  -1.3, -0.9 , -0.6 , -0.3, 0., 0.3, 0.6, 0.9 , 1.3, 2.5
};
static const double mttcms[] = {
  345., 400., 470., 550., 650., 800., 1100. , 1600.
};

long unsigned int runs  =  0;
bool isBooked           =  false;
string glabel           =  "";

void getObservable( const double evt[][mxpart] );
int  cuts(int);



extern "C" void book_grid_()  // inital grid booking
{
  if (isBooked) return;
  
  time_t _t;
  time(&_t);
  
  std::cout<<" ***********************************************"<<std::endl;
  std::cout<<" booking the grids " << ctime(&_t) << std::endl;
  
  // binning information for the grid constructor
  int    nXbins  = 40;
  double xLow    = 1.0e-9, xUp = 1.0;
  int    xorder  = 6;
  int    nQ2bins = 3;
  double q2Low   = 6399.99, q2Up = 6400.01;
  int    qorder  = 0;
  // set transform2 value
  double apramval=5.;
  appl::igrid::transformvar(apramval);
  
  // lowest order in alphas	
  int lowest_order = 0;
  // how many loops
  int nloops = 1;
  
  // number of observables and binning for observables  
  //const double *obsBins[_Ngrids] = { eta, pt, eta, pt };
  const double *obsBins[_Ngrids] = { eta, pt, pt, eta, pt, 
				     ptatlas, ptttatlas,ptttatlas5fb, mttatlas, yttatlas, 
                                   ptcms, ptttcms  , mttcms  , yttcms                                      
                             };

  string labels[ _Ngrids ] = { "_eta3"    , "_ptmin"   , "_ptmax"    , "_eta34", "_pt34", 
			       "_ptatlas", "_ptttatlas","ptttatlas5fb", "_mttatlas" , "_yttatlas", 
                               "_ptcms"  , "_ptttcms"  , "_mttcms"   , "_yttcms" 
                              };

  
  string pdf_function;
  //string labels[ _Ngrids ] = { "_eta3", "_pt3", "_eta4", "_pt4" };
  glabel = "grid-";
  
  cout << "Process : " << nproc_.nproc;
  if      ( nproc_.nproc == 1 )  
    {
      cout << " W+ production"; 
      pdf_function = "mcfm-wp"; 
      glabel+="-Wplus";
    }  
  else if ( nproc_.nproc == 6 )  
    {
      cout << " W- production"; 
      pdf_function = "mcfm-wm"; 
      glabel+="-Wminus";
    }  
  else if ( nproc_.nproc == 13 )  
    {
      cout << " W+ + Cbar production"; 
      pdf_function = "mcfm-wpc"; 
      glabel+="-WplusCbar";

      lowest_order = 1;
    }  
  else if ( nproc_.nproc == 18 )  
    {
      cout << " W- + C production"; 
      pdf_function = "mcfm-wmc"; 
      glabel+="-WminusC";
      lowest_order = 1;

    }  
  else if ( nproc_.nproc == 31 ) 
    {
      cout << " Z production"; 
      pdf_function = "mcfm-z"; 
      glabel+="-Z0";
      q2Low = 8280.99, q2Up = 8281.01;

      nXbins  = 45;
      xorder=5;
      nQ2bins = 15;
      qorder=3;
      lowest_order = 2;
    }  
  else if ( (nproc_.nproc == 157) || (nproc_.nproc == 158) || (nproc_.nproc==159) )
    {

      nXbins  = 45;
      xorder=5;
      nQ2bins = 15;
      qorder=3;
      lowest_order = 2;
      //q2Low = std::pow(1.,2), q2Up = std::pow(3500.,2);
      q2Low = 170.*170., q2Up = 1.e08+1.;

      if (nproc_.nproc == 157)
	{
	  cout << " TTbar production"; 
	  pdf_function = "mcfm-TT"; 
	  glabel+="-TTbar_massdn";
	}
      else if(nproc_.nproc == 158)
	{
	  cout << " BBbar production"; 
	  pdf_function = "mcfm-BB"; 
	  glabel+="-BBbar";
	}
      else if (nproc_.nproc == 159)
	{
	  cout << " CCbar production"; 
	  pdf_function = "mcfm-CC"; 
	  glabel+="-CCbar";
	}
    }  
  else                           
    { 
      cerr << "don't know which process" << endl; 
      exit(-1); 
    } 
  cout << endl;
  
  for(int igrid=0; igrid < Ngrids; igrid++) 
    {

      //cout<<" start loop igrid= "<<igrid<<endl;
      //cout<<" gridFiles["<<igrid<<"]= "<<gridFiles[igrid]<<endl;

      bool create_new = false;

      // if the file does not exist, create a new grid...
      if ( !file_exists( glabel+gridFiles[igrid]) )  create_new = true;

      // or if it does exists but root file is a zombie...
      if ( !create_new ) {  
	TFile testFile( (glabel+gridFiles[igrid]).c_str() );
	if ( testFile.IsZombie() ) create_new = true;
	testFile.Close();
      }

      //cout<<" after testfile igrid= "<<igrid<<endl;

      if ( create_new ) 
	{ 
	  cout<<"Creating NEW grid... "<<labels[igrid]<<endl;
	  
	  mygrid[igrid] = new appl::mcfm_grid( nObsBins[igrid], obsBins[igrid],      // obs bins
					       nQ2bins, q2Low, q2Up, qorder,         // Q2 bins and interpolation order
					       nXbins,   xLow,  xUp, xorder,         // x bins and interpolation order
					       pdf_function, lowest_order, nloops ); 
	  

	  mygrid[igrid]->setCMSScale( energy_.sqrts );
	  grid_.nSubProcess = mygrid[igrid]->subProcesses();
	  
	  cout << "reference histo name = " 
	       << mygrid[igrid]->getReference()->GetName() << endl;
	  
	  std::cout<<*mygrid[igrid]<<std::endl;  
          //cout<<" finished "<<labels[igrid]<<endl;
	}
      else 
	{
	  std::cout << "Using existing grid file " << (glabel+gridFiles[igrid]) << std::endl;
	  
	  mygrid[igrid] = new appl::mcfm_grid(glabel+gridFiles[igrid]); //optimise grid x,Q2 bins
	  grid_.nSubProcess = mygrid[igrid]->subProcesses();
	  mygrid[igrid]->getReference()->Reset();
	  mygrid[igrid]->optimise(nQ2bins, nXbins);
	  
	  std::cout<<*(mygrid[igrid])<<std::endl;  
	}

          cout<<" end of loop igrid= "<<igrid<<endl;

    }

  runs = 0;
  isBooked = true;
  std::cout<<" ***********************************************"<<std::endl;
}


extern "C" void book_grid__()  // inital grid booking
{
  book_grid_();
}

extern "C" void  fill_grid_( const double evt[][mxpart] )
{
  if (!isBooked) 
    {    
      book_grid_();
      return;
    }

  getObservable( evt );
  
  for(int igrid = 0; igrid < Ngrids; igrid++)
    if(cuts(igrid))
      mygrid[igrid]->fillMCFM( Observable[igrid] );
  
  
  runs++; // counter of number of events (shouldn't this be after cuts)? or is it the number of runs?"
}

extern "C" void  fill_grid__( const double evt[][mxpart] )
{
  fill_grid_( evt );
}

//
// just normalise to bin width
//
void Normalise(TH1D* h) 
{ 
  for ( int ibin=1 ; ibin<=h->GetNbinsX() ; ibin++ ) 
    { 
      double width = h->GetBinLowEdge(ibin+1) - h->GetBinLowEdge(ibin);
      h->SetBinContent( ibin, h->GetBinContent(ibin)/width );
    }
  return;
}

extern "C" void write_grid_(double& xstotal)   // writes out grid after some events
{
  std::cout<<"\t Write out the grid ..."<<std::endl;
  for(int igrid = 0; igrid < Ngrids; igrid++)
    {
      cout << "\tsaving grid N=" << igrid+1 << "\tof total " << Ngrids << endl;
      
      mygrid[igrid]->setNormalised( true );
      mygrid[igrid]->run() = runs;
      
      mygrid[igrid]->trim();
      int trim_size = mygrid[igrid]->size();

      mygrid[igrid]->untrim();
      int untrim_size = mygrid[igrid]->size();
      
      // normalise the reference histogram by bin width
      Normalise( mygrid[igrid]->getReference() );
      
      mygrid[igrid]->Write(glabel+gridFiles[igrid]);
      
      cout << "size(untrimmed)=" << untrim_size 
	   << "\tsize(trimmed)=" << trim_size 
	   << "\tfraction="      << 100.*trim_size/untrim_size << " %" << endl;

      int nsub = mygrid[igrid]->subProcesses();

      delete mygrid[igrid];
      
    }
  
  time_t _t;
  time(&_t);
  
  std::cout<<" ***********************************************"<<std::endl;
  std::cout<<" saved grids " << ctime(&_t) << std::endl;
  std::cout<<" ***************************************************"<<std::endl;
}
 

extern "C" void write_grid__(double& xstotal)   // writes out grid after some events
{
  write_grid_(xstotal);   // writes out grid after some events
}

//
// ----------------------------------------------
//    analysis
// ----------------------------------------------
//

void getObservable(const double evt[][mxpart])
{
  // evt[momentum][particle number-1]
  // momentum[0,1,2,3] = (x,y,z,E)
  //

  // calculate observables
  for(int igrid = 0; igrid < Ngrids; igrid++)Observable[igrid] = 0.0; // initialize
  
  double p3[4] = {evt[3][2],evt[0][2],evt[1][2],evt[2][2]}; // (E,x,y,z)
  double p4[4] = {evt[3][3],evt[0][3],evt[1][3],evt[2][3]};
  
  
  double rapidity3 = 0.0;
  rapidity3 = (p3[0] + p3[3])/(p3[0] - p3[3]);
  (rapidity3 < 1e-13) ? rapidity3 = 100.0 : rapidity3 = 0.5*std::log(rapidity3);
  double absrapidity3=fabs(rapidity3);

  double rapidity4 = 0.0;
  rapidity4 = (p4[0] + p4[3])/(p4[0] - p4[3]);
  (rapidity4 < 1e-13) ? rapidity4 = 100.0 : rapidity4 = 0.5*std::log(rapidity4);
  
  double rapidity34 = 0.0;                      // rapidity of particle (3+4) in event record
  rapidity34  = (p3[0] + p4[0]) + (p3[3] + p4[3]);
  rapidity34 /= (p3[0] + p4[0]) - (p3[3] + p4[3]);
  
  (rapidity34 < 1e-13) ? rapidity34 = 100.0 : rapidity34 = 0.5*std::log(rapidity34);
  double absrapidity34=fabs(rapidity34);

  double pt3 = 0;
  pt3 = std::sqrt( p3[1]*p3[1] + p3[2]*p3[2] );
  
  double pt4 = 0;
  pt4 = std::sqrt( p4[1]*p4[1] + p4[2]*p4[2] );
  
  double pt34 = 0;
  pt34 = std::sqrt( std::pow(p3[1] + p4[1],2) + std::pow(p3[2] + p4[2],2) );

 double mass34 = 0;
  mass34 = std::sqrt( std::pow(p3[0] + p4[0],2) - std::pow(p3[1] + p4[1],2) - std::pow(p3[2] + p4[2],2) - std::pow(p3[3] + p4[3],2) );

  double ptmin=0., ptmax=0.;
  if (pt3>pt4) {ptmin=pt4; ptmax=pt3;}
  else         {ptmin=pt3; ptmax=pt4;}
/*
  cout<<" rap3= "<<rapidity3<<" pt3= "<<pt3
      <<" rap4= "<<rapidity4<<" pt4= "<<pt4
      <<" ptmin= "<<ptmin<<" ptmax= "<<ptmax
      <<" rapidity34= "<<rapidity34<<" pt34= "<<pt34<<" mass= "<<mass34<<endl;
*/  


  Observable[ 0 ] = absrapidity3;
  Observable[ 1 ] = ptmin;
  Observable[ 2 ] = ptmax;
  Observable[ 3 ] = absrapidity34;
  Observable[ 4 ] = pt34;
  Observable[ 5 ] = pt3;
  Observable[ 6 ] = pt3;
  Observable[ 7 ] = pt34;
  Observable[ 8 ] = mass34;
  Observable[ 9 ] = absrapidity34;
  Observable[ 10 ] = pt3;
  Observable[ 11 ] = pt34;
  Observable[ 12 ] = mass34;
  Observable[ 13 ] = rapidity34;


  /*
  Observable[ 0 ] = rapidity3;
  Observable[ 1 ] = pt3;
  Observable[ 2 ] = rapidity4;
  Observable[ 3 ] = pt4;
  */
}

int cuts(int igrid)
{
  int fill;
  switch(igrid)
    {
    case(0):
      fill = 1;
      break;
    case(1):
      fill = 1;
      //      (std::abs(Observable[0]) <= 0.5) ? fill = 1 : fill = 0;
      //TC    (std::abs(Observable[0] <= 0.5)) ? fill = 1 : fill = 0;
      break;
    case(2):
      fill = 1;
      //TC      (std::abs(Observable[0] >= 3.0)) ? fill = 1 : fill = 0;
      //      (std::abs(Observable[0]) >= 3.0) ? fill = 1 : fill = 0;
      break;
    case(3):
      fill = 1;
      break;
    case(4):
      fill = 1;
      break;
    case(5):
      fill = 1;
      break;
    case(6):
      fill = 1;
      break;
    case(7):
      fill = 1;
      break;
    case(8):
      fill = 1;
      break;
    case(9):
      fill = 1;
      break;
    case(10):
      fill = 1;
      break;
    case(11):
      fill = 1;
      break;
    case(12):
      fill = 1;
      break;
    case(13):
      fill = 1;
      break;
    default: 
      std::cerr<<" In gridwrap.cpp::cuts(int). No such process : "<<igrid<<std::endl;
      exit(-1);
    }
  return fill;
}


// extern "C" void  fill_grid_reference_(const double evt[][mxpart], 
// 				      const double &wt, 
// 				      const double &wt2) // fills reference
// {
//   getObservable(evt);
//   for(int igrid = 0; igrid < Ngrids; igrid++)
//     if ( mygrid[igrid]->isOptimised() )
//       if( cuts(igrid) ) 
// 	if ( !( (Observable[igrid] > mygrid[igrid]->obsmax()) || (Observable[igrid] < mygrid[igrid]->obsmin())) )
// 	  mygrid[igrid]->getReference()->Fill(Observable[igrid], wt);
  
//   return;
// }

// extern "C" void  fill_grid_reference__(const double evt[][mxpart], 
// 				       const double &wt, 
// 				       const double &wt2) // fills reference
// { 
//   fill_grid_reference_(evt, wt, wt2);
// }

