//************************************************************/
//
//	Math Utilities Header
//
//	Some useful math utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		29.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXMATHUTILITIES_H
#define SPXMATHUTILITIES_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

#include "SPXException.h"

class SPXMathUtilities {

public:
    static double AddErrorsInQuadrature(std::vector<double> errors) {

        double sum = 0;

        for(int i = 0; i < errors.size(); i++) {
            sum += pow(errors[i], 2.0);
        }

        return sqrt(sum);
    }
};

#endif
