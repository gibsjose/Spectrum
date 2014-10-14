//************************************************************/
//
//	Graph Utilities Header
//
//	Some useful graph utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		13.10.2014
//
//************************************************************/

#ifndef SPXGRAPHUTILITIES_H
#define SPXGRAPHUTILITIES_H

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
			
			if(dividedByBinWidth) {
				totalSigma += sigma * binWidth;
			} else {
				totalSigma += sigma;
			}

			//@TODO Check normalization and use of double += sigma here
			//totalSigma += sigma;
		}

		double scale = yScale;

		//Normalize
		for(int i = 0; i < numberOfBins; i++) {
			if(normalizedToTotalSigma) {
				scale = yScale / totalSigma;
			} //@TODO move scale = yScale to an else? What about the second time around the for loop???
			
			double binWidth = 1;

			if(dividedByBinWidth) {
				binWidth = pEXlow[i] + pEXhigh[i];
			}

			double y = pY[i] * scale * binWidth;
			double eyl = pEYlow[i] * scale * binWidth;
			double eyh = pEYhigh[i] * scale * binWidth;

			if(dividedByBinWidth) {
				binWidth *= xScale;
				eyl /= binWidth;
				eyh /= binWidth;
				y /= binWidth;
			}

			graph->SetPoint(i, pX[i], y);
			graph->SetPointError(i, pEXlow[i], pEXhigh[i], eyl, eyh);

		}
	}
};

#endif