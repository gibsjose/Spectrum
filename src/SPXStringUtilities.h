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
	static std::string RemoveCharacters(std::string s, const std::string &remove) {

		s.erase(std::remove_if(s.begin(), s.end(), IsChars(remove.c_str())), s.end());

		return s;
	}

	//Delete's the first occurrance of a substring from a string (if it exists)
	static std::string RemoveFirstSubstring(std::string s, const std::string &substring) {

		size_t pos = s.find(substring);

		if(pos != std::string::npos) {
			s.erase(pos, substring.length());
		}

		return s;
	}

	static std::string ReplaceAll(std::string s, const std::string &from, const std::string &to) {
		size_t start_pos = 0;
		while((start_pos = s.find(from, start_pos)) != std::string::npos) {
			s.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return s;
	}

	//First split a string based on a delimiter, and for each token remove the given characters
	static std::vector<std::string> SplitStringAndRemoveCharacters(std::string s, const std::string &delimiter, const std::string &remove) {
		size_t pos = 0;
		std::vector<std::string> tokens;
		std::string token;
		bool debug = false;

		if(debug) std::cout << "s = " << s << ", delimiter = " << delimiter;

		while((pos = s.find(delimiter)) != std::string::npos) {
			if(debug) std::cout << "Found delimiter at pos = " << pos << std::endl;
			token = s.substr(0, pos);
			token = RemoveCharacters(token, remove);
			tokens.push_back(token);
			if(debug) std::cout << "Added token to vector: " << s.substr(0, pos) << std::endl;
			s.erase(0, pos + delimiter.length());
			if(debug) std::cout << "Erased token and delimiter" << std::endl;
		}

		if(debug) std::cout << "Adding last token to vector: " << s << std::endl;
		token = s;
		token = RemoveCharacters(token, remove);
		tokens.push_back(token);

		return tokens;
	}

	//Splits a string based on a delimiter string
	static std::vector<std::string> SplitString(std::string s, const std::string &delimiter) {
		size_t pos = 0;
		std::vector<std::string> tokens;

		bool debug = false;

		if(debug) std::cout << "s = " << s << ", delimiter = " << delimiter;

		while((pos = s.find(delimiter)) != std::string::npos) {
			if(debug) std::cout << "Found delimiter at pos = " << pos << std::endl;
			tokens.push_back(s.substr(0, pos));
			if(debug) std::cout << "Added token to vector: " << s.substr(0, pos) << std::endl;
			s.erase(0, pos + delimiter.length());
			if(debug) std::cout << "Erased token and delimiter" << std::endl;
		}

		if(debug) std::cout << "Adding last token to vector: " << s << std::endl;
		tokens.push_back(s);

		return tokens;
	}

	static std::string ToUpper(const std::string & s) {
		std::string str = s;
		std::transform(str.begin(), str.end(),str.begin(), ::toupper);
		return str;
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

	static std::vector<double> ParseStringToDoubleVector(std::string rawData, char delimiter) {
		std::vector<double> dVector;
		std::stringstream lineStream(rawData);
		std::string cell;

		std::cout << "==========" << rawData << std::endl;

		dVector.clear();

		while(getline(lineStream, cell, delimiter)) {
			if(!isdigit(cell.at(0))) {
				throw SPXParseException("SPXStringUtilities::ParseStringToDoubleVector: Token " + cell + " cannot be converted to double");
			}

			dVector.push_back((double)atof(cell.c_str()));
		}

		return dVector;
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
