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
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	std::string numBlob;
	std::string denBlob;

    if(debug) std::cout << cn << mn << "Parsing ratio string: " << s << std::endl;

    //Check for special cases (data stat and data tot)
    if(ratioStyle.IsDataStat()) {
        //Check for alias
        std::string alias = CheckForAlias(s, "data");

        //If there was no alias, prepend data directory, and append _stat
        if(!s.compare(alias)) {
            numeratorDataFile = dataDirectory + "/" + s + "_stat";
            denominatorDataFile = dataDirectory + "/" + s + "_stat";
        }

        //If there was an alias, just append _stat (alias already contains directory)
        else {
            numeratorDataFile = alias + "_stat";
            denominatorDataFile = alias + "_stat";
        }

        if(debug) std::cout << cn << mn << "Successfully parsed data stat ratio with data file: " << numeratorDataFile << std::endl;
        return;
    }

    if(ratioStyle.IsDataTot()) {
        //Check for alias
        std::string alias = CheckForAlias(s, "data");

        //If there was no alias, prepend data directory
        if(!s.compare(alias)) {
            numeratorDataFile = dataDirectory + "/" + s;
            denominatorDataFile = dataDirectory + "/" + s;
        }

        //If there was an alias, just use it (alias already contains directory)
        else {
            numeratorDataFile = alias;
            denominatorDataFile = alias;
        }

        if(debug) std::cout << cn << mn << "Successfully parsed data tot ratio with data file: " << numeratorDataFile << std::endl;
        return;
    }

    if(ratioStyle.IsConvoluteOverReference() || ratioStyle.IsConvoluteOverNominal()) {
        std::string convoluteBlob = s;

        if(ratioStyle.IsConvoluteOverReference()) {
            if(debug) std::cout << cn << mn << "Ratio Style is Convolute / Reference" << std::endl;
        } else {
            if(debug) std::cout << cn << mn << "Ratio Style is Convolute / Nominal" << std::endl;
        }

        //Make sure the convolute blob matches the correct format
        if(!MatchesConvoluteString(convoluteBlob)) {
            throw SPXParseException(cn + mn + "Convolute blob should have a \"convolute\" style, but does not: " + convoluteBlob);
        }

        //Remove the '()' and '[]' from the string
        convoluteBlob = SPXStringUtilities::RemoveCharacters(convoluteBlob, "()");
        convoluteBlob = SPXStringUtilities::RemoveCharacters(convoluteBlob, "[]");

        //Delimit with ',' to get the grid and pdf file
        std::vector<std::string> v = SPXStringUtilities::CommaSeparatedListToVector(convoluteBlob);

        if(v.size() != 2) {
            throw SPXParseException(cn + mn + "Convolute blob is NOT of the form \"[grid_file, pdf_file]\": " + convoluteBlob);
        }

        numeratorConvoluteGridFile = v.at(0);
        numeratorConvolutePDFFile = v.at(1);

        //Check for alias
        std::string numeratorGridAlias = CheckForAlias(numeratorConvoluteGridFile, "grid");
        std::string numeratorPDFAlias = CheckForAlias(numeratorConvolutePDFFile, "pdf");

        //Use alias, if there is one, otherwise prepend directories
        if(!numeratorConvoluteGridFile.compare(numeratorGridAlias)) {
            numeratorConvoluteGridFile = gridDirectory + "/" + numeratorConvoluteGridFile;
        } else {
            numeratorConvoluteGridFile = numeratorGridAlias;
        }
        if(!numeratorConvolutePDFFile.compare(numeratorPDFAlias)) {
            numeratorConvolutePDFFile = pdfDirectory + "/" + numeratorConvolutePDFFile;
        } else {
            numeratorConvolutePDFFile = numeratorPDFAlias;
        }

        if(debug) {
            std::cout << cn << mn << "Successfully parsed convolute blob: " << std::endl;
            std::ostringstream oss;
            oss << "[" << numeratorConvoluteGridFile << ", " << numeratorConvolutePDFFile << "]";
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;
        }

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

        //Check for alias
        std::string numeratorDataAlias = CheckForAlias(numeratorDataFile, "data");
        std::string denominatorGridAlias = CheckForAlias(denominatorConvoluteGridFile, "grid");
        std::string denominatorPDFAlias = CheckForAlias(denominatorConvolutePDFFile, "pdf");

        //Use alias, if there is one, otherwise prepend directories
        if(!numeratorDataFile.compare(numeratorDataAlias)) {
            numeratorDataFile = dataDirectory + "/" + numeratorDataFile;
        } else {
            numeratorDataFile = numeratorDataAlias;
        }
        if(!denominatorConvoluteGridFile.compare(denominatorGridAlias)) {
            denominatorConvoluteGridFile = gridDirectory + "/" + denominatorConvoluteGridFile;
        } else {
            denominatorConvoluteGridFile = denominatorGridAlias;
        }
        if(!denominatorConvolutePDFFile.compare(denominatorPDFAlias)) {
            denominatorConvolutePDFFile = pdfDirectory + "/" + denominatorConvolutePDFFile;
        } else {
            denominatorConvolutePDFFile = denominatorPDFAlias;
        }

        if(debug) {
            std::cout << cn << mn << "Successfully parsed data / convolute string: " << std::endl;

            std::ostringstream oss;
            oss << numeratorDataFile << " / " << "[" << denominatorConvoluteGridFile << ", " << denominatorConvolutePDFFile << "]";
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;
        }
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

        //Check for alias
        std::string numeratorGridAlias = CheckForAlias(numeratorConvoluteGridFile, "grid");
        std::string numeratorPDFAlias = CheckForAlias(numeratorConvolutePDFFile, "pdf");
        std::string denominatorDataAlias = CheckForAlias(denominatorDataFile, "data");

        //Use alias, if there is one, otherwise prepend directories
        if(!numeratorConvoluteGridFile.compare(numeratorGridAlias)) {
            numeratorConvoluteGridFile = gridDirectory + "/" + numeratorConvoluteGridFile;
        } else {
            numeratorConvoluteGridFile = numeratorGridAlias;
        }
        if(!numeratorConvolutePDFFile.compare(numeratorPDFAlias)) {
            numeratorConvolutePDFFile = pdfDirectory + "/" + numeratorConvolutePDFFile;
        } else {
            numeratorConvolutePDFFile = numeratorPDFAlias;
        }
        if(!denominatorDataFile.compare(denominatorDataAlias)) {
            denominatorDataFile = dataDirectory + "/" + denominatorDataFile;
        } else {
            denominatorDataFile = denominatorDataAlias;
        }

        if(debug) {
            std::cout << cn << mn << "Successfully parsed convolute / data string: " << std::endl;
            std::ostringstream oss;
            oss << "[" << numeratorConvoluteGridFile << ", " << numeratorConvolutePDFFile << "]" << " / " << denominatorDataFile;
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;
        }
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

        //Check for alias
        std::string numeratorDataAlias = CheckForAlias(numeratorDataFile, "data");
        std::string denominatorDataAlias = CheckForAlias(denominatorDataFile, "data");

        //Use alias, if there is one, otherwise prepend directories
        if(!numeratorDataFile.compare(numeratorDataAlias)) {
            numeratorDataFile = gridDirectory + "/" + numeratorDataFile;
        } else {
            numeratorDataFile = numeratorDataAlias;
        }
        if(!denominatorDataFile.compare(denominatorDataAlias)) {
            denominatorDataFile = dataDirectory + "/" + denominatorDataFile;
        } else {
            denominatorDataFile = denominatorDataAlias;
        }

        if(debug) {
            std::cout << cn << mn << "Successfully parsed data / data string: " << std::endl;
            std::ostringstream oss;
            oss << numeratorDataFile << " / " << denominatorDataFile;
            ratioString = oss.str();
            std::cout << "\t " << ratioString << std::endl;
        }
    }
}


