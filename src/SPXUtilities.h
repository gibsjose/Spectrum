//************************************************************/
//
//	Spectrum Utilities Header
//
//	Some usefull file utilities, string utilities, etc.
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		25.09.2014
//
//************************************************************/

#ifndef SPXUTILITIES_H
#define SPXUTILITIES_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


class SPXUtilities {

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
