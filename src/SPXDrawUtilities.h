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
#include "SPXROOT.h"


class SPXDrawUtilities {
public:

  static void BoxText(double x, double y, double xboxsize, double yboxsize, int mcolor, std::string text, int lcol=1, int lstyle=1, double tsize=0.03);
  static TString FormatwithExp(double xinput);
};

#endif
