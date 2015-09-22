//************************************************************/
//
//	Grid Implementation
//
//	Implements the SPXGrid class, which contains the grid data
//  and manages the grid ROOT file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXGrid.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXGrid::";

//Must define the static debug variable in the implementation
bool SPXGrid::debug;

TH1D * SPXGrid::CreateGrid(void) {
 debug=true;
 std::string mn = "CreateGrid: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 //std::string gridFile = pci->gridSteeringFile.GetGridFilepath();

 TH1D *referenceHistogramsum;

 std::vector <std::string> vgridfiles=pci->gridSteeringFile.GetGridFilepathVector();

 if (debug) std::cout<<cn<<mn<<" Number of grids= "<< vgridfiles.size()<<std::endl;

 for (int igrid=0; igrid<vgridfiles.size(); igrid++) {
  std::string gridFile= vgridfiles.at(igrid);
  
  if (debug) {
   std::cout <<cn<<mn<<" igrid= "<<igrid<<" gridFile "<<gridFile.c_str() << std::endl;
  }

  if(!SPXFileUtilities::FileExists(gridFile)) {
   throw SPXFileIOException(gridFile, cn+mn+"Unable to open grid file");
  }

  //Create a new grid object from the grid file
  appl::grid * grid = new appl::grid(gridFile);
  if (!grid) {
   throw SPXGeneralException(cn+mn+"APPLGrid: appl::grid(" + gridFile + ") did not return a valid object pointer");
  }
  vgrid.push_back(grid);

  //Create a reference histogram from the grid
  TH1D *referenceHistogram = (TH1D *)grid->getReference();

  if (!referenceHistogram) {
   throw SPXGeneralException("Reference histogram from appl::grid::getReference() for grid file " + gridFile + " was unsuccessful");
   referenceHistogramCorrupted = true;
  }

  int nTot = grid->run();
  referenceHistogram->Scale(1.0 / nTot);

  if (nTot < 0) {
   referenceHistogramCorrupted = true;
  }

  this->referenceHistogram = referenceHistogram;

  if (debug) {
   std::cout <<cn<<mn<<"Printing reference histogram: " << std::endl;
   this->referenceHistogram->Print("all");
  }

  if (igrid==0) {
   referenceHistogramsum=referenceHistogram;
  } else {
   SPXGraphUtilities::Add( referenceHistogramsum, referenceHistogram);
  }

 }

 std::vector <std::string> vgridfilesAlternativeScaleChoice=pci->gridSteeringFile.GetGridFilepathAlternativeScaleChoiceVector();
 std::cout<<cn<<mn<<" Number of alternative scale choice grids= "<< vgridfilesAlternativeScaleChoice.size()<<std::endl;

 for (int igrid=0; igrid<vgridfilesAlternativeScaleChoice.size(); igrid++) {
  std::string gridFileAlternativeScaleChoice= vgridfilesAlternativeScaleChoice.at(igrid);
  
  if (debug) {
   std::cout <<cn<<mn<<" igrid= "<<igrid<<" gridFileAlternativeScaleChoice "<<gridFileAlternativeScaleChoice.c_str() << std::endl;
  }

  if(!SPXFileUtilities::FileExists(gridFileAlternativeScaleChoice)) {
   throw SPXFileIOException(gridFileAlternativeScaleChoice, cn+mn+"Unable to open AlternativeScaleChoice grid file");
  }

  std::cout <<cn<<mn<<"Get alternative scale choice grid igrid= "<< igrid << std::endl;

  appl::grid * gridAlternativeScaleChoice = new appl::grid(gridFileAlternativeScaleChoice);
  if (!gridAlternativeScaleChoice) {
   throw SPXGeneralException(cn+mn+"APPLGrid: appl::grid(" + gridFileAlternativeScaleChoice + ") did not return a valid object pointer");
  }
  vgridAlternativeScaleChoice.push_back(gridAlternativeScaleChoice);

 }
 
 referenceHistogram=referenceHistogramsum;
 return referenceHistogram;
}
