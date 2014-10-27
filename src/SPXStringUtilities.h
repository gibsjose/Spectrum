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
#include <boost/algorithm/string.hpp>

#include "SPXException.h"

class SPXStringUtilities {

private:

	//Class used for RemoveCharacters method
	class IsChars
	{
	public:
		IsChars(const char* charsToRemove) : chars(charsToRemove) {};

		bool operator()(char c)
		{
		    for(const char* testChar = chars; *testChar != 0; ++testChar)
		    {
		        if(*testChar == c) { return true; }
		    }
		    return false;
		}

	private:
		const char* chars;
	};

public:

	//Removes any of the characters in the 'remove' string from the string s
	static std::string RemoveCharacters(std::string s, std::string remove) {

		s.erase(std::remove_if(s.begin(), s.end(), IsChars(remove.c_str())), s.end());

		return s;
	}

	static std::string ToUpper(const std::string & s) {
		return boost::to_upper_copy(s);
	}

	static unsigned int GetIndexOfStringInVector(std::vector<std::string> v, std::string s) {
		unsigned int index = std::find(v.begin(), v.end(), s) - v.begin();
		if(index >= v.size()) {
			throw SPXGeneralException("SPXStringUtilities::GetIndexOfStringInVector: String not found in vector: Index out of bounds");
		} else {
			return index;
		}
	}

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
