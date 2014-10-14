//************************************************************/
//
//	Grid Implementation
//
//	Implements the SPXGrid class, which contains the grid data
//  and manages the grid ROOT file.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		14.10.2014
//
//************************************************************/

#include "SPXGrid.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXGrid::";

//Must define the static debug variable in the implementation
bool SPXGrid::debug;
