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

	//const std::string & GetAlternativeScaleChoiceName(void) const {
	// return pci->gridSteeringFile.GetAlternativeScaleChoiceName();
	//}

	const std::string & GetNLOProgramNameName(void) const {
	 return pci->gridSteeringFile.GetNLOProgramName();
	}

	const std::string & GetScaleFunctionalForm(void) const {
	 return pci->gridSteeringFile.GetScaleFunctionalForm();
	}

	//Creates the Grid and return the reference histogram
	TH1D * CreateGrid(void);

        appl::grid *GetGrid(int i){ 
	 if (i>=vgrid.size()) {
          std::ostringstream oss;
          oss << "SPXGrid::GetGrid:: something is wrong i= "<<i<<" but vector size "<<vgrid.size();
	  std::cout<<oss.str()<<std::endl;
          throw SPXGeneralException(oss.str());
	 }
         return vgrid.at(i);
        };

        appl::grid *GetGridAlternativeScaleChoice(int i){ 
	 if (i>=vgridAlternativeScaleChoice.size()) {
          std::ostringstream oss;
          oss << "SPXGrid::GetGridAlternativeScaleChoice:: something is wrong i= "<<i<<" but vector size "<<vgrid.size();
	  std::cout<<oss.str()<<std::endl;
          throw SPXGeneralException(oss.str());
	 }
         return vgridAlternativeScaleChoice.at(i);
        };

        int GetNumberofGrids() {
	 return vgrid.size();
        }

        int GetNumberofAlternativeScaleChoiceGrids() {
	 return vgridAlternativeScaleChoice.size();
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

        double GetParameterValue(void) const {
         return this->parameterValue;
        }            

	std::string GetParameterName(void) const {
         return this->parameterName;
        }            

	std::string GetParameterUnit(void) const {
         return this->parameterUnit;
        }            

        void SetParameterValue(double par) {
         parameterValue=par;
        }            

	void SetParameterName(std::string name) {
         parameterName=name;
        }            

	void SetParameterUnit(std::string name) {
         parameterUnit=name;
        }            


private:
	static bool debug;		     // Flag indicating debug mode
	SPXPlotConfigurationInstance *pci;   // Plot configuration instance

	//appl::grid *grid;		     // APPLGrid Grid
	std::vector <appl::grid *> vgrid;    // vector of APPLGrid Grid

	std::vector <appl::grid *> vgridAlternativeScaleChoice; // vector of APPLGrid Grid for alternative scale choice

	bool referenceHistogramCorrupted;    // Flag indicating that the reference histogram has been corrupted
	TH1D * referenceHistogram;	     // Reference histogram
	std::string gridname;                // will give name to root object (graphs, histograms)

        double parameterValue;              // value of paramter of Grid, e.g. top mass
	std::string parameterName;          // name of parameter 
	std::string parameterUnit;          // unit of parameter 

};

#endif
