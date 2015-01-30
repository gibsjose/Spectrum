//************************************************************/
//
//	CrossSection Header
//
//	Outlines the SPXCrossSection class, which performs the
//	convolution and all math required to calculate the cross-
//	section from a PDF and a grid file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXCROSSSECTION_H
#define SPXCROSSSECTION_H

#include "SPXROOT.h"

#include "SPXPDFSteeringFile.h"
#include "SPXPlotConfiguration.h"
#include "SPXGridCorrections.h"
#include "SPXSteeringFile.h"
#include "SPXUtilities.h"

#include "SPXGrid.h"
#include "SPXPDF.h"

class SPXCrossSection {

public:

     explicit SPXCrossSection(SPXPlotConfigurationInstance *pci) {

                this->psf =&pci->pdfSteeringFile;
		this->pci = pci;
             
		//Create graphs for grid reference and nominal
		gridReference = new TGraphAsymmErrors();
		nominal = new TGraphAsymmErrors();

	}

	~SPXCrossSection(void) {
	 //delete grid;
	 //delete pdf;
	}

	void Create(SPXSteeringFile *mainsteeringFile);
	void ParseCorrections(void);
	void ApplyCorrections();

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	bool IsDividedByBinWidth(void) const {
		return dividedByBinWidth;
	}

	SPXPDFSteeringFile *GetPDFSteeringFile(void) {
		return psf;
	}

	SPXPlotConfigurationInstance *GetPlotConfigurationInstance(void) {
		return pci;
	}

	SPXGrid *GetGrid(void) {
		return grid;
	}

	SPXPDF *GetPDF(void) {
		return pdf;
	}

	TGraphAsymmErrors * GetGridReference(void) {
		return gridReference;
	}

	TGraphAsymmErrors * GetNominal(void) {
		return nominal;
	}

        std::vector<std::string> GetCorrectionLabels();

        void MatchBinning(StringGraphMap_T dataFileGraphMap);
        void UpdateBandandHisto(void);


private:
	static bool debug;		       // Flag indicating debug mode

	SPXPDFSteeringFile *psf;	       // Fully parsed PDF Steering File
	SPXPlotConfigurationInstance *pci;     // Fully parsed Plot configuration instance (contains grid steering file)
        SPXSteeringFile *mainsteeringFile;       // pointer to main steering file frpm SPXPlot

	SPXGrid *grid;			       // Grid
	SPXPDF *pdf;			       // PDF
	SPXGridCorrections *corrections;       // Grid corrections
	bool dividedByBinWidth;		       // Flag indicating that the grid was already divided by the bin width

	TGraphAsymmErrors *gridReference;      // Reference Graph from Grid
	TGraphAsymmErrors *nominal;	       // Nominal (default) graph from PDF

        std::vector<std::string> correctionlabels; // labels of corrections from Grid Steering file
       	                                           // corrections can be Hadcorr, e.w.


};

#endif
