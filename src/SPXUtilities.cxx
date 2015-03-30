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

std::map<int, TGraphAsymmErrors * >  SPXUtilities::OrderBandMap(std::vector< TGraphAsymmErrors *> inputgraph){
  std::string mn = "OrderBandMap: ";
  //
  // order bands such that largest band is entered first, i.e. plotted last
  //

  std::map<int, TGraphAsymmErrors * > bands;
  bands.clear();  

  if (inputgraph.size()==0) {
   std::cout<<cn<<mn<<"WARNING no input graph found ! "<<std::endl;
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
