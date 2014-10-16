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
#include <sstream>
#include <vector>

#include "SPXROOT.h"
#include "SPXException.h"

typedef enum DivideErrorType {
	//@TODO Fill in options here...
	ZeroAllErrors = 0,				//Set all errors to zero
	AddErrors = 1,					//Add errors quadratically
	ZeroGraph2Errors = 2 			//Set Graph 2 errors to zero
} DivideErrorType;

class SPXGraphUtilities {

public:

	static double GetXMin(std::vector<TGraphAsymmErrors *> graphs) {

		double min = 1e30;

		for(int i = 0; i < graphs.size(); i++) {
			double xmin, xmax, ymin, ymax;

			graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

			if(xmin < min) {
				min = xmin;
			}
		}

		return min;
	}

	static double GetXMax(std::vector<TGraphAsymmErrors *> graphs) {
		
		double max = -1e30;

		for(int i = 0; i < graphs.size(); i++) {
			double xmin, xmax, ymin, ymax;

			graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

			if(xmax > max) {
				max = xmax;
			}
		}

		return max;
	}

	static double GetYMin(std::vector<TGraphAsymmErrors *> graphs) {
		
		double min = 1e30;

		for(int i = 0; i < graphs.size(); i++) {
			double xmin, xmax, ymin, ymax;

			graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

			if(ymin < min) {
				min = ymin;
			}
		}

		return min;
	}

	static double GetYMax(std::vector<TGraphAsymmErrors *> graphs) {
		
		double max = -1e30;

		for(int i = 0; i < graphs.size(); i++) {
			double xmin, xmax, ymin, ymax;

			graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

			if(ymax > max) {
				max = ymax;
			}
		}

		return max;
	}

	static TGraphAsymmErrors * Divide(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, DivideErrorType dt) {
		
		//Make sure graphs are valid
		if((!g1) || (!g2)) {
			throw SPXGraphException("SPXGraphUtilities::Divide: At least one of the operand graphs is invalid");
		}

		int n1 = g1->GetN();
		int n2 = g2->GetN();

		//Make sure the two graphs are the same size
		if(n1 != n2) {
			std::ostringstream oss;
			oss << "SPXGraphUtilities::Divide: Graphs do not contain the same number of bins: G1 N= " << n1 << " G2 N = " << n2;
			throw SPXGraphException(oss.str());
		}

		//@TODO Pass result as parameter and return that way! Don't call 'new' in a function
		TGraphAsymmErrors *result = new TGraphAsymmErrors();

		result->SetName(g1->GetName());
		result->SetMarkerStyle(g1->GetMarkerStyle());
		result->SetMarkerColor(g1->GetMarkerColor());
		result->SetLineColor(g1->GetLineColor());

		Double_t   	x1=0.0,   y1=0.0, x2=0.0, y2=0.0;
		Double_t 	dx1h=0.0, dx1l=0.0;
		Double_t 	dy1h=0.0, dy1l=0.0;
		Double_t 	dy2h=0.0, dy2l=0.0;

		Double_t* 	EXhigh1 = g1->GetEXhigh();
		Double_t* 	EXlow1 =  g1->GetEXlow();
		Double_t* 	EYhigh1 = g1->GetEYhigh();
		Double_t* 	EYlow1 =  g1->GetEYlow();

		Double_t* 	EXhigh2 = g2->GetEXhigh();
		Double_t* 	EXlow2 =  g2->GetEXlow();
		Double_t* 	EYhigh2 = g2->GetEYhigh();
		Double_t* 	EYlow2 =  g2->GetEYlow();

		Int_t iv = 0;

		for(Int_t i1 = 0; i1 < n1; i1++) {
			Int_t matchcount = 0;

			for(Int_t i2 = 0; i2 < n2; i2++) {
				g1->GetPoint(i1, x1, y1);
				g2->GetPoint(i2, x2, y2);

				Double_t emean=(EXhigh1[i1] + EXhigh2[i2] + EXlow1[i1] + EXlow2[i2]) / 4.0;

				if(fabs(x1 - x2) > emean) {
					//
				} else {
					matchcount++;

					dx1h = EXhigh1[i1];
					dx1l = EXlow1[i1];

					if(y1 != 0.0) {
						dy1h = EYhigh1[i1] / y1;
						dy1l = EYlow1[i1] / y1;
					} else {
						dy1h = 0;
						dy1l = 0;
					}

					if(y2 != 0.0) {
						dy2h = EYhigh2[i2] / y2;
						dy2l = EYlow2[i2] / y2;
					} else {
						dy2h = 0;
						dy2l = 0;
					}

					if(y2 != 0.0) {
						result->SetPoint(iv, x1, y1/y2);
					} else {
						result->SetPoint(iv, x1, y2);
					}

					Double_t el = 0.0;
					Double_t eh = 0.0;

					if(dt == ZeroGraph2Errors) {
						dy2l = 0.0;
						dy2h = 0.0;
					}

					if((y1 != 0.0) && (y2 != 0.0)) {
						el = sqrt(dy1l * dy1l + dy2l * dy2l) * (y1 / y2);
						eh = sqrt(dy1h * dy1h + dy2h * dy2h) * (y1 / y2);
					}

					if(dt == ZeroAllErrors) {
						result->SetPointError(iv, dx1l, dx1h, 0, 0);
					} else {
						result->SetPointError(iv, dx1l, dx1h, el, eh);
					}

					iv++;
				}
			}

			if(matchcount > 1) {
				throw SPXGraphException("SPXGraphUtilities::Divide: Too many X-Points matched");
			}
		}

		return result;
	}

	static TGraphAsymmErrors * HistogramToGraph(TH1 *h) {
		
		//Make sure histogram is valid
		if(!h) {
			throw SPXGraphException("SPXGraphUtilities::HistogramToGraph: Histogram provided was invalid");
		}

		//@TODO Don't call 'new' in a function call...
		TGraphAsymmErrors *graph = new TGraphAsymmErrors();

		double x, y, ex, ey;

		for(int i = 0; i < h->GetNbinsX(); i++) {
			y =  h->GetBinContent(i + 1);
			ey = h->GetBinError(i + 1);
			x =  h->GetBinCenter(i + 1);
			ex = h->GetBinWidth(i + 1) / 2.0;
			
			graph->SetPoint(i, x, y);
			graph->SetPointError(i, ex, ex, ey, ey);
		}

		return graph;
	}

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