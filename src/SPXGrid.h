//************************************************************/
//
//	Grid Header
//
//	Outlines the SPXGrid class, which contains the grid data
//  and manages the grid ROOT file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
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

	explicit SPXGrid(const SPXPlotConfigurationInstance &pci) {
		this->pci = pci;
	}

	//@TODO Delete here: Could be a source of errors
	~SPXGrid(void) {
		if(grid) {
			delete grid;
		}
	}
	
	void Parse(void);
	void Print(void);
	
	static bool GetDebug(void) {
		return debug;
	}
	
	static void SetDebug(bool b) {
		debug = b;
	}

	TH1D *GetReferenceHistogram(void);

private:
	static bool debug;					//Flag indicating debug mode
	SPXPlotConfigurationInstance pci;	//Frame options instance
	appl::grid *grid;					//APPLGrid Grid
};

#endif
