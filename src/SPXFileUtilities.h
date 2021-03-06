//************************************************************/
//
//	File Utilities Header
//
//	Some useful file utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		14.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXFILEUTILITIES_H
#define SPXFILEUTILITIES_H

#include <iostream>

class SPXFileUtilities {

public:
	static bool FileExists(const std::string &filepath) {
		if(FILE *file = fopen(filepath.c_str(), "r")) {
	    	fclose(file);
	    	return true;
	  	} else {
	  		return false;
	  	}
	}
};

#endif
