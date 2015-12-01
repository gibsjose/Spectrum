//************************************************************/
//
//	Spectrum Utilities Header
//
//	Simply includes all the other utility classes
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN 
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXDRAWUTILITIES_H
#define SPXDRAWUTILITIES_H

#include "SPXMathUtilities.h"
#include "SPXUtilities.h"
#include "SPXROOT.h"


class SPXDrawUtilities {
public:

  static void BoxText(double x, double y, double xboxsize, double yboxsize, int mcolor, std::string text, int lcol=1, int lstyle=1, double tsize=0.03);
  static TString FormatwithExp(double xinput);

  static void DrawBox(double xbox, double ybox, int mcolor, bool isdatatot=true, bool isdatastat=true, double boxsize=0.05);

  static void PlotAndSave2Dmatrix( TMatrixD *covMat_, Int_t N, Double_t binLimits[], TH2D* frame2D, TString *name, Int_t SetZrage=0, Double_t min_=0., Double_t max_=0. );
};

#endif