void SPXRatio::Divide(void) {
    std::string mn = "Divide: ";
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

    //Grab the plot configuration instance
    SPXPlotConfigurationInstance pci;

    if(debug) std::cout <<cn<<mn<< "Starting " << std::endl;

    if(ratioStyle.IsDataStat()) {
        try {
	    //TC ratioGraph = SPXGraphUtilities::Divide(numeratorGraph, denominatorGraph, ZeroDenGraphErrors);
	    TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraph.back(), denominatorGraph, ZeroDenGraphErrors);
            graph->SetFillStyle(1001);
            graph->SetFillColor(kGray);
            ratioGraph.push_back(graph);
	    //ratioGraph->SetFillStyle(1001);
            //ratioGraph->SetFillColor(kGray);
            if(debug) std::cout <<cn<<mn<< "Successfully divided data stat graph with options: " << std::endl;
            if(debug) std::cout << "\t Fill Style = " << 1001 << std::endl;
            if(debug) std::cout << "\t Fill Color = " << kGray << std::endl;
        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;
            throw SPXGraphException(cn + mn + "Unable to divide data stat graphs");
        }

        return;
    }

    else if(ratioStyle.IsDataTot()) {
        try {
	    //ratioGraph = SPXGraphUtilities::Divide(numeratorGraph, denominatorGraph, ZeroDenGraphErrors);
	    TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraph.back(), denominatorGraph, ZeroDenGraphErrors);
            graph->SetFillStyle(1001);
            graph->SetFillColor(kGray);
            ratioGraph.push_back(graph);
            //ratioGraph->SetFillStyle(1001);
            //ratioGraph->SetFillColor(kGray);
            if(debug) std::cout << cn<<mn<<"Successfully divided data tot graph with options: " << std::endl;
            if(debug) std::cout << "\t Fill Style = " << 1001 << std::endl;
            if(debug) std::cout << "\t Fill Color = " << kGray << std::endl;
        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;
            throw SPXGraphException(cn + mn + "Unable to divide data tot graphs");
        }

        return;
    }

    if(ratioStyle.IsConvoluteOverData() || ratioStyle.IsConvoluteOverReference() || ratioStyle.IsConvoluteOverNominal()) {
        pci = plotConfiguration.GetPlotConfigurationInstance(numeratorConvolutePDFFile);
        if(debug) {
            pci.Print();
        }

        //Match the convolute binning to the data binning
        if(ratioStyle.IsConvoluteOverData()) {
            try {
	      for (int i=0; i<numeratorGraph.size(); i++){
                if (debug) std::cout<<cn<<mn<<" Matchbinning for numeratorGraph["<<i<<"]= "<<i<<std::endl;
                SPXGraphUtilities::MatchBinning(denominatorGraph, numeratorGraph[i], true);
              }
            } catch(const SPXException &e) {
                std::cerr << e.what() << std::endl;
                throw SPXGraphException(cn + mn + "Unable to match convolute binning to data binning");
            }
        }
    }

    else if(ratioStyle.IsDataOverConvolute()) {
        pci = plotConfiguration.GetPlotConfigurationInstance(denominatorConvolutePDFFile);
        if(debug) {
            pci.Print();
        }

        //Match the convolute binning to the data binning
        try {
	   for (int i=0; i<numeratorGraph.size(); i++){
            SPXGraphUtilities::MatchBinning(numeratorGraph[i], denominatorGraph, true);
           }
        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;
            throw SPXGraphException(cn + mn + "Unable to match convolute binning to data binning");
        }
    }

    //@TODO What if it's Data/Data???
    else if(ratioStyle.IsDataOverData()) {
      if(debug) std::cout <<cn<<mn<<"Data/Data: Could not get pci" << std::endl;

        //@TODO Match binning here? Who is the master?
    }

    try {
        //Determine how to divide based on ratioStyle: zeroNumErrors/zeroDenErrors set with '!'
        DivideErrorType_t divideType;

        if(ratioStyle.GetZeroNumeratorErrors() && ratioStyle.GetZeroDenominatorErrors()) {
            divideType = ZeroAllErrors;
        } else if(ratioStyle.GetZeroNumeratorErrors()) {
            divideType = ZeroNumGraphErrors;
        } else if(ratioStyle.GetZeroDenominatorErrors()) {
            divideType = ZeroDenGraphErrors;
        } else {
            divideType = AddErrors;
        }

        //Divide graphs
	for (int i=0; i<numeratorGraph.size(); i++){
	 //ratioGraph = SPXGraphUtilities::Divide(numeratorGraph, denominatorGraph, divideType);
	 TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraph[i], denominatorGraph,divideType); 

        //Style ratio graph
	//if(debug) std::cout << cn<<mn<<"Obtaining PDF Fill Options..." << std::endl;
        // graph->SetFillStyle(pci.totalFillStyle);
        // graph->SetFillColor(pci.totalFillColor);
        // graph->SetMarkerStyle(pci.totalMarkerStyle);
        // graph->SetMarkerColor(pci.totalFillColor);
        // graph->SetLineColor(pci.totalFillColor);
        // graph->SetMarkerSize(1.2);

         //NOTE: ROOT Color Transparencies are only supported in ROOT v6.0.0+ via the method
         //          SetFillColorAlpha(ci, alpha_percentage)
         //          SetMarkerColorAlpha(ci, alpha_percentage)
         //          etc...

         //If it is a convolute / reference graph, darken the fill color and change the style
         if(ratioStyle.IsConvoluteOverReference()) {
          graph->SetFillStyle(3002);
          graph->SetFillColor  (pci.pdfFillColor + 1);
          graph->SetMarkerColor(pci.pdfFillColor + 1);
          graph->SetLineStyle(3);
         }

         //Same for nominal
         if(ratioStyle.IsConvoluteOverNominal()) {
          graph->SetFillStyle(3017);
          graph->SetFillColor  (pci.pdfFillColor + 2);
          graph->SetMarkerColor(pci.pdfFillColor + 2);
          graph->SetLineStyle(2);
         }

         if(debug) std::cout << cn + mn + "Set PDF Fill Options:" << std::endl;
         if(debug) std::cout << "\t Fill Style = " << graph->GetFillStyle() << std::endl;
         if(debug) std::cout << "\t Fill Color = " << graph->GetFillColor() << std::endl;
         if(debug) std::cout << "\t Marker Style= "<< graph->GetMarkerStyle() << std::endl;

         ratioGraph.push_back(graph);
        }
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;
        throw SPXGraphException(cn + mn + "Unable to divide numerator and denominator to calculate ratio");
    }
}

