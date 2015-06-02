//************************************************************/
//
//	Grid Header
//
//	Outlines the SPXGrid class, which contains the grid data
//  and manages the grid ROOT file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXGRID_H
#define SPXGRID_H

#include "appl_grid/appl_grid.h"
#include "appl_grid/generic_pdf.h"

#include "SPXROOT.h"

#include "SPXPlotConfiguration.h"
#include "SPXException.h"

class SPXGrid {

public:
	SPXGrid(void) {}

	explicit SPXGrid(SPXPlotConfigurationInstance *pci) {
		this->pci = pci;

                gridname=pci->gridSteeringFile.GetGridFilepath();

		//CreateGrid();
	}

	//@TODO Delete here: Could be a source of errors
	~SPXGrid(void) {
		if(grid) {
			//delete grid;
		}
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	const std::string & GetGridName(void) const {
	 return gridname;
	  //	return pci->gridSteeringFile.GetGridFilepath();
	}

        void SetGridName(std::string newname) {
	 gridname=newname;
         return;
        }

	const std::string & GetName(void) const {
	 return pci->gridSteeringFile.GetName();
	}

	//Creates the Grid and return the reference histogram
	TH1D * CreateGrid(void);

        appl::grid *GetGrid(){ return grid;};

	//Returns the Grid Reference histogram
	TH1D * GetReference(void) {
		return referenceHistogram;
	}

	bool IsReferenceHistogramCorrupted(void) {
		return referenceHistogramCorrupted;
	}


private:
	static bool debug;		     // Flag indicating debug mode
	SPXPlotConfigurationInstance *pci;   // Plot configuration instance
	appl::grid *grid;		     // APPLGrid Grid
	bool referenceHistogramCorrupted;    // Flag indicating that the reference histogram has been corrupted
	TH1D * referenceHistogram;	     //Reference histogram
	std::string gridname;                     // will give name to root object (graphs, histograms)
};

#endif
