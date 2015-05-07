//************************************************************/
//
//      Dump latex tables from Spectrum tables
//
//      @Author:        T. Carli
//      @Date:          2.5.2015
//      @Email:         
//
//************************************************************/
#include <sstream>
#include <string.h>

#include "SPXLatexTable.h"
#include "SPXUtilities.h"

const std::string cn = "SPXLatexTable::";

//Must define the static debug variable in the implementation
bool SPXLatexTable::debug;

SPXLatexTable::SPXLatexTable(std::vector<SPXData*> mydata, std::vector<SPXCrossSection> mycrossSections, SPXSteeringFile *mysteeringFile) {
 std::string mn = "SPXLatexTable: ";
 debug=true;
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn); 
 
 data=mydata;
 crossSections= mycrossSections;
 steeringFile=mysteeringFile;

 if (!steeringFile)  std::cout<<cn<<mn<<"Steering file not found "<<std::endl;

 if (data.size()==0) std::cout<<cn<<mn<<"No data object in vector"<<std::endl;
 else	             std::cout<<cn<<mn<<"Number of data objects= "<<data.size()<<std::endl;

 if (crossSections.size()==0) std::cout<<cn<<mn<< "No cross section object in vector"<<std::endl;
 else                         std::cout<<cn<<mn<<"Number of CrossSection objects: "<<crossSections.size()<<std::endl;

 this->DumpTable(); 

}

void SPXLatexTable::DumpTable(void) {
 std::string mn = "DumpTable: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);


 for(int idata = 0; idata < data.size(); idata++) {                   

  TString datalabel=data.at(idata)->GetLegendLabel();

  if(debug) std::cout<<cn<<mn<<"Data name= "<< datalabel.Data() << std::endl;

  if (debug) {
   std::cout<<cn<<mn<<"Information on removed bins from main steering "<<std::endl;

   if (steeringFile->GetDataRemoveXbinsFlag(idata)) {
    double xmax=steeringFile->GetDataCutXmax(idata);
    double xmin=steeringFile->GetDataCutXmin(idata); 

    std::cout<<cn<<mn<<"Removed bins from data with value > "<<xmax<<" and < "<<xmin<<std::endl;
   } else 
    std::cout<<cn<<mn<<"No bins removed via main steering "<<std::endl;
  }

  if (debug) {
   std::cout<<cn<<mn<<"  "<<std::endl;
   std::cout<<cn<<mn<<"Information on removed bins from data objects "<<datalabel.Data()<<std::endl;
   if (data.at(idata)-> GetDataRemoveXbinsFlag()) {
    std::cout<<cn<<mn<<"Bins removed from data: "<<std::endl;
    double xmin=data.at(idata)->GetDataCutXmin();
    if (xmin!=-HUGE_VAL)
     std::cout<<cn<<mn<<" xmin= "<<xmin<<std::endl;

    double xmax=data.at(idata)->GetDataCutXmin();
    if (xmax!=HUGE_VAL)
     std::cout<<cn<<mn<<" xmax= "<<xmax<<std::endl;
   } else
    std::cout<<cn<<mn<<"No bins removed  "<<std::endl;

  }

  if(debug) {
   std::cout<<cn<<mn<< "  " << std::endl;
   std::cout<<cn<<mn<<"Example how to access stat, total syst and total uncertainties "<< std::endl;
  }

  TGraphAsymmErrors *statGraph = data[idata]->GetStatisticalErrorGraph();
  if (!statGraph) throw SPXGeneralException(cn+mn+"Can not open statistical uncertainty for"+datalabel.Data());

  TGraphAsymmErrors *systGraph = data[idata]->GetSystematicErrorGraph();
  if (!systGraph) throw SPXGeneralException(cn+mn+"Can not open systematic uncertainty for"+datalabel.Data());

  TGraphAsymmErrors *totGraph  = data[idata]->GetTotalErrorGraph();
  if (!totGraph) throw SPXGeneralException(cn+mn+"Can not open statistical total for"+datalabel.Data());

  if (debug) {
   std::cout<<cn<<mn<<"Print "<<systGraph->GetName() << std::endl;
   //systGraph->Print();
  }

  if (steeringFile->ShowIndividualSystematics()!=0) {

   std::vector <TGraphAsymmErrors *> vsyst;
   vsyst=data[idata]->GetSystematicsErrorGraphs();

   if(debug) {
    std::cout<<cn<<mn<<"Total number of systematics uncertainty components found= " << vsyst.size() <<std::endl;
    std::cout<<cn<<mn<<"Only show systematics above: "<<steeringFile->ShowIndividualSystematics()<<"%"<<std::endl;
   }

   std::vector<TGraphAsymmErrors * > vsystord=SPXUtilities::OrderLargestRelativeErrorGraphVector(vsyst);

   for (int isyst=0; isyst<vsystord.size(); isyst++) {
    std::string systname=vsystord.at(isyst)->GetName();

    //vsyst.at(isyst)->Print();

    double emax=  SPXGraphUtilities::GetLargestRelativeError(vsystord.at(isyst))*100.; // in percent
    //if (debug) std::cout<<cn<<mn<<"emax= "<<emax <<std::endl;

    if (fabs(emax)>=steeringFile->ShowIndividualSystematics()) {
     if (debug) {
      std::cout<<cn<<mn<<" "<<std::endl;
      std::cout<<cn<<mn<<"systematics "<<systname.c_str()<<" largest syst= "<< emax<<"% cut= "<<steeringFile->ShowIndividualSystematics()<<"%"<< std::endl;
      // show relative error
      TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(vsystord.at(isyst),totGraph,ZeroDenGraphErrors);
      graph->Print(); 
     }
    } else {
      //if (debug) std::cout<<cn<<mn<<"systematics "<<systname.c_str()<<" too small ! Largest systematics= "<<emax<<"% < " << steeringFile->ShowIndividualSystematics()<<"%" <<std::endl;
    }
   }
  }
 }

 return;
}
