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

    //Check for special cases (data stat and data tot)
    if(ratioStyle.IsDataStat()) {
        numeratorDataFile = dataDirectory + "/" + s + "_stat";
        denominatorDataFile = dataDirectory + "/" + s + "_stat";
        if(debug) std::cout << cn << mn << "Successfully parsed data stat ratio with data file: " << numeratorDataFile << std::endl;
        return;
    }

    if(ratioStyle.IsDataTot()) {
        numeratorDataFile = dataDirectory + "/" + s;
        denominatorDataFile = dataDirectory + "/" + s;
        if(debug) std::cout << cn << mn << "Successfully parsed data tot ratio with data file: " << numeratorDataFile << std::endl;
        return;
    }

    //Parse the string into numerator and denominator (delimit with ' / ')
    std::vector<std::string> v = SPXStringUtilities::SplitString(s, " / ");

    //Make sure vector is EXACTLY 2 strings long
    if(v.size() != 2) {
    	throw SPXParseException(cn + mn + "Ratio string: " + s + "\n is NOT in the form (numerator) / (denominator): DON'T FORGET THE SPACES AROUND ' / '!");
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

    if(debug) std::cout << cn << mn << "Ratio style is: " << ratioStyle.ToString() << " (" << ratioStyle.GetNumerator() \
        << " / " << ratioStyle.GetDenominator() << ")" << std::endl;

    //Check the RatioStyle:
    if(ratioStyle.IsDataOverConvolute()) {
        if(debug) std::cout << cn << mn << "Data over Convolute" << std::endl;

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
        denBlob = SPXStringUtilities::RemoveCharacters(denBlob, "()");
        denBlob = SPXStringUtilities::RemoveCharacters(denBlob, "[]");
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

        //Prepend directory paths
        numeratorDataFile = dataDirectory + "/" + numeratorDataFile;
        denominatorConvoluteGridFile = gridDirectory + "/" + denominatorConvoluteGridFile;
        denominatorConvolutePDFFile = pdfDirectory + "/" + denominatorConvolutePDFFile;
    }

    else if(ratioStyle.IsConvoluteOverData()) {
        if(debug) std::cout << cn << mn << "Convolute over Data" << std::endl;

        //Error if numBlob does NOT match a convolute string
        if(!MatchesConvoluteString(numBlob)) {
            throw SPXParseException(cn + mn + "Numerator blob should have a \"convolute\" style, but does not");
        }

        //Error if denBlob matches a convolute string
        if(MatchesConvoluteString(denBlob)) {
            throw SPXParseException(cn + mn + "Denominator blob should have a \"data\" style, but has \"convolute\" style");
        }

        //Get the grid/pdf steering files from the numerator
        numBlob = SPXStringUtilities::RemoveCharacters(numBlob, "()");
        numBlob = SPXStringUtilities::RemoveCharacters(numBlob, "[]");
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

        //Prepend directory paths
        numeratorConvoluteGridFile = gridDirectory + "/" + numeratorConvoluteGridFile;
        numeratorConvolutePDFFile = pdfDirectory + "/" + numeratorConvolutePDFFile;
        denominatorDataFile = dataDirectory + "/" + denominatorDataFile;
    }

    else if(ratioStyle.IsConvoluteOverReference()) {
        if(debug) std::cout << cn << mn << "Convolute over reference" << std::endl;

        //Error if numBlob does NOT match a convolute string
        if(!MatchesConvoluteString(numBlob)) {
            throw SPXParseException(cn + mn + "Numerator blob should have a \"convolute\" style, but does not");
        }

        //Error if denBlob matches a convolute string
        if(MatchesConvoluteString(denBlob)) {
            throw SPXParseException(cn + mn + "Denominator blob should have a \"reference\" style, but has \"convolute\" style");
        }

        //Get the grid/pdf steering files from the numerator
        numBlob = SPXStringUtilities::RemoveCharacters(numBlob, "()");
        numBlob = SPXStringUtilities::RemoveCharacters(numBlob, "[]");
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

        //Prepend directory paths
        numeratorConvoluteGridFile = gridDirectory + "/" + numeratorConvoluteGridFile;
        numeratorConvolutePDFFile = pdfDirectory + "/" + numeratorConvolutePDFFile;
        denominatorReferenceGridFile = gridDirectory + "/" + denominatorReferenceGridFile;
    }

    else if(ratioStyle.IsDataOverData()) {
        if(debug) std::cout << cn << mn << "Data Over Data" << std::endl;

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

        numeratorDataFile = dataDirectory + "/" + numeratorDataFile;
        denominatorDataFile = dataDirectory + "/" + denominatorDataFile;
    }
}

void SPXRatio::AddDataFileGraphMap(StringGraphMap_T &dataFileGraphMap) {
    this->dataFileGraphMap = &dataFileGraphMap;
}

void SPXRatio::AddReferenceFileGraphMap(StringGraphMap_T &referenceFileGraphMap) {
    this->referenceFileGraphMap = &referenceFileGraphMap;
}

void SPXRatio::AddConvoluteFileGraphMap(StringPairGraphMap_T &convoluteFileGraphMap) {
    this->convoluteFileGraphMap = &convoluteFileGraphMap;
}

