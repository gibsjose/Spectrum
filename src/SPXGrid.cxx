//************************************************************/
//
//	Grid Implementation
//
//	Implements the SPXGrid class, which contains the grid data
//  and manages the grid ROOT file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
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
	std::string mn = "CreateGrid: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	std::string gridFile = pci->gridSteeringFile.GetGridFilepath();

	if(!SPXFileUtilities::FileExists(gridFile)) {
		throw SPXFileIOException(gridFile, "Unable to open grid file");
	}

	//Create a new grid object from the grid file
	grid = new appl::grid(gridFile);

	if(!grid) {
		throw SPXGeneralException("APPLGrid: appl::grid(" + gridFile + ") did not return a valid object pointer");
	}

	//Create a reference histogram from the grid
	TH1D *referenceHistogram = (TH1D *)grid->getReference();

	if(!referenceHistogram) {
		throw SPXGeneralException("Reference histogram from appl::grid::getReference() for grid file " + gridFile + " was unsuccessful");
		referenceHistogramCorrupted = true;
	}

	int nTot = grid->run();
	referenceHistogram->Scale(1.0 / nTot);

	if(nTot < 0) {
		referenceHistogramCorrupted = true;
	}

	this->referenceHistogram = referenceHistogram;

	return referenceHistogram;
}
