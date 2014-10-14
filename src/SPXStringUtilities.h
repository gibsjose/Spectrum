//************************************************************/
//
//	String Utilities Header
//
//	Some useful string utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		13.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXSTRINGUTILITIES_H
#define SPXSTRINGUTILITIES_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


class SPXStringUtilities {

public:
	static std::vector<std::string> ParseString(std::string rawData, char delimeter) {
	  std::stringstream lineStream(rawData);
	  std::string cell;
	  std::vector<std::string> parsedDataVec;
	  parsedDataVec.clear();

	  while(getline(lineStream,cell,delimeter)) {
		cell.erase( std::remove(cell.begin(), cell.end(), ' '), cell.end() ); //remove any whitespace
		parsedDataVec.push_back(cell);
	  }
		
	  if(parsedDataVec.size()==0) parsedDataVec.push_back(rawData);    
		
	  return parsedDataVec;
	}

	static std::vector<std::string> CommaSeparatedListToVector(const std::string &csl) {
	  	return ParseString(csl, ',');
	}

	static std::string VectorToCommaSeparatedList(std::vector<std::string> &v) {
		std::string csl;
		
		csl.clear();
	 	
	 	for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
	 		csl += *it;
	 		
	 		if((it + 1) != v.end()) {
	 			csl += ", ";
	 		}
	 	}
	 	
	 	return csl;
	}
};

#endif