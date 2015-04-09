//************************************************************/
//
//      Utilities Implementation
//
//
//      @Author:        J. Gibson, C. Embree, T. Carli - CERN ATLAS
//      @Date:          20.11.2014
//      @Email:         gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXUtilities.h"

const std::string cn = "SPXUtilities::";

bool SPXUtilities::SortLargestRelativeError(TGraphAsymmErrors * g1, TGraphAsymmErrors * g2){
  std::string mn = "SortLargestRelativeError: ";

  if (!g1) {
   std::cout<<cn<<mn<<"WARNING: no input graph g1 found ! "<<std::endl;
   return false;
  }

  if (!g2) {
   std::cout<<cn<<mn<<"WARNING: no input graph g2 found ! "<<std::endl;
   return false;
  }

  double emax1= SPXGraphUtilities::GetLargestRelativeError(g1);  
  double emax2= SPXGraphUtilities::GetLargestRelativeError(g2);  

  bool sort=false;
  if (fabs(emax1)>fabs(emax2)) sort=true;

  //if (sort) std::cout<<cn<<mn<<" sort= "<<sort<<std::endl;
  //std::cout<<cn<<mn<<" g1= "<<g1->GetName()<<" emax1= "<<emax1<<" g2= "<<g2->GetName()<<" emax2= "<<emax2<<std::endl;
 
  return sort;
};

bool SPXUtilities::SortLargestBinNumber(TGraphAsymmErrors * g1, TGraphAsymmErrors * g2){
  std::string mn = "SortLargestBinNumber: ";

  if (!g1) {
   std::cout<<cn<<mn<<"WARNING: no input graph g1 found ! "<<std::endl;
   return false;
  }

  if (!g2) {
   std::cout<<cn<<mn<<"WARNING: no input graph g2 found ! "<<std::endl;
   return false;
  }

  int n1=SPXGraphUtilities::CompareValues(g1, g2, true);
  int n2=SPXGraphUtilities::CompareValues(g2, g1, true);

  bool sort=false;
  if (n1>n2) sort=true;

  //if (sort) std::cout<<cn<<mn<<" sort= "<<sort<<std::endl;
  //std::cout<<cn<<mn<<" g1= "<<g1->GetName()<<" n1= "<<n1<<" g2= "<<g2->GetName()<<" n2= "<<n2<<std::endl;
 
  return sort;
};

std::vector<TGraphAsymmErrors * >  SPXUtilities::OrderLargestRelativeErrorGraphVector(std::vector< TGraphAsymmErrors *> inputgraph){
  std::string mn = "OrderLargestRelativeGraphVector: ";
  //
  // order graphs according to largest relative errors in nay of the bins
  //

  std::vector<TGraphAsymmErrors * > outputgraph;
  outputgraph.clear();  

  if (inputgraph.size()==0) {
   std::cout<<cn<<mn<<"WARNING: no input graph found ! "<<std::endl;
   return outputgraph;
  }

  sort(inputgraph.begin(), inputgraph.end(), SortLargestRelativeError);
  outputgraph=inputgraph;
  
  //std::cout<<cn<<mn<<" After ordering: "<<std::endl;
  //for (int igraph1=0; igraph1 < inputgraph.size(); igraph1++) {
  // TGraphAsymmErrors *graph1 = inputgraph.at(igraph1);
  // std::cout<<cn<<mn<<" graph "<<graph1->GetName()<<" emax= "<< SPXGraphUtilities::GetLargestRelativeError(graph1)<<std::endl;
  //}

}

std::vector<TGraphAsymmErrors * > SPXUtilities::OrderLargestBinNumberGraphVector (std::vector< TGraphAsymmErrors *> inputgraph){
  std::string mn = "OrderLargestBinNumberGraphVector: ";
  //
  // order graphs according to the largest number of bins that have larger errors 
  //

  std::vector<TGraphAsymmErrors * > outputgraph;
  outputgraph.clear();  

  if (inputgraph.size()==0) {
   std::cout<<cn<<mn<<"WARNING: no input graph found ! "<<std::endl;
   return outputgraph;
  }

  sort(inputgraph.begin(), inputgraph.end(), SortLargestBinNumber);

  outputgraph=inputgraph;

};

/*
std::vector<TGraphAsymmErrors * > SPXUtilities::OrderGraphVector(std::vector< TGraphAsymmErrors *> inputgraph){
  std::string mn = "OrderGraphVector: ";
  //
  // order bands such that largest band is entered first
  //
  std::vector<TGraphAsymmErrors * > outputgraph;
  outputgraph.clear();  

  std::map<int, TGraphAsymmErrors * > bands;
  bands.clear();  

  if (inputgraph.size()==0) {
   std::cout<<cn<<mn<<"WARNING: no input graph found ! "<<std::endl;
   return outputgraph;
  }
 
  for (int igraph1=0; igraph1 < inputgraph.size(); igraph1++) {
   TGraphAsymmErrors *graph1 = inputgraph.at(igraph1);
   if (!graph1) {
    throw SPXGraphException(cn + mn + "graph not found");
   }
   //
   // fill map to order graph according to their maximal values
   // i.e. number of bins for which first graph has larger band
   //
   int nmax=-1;
   for (int igraph2=0; igraph2 < inputgraph.size(); igraph2++) {
    if (igraph1==igraph2) continue;
    TGraphAsymmErrors *graph2 = inputgraph.at(igraph2);
    int nbin=SPXGraphUtilities::CompareValues(graph1, graph2);
    if (nbin>nmax) nmax=nbin;
   }
   //
   // check if key already exists in map -> otherwise look for different locations
   //
   if (bands.count(nmax)) {
    //std::cout<<cn<<mn<<" key nmax= "<<nmax<<" is occupied for " <<graph1->GetName()<<std::endl;
    TGraphAsymmErrors *graphold=bands[nmax]; 
    int nold=SPXGraphUtilities::CompareValues(graph1, graphold);
    //std::cout<<cn<<mn<<" key nold= "<<nold<<" " <<std::endl;
    bool filled=false;
    int nbin=graphold->GetN();
    for (int ibin=1; ibin<nbin; ibin++) { // can not have more location than nbin
     int inew=0;

     if (nold>nbin/2) inew=nmax+ibin;      // fill graph1 with index larger than graphold
     else             inew=nmax-ibin;      // fill graph1 with index larger than graphold

     //std::cout<<cn<<mn<<" inew= "<<inew<<std::endl;

     for(std::map<int, TGraphAsymmErrors *>::reverse_iterator it=bands.rbegin(); it!=bands.rend(); ++it) {
      //std:cout<<cn<<mn<<it->first<<" "<<it->second->GetName()<<std::endl;
      if (bands.count(inew)==0) {
       filled=true;
       //std::cout<<cn<<mn<<" fill new locations inew= "<<inew<<std::endl;
       bands[inew]=graph1;
       outputgraph.push_back(graph1);
       break;
      }
     }
     if (filled) break;
    }
   } else {
    bands[nmax]=graph1;
   }
  } 
  return outputgraph;
}  
*/

std::map<int, TGraphAsymmErrors * >  SPXUtilities::OrderBandMap(std::vector< TGraphAsymmErrors *> inputgraph){
  std::string mn = "OrderBandMap: ";
  //
  // order bands such that largest band is entered first, i.e. plotted last
  //

  std::map<int, TGraphAsymmErrors * > bands;
  bands.clear();  

  if (inputgraph.size()==0) {
   std::cout<<cn<<mn<<"WARNING: no input graph found ! "<<std::endl;
   return bands;
  }

  
  for (int igraph1=0; igraph1 < inputgraph.size(); igraph1++) {
   TGraphAsymmErrors *graph1 = inputgraph.at(igraph1);
   if (!graph1) {
    throw SPXGraphException(cn + mn + "graph not found");
   }
   //
   // fill map to order graph according to their maximal values
   // i.e. number of bins for which first graph has larger band
   //
   int nmax=-1;
   for (int igraph2=0; igraph2 < inputgraph.size(); igraph2++) {
    if (igraph1==igraph2) continue;
    TGraphAsymmErrors *graph2 = inputgraph.at(igraph2);
    int nbin=SPXGraphUtilities::CompareValues(graph1, graph2);
    if (nbin>nmax) nmax=nbin;
    //std::cout<<cn<<mn<<" "<<graph1->GetName()<<" "<<graph2->GetName()<<" nbin= "<<nbin<<std::endl;
   }
   //
   // check if key already exists in map -> otherwise look for different locations
   //
   if (bands.count(nmax)) {
    //std::cout<<cn<<mn<<" key nmax= "<<nmax<<" is occupied for " <<graph1->GetName()<<std::endl;
    TGraphAsymmErrors *graphold=bands[nmax]; 
    int nold=SPXGraphUtilities::CompareValues(graph1, graphold);
    //std::cout<<cn<<mn<<" key nold= "<<nold<<" " <<std::endl;
    bool filled=false;
    int nbin=graphold->GetN();
    for (int ibin=1; ibin<nbin; ibin++) { // can not have more location than nbin
     int inew=0;

     if (nold>nbin/2) inew=nmax+ibin;      // fill graph1 with index larger than graphold
     else             inew=nmax-ibin;      // fill graph1 with index larger than graphold

     //std::cout<<cn<<mn<<" inew= "<<inew<<std::endl;

     for(std::map<int, TGraphAsymmErrors *>::reverse_iterator it=bands.rbegin(); it!=bands.rend(); ++it) {
      //std:cout<<cn<<mn<<it->first<<" "<<it->second->GetName()<<std::endl;
      if (bands.count(inew)==0) {
       filled=true;
       //std::cout<<cn<<mn<<" fill new locations inew= "<<inew<<std::endl;
       bands[inew]=graph1;
       break;
      }
     }
     if (filled) break;
    }
   } else {
    bands[nmax]=graph1;
   }
  } 
  return bands;
}