//Checks the 'original' string to see whether it contains an alias, and attempts to
// resolve that alias based on the alias type
std::string SPXRatio::CheckForAlias(std::string &original, const std::string alias_type) {
    std::string mn = "CheckForAlias: ";
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

    std::string tmp;
    std::string s_index;
    int index = -1;
    std::string alias;

    if(debug) std::cout << cn << mn << "Checking \"" << original << "\" for alias with type \"" << alias_type << "\"" << std::endl;

    //Make sure the alias type is only: data, reference, or convolute
    if(alias_type.compare("data") && alias_type.compare("grid") && alias_type.compare("pdf")) {
        throw SPXParseException(cn + mn + "Alias Type: " + alias_type + " is not valid: Must be \"data\", \"grid\", or \"pdf\"");
    }

    tmp = alias_type + "_";

    //Try to follow the alias
    if(SPXStringUtilities::BeginsWith(original, tmp)) {
        try {
            s_index = SPXStringUtilities::RemoveFirstSubstring(original, tmp);
            if(!s_index.empty()) {
                index = SPXStringUtilities::StringToNumber<int>(s_index);
            } else {
                throw SPXParseException(cn + mn + "Empty alias: " + tmp + ": Alias MUST contain an index");
            }
        } catch(const SPXException &e) {
            if(debug) std::cout << cn << mn << "Ratio string: " << original << " could not be converted to an alias index: Will assume it's an actual file and not an alias" << std::endl;
            return original;
        }

        //Alias
        if(debug) std::cout << cn << mn << "Ratio string: " << original << " is assumed to be an alias to " << alias_type << "_steering_files[" << index << "]" << std::endl;

        try {
            if(!alias_type.compare("data")) {
                alias = plotConfiguration.GetPlotConfigurationInstance(index).dataSteeringFile.GetFilename();
                dataDirectory = plotConfiguration.GetPlotConfigurationInstance(index).dataDirectory;
            } else if(!alias_type.compare("grid")) {
                alias = plotConfiguration.GetPlotConfigurationInstance(index).gridSteeringFile.GetFilename();
                gridDirectory = plotConfiguration.GetPlotConfigurationInstance(index).gridDirectory;
            } else if(!alias_type.compare("pdf")) {
                alias = plotConfiguration.GetPlotConfigurationInstance(index).pdfSteeringFile.GetFilename();
                pdfDirectory = plotConfiguration.GetPlotConfigurationInstance(index).pdfDirectory;
            }

            if(debug) std::cout << cn << mn << "Successfully aliased \"" << original << "\" to " << alias << std::endl;
            return alias;

        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;
            throw SPXParseException(cn + mn + "Index " + s_index + " is out of bounds/invalid");
        }
    } else {
        if(debug) std::cout << cn << mn << "\"" << original << "\" is not an alias" << std::endl;
        return original;
    }
}

