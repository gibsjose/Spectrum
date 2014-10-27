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
void SPXRatio::Parse(std::string s) {
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
    
    }
    
   	else if(ratioStyle.IsConvoluteOverData()) {
    
    }
    
    else if(ratioStyle.IsConvoluteOverReference()) {
    
    }
    
    else if(ratioStyle.IsDataOverData()) {
    
    }
}

//Print displays the output of ToString to the console
void SPXRatio::Print(void) {
    std::string mn = "Print: ";
    std::cout << this->ToString() << std::endl;
}

//ToString does the opposite of Parse: it assembles a string based on
// the numerator and denominator bitfields
std::string SPXRatio::ToString(void) {
    std::string mn = "ToString: ";

    //Empty style
    if(this->IsEmpty()) {
        return "";
    }

    //Check for validity
    if(!this->IsValid()) {
        return "INVALID_RATIO_STYLE";
    }

    //Build numerator and denominator
    std::vector<std::string> num;
    std::string den;

    num.clear();

    if(numerator & RS_DATA) {
        num.push_back("data");
    }
    if(numerator & RS_REFERENCE) {
        num.push_back("reference");
    }
    if(numerator & RS_CONVOLUTE) {
        num.push_back("convolute");
    }

    if(denominator == RS_DATA) {
        den = "data";
    }
    else if(denominator == RS_REFERENCE) {
        den = "reference";
    }
    else if(denominator == RS_CONVOLUTE) {
        den = "convolute";
    }

    return SPXStringUtilities::VectorToCommaSeparatedList(num) + " / " + den;
}

//Determines whether the ratio style is empty or not
bool SPXRatio::IsEmpty(void) {
    std::string mn = "IsEmpty: ";

    if(!(numerator | denominator)) {
        return true;
    }

    return false;
}

//Determines the validity of the ratio style
bool SPXRatio::IsValid(void) {
    std::string mn = "IsValid: ";

    //Empty numerator and denominator: valid, but empty
    if(this->IsEmpty()){
        if(debug) std::cout << cn << mn << "Empty numerator and denominator" << std::endl;
        return true;
    }

    //Only one of numerator OR denominator empty
    if((!!numerator) ^ (!!denominator)) {
        if(debug) std::cout << cn << mn << "Mismatch: numerator = " << numerator << " denominator = " << denominator << std::endl;
        return false;
    }

    if(numerator > (RS_DATA | RS_REFERENCE | RS_CONVOLUTE)) {
        if(debug) std::cout << cn << mn << "Numerator is out of bounds: numerator = " << numerator << std::endl;
        return false;
    }

    if((denominator != RS_DATA) && (denominator != RS_REFERENCE) && (denominator != RS_CONVOLUTE)) {
        if(debug) std::cout << cn << mn << "Denominator is invalid: denominator = " << denominator << std::endl;
        return false;
    }

    return true;
}
