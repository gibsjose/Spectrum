//************************************************************/
//
//	PDF Implementation
//
//	Implements the SPXPDF class, which manages the PDF data
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//
//************************************************************/

#include "SPXPDF.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXPDF::";

//Must define the static debug variable in the implementation
bool SPXPDF::debug;
