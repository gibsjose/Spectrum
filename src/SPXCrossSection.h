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

class SPXCrossSection {

public:
	explicit SPXCrossSection(SPXPDFSteeringFile *psf, SPXPlotConfigurationInstance *pci) {
		this->psf = psf;
		this->pci = pci;
	}

	~SPXCrossSection(void) {
		//delete grid;
		//delete pdf;
	}

	void Create(void);
	//void ConfigureStyle();

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	TGraphAsymmErrors *GetPDFBandResults(void) {
		return pdf->h_PDFBand_results;
	}

	//Pseudo-Method for returning Alpha S results: (Just scales PDF errors by 1/3)
	TGraphAsymmErrors *GetAlphaSResults(void) {
		//return pdf->h_AlphaS_results;
		TGraphAsymmErrors *as = new TGraphAsymmErrors();
		&as = &pdf->h_PDFBand_results;
		SPXGraphUtilities::ScaleYErrors(as, (1.0/3.0));
		return as;
	}

	//Pseudo-Method for returning Scale results: (Just scales PDF errors by 1/3)
	TGraphAsymmErrors *GetScaleResults(void) {
		TGraphAsymmErrors *sc = new TGraphAsymmErrors();
		&sc = &pdf->h_PDFBand_results;
		SPXGraphUtilities::ScaleYErrors(sc, (1.0/4.0));
		return sc;
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

	/*
	TGraphAsymmErrors *GetReference(void) const {
		return SPXGraphUtilities::HistogramToGraph(grid->GetReference())
	}
	*/

private:
	static bool debug;					//Flag indicating debug mode
	unsigned int plotID;				//Plot ID
	SPXPDFSteeringFile *psf;			//Fully parsed PDF Steering File
	SPXPlotConfigurationInstance *pci;	//Fully parsed Plot configuration instance (contains grid steering file)
	SPXGrid *grid;						//Grid
	SPXPDF *pdf;						//PDF
};

#endif