void SPXRatio::AddDataFileGraphMap(StringGraphMap_T &dataFileGraphMap) {
    this->dataFileGraphMap = &dataFileGraphMap;
}

void SPXRatio::AddReferenceFileGraphMap(StringPairGraphMap_T &referenceFileGraphMap) {
    this->referenceFileGraphMap = &referenceFileGraphMap;
}

void SPXRatio::AddNominalFileGraphMap(StringPairGraphMap_T &nominalFileGraphMap) {
    this->nominalFileGraphMap = &nominalFileGraphMap;
}

void SPXRatio::AddConvoluteFileGraphMap(StringPairGraphMap_T &convoluteFileGraphMap) {
    this->convoluteFileGraphMap = &convoluteFileGraphMap;
}

void SPXRatio::GetGraphs(void) {
    std::string mn = "GetGraphs: ";
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

    SPXOverlayStyle &os = plotConfiguration.GetOverlayStyle();

    if(ratioStyle.IsDataStat() || ratioStyle.IsDataTot()) {

        if(!os.ContainsData()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"data\", yet a ratio with data is specified: " + ratioStyle.ToString());
        }

        std::string key = numeratorDataFile;
        if(debug) std::cout << cn << mn << "Key = [" << key << "]" << std::endl;

        //Check for existence of data key
        if(dataFileGraphMap->count(key) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "dataFileGraphMap[" << key << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //numeratorGraph = (*dataFileGraphMap)[key];
        numeratorGraph.push_back((*dataFileGraphMap)[key]);
        denominatorGraph = (*dataFileGraphMap)[key];

        //Make sure graphs are valid
        //if(!numeratorGraph || !denominatorGraph) {
        if(!numeratorGraph.back() || !denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << key << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }
    }

    if(ratioStyle.IsConvoluteOverReference()) {
        StringPair_T convoluteKey = StringPair_T(numeratorConvoluteGridFile, numeratorConvolutePDFFile);

        if(debug) std::cout << cn << mn << "Getting Graphs for convolute / reference" << std::endl;

        if(!os.ContainsConvolute()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"convolute\", yet a ratio with convolute is specified: " + ratioStyle.ToString());
        }

        if(debug) {
            std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
        }

        //Check for existence of convolute key in convolute graph map
        if(convoluteFileGraphMap->count(convoluteKey) == 0) {
            PrintConvoluteFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Check for existence of convolute key in reference graph map
        if(referenceFileGraphMap->count(convoluteKey) == 0) {
            PrintReferenceFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Keys exist, grab graphs
        //numeratorGraph = (*convoluteFileGraphMap)[convoluteKey];
        numeratorGraph.push_back((*convoluteFileGraphMap)[convoluteKey]);
        denominatorGraph = (*referenceFileGraphMap)[convoluteKey];

        //Make sure graphs are valid
        //if(!numeratorGraph) {
        if(!numeratorGraph.back()) {
            std::ostringstream oss;
            oss << "TGraph pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at referenceFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Print graphs (debug)
        if(debug) {
            std::cout << cn << mn << "\n Printing Numerator (convolute) Graph:" << std::endl;
            //numeratorGraph->Print();
            numeratorGraph.back()->Print();

            std::cout << cn << mn << "\n Printing Denominator (reference) Graph: " << std::endl;
            denominatorGraph->Print();
        }
    }

    if(ratioStyle.IsConvoluteOverNominal()) {
        StringPair_T convoluteKey = StringPair_T(numeratorConvoluteGridFile, numeratorConvolutePDFFile);

        if(debug) std::cout << cn << mn << "Getting Graphs for convolute / nominal" << std::endl;

        if(!os.ContainsConvolute()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"convolute\", yet a ratio with convolute is specified: " + ratioStyle.ToString());
        }

        if(debug) {
            std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
        }

        //TC>> Introduce loop over convoluteMap instead of checking for convoluteKey
        //Check for existence of convolute key in convolute graph map
        //if(convoluteFileGraphMap->count(convoluteKey) == 0) {
	//    PrintConvoluteFileGraphMapKeys(std::cerr);

	//    std::ostringstream oss;
	//    oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
	//    throw SPXGraphException(cn + mn + oss.str());
	//}

        //Check for existence of convolute key in nominal graph map
        if(nominalFileGraphMap->count(convoluteKey) == 0) {
	    PrintReferenceFileGraphMapKeys(std::cerr);

	    std::ostringstream oss;
	    oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
	    throw SPXGraphException(cn + mn + oss.str());
	}

	denominatorGraph = (*nominalFileGraphMap)[convoluteKey];
        if(!denominatorGraph) {
         std::ostringstream oss;
         oss << "TGraph pointer at nominalFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
         throw SPXGraphException(cn + mn + oss.str());
        }

        for(StringPairGraphMap_T::const_iterator it = convoluteFileGraphMap->begin(); it !=  convoluteFileGraphMap->end(); ++it) 
        {
	 numeratorGraph.push_back(it->second);
       
        //Keys exist, grab graphs
        //TC numeratorGraph = (*convoluteFileGraphMap)[convoluteKey];
        //Make sure graphs are valid
        //if(!numeratorGraph) {
        //    std::ostringstream oss;
        //    oss << "TGraph pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
        //    throw SPXGraphException(cn + mn + oss.str());
        //}

         //Print graphs (debug)
         if(debug) {
            std::cout << cn << mn << "\n Printing Numerator (convolute) Graph:" << std::endl;
            numeratorGraph.back()->Print();

            std::cout << cn << mn << "\n Printing Denominator (nominal) Graph: " << std::endl;
            denominatorGraph->Print();
         }
        }
    }

    if(ratioStyle.IsDataOverConvolute()) {
        //Create keys
        std::string dataKey = numeratorDataFile;
        StringPair_T convoluteKey = StringPair_T(denominatorConvoluteGridFile, denominatorConvolutePDFFile);

        if(!os.ContainsData()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"data\", yet a ratio with data is specified: " + ratioStyle.ToString());
        }

        if(!os.ContainsConvolute()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"convolute\", yet a ratio with convolute is specified: " + ratioStyle.ToString());
        }

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
        //if(convoluteFileGraphMap->count(convoluteKey) == 0) {
        //    PrintConvoluteFileGraphMapKeys(std::cerr);

        //    std::ostringstream oss;
        //    oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
        //    throw SPXGraphException(cn + mn + oss.str());
        //}

        //Keys exist, grab graphs
        //numeratorGraph = (*dataFileGraphMap)[dataKey];
        numeratorGraph.push_back((*dataFileGraphMap)[dataKey]);
        //denominatorGraph = (*convoluteFileGraphMap)[convoluteKey];

        //Make sure graphs are valid
        if(!numeratorGraph.back()) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << dataKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
        }
        for(StringPairGraphMap_T::const_iterator it = convoluteFileGraphMap->begin(); it !=  convoluteFileGraphMap->end(); ++it) 
        {
	 TGraphAsymmErrors *denomitatorGraph=it->second;
         if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
         }
        }
    }

    else if(ratioStyle.IsConvoluteOverData()) {
        //Create keys
        StringPair_T convoluteKey = StringPair_T(numeratorConvoluteGridFile, numeratorConvolutePDFFile);
        std::string dataKey = denominatorDataFile;

        if(!os.ContainsConvolute()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"convolute\", yet a ratio with convolute is specified: " + ratioStyle.ToString());
        }

        if(!os.ContainsData()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"data\", yet a ratio with data is specified: " + ratioStyle.ToString());
        }

        if(debug) {
            std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
            std::cout << cn << mn << "Data Key = [" << dataKey << "]" << std::endl;
        }

        //Check for existence of convolute key
        //if(convoluteFileGraphMap->count(convoluteKey) == 0) {
        //    PrintConvoluteFileGraphMapKeys(std::cerr);

        //    std::ostringstream oss;
        //    oss << "convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] was not found: Invalid key";
        //    throw SPXGraphException(cn + mn + oss.str());
        //}

        //Check for existence of data key
        if(dataFileGraphMap->count(dataKey) == 0) {
            PrintDataFileGraphMapKeys(std::cerr);

            std::ostringstream oss;
            oss << "dataFileGraphMap[" << dataKey << "] was not found: Invalid key";
            throw SPXGraphException(cn + mn + oss.str());
        }

        //Keys exist, grab graphs
        denominatorGraph = (*dataFileGraphMap)[dataKey];
        for(StringPairGraphMap_T::const_iterator it = convoluteFileGraphMap->begin(); it !=  convoluteFileGraphMap->end(); ++it) 
        {
	 numeratorGraph.push_back(it->second);
         //numeratorGraph = (*convoluteFileGraphMap)[convoluteKey];

         //Make sure graphs are valid
         if(!numeratorGraph.back()) {
            std::ostringstream oss;
            oss << "TGraph pointer at convoluteFileGraphMap not found";
            throw SPXGraphException(cn + mn + oss.str());
         }

         if(!denominatorGraph) {
            std::ostringstream oss;
            oss << "TGraph pointer at dataFileGraphMap[" << dataKey << "] is NULL";
            throw SPXGraphException(cn + mn + oss.str());
         }
	}
    }

    else if(ratioStyle.IsDataOverData()) {
        //Create keys
        std::string numDataKey = numeratorDataFile;
        std::string denDataKey = denominatorDataFile;

        if(!os.ContainsData()) {
            throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"data\", yet a ratio with data is specified: " + ratioStyle.ToString());
        }

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
        //numeratorGraph = (*dataFileGraphMap)[numDataKey];
        numeratorGraph.push_back((*dataFileGraphMap)[numDataKey]);
        denominatorGraph = (*dataFileGraphMap)[denDataKey];

        //Make sure graphs are valid
        if(!numeratorGraph.back()) {
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
