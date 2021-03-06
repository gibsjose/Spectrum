//************************************************************/
//
//	String Utilities Header
//
//	Some useful string utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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
#include <functional>
#include <cctype>
#include <locale>

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

	//static std::string CompareTwoSTring(std::string firstValue, std::string secondValue);
        //static std::string LCSubstr(std::string x, std::string y);

        static std::string ReplaceString(std::string org, std::string replacedstring, std::string replacingstring){
         std::string fline=org;
         fline.replace(fline.find(replacedstring),replacedstring.length(),replacingstring);

	 //std::cout<<"ReplaceString: org string= "<<org<<" replace string= "<<replacedstring<<" replacing string= "<< replacingstring <<" new string= "<<fline<< std::endl;

         return fline;
        };


	template<typename T>
	static T StringToNumber(const std::string& numberAsString) {
		T valor;
 
		// Replace D (from Fortran written files) to standard scientific notation
		std::string str1=numberAsString;
		if (isdigit((int)LeftTrim(str1).at(0)) || LeftTrim(str1).at(0)=='-') {
                 if (str1.find("D")!=std::string::npos) {
		  //std::cout<<"Replace old string "<<str1<<std::endl;
                  str1.replace(numberAsString.find("D"),1,"e");
		  //std::cout<<"Replace D to E new string "<<str1<<std::endl;
                  //double valor1=atof(str1.c_str());
 		  //std::cout<<"valor1= "<< std::scientific << valor1 <<std::endl;
                 }
                }
		std::stringstream stream(numberAsString);
                //std::stringstream stream(str1);
		stream >> valor;
		if (stream.fail()) {
		  //throw SPXParseException("Could not convert string " + numberAsString + " to a number");
                  throw SPXParseException("Could not convert string " + str1 + " to a number");
		}
		//std::cout<<"valor= "<< std::scientific <<valor<<std::endl;
		return valor;
	}

        template<typename T> 
        static T GetNumberfromStringVector(std::vector <std::string> names, const std::string& String) {
        //
        for(int i = 0; i < names.size(); i++) {
         //    if (TString(names.at(i)).Contains(String)) {
         if (names.at(i).find(String) != std::string::npos) {
          for(int j = i+1; j < names.size(); j++) {
           if (!names.at(j).empty()) { 
            return StringToNumber<T>(names.at(j));         
           }
          }
         } 
        }
        return 0;
}


	static bool BeginsWith(std::string &s, std::string &b) {
		if(s.find(b) == 0) {
			return true;
		} else {
			return false;
		}
	}

	//Trim LEADING whitespace
	static std::string LeftTrim(std::string s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	//Trim TRAILING whitespace
	static std::string RightTrim(std::string s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	//Trim all whitespace
	static std::string Trim(std::string s) {
		return LeftTrim(RightTrim(s));
	}

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

	//Splits a string based on a delimiter string, stopping after 'n' splits
	static std::vector<std::string> SplitString(std::string s, const std::string &delimiter, int n) {
		size_t pos = 0;
		std::vector<std::string> tokens;
		unsigned int count = 0;

		bool debug = false;

		if(debug) std::cout << "s = " << s << ", delimiter = " << delimiter;

		while((pos = s.find(delimiter)) != std::string::npos) {
			if(count >= n) {
				break;
			}
			if(debug) std::cout << "Found delimiter at pos = " << pos << std::endl;
			tokens.push_back(s.substr(0, pos));
			if(debug) std::cout << "Added token to vector: " << s.substr(0, pos) << std::endl;
			s.erase(0, pos + delimiter.length());
			if(debug) std::cout << "Erased token and delimiter" << std::endl;
			count++;
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

		dVector.clear();

		while(getline(lineStream, cell, delimiter)) {
			//Remove whitespace
			cell = Trim(cell);

			//Skip if cell is empty
			if(!cell.empty()) {
				try {
					double val = StringToNumber<double>(cell);
					//dVector.push_back((double)atof(cell.c_str()));
                                        dVector.push_back(val);
					//std::cout<<" val= "<<std::scientific <<val<<" vec= "<<dVector.back()<<std::endl;
				} catch(const SPXException &e) {
					throw;
				}
			}
		}

		return dVector;
	}

       static std::vector<bool> ParseStringToBooleanVector(std::string rawData, char delimiter) {
       	std::vector<bool> dVector;
	std::stringstream lineStream(rawData);
	std::string cell;

	dVector.clear();
	while(getline(lineStream, cell, delimiter)) {
	 //Remove whitespace
	 cell = Trim(cell);

	 //Skip if cell is empty
	 if(!cell.empty()) {
	  try {
           std::transform(cell.begin(), cell.end(), cell.begin(), ::tolower);
           bool val=false;
           if (cell == "true" || cell == "yes" || cell == "on" || cell == "1")
            val=true;
           else if (cell == "false" || cell == "no" || cell == "off" || cell == "0")
            val=false;
	   
	   dVector.push_back(val);
	  } catch(const SPXException &e) {
	   throw;
	  }
	 }
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

 template<typename T> static T GetStringfromVector(std::vector <std::string> names, const std::string& String);
 

#endif
