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

#include "SPXGrid.h"
#include "SPXPDF.h"
#include "SPXPDFSteeringFile.h"
#include "SPXPlotConfiguration.h"
#include "SPXGridCorrections.h"
#include "SPXSteeringFile.h"
#include "SPXUtilities.h"

class SPXCrossSection {

public:
   // TC changed this grid steering is in plot configuration
   //explicit SPXCrossSection(SPXPDFSteeringFile *psf, SPXPlotConfigurationInstance *pci) {
     explicit SPXCrossSection(SPXPlotConfigurationInstance *pci) {

              //this->psf = psf;
                this->psf =&pci->pdfSteeringFile;
		this->pci = pci;
             
		//Create graphs for grid reference and nominal
		gridReference = new TGraphAsymmErrors();
		nominal = new TGraphAsymmErrors();

                //RenScales.clear();
                //FacScales.clear();
                //RenScales.push_back(1.);
                //FacScales.push_back(1.);
                //RenScales.push_back(0.5);
                //FacScales.push_back(0.5);
                //RenScales.push_back(2.0);
                //FacScales.push_back(2.0);
                //RenScales.push_back(0.5);
                //FacScales.push_back(1.0);
                //RenScales.push_back(1.0);
                //FacScales.push_back(0.5);

	}

	~SPXCrossSection(void) {
		//delete grid;
		//delete pdf;
	}

	void Create(SPXSteeringFile *mainsteeringFile);
	void ParseCorrections(void);
	void ApplyCorrections(void);

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	bool IsDividedByBinWidth(void) const {
		return dividedByBinWidth;
	}

	TGraphAsymmErrors *GetPDFBandResults(void) {
		return pdf->h_PDFBand_results;
	}

	//Pseudo-Method for returning Alpha S results: (Just scales PDF errors by 1/3)
	TGraphAsymmErrors *GetAlphaSBandResults(void) {
		return pdf->h_AlphaS_results;
		//static TGraphAsymmErrors as;
		//as = *(pdf->h_PDFBand_results);
		//SPXGraphUtilities::ScaleYErrors(&as, (1.0/3.0));
		//return &as;
	}

	//Pseudo-Method for returning Scale results: (Just scales PDF errors by 1/3)
	TGraphAsymmErrors *GetScaleBandResults(void) {
		return pdf->h_Scale_results;
	        //static TGraphAsymmErrors sc;
		//sc = *(pdf->h_PDFBand_results);
		//SPXGraphUtilities::ScaleYErrors(&sc, (1.0/4.0));
		//return &sc;
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

private:
	static bool debug;					 //Flag indicating debug mode
	unsigned int plotID;					//Plot ID
	SPXPDFSteeringFile *psf;				//Fully parsed PDF Steering File
	SPXPlotConfigurationInstance *pci;			//Fully parsed Plot configuration instance (contains grid steering file)
	SPXGrid *grid;						//Grid
	SPXPDF *pdf;						//PDF
	SPXGridCorrections *corrections;			//Grid corrections
	bool dividedByBinWidth;					//Flag indicating that the grid was already divided by the bin width

	TGraphAsymmErrors *gridReference;			//Reference Graph from Grid
	TGraphAsymmErrors *nominal;				//Nominal (default) graph from PDF

};

#endif
