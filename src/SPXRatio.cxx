//************************************************************/
//
//	Ratio Implementation
//
//	Implements the SPXRatio class, which validates the ratio
//  type string, and then grabs the required TGraphs and
//  performs the necessary divisions, returning a fully
//  configured and ready-to-draw TGraph.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		25.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <sstream>

#include "SPXRatio.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXRatio::";

//Must define the static debug variable in the implementation
bool SPXRatio::debug;

//Takes a string argument in the following form:
//	
//	(numerator) / (denominator) 							<-- MUST have the parenthesis!
//
// 	Where the options for numerator and denominator are:
//
//		[grid_steering_file.txt, pdf_steering_file.txt]		<-- Convolute
//		data_steering file									<-- Data
//		grid_steering file 									<-- Reference
//
//	And the numerator/denominator pair options are:
//
//		data / convolute
//		convolute / data
//		data / data
//		convolute / reference
//
void SPXRatio::Parse(std::string &s) {
    std::string mn = "Parse: ";
	
	std::string numBlob;
	std::string denBlob;
	
    if(debug) std::cout << cn << mn << "Parsing ratio string: " << s << std::endl;

    //Parse the string into numerator and denominator (delimit with '/')
    std::vector<std::string> v = SPXStringUtilities::ParseString(s, '/');
    
    //Make sure vector is EXACTLY 2 strings long
    if(v.size() != 2) {
    	throw SPXParseException(cn + mn + "Ratio string: " + s + " is NOT in the form (numerator) / (denominator)");
    }
    
    //Obtain the numerator and denominator blobs from parsed list
	numBlob = v.at(0);
	denBlob = v.at(1);
	
	if(numBlob.empty()) {
		throw SPXParseException(cn + mn + "Numerator blob is empty");
	} else {
		numeratorBlob = numBlob;
	}
	
	if(denBlob.empty()) {
		throw SPXParseException(cn + mn + "Denominator blob is empty");
	} else {
		denominatorBlob = denBlob;
	}
	
	if(debug) {
		std::cout << cn << mn << "Numerator blob parsed as: " << numeratorBlob << std::endl;
		std::cout << cn << mn << "Denominator blob parsed as: " << denominatorBlob << std::endl;
	}
 
    //Check the RatioStyle:
    if(ratioStyle.IsDataOverConvolute()) {
        
        //Error if numBlob matches a convolute string
        if(MatchesConvoluteString(numBlob)) {
            throw SPXParseException(cn + mn + "Numerator blob should have a \"data\" style, but has \"convolute\" style");
        }

        //Error if denBlob does NOT match a convolute string
        if(!MatchesConvoluteString(denBlob)) {
            throw SPXParseException(cn + mn + "Denominator blob should have a \"convolute\" style, but does not");
        }

        //Get the data steering file from the numerator
        numeratorDataFile = SPXStringUtilities::RemoveCharacters(numBlob, "()");

        //Get the grid/pdf steering files from the denominator
        SPXStringUtilities::RemoveCharacters(denBlob, "()");
        SPXStringUtilities::RemoveCharacters(denBlob, "[]");
        std::vector<std::string> v_den = SPXStringUtilities::CommaSeparatedListToVector(denBlob);
        if(v_den.size() != 2) {
            throw SPXParseException(cn + mn + "Denominator blob is NOT of the form \"[grid_file, pdf_file]\"");
        }
        denominatorConvoluteGridFile = v_den.at(0);
        denominatorConvolutePDFFile = v_den.at(1);

        if(debug) {
            std::cout << cn << mn << "Successfully parsed data / convolute string: " << std::endl;

            std::ostringstream oss;
            oss << numeratorDataFile << " / " << "[" << denominatorConvoluteGridFile << ", " << denominatorConvolutePDFFile << "]";
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;        
        }
    }
    
   	else if(ratioStyle.IsConvoluteOverData()) {
        
        //Error if numBlob does NOT match a convolute string
        if(!MatchesConvoluteString(numBlob)) {
            throw SPXParseException(cn + mn + "Numerator blob should have a \"convolute\" style, but does not");
        }

        //Error if denBlob matches a convolute string
        if(MatchesConvoluteString(denBlob)) {
            throw SPXParseException(cn + mn + "Denominator blob should have a \"data\" style, but has \"convolute\" style");
        }

        //Get the grid/pdf steering files from the numerator
        SPXStringUtilities::RemoveCharacters(numBlob, "()");
        SPXStringUtilities::RemoveCharacters(numBlob, "[]");
        std::vector<std::string> v_num = SPXStringUtilities::CommaSeparatedListToVector(numBlob);
        if(v_num.size() != 2) {
            throw SPXParseException(cn + mn + "Numerator blob is NOT of the form \"[grid_file, pdf_file]\"");
        }
        numeratorConvoluteGridFile = v_num.at(0);
        numeratorConvolutePDFFile = v_num.at(1);

        //Get the data steering file from the denominator
        denominatorDataFile = SPXStringUtilities::RemoveCharacters(denBlob, "()");

        if(debug) {
            std::cout << cn << mn << "Successfully parsed convolute / data string: " << std::endl;
            std::ostringstream oss;
            oss << "[" << numeratorConvoluteGridFile << ", " << numeratorConvolutePDFFile << "]" << " / " << denominatorDataFile;
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;  
        }
    }
    
    else if(ratioStyle.IsConvoluteOverReference()) {

        //Error if numBlob does NOT match a convolute string
        if(!MatchesConvoluteString(numBlob)) {
            throw SPXParseException(cn + mn + "Numerator blob should have a \"convolute\" style, but does not");
        }

        //Error if denBlob matches a convolute string
        if(MatchesConvoluteString(denBlob)) {
            throw SPXParseException(cn + mn + "Denominator blob should have a \"reference\" style, but has \"convolute\" style");
        }

        //Get the grid/pdf steering files from the numerator
        SPXStringUtilities::RemoveCharacters(numBlob, "()");
        SPXStringUtilities::RemoveCharacters(numBlob, "[]");
        std::vector<std::string> v_num = SPXStringUtilities::CommaSeparatedListToVector(numBlob);
        if(v_num.size() != 2) {
            throw SPXParseException(cn + mn + "Numerator blob is NOT of the form \"[grid_file, pdf_file]\"");
        }
        numeratorConvoluteGridFile = v_num.at(0);
        numeratorConvolutePDFFile = v_num.at(1);

        //Get the reference grid steering file from the denominator
        denominatorReferenceGridFile = SPXStringUtilities::RemoveCharacters(denBlob, "()");

        //Error if reference grid steering file does NOT match the convolute grid file
        if(numeratorConvoluteGridFile.compare(denominatorReferenceGridFile) != 0) {
            throw SPXParseException(cn + mn + "Numerator's convolute grid file \"" + numeratorConvoluteGridFile + \
                "\" MUST match the denominator's refererence grid file: \"" + denominatorReferenceGridFile + "\"");
        }

        if(debug) {
            std::cout << cn << mn << "Successfully parsed convolute / reference string: " << std::endl;
            std::ostringstream oss;
            oss << "[" << numeratorConvoluteGridFile << ", " << numeratorConvolutePDFFile << "]" << " / " << denominatorReferenceGridFile;
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;  
        }
    }
    
    else if(ratioStyle.IsDataOverData()) {
        
        //Error if numerator or denominator matches convolute string
        if(MatchesConvoluteString(numBlob) || MatchesConvoluteString(denBlob)) {
            throw SPXParseException(cn + mn + "Numerator AND denominator blob should have a \"data\" style, but at least one has \"convolute\" style");
        }

        //Get the data file from the numerator
        numeratorDataFile = SPXStringUtilities::RemoveCharacters(numBlob, "()");

        //Get the data file from the denominator
        denominatorDataFile = SPXStringUtilities::RemoveCharacters(denBlob, "()");

        if(debug) {
            std::cout << cn << mn << "Successfully parsed data / data string: " << std::endl;
            std::ostringstream oss;
            oss << numeratorDataFile << " / " << denominatorDataFile;
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;  
        }
    }

    //Get the graphs based on the steering files given
    GetGraphs();
}

void GetGraphs(void) {
    ;
}

bool SPXRatio::MatchesConvoluteString(std::string &s) {

    //Convolute strings MUST begin with '[', end with ']', and have a ',' somewhere in the middle
    if((s.at(0) == '[') && (s.at(s.size() - 1) == ']') && (s.contains(","))) {
        return true;
    } else {
        return false;
    }
}