void SPXRatio::GetGraphs(void) {
    std::string mn = "GetGraphs: ";

    if(ratioStyle.IsDataStat() || ratioStyle.IsDataTot()) {
        std::string key = numeratorDataFile;
        if(debug) std::cout << cn << mn << "Key = [" << key << "]" << std::endl;

        //Check for existence of data key
        if(dataFileGraphMap->count(key) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "dataFileGraphMap[" << key << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        numeratorGraph = (*dataFileGraphMap)[key];
        denominatorGraph = (*dataFileGraphMap)[key];

        //Make sure graphs are valid
        if(!numeratorGraph || !denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << key << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        if(debug) {
            std::cout << cn << mn << "Printing graphs: " << std::endl;
            std::cout << "\t Numerator: " << std::endl;
            numeratorGraph->Print();
            std::cout << std::endl;
            std::cout << "\t Denominator: " << std::endl;
            denominatorGraph->Print();
            std::cout << std::endl;
        }
    }

    if(ratioStyle.IsDataOverConvolute()) {
        //Create keys
        std::string dataKey = numeratorDataFile;
        StringPair_T convoluteKey = StringPair_T(denominatorConvoluteGridFile, denominatorConvolutePDFFile);

        if(debug) {
            std::cout << cn << mn << "Data Key = [" << dataKey << "]" << std::endl;
            std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
        }

        //Check for existence of data key
        if(dataFileGraphMap->count(dataKey) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "dataFileGraphMap[" << dataKey << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Check for existence of convolute key
        if(convoluteFileGraphMap->count(convoluteKey) == 0) {
            PrintConvoluteFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Keys exist, grab graphs
        numeratorGraph = (*dataFileGraphMap)[dataKey];
        denominatorGraph = (*convoluteFileGraphMap)[convoluteKey];

        //Make sure graphs are valid
        if(!numeratorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << dataKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }
    }

    else if(ratioStyle.IsConvoluteOverData()) {
        //Create keys
        StringPair_T convoluteKey = StringPair_T(numeratorConvoluteGridFile, numeratorConvolutePDFFile);
        std::string dataKey = denominatorDataFile;

        if(debug) {
            std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
            std::cout << cn << mn << "Data Key = [" << dataKey << "]" << std::endl;
        }

        //Check for existence of convolute key
        if(convoluteFileGraphMap->count(convoluteKey) == 0) {
            PrintConvoluteFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Check for existence of data key
        if(dataFileGraphMap->count(dataKey) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "dataFileGraphMap[" << dataKey << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Keys exist, grab graphs
        numeratorGraph = (*convoluteFileGraphMap)[convoluteKey];
        denominatorGraph = (*dataFileGraphMap)[dataKey];

        //Make sure graphs are valid
        if(!numeratorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << dataKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }
    }

    else if(ratioStyle.IsConvoluteOverReference()) {
        //Create keys
        StringPair_T convoluteKey = StringPair_T(numeratorConvoluteGridFile, numeratorConvolutePDFFile);
        std::string referenceKey = denominatorReferenceGridFile;

        if(debug) {
            std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
            std::cout << cn << mn << "Reference Key = [" << referenceKey << "]" << std::endl;
        }

        //Check for existence of convolute key
        if(convoluteFileGraphMap->count(convoluteKey) == 0) {
            PrintConvoluteFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Check for existence of reference key
        if(referenceFileGraphMap->count(referenceKey) == 0) {
            PrintReferenceFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "referenceFileGraphMap[" << referenceKey << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Keys exist, grab graphs
        numeratorGraph = (*convoluteFileGraphMap)[convoluteKey];
        denominatorGraph = (*referenceFileGraphMap)[referenceKey];

        //Make sure graphs are valid
        if(!numeratorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at referenceFileGraphMap[" << referenceKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }
    }

    else if(ratioStyle.IsDataOverData()) {
        //Create keys
        std::string numDataKey = numeratorDataFile;
        std::string denDataKey = denominatorDataFile;

        if(debug) {
            std::cout << cn << mn << "Numerator Data Key = [" << numDataKey << "]" << std::endl;
            std::cout << cn << mn << "Denominator Data Key = [" << denDataKey << "]" << std::endl;
        }

        //Check for existence of numerator data key
        if(dataFileGraphMap->count(numDataKey) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "dataFileGraphMap[" << numDataKey << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Check for existence of denominator data key
        if(dataFileGraphMap->count(denDataKey) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);
            
            std::ostringstream oss;
            oss << "dataFileGraphMap[" << denDataKey << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Keys exist, grab graphs
        numeratorGraph = (*dataFileGraphMap)[numDataKey];
        denominatorGraph = (*dataFileGraphMap)[denDataKey];

        //Make sure graphs are valid
        if(!numeratorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << numDataKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << denDataKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }
    }

    return;
}

bool SPXRatio::MatchesConvoluteString(std::string &s) {
    std::string mn = "MatchesConvoluteString: ";

    s = SPXStringUtilities::RemoveCharacters(s, "()");

    if(debug) std::cout << cn << mn << "Checking \"" << s << "\" against convolute pattern" << std::endl;

    //Convolute strings MUST begin with '[', end with ']', and have a ',' somewhere in the middle
    if((s.at(0) == '[') && (s.at(s.size() - 1) == ']') && (s.find(",") != std::string::npos)) {
        if(debug) std::cout << cn << mn << s << " matches convolute pattern" << std::endl;
        return true;
    } else {
        if(debug) std::cout << cn << mn << s << " does NOT match convolute pattern" << std::endl;
        return false;
    }
}
