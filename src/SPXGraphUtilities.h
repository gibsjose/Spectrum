//************************************************************/
//
//	Graph Utilities Header
//
//	Some useful graph utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		13.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXGRAPHUTILITIES_H
#define SPXGRAPHUTILITIES_H

#include <iostream>

#include "SPXROOT.h"
#include "SPXException.h"

class SPXGraphUtilities {

public:

	static void Scale(TGraphAsymmErrors *graph, double xScale, double yScale) {
		double *x = graph->GetX();
		double *y = graph->GetY();
		double *exh = graph->GetEXhigh();
		double *exl = graph->GetEXlow();
		double *eyh = graph->GetEYhigh();
		double *eyl = graph->GetEYlow();

		for(int i = 0; i < graph->GetN(); i++) {
			graph->SetPoint(i, (x[i] * xScale), (y[i] * yScale));
			graph->SetPointError(i, (exl[i] * xScale), (exh[i] * xScale), 
				(eyl[i] * yScale), (eyh[i] * yScale));
		}
	}

	static void Normalize(TGraphAsymmErrors *graph, double xScale, double yScale, 
		bool normalizedToTotalSigma, bool dividedByBinWidth) {

		//TEMPORARY DEBUG
		bool debug = true;

		//NOTE:
		//normalizedToTotalSigma indicates that the graph is ALREADY normalized to the total sigma value
		//dividedByBinWidth indicates that the graph is ALREADY divided by the bin width

		double totalSigma = 0;

		double *pX = graph->GetX();
		double *pY = graph->GetY();
		double *pEXhigh = graph->GetEXhigh();
		double *pEXlow = graph->GetEXlow();
		double *pEYhigh = graph->GetEYhigh();
		double *pEYlow = graph->GetEYlow();

		unsigned int numberOfBins = graph->GetN();

		double sigma = 0;

		//Compute total sigma
		for(int i = 0; i < numberOfBins; i++) {
			sigma = pY[i] * yScale;
			double binWidth = pEXlow[i] + pEXhigh[i];

			if(debug) std::cout << "sigma = " << sigma << " : pY[i] * yScale = " << pY[i] << " * " << yScale << std::endl;

			if(debug) std::cout << "Bin[" << i << "]: binWidth = " << binWidth << std::endl;
			
			if(dividedByBinWidth) {
				totalSigma += sigma * binWidth;
			} else {
				totalSigma += sigma;
			}

			if(debug) std::cout << "Bin[" << i << "]: totalSigma = " << totalSigma << std::endl;
			//@TODO Check normalization and use of double += sigma here
			//totalSigma += sigma;
		}

		double scale = yScale;

		//Normalize
		for(int i = 0; i < numberOfBins; i++) {
			if(normalizedToTotalSigma) {
				scale = yScale / totalSigma;
			} //@TODO move scale = yScale to an else? What about the second time around the for loop???
			
			if(debug) std::cout << "Bin[" << i << "]: scale = "  << scale << std::endl;

			double binWidth = 1;

			if(dividedByBinWidth) {
				binWidth = pEXlow[i] + pEXhigh[i];
			}

			if(debug) std::cout << "Bin[" << i << "]: binWidth = " << binWidth << std::endl;

			double y = pY[i] * scale * binWidth;
			double eyl = pEYlow[i] * scale * binWidth;
			double eyh = pEYhigh[i] * scale * binWidth;

			if(dividedByBinWidth) {
				binWidth *= xScale;
				eyl /= binWidth;
				eyh /= binWidth;
				y /= binWidth;
			}

			if(debug) {
				std::cout << "Bin[" << i << "]:" << std::endl;
				std::cout << "\t x = " << pX[i] << std::endl;
				std::cout << "\t y = " << y << std::endl;
				std::cout << "\t exl = " << pEXlow[i] << std::endl;
				std::cout << "\t exh = " << pEXhigh[i] << std::endl;
				std::cout << "\t eyl = " << eyl << std::endl;
				std::cout << "\t eyh = " << eyh << std::endl;
			}

			graph->SetPoint(i, pX[i], y);
			graph->SetPointError(i, pEXlow[i], pEXhigh[i], eyl, eyh);
		}
	}
};

#endif