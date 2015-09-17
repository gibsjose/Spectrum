//************************************************************/
//
//	Grid Header
//
//	Outlines the SPXGrid class, which contains the grid data
//  and manages the grid ROOT file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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
          if (debug) std::cout<<" SPXGrid() constructor "<<std::endl;
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	const std::string & GetGridName(void) const {
	 return gridname;
	 //return pci->gridSteeringFile.GetGridFilepath();
	}

        void SetGridName(std::string newname) {
	 gridname=newname;
         return;
        }

	const std::string & GetName(void) const {
	 return pci->gridSteeringFile.GetName();
	}

	const std::string & GetNLOProgramNameName(void) const {
	 return pci->gridSteeringFile.GetNLOProgramName();
	}

	const std::string & GetScaleFunctionalForm(void) const {
	 return pci->gridSteeringFile.GetScaleFunctionalForm();
	}


	//Creates the Grid and return the reference histogram
	TH1D * CreateGrid(void);

        appl::grid *GetGrid(int i){ 
	 if (i>vgrid.size()) {
	  std::cout<<"GetGrid:: something is wrong i= "<<i<<" but vector size "<<vgrid.size()<<std::endl;
	 }
         return vgrid.at(i);
        };

        int GetNumberofGrids() {
	 return vgrid.size();
        }

	//Returns the Grid Reference histogram
	TH1D * GetReference(void) {
		return referenceHistogram;
	}

	bool IsReferenceHistogramCorrupted(void) {
		return referenceHistogramCorrupted;
	}


        double GetChangeSqrtS(void) const {
	 return pci->gridSteeringFile.GetChangeSqrtS();
	}

private:
	static bool debug;		     // Flag indicating debug mode
	SPXPlotConfigurationInstance *pci;   // Plot configuration instance

	//appl::grid *grid;		     // APPLGrid Grid
	std::vector <appl::grid *> vgrid;    // vector of APPLGrid Grid

	bool referenceHistogramCorrupted;    // Flag indicating that the reference histogram has been corrupted
	TH1D * referenceHistogram;	     // Reference histogram
	std::string gridname;                // will give name to root object (graphs, histograms)


};

#endif