Color_t SPXUtilities::ICol(int i){
  if (i== 0) return kPink+9;
  if (i== 1) return kBlue;
  if (i== 2) return kAzure+10;
  if (i== 3) return kViolet;
  if (i== 4) return kViolet+2;
  if (i== 5) return kGreen+1;
  if (i== 6) return kCyan+2;
  if (i== 7) return kRed+1;
  if (i== 8) return kRed-1;
  if (i== 9) return kOrange-2;
  if (i==10) return kOrange+7;
  if (i==11) return kSpring-1;
  if (i==12) return kSpring+3;
  if (i==13) return kYellow+2;
  if (i==14) return kYellow-7;
  if (i==15) return kYellow-8;
  if (i==16) return kYellow+4;
  if (i==17) return kMagenta-10;
  if (i==18) return kMagenta-5;
  if (i==19) return kCyan-10;
  if (i==20) return kCyan-9;
  if (i>20 && i<=30) return kPink  +i-20;
  if (i>30 && i<=40) return kOrange+i-30;
  if (i>40 && i<=50) return kViolet+i-40;
  if (i>50 && i<=60) return kTeal  -i+50;
  if (i>60 && i<=70) return kOrange-i+60;
  if (i>70 && i<=80) return kViolet-i+70;
  if (i>80 && i< 90) return kAzure -i+80;
  if (i>90 && i<=100) return kPink +i-90;

  std::cout<<cn<<"IColBlue: WARNING: not optimized for i> "<<i<<std::endl;

}

Color_t SPXUtilities::IColBlue(int i){
  if (i== 0) return kPink+9;
  if (i== 1) return kBlue;
  if (i== 2) return kViolet+1;
  if (i== 3) return kBlue+2;
  if (i== 4) return kAzure+6;
  if (i== 5) return kBlue-6;
  if (i== 6) return kBlue-9;
  if (i== 7) return kAzure+1;
  if (i== 8) return kViolet-2;
  if (i== 9) return kCyan-4;
  if (i==10) return kCyan-8;
  //if (i==11) return kCyan-2;
  if (i>10 && i<=20) return kPink  +i-10;
  if (i>20 && i<=30) return kPink  -i+20;
  if (i>30 && i< 40) return kAzure -i+30;
  if (i>40 && i< 50) return kAzure +i-40;
  if (i>50 && i<=60) return kViolet+i-50;
  if (i>60 && i<=70) return kViolet-i+60;
  if (i>70 && i<=80) return kCyan  +i-70;
  if (i>80 && i<=90) return kCyan  -i+80;

  std::cout<<cn<<"IColBlue: WARNING: not optimized for i> "<<i<<std::endl;

  return kAzure-i;
}

Color_t SPXUtilities::IColRed(int i){
  if (i== 0) return kRed-10;
  if (i== 1) return kRed;
  if (i== 2) return kRed+2;
  if (i== 3) return kRed+3;
  if (i== 4) return kOrange;
  if (i== 5) return kOrange+1;
  if (i== 6) return kOrange+2;
  if (i== 7) return kOrange+3;
  if (i== 8) return kOrange+5;
  if (i== 9) return kOrange+9;
  if (i==10) return kOrange-1;
  if (i==11) return kMagenta;
  if (i==12) return kMagenta+3;
  if (i==13) return kMagenta-6;
  if (i==14) return kMagenta+9;

 return kPink+1;

 std::cout<<cn<<"IColRed: WARNING: not optimized for i> "<<i<<std::endl;

}

Color_t SPXUtilities::IColGreen(int i){
  if (i== 0) return kSpring;
  if (i== 1) return kGreen;
  if (i== 2) return kGreen+2;
  if (i== 3) return kTeal+3;
  if (i== 4) return kGreen-3;
  if (i== 5) return kGreen-8;
  if (i== 6) return kSpring-2;
  if (i== 7) return kSpring-5;
  if (i== 8) return kSpring-7;
  if (i== 9) return kSpring-9;
  if (i==10) return kCyan+1;
  if (i==11) return kCyan-4;
  if (i==12) return kCyan-6;
  if (i==13) return kYellow-1;
  if (i==14) return kYellow-7;

 return kSpring+i;

 std::cout<<cn<<"IColGreen: WARNING: not optimized for i> "<<i<<std::endl;

}

