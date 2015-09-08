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
//	(numerator) / (denominator) 				       <-- MUST have the parenthesis!
//
// 	Where the options for numerator and denominator are:
//
//		[grid_steering_file.txt, pdf_steering_file.txt]		<-- Convolute
//		data_steering file					<-- Data
//		grid_steering file 					<-- Reference
//
//	And the numerator/denominator pair options are:
//
//		data / convolute
//		convolute / data
//		data / data
//		convolute / reference
//
void SPXRatio::Parse(std::string &s) {
    std::string mn = "Parse:";
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

        if (debug) std::cout<<cn<<mn<<"numeratorConvoluteGridFile= "<<numeratorConvoluteGridFile.c_str()
			    <<" numeratorConvolutePDFFile= "<<numeratorConvolutePDFFile.c_str()<<std::endl;


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
        std::string numeratorDataAlias  = CheckForAlias(numeratorDataFile, "data");
        std::string denominatorGridAlias= CheckForAlias(denominatorConvoluteGridFile, "grid");
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
        std::string numeratorPDFAlias  = CheckForAlias(numeratorConvolutePDFFile, "pdf");
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
        std::string numeratorDataAlias   = CheckForAlias(numeratorDataFile, "data");
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

   TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraph.back(), denominatorGraph, ZeroDenGraphErrors);
   graph->SetFillStyle(1001);
   graph->SetFillColor(kGray);
   ratioGraph.push_back(graph);

   if(debug) std::cout<<cn<<mn<< "Successfully divided data stat graph with options: " << std::endl;
   if(debug) std::cout<< "\t Fill Style = " << 1001 << std::endl;
   if(debug) std::cout<< "\t Fill Color = " << kGray << std::endl;
  } catch(const SPXException &e) {
   std::cerr << e.what() << std::endl;
   throw SPXGraphException(cn + mn + "Unable to divide data stat graphs");
  }

  return;

 } else if(ratioStyle.IsDataTot()) {
  try {

   TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraph.back(), denominatorGraph, ZeroDenGraphErrors);
   graph->SetFillStyle(1001);
   graph->SetFillColor(kGray);
   ratioGraph.push_back(graph);

   if(debug) std::cout<<cn<<mn<<"Successfully divided data tot graph with options: " << std::endl;
   if(debug) std::cout<< "\t Fill Style = " << 1001 << std::endl;
   if(debug) std::cout<< "\t Fill Color = " << kGray << std::endl;

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

   if (debug) std::cout<<cn<<mn<<" ratioStyle.IsConvoluteOverData "<<std::endl;

   if (numeratorGraph.size()==0)
    throw SPXGraphException(cn + mn + "No numeratorGraph found !");

   //plotConfiguration.GetPlotConfigurationInstance(index).SteeringFile;

   if (MatchBinning) {
    if (debug) {
     std::cout<<cn<<mn<<"Match binning for denominatorGraph= "<<denominatorGraph->GetName()<<std::endl;
     //denominatorGraph->Print();
    }

    for (int i=0; i<numeratorGraph.size(); i++){
     try {
      if (debug) std::cout<<cn<<mn<<"Match binning for numeratorGraph["<<i<<"]= "<<numeratorGraph[i]->GetName()
                                  <<" denominatorGraph= "<<denominatorGraph->GetName()<<std::endl; 

      SPXGraphUtilities::MatchBinning(denominatorGraph, numeratorGraph[i], true);

      if (debug) {
       std::cout<<cn<<mn<<"After Match binning for numeratorGraph["<<i<<"]= "<<numeratorGraph[i]->GetName()
                       <<" denominatorGraph= "<<denominatorGraph->GetName()<<std::endl;
       numeratorGraph[i]->Print();
      }
     } catch(const SPXException &e) {
      std::cerr << e.what() << std::endl;
      std::ostringstream oss;
      oss << cn <<mn<<"Unable to match numerator "<<numeratorGraph[i]->GetName()
          <<" with nbins= "<<numeratorGraph[i]->GetN()
          <<" with denominator= "<<denominatorGraph->GetName()<<" nbins= "<<denominatorGraph->GetN();
      throw SPXGraphException(oss.str());
     }
    }
   }
  }
 } else if(ratioStyle.IsDataOverConvolute()) {
  pci = plotConfiguration.GetPlotConfigurationInstance(denominatorConvolutePDFFile);
  if(debug) {
   pci.Print();
  }

  if (debug) std::cout<<cn<<mn<<"ratioStyle.IsDataOverConvolute "<<std::endl;

  if (numeratorGraph.size()==0)
   throw SPXGraphException(cn + mn + "No numeratorGraph found !");

  //Match the convolute binning to the data binning
  if (MatchBinning) {
   try {
    for (int i=0; i<numeratorGraph.size(); i++){
     SPXGraphUtilities::MatchBinning(numeratorGraph[i], denominatorGraph, true);
    }
   } catch(const SPXException &e) {
    std::cerr << e.what() << std::endl;
    throw SPXGraphException(cn + mn + "Unable to match convolute binning to data binning");
   }

   //Match the convolute binning to the data binning
   try {
    for (int i=0; i<numeratorGraphstatonly.size(); i++){
     SPXGraphUtilities::MatchBinning(numeratorGraphstatonly[i], denominatorGraph, true);
    }
   } catch(const SPXException &e) {
    std::cerr << e.what() << std::endl;
    throw SPXGraphException(cn + mn + "Unable to match convolute binning to data binning");
   }
  }
 }
//
 else if(ratioStyle.IsDataOverData()) {

  if (debug) {
   std::cout<<cn<<mn<<"ratioStyle.IsDataOverData() "<<std::endl;
   std::cout<<cn<<mn<<"Print denominator graph: "<<std::endl;
   SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)denominatorGraph);
   denominatorGraph->Print();
  }

  if (MatchBinning) {

   // Only keep common bins 
   if (debug) {
    std::cout<<cn<<mn<<"First try to find common bins "<<std::endl;
   }

   for (int i=0; i<numeratorGraph.size(); i++){
    TGraphAsymmErrors* gnom=SPXGraphUtilities::FindCommonBins(denominatorGraph,numeratorGraph.at(i));
    if (!gnom) {
     std::ostringstream oss;
     oss << cn <<mn<<"Unable to create graph gnom from "<<numeratorGraph[i]->GetName()
	                                       <<" and "<< denominatorGraph->GetName();
      throw SPXGraphException(oss.str());
    }

    TGraphAsymmErrors* gden=SPXGraphUtilities::FindCommonBins(numeratorGraph.at(i),denominatorGraph);
    if (!gden) {
     std::ostringstream oss;
     oss << cn <<mn<<"Unable to create graph gden from "<<numeratorGraph[i]->GetName()
	                                       <<" and "<< denominatorGraph->GetName();
      throw SPXGraphException(oss.str());
    }

    if (debug) {
     std::cout<<cn<<mn<<"Merged binning gnom "<<gnom->GetName()<<std::endl;
     SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)gnom);
     gnom->Print();

     std::cout<<cn<<mn<<"Merged binning gden "<<gden->GetName()<<std::endl;
     gden->Print();
    }

    denominatorGraph=gden;
    numeratorGraph.at(i)=gnom;
   }

   for (int i=0; i<numeratorGraphstatonly.size(); i++){

    TGraphAsymmErrors* gnomstatonly=SPXGraphUtilities::FindCommonBins(denominatorGraphstatonly,numeratorGraphstatonly.at(i));
    if (!gnomstatonly) {
     std::ostringstream oss;
     oss << cn <<mn<<"Unable to create graph gnomstatonly from "<<numeratorGraphstatonly[i]->GetName()
	                                              <<" and "<< denominatorGraphstatonly->GetName();
      throw SPXGraphException(oss.str());
    }

    TGraphAsymmErrors* gdenstatonly=SPXGraphUtilities::FindCommonBins(numeratorGraphstatonly.at(i),denominatorGraphstatonly);
    if (!gdenstatonly) {
     std::ostringstream oss;
     oss << cn <<mn<<"Unable to create graph gdenstatonly from "<<numeratorGraphstatonly[i]->GetName()
	                                              <<" and "<< denominatorGraphstatonly->GetName();
      throw SPXGraphException(oss.str());
    }

    //if (debug) {
    // SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)gnom);
    //}

    std::cout<<cn<<mn<<"Divison stat only done"<<std::endl;

    denominatorGraphstatonly=gdenstatonly;
    numeratorGraphstatonly.at(i)=gnomstatonly;

   }
 
   try { 
    for (int i=0; i<numeratorGraph.size(); i++){
     if (debug) {
      std::cout<<cn<<mn<<"before calling MatchBinning "<<std::endl;
      std::cout<<cn<<mn<<"numeratorGraph["<<i<<"]: " << numeratorGraph[i]->GetName()<<std::endl;
      SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)numeratorGraph[i]);
      numeratorGraph[i]->Print();
     }
     SPXGraphUtilities::MatchBinning(numeratorGraph[i], denominatorGraph, true);
     if (debug) {
      std::cout<<cn<<mn<<"after calling MatchBinning "<<std::endl;
      std::cout<<cn<<mn<<"numeratorGraph["<<i<<"]: " << numeratorGraph[i]->GetName()<<std::endl;
      SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)numeratorGraph[i]);
      numeratorGraph[i]->Print();
     }
    }
   } catch(const SPXException &e) {
    std::cerr << e.what() << std::endl;
    throw SPXGraphException(cn + mn + "ratioStyle DataOverData Unable to match data binning to data binning");
   }
  }
 } else if (ratioStyle.IsConvoluteOverNominal()) {
  if (debug) std::cout<<cn<<mn<<" ratioStyle.IsConvoluteOverNominal "<<std::endl;

  std::cout<<cn<<mn<<" ratioStyle.IsConvoluteOverNominal() nothing implemented yet "<<std::endl;
 
 } else {
  if (debug) std::cout<<cn<<mn<<"No special ratiostyle  "<<std::endl;
 }
 
 try {
  //Determine how to divide based on ratioStyle: zeroNumErrors/zeroDenErrors set with '!'
  DivideErrorType_t divideType;

  if (ratioStyle.GetZeroNumeratorErrors() && ratioStyle.GetZeroDenominatorErrors()) {
   if (debug) std::cout<<cn<<mn<<"Zero all error "<<std::endl;
   divideType = ZeroAllErrors;
  } else if (ratioStyle.GetZeroNumeratorErrors()) {
   if (debug) std::cout<<cn<<mn<<"Zero numerator error "<<std::endl;
   divideType = ZeroNumGraphErrors;
  } else if (ratioStyle.GetZeroDenominatorErrors()) {
   if (debug) std::cout<<cn<<mn<<"Zero denominator error "<<std::endl;
   divideType = ZeroDenGraphErrors;
  } else {
   if (debug) std::cout<<cn<<mn<<"Add errors "<<std::endl;
   divideType = AddErrors;
  }

  //Divide graphs
  if (debug) std::cout<<cn<<mn<<"Now divide graph "<<std::endl;

  if (numeratorGraph.size()==0)
   throw SPXGraphException(cn + mn + "No numeratorGraph found !");

  if (debug) std::cout <<cn<<mn<<"Number of numeratorGraphs= "<<numeratorGraph.size()<<std::endl;

  for (int i=0; i<numeratorGraph.size(); i++){

   if (debug) {
     std::cout <<cn<<mn<<"numeratorGraph["<<i<<"]= "<<numeratorGraph[i]->GetName()
               <<" nbin= "<<numeratorGraph[i]->GetN()
               <<" denominatorGraph= "<<denominatorGraph->GetName()
               <<" nbin= "<<denominatorGraph->GetN()
               <<" divideType= "<<divideType<<std::endl;

     std::cout <<cn<<mn<<"numeratorGraph["<<i<<"]= "<<numeratorGraph[i]->GetName()
               <<" denominatorGraph= "<<denominatorGraph->GetName()<<" divideType= "<<divideType<<std::endl;
 
     //SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)numeratorGraph[i]);
   }

   if ( numeratorGraph[i]->GetN()!=denominatorGraph->GetN()) {
    if (MatchBinning) {
     if (debug) {
      std::cout<<cn<<mn<<"Bins are different "<<std::endl;
      std::cout<<cn<<mn<<"Match binning for numeratorGraph["<<i<<"]= "<<numeratorGraph[i]->GetName()
                       <<" denominatorGraph= "<<denominatorGraph->GetName()<<std::endl;
     }
     if (numeratorGraph[i]->GetN()>denominatorGraph->GetN() ) {
      SPXGraphUtilities::MatchBinning(denominatorGraph, numeratorGraph[i], true);
     } else {
      SPXGraphUtilities::MatchBinning(numeratorGraph[i], denominatorGraph, true);
     }
    } else {
      std::cout<<cn<<mn<<"Bins are different numerator nbins= "<<numeratorGraph[i]->GetN() 
  	               <<" denominatorGraph nbins= "<<denominatorGraph->GetN()<<std::endl;
      std::cout<<cn<<mn<<"..but MatchBinning is OFF "<<std::endl;
      std::cout<<cn<<mn<<"...do not know what to do !"<<std::endl;

    }
   }

   TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraph[i], denominatorGraph,divideType); 
   if (!graph) throw SPXGraphException(cn + mn + "Ratio Graph not found !");
  
   if (debug) {
    std::cout<<cn+mn+"After Divide numeratorGraph["<<i<<"]/denominatorGraph "<<numeratorGraph[i]->GetName()<< std::endl;
    SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)graph);
    graph->Print();
   }
   ratioGraph.push_back(graph);
  }

  // add statistical error from Data as separate graph 
  if (ratioStyle.IsDataOverConvolute() || ratioStyle.IsDataOverData()) {

   if (debug) cout<<cn<<mn<<"Number of numeratorGraphstatonly= "<<numeratorGraphstatonly.size()<<std::endl;
   if (numeratorGraphstatonly.size()==0) cout<<cn<<mn<<"WARNING No statistical ratio graphs found !"<<std::endl;

   for (int i=0; i<numeratorGraphstatonly.size(); i++){

    if (!numeratorGraphstatonly.at(i)) {
     std::ostringstream oss;
     oss << "numeratorGraphstatonly["<<i<<"] was not found !";
     std::cout<<cn<<mn<<oss.str()<<std::endl;
     throw SPXGraphException(cn + mn + oss.str());
    }

    if (ratioStyle.IsDataOverData()) {
     if (!denominatorGraphstatonly) {
      std::ostringstream oss;
      oss << "denominatorGraphstatonly was not found !";
      std::cout<<cn<<mn<<oss.str()<<std::endl;
      throw SPXGraphException(cn + mn + oss.str());
     }

     if (debug) {
      std::cout<<cn<<mn<<"Divide graph"<<numeratorGraphstatonly.at(i)->GetName()
               <<" and "<<denominatorGraphstatonly->GetName()
      <<std::endl;
     }

     TGraphAsymmErrors *graph = SPXGraphUtilities::Divide(numeratorGraphstatonly.at(i), denominatorGraphstatonly,divideType);
     if (!graph) throw SPXGraphException(cn+mn+"Ratio graph statistical only not found !");

     if (debug) {
      std::cout<<cn<<mn<<"Ratio graph"<<graph->GetName()<<std::endl;
      graph->Print();
     }
     ratioGraphstatonly.push_back(graph);
    }
   }
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
    // special case if the same grid directory is given
    // In this case key name is _i (see SPXPlot::InitializeCrossSections)

    if (index>0) { 
     bool doubleentry=false;
     for (int i=0; i<index; i++) {
      std::string aliasold = plotConfiguration.GetPlotConfigurationInstance(i).gridSteeringFile.GetFilename();
      if (alias==aliasold) {
       doubleentry=true;
       std::cout<<cn<<mn<<"Double entry in grid directories for index= "<<i<<" "<<alias.c_str()<<std::endl;
      }
     }
 
     if (doubleentry) {
      std::stringstream newname;
      newname << alias<<"_"<< index;
      alias=newname.str();
      std::cout<<cn<<mn<<"Same grid directories are used !  set alias to"<<alias.c_str()<<std::endl;
     }
    }
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

void SPXRatio::AddDataFileLabelMap(StringLabelMap_T &dataFileLabelMap) {
 this->dataFileLabelMap = &dataFileLabelMap;
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

void SPXRatio::AddConvoluteFilePDFMap(StringPairPDFMap_T &convoluteFilePDFMap) {
 this->convoluteFilePDFMap = &convoluteFilePDFMap;
}

void SPXRatio::GetGraphs(void) {
 std::string mn = "GetGraphs: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 SPXOverlayStyle &os = plotConfiguration.GetOverlayStyle();

 if(debug) std::cout<<cn<<mn<<"Ratio style is: " << ratioStyle.ToString() << " (" << ratioStyle.GetNumerator() \
                     << " / " << ratioStyle.GetDenominator() << ")" << std::endl;

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

  numeratorGraph.push_back((*dataFileGraphMap)[key]);

  denominatorGraph = (*dataFileGraphMap)[key];

  //Make sure graphs are valid
  if(!denominatorGraph) {
   std::ostringstream oss;
   oss << "TGraph denominatorGraph pointer at dataFileGraphMap[" << key << "] is NULL";
   throw SPXGraphException(cn + mn + oss.str());
  }

  for (int i=0; i<numeratorGraph.size(); i++) {
   if(!numeratorGraph[i]) {
    std::ostringstream oss;
    oss << "TGraph numeratorGraph["<<i<<"] pointer at dataFileGraphMap[" << key << "] is NULL";
    throw SPXGraphException(cn + mn + oss.str());
   }
  }
 }

 if(debug) std::cout << cn << mn << "data style passed" << std::endl;

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

  //if (debug)std::cout << cn << mn << "\n Key found push in nunmeratorGraph " <<std::endl;

  //Keys exist, grab graphs
  numeratorGraph.push_back((*convoluteFileGraphMap)[convoluteKey]);
  denominatorGraph = (*referenceFileGraphMap)[convoluteKey];

  //Make sure graphs are valid

  if(!numeratorGraph.size()==0) {
   std::ostringstream oss;
   oss << "TGraph numeratorGraph has zero size at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
   throw SPXGraphException(cn + mn + oss.str());
  }

  if (debug) 
   std::cout << cn << mn << "\n 1 Number of numeratorGraph " << numeratorGraph.size()<<std::endl;

  for (int i=0; i< numeratorGraph.size(); i++) {
   if (!numeratorGraph[i]) {
    std::ostringstream oss;
    oss << "TGraph numeratorGraph["<<i<<"] pointer at convoluteFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
    throw SPXGraphException(cn + mn + oss.str());
   }
  }

  if(!denominatorGraph) {
   std::ostringstream oss;
   oss << "TGraph pointer at referenceFileGraphMap[" << convoluteKey.first << ", " << convoluteKey.second << "] is NULL";
   throw SPXGraphException(cn + mn + oss.str());
  }

  //Print graphs (debug)
  if(debug) {
   for (int i=0; i<numeratorGraph.size(); i++) {
    std::cout << cn << mn << "\n Printing  numeratorGraph["<<i<<"] (convolute) Graph: " << numeratorGraph[i]->GetName()<<std::endl;
    numeratorGraph[i]->Print();
   }
   std::cout << cn << mn << "\n Printing Denominator (reference) Graph: "<< denominatorGraph->GetName()<< std::endl;
   denominatorGraph->Print();
  }
 }

 if(debug) std::cout << cn << mn << "data over reference passed" << std::endl;

 if(ratioStyle.IsConvoluteOverNominal()) {
  StringPair_T convoluteKey = StringPair_T(numeratorConvoluteGridFile, numeratorConvolutePDFFile);

  if(debug) std::cout << cn << mn << "Getting Graphs for convolute / nominal" << std::endl;

  if(!os.ContainsConvolute()) {
   throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"convolute\", yet a ratio with convolute is specified: " + ratioStyle.ToString());
  }

  if(debug) {
   std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;
  }

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

  if (debug) {
   std::cout<<cn<<mn<<"ConvoluteOverNominal Look for convolute key [" <<convoluteKey.first.c_str()<<", "<< convoluteKey.second.c_str()<<"]"<< std::endl;
  }

  SPXPDF *pdf= (*convoluteFilePDFMap)[convoluteKey];
  if (!pdf) throw SPXGraphException(cn + mn + "PDF not found for convolutekey="+convoluteKey.first);
  else 
   std::cout << cn << mn << "Found pdf= " << pdf->GetPDFName() << std::endl;
  int nbands=pdf->GetNBands();
   
  for (int iband=0; iband<nbands; iband++) {
   TGraphAsymmErrors * gband   =pdf->GetBand(iband);
   std::string         gtype   =pdf->GetBandType(iband);
   if (!gband) throw SPXParseException(cn+mn+" gband not found !");
   if (debug) std::cout << cn <<mn<<"Band "<<gband->GetName()<<" type= "<<gtype.c_str()<<std::endl;
   numeratorGraph.push_back(gband);
  }

  if(numeratorGraph.size()==0) {
   std::ostringstream oss;
   oss << "TGraph numeratorGraph has zero size at convoluteFileGraphMap[] is NULL";
   throw SPXGraphException(cn + mn + oss.str());
  }

  for (int i=0; i< numeratorGraph.size(); i++) {
   if (!numeratorGraph[i]) {
    std::ostringstream oss;
    oss << "TGraph numeratorGraph["<<i<<"] pointer at convoluteFilePDFMap[ ] is NULL";
    throw SPXGraphException(cn + mn + oss.str());
   }
  }       

  if(debug) {
   for (int i=0; i<numeratorGraph.size(); i++) {
    std::cout << cn << mn << "\n Printing numeratorGraph["<<i<<"] (convolute) Graph:"<< numeratorGraph[i]->GetName() << std::endl;
    numeratorGraph[i]->Print();
    //if (!denominatorGraph) std::cout<<"denominatorGraph not found "<<std::endl;
    std::cout << cn << mn << "\n Printing Denominator (nominal) Graph: " << denominatorGraph->GetName() << std::endl;
    denominatorGraph->Print();
   }
  }
 }

 if(debug) std::cout << cn << mn << "data over nominal passed" << std::endl;

 if(ratioStyle.IsDataOverConvolute()) {
  //Create keys
  std::string dataKey = numeratorDataFile;
  StringPair_T convoluteKey = StringPair_T(denominatorConvoluteGridFile, denominatorConvolutePDFFile);

  std::string dataKeystat = numeratorDataFile+"_stat";

  if(debug) std::cout << cn << mn << "Data over Convolute" << std::endl;

  if(!os.ContainsData()) {
   throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"data\", yet a ratio with data is specified: " + ratioStyle.ToString());
  }

  if(!os.ContainsConvolute()) {
   throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"convolute\", yet a ratio with convolute is specified: " + ratioStyle.ToString());
  }

  if(debug) {
   std::cout << cn << mn << "Data Key = [" << dataKey << "]" << std::endl;
   std::cout << cn << mn << "Data Key stat only= [" << dataKeystat << "]" << std::endl;
   std::cout << cn << mn << "Convolute Key = [" << convoluteKey.first << ", " << convoluteKey.second << "]" << std::endl;

   PrintDataFileGraphMapKeys(std::cout);
   PrintConvoluteFilePDFMapKeys(std::cout);

  }

  //Check for existence of data key
  if(dataFileGraphMap->count(dataKey) == 0) {
   PrintDataFileGraphMapKeys(std::cerr);
   std::ostringstream oss;
   oss <<"dataFileGraphMap[" << dataKey << "] was not found: Invalid key";
   throw SPXGraphException(cn + mn + oss.str());
  }

  //Check for existence of data key statistical only
  if(dataFileGraphMap->count(dataKeystat) == 0) {
   PrintDataFileGraphMapKeys(std::cerr);
   std::ostringstream oss;
   oss <<"dataFileGraphMap[" << dataKeystat << "] was not found: Invalid key";
   throw SPXGraphException(cn + mn + oss.str());
  }

  // Keys exist, grab graphs
  numeratorGraph.push_back((*dataFileGraphMap)[dataKey]);

  //Make sure graphs are valid
  if(numeratorGraph.size()==0) {
   std::ostringstream oss;
   oss <<"numeratorGraph has zero size at dataFileGraphMap[" << dataKey << "]";
   throw SPXGraphException(cn + mn + oss.str());
  }

  for (int i=0; i< numeratorGraph.size(); i++) {
   if (!numeratorGraph[i]) {
    std::ostringstream oss;
    oss <<"numeratorGraph["<<i<<"] pointer at dataFileGraphMap[" << dataKey << "] is zero";
    throw SPXGraphException(cn + mn + oss.str());
   }
   if (debug) std::cout << cn << mn << "data graph is  "<< numeratorGraph[i]->GetName() << std::endl;
  }

  // Keys exist, grab graphs
  numeratorGraphstatonly.push_back((*dataFileGraphMap)[dataKeystat]);

  //Make sure graphs are valid
  if(numeratorGraphstatonly.size()==0) {
   std::ostringstream oss;
   oss<< "TGraph numeratorGraphstatonly has zero size at dataFileGraphMap[" << dataKey << "]";
   throw SPXGraphException(cn + mn + oss.str());
  }

  for (int i=0; i< numeratorGraphstatonly.size(); i++) {
   if (!numeratorGraph[i]) {
    std::ostringstream oss;
    oss << "TGraph numeratorGraphstatonly["<<i<<"] pointer at dataFileGraphMap[" << dataKeystat << "] is zero";
    throw SPXGraphException(cn + mn + oss.str());
   }
   if (debug) std::cout << cn << mn << "data stat only graph is  "<< numeratorGraphstatonly[i]->GetName() << std::endl;
  }

  if (debug) {
   std::cout<<cn<<mn<<"DataOverConvolute Look for convolute key [" <<convoluteKey.first.c_str()<<", "<< convoluteKey.second.c_str()<<"]"<< std::endl;
  }

  //denominatorGraph = (*convoluteFileGraphMap)[convoluteKey];
  SPXPDF *pdf= (*convoluteFilePDFMap)[convoluteKey];
  if (convoluteFilePDFMap->count(convoluteKey) == 0) {
   std::cout<<cn<<mn<<"DataOverConvolute key [" <<convoluteKey.first.c_str()<<", "<< convoluteKey.second.c_str()<<"] not found !"<< std::endl;   

   //PrintConvoluteFilePDFMapKeys(std::cout);
  } else if (debug) {
   std::cout<<cn<<mn<<"DataOverConvolute key [" <<convoluteKey.first.c_str()<<", "<< convoluteKey.second.c_str()<<"] found !"<< std::endl;   
  }

  if (!pdf) {
   std::cout<<cn<<mn<<"DataOverConvolute PDF not found !"<< std::endl;      
   PrintConvoluteFilePDFMapKeys(std::cout);
   throw SPXGraphException(cn + mn + "PDF object not found ! ");
  }
  else 
   std::cout << cn << mn << "Found pdf= " << pdf->GetPDFName() << std::endl;

  int nbands=pdf->GetNBands();
  if (nbands==0)  std::cout << cn << mn << "No bands found " << std::endl;   
  else std::cout << cn << mn << "Number of band nbands= " << nbands << std::endl;   

  TGraphAsymmErrors * mygband=0;
  for (int iband=0; iband<nbands; iband++) {
   TGraphAsymmErrors * gband   =pdf->GetBand(iband);
   std::string         gtype   =pdf->GetBandType(iband);
   if (!gband) throw SPXParseException(cn+mn+" gband not found !");
   if (debug) std::cout << cn <<mn<<"Band "<<gband->GetName()<<" type= "<<gtype.c_str()<<std::endl;
   TString gname=gband->GetName();
   if (gname.Contains("_total_")) mygband=gband; 
  }

  if (!mygband) {
   if (!pdf->GetBand(0)) throw SPXParseException(cn+mn+"no band found in PDF !");
   mygband=pdf->GetBand(0);
  }

  if (debug) std::cout << cn << mn << "use as denominator graph " << mygband->GetName() << std::endl;

  denominatorGraph=mygband;

 } else if(ratioStyle.IsConvoluteOverData()) {
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

  //Check for existence of data key
  if(dataFileGraphMap->count(dataKey) == 0) {
   PrintDataFileGraphMapKeys(std::cerr);

   std::ostringstream oss;
   oss << "dataFileGraphMap[" << dataKey << "] was not found: Invalid key";
   throw SPXGraphException(cn + mn + oss.str());
  } else if(debug) std::cout << cn << mn << "dataFileGraphMap["<<dataKey<<"] " << " is  ok " << std::endl;

  //Keys exist, grab graphs
  denominatorGraph = (*dataFileGraphMap)[dataKey];
  if(!denominatorGraph) {
   std::ostringstream oss;
   oss << "TGraph pointer at dataFileGraphMap[" << dataKey << "] is NULL";
   throw SPXGraphException(cn + mn + oss.str());
  } else
   if(debug) std::cout << cn << mn << "Found denominatorGraph= " << denominatorGraph->GetName() << std::endl;


  if( convoluteFilePDFMap->count(convoluteKey) == 0) {
   PrintConvoluteFilePDFMapKeys(std::cerr);

   std::ostringstream oss;
   oss << "convoluteFilePDFMap[" << convoluteKey.first<<","<<convoluteKey.second << "] was not found: Invalid key";
   throw SPXGraphException(cn + mn + oss.str());
  } 

  // for(StringPairGraphMap_T::const_iterator it = convoluteFileGraphMap->begin(); it !=  convoluteFileGraphMap->end(); ++it) 
  //{

  // StringPair_T convoluteKeytmp =it->first;
  //if(debug) std::cout << cn << mn << "convoluteFileGraphMap["<<convoluteKeytmp.first << ", " << convoluteKeytmp.second << "]"<<std::endl; 
  //numeratorGraph.push_back((*convoluteFileGraphMap)[convoluteKey]);
  //numeratorGraph.push_back(it->second);

  if (debug) {
   std::cout<<cn<<mn<<"Look for convolute key [" <<convoluteKey.first.c_str()<<", "<< convoluteKey.second.c_str()<<"]"<< std::endl;
  }

 

  SPXPDF *pdf= (*convoluteFilePDFMap)[convoluteKey];
  if (!pdf) throw SPXGraphException(cn + mn + "PDF not found ");
  else 
   std::cout << cn << mn << "Found pdf= " << pdf->GetPDFName() << std::endl;
  int nbands=pdf->GetNBands();
   
  for (int iband=0; iband<nbands; iband++) {
   TGraphAsymmErrors * gband   =pdf->GetBand(iband);
   std::string         gtype   =pdf->GetBandType(iband);
   if (!gband) throw SPXParseException(cn+mn+" gband not found !");
   if (debug) std::cout << cn <<mn<<"Band "<<gband->GetName()<<" type= "<<gtype.c_str()<<std::endl;
   numeratorGraph.push_back(gband);
  }
   
  //Make sure graphs are valid
  if(numeratorGraph.size()==0) {
   std::ostringstream oss;
   oss << "TGraph numeratorGraph has zero size at convoluteFileGraphMap[" << dataKey << "]";
   throw SPXGraphException(cn + mn + oss.str());
  }

  if(debug) std::cout << cn << mn << "Number of numeratorGraphs= "<<numeratorGraph.size() << std::endl;

  for (int i=0; i< numeratorGraph.size(); i++) {
   if (!numeratorGraph[i]) {
    std::ostringstream oss;
    oss << "TGraph numeratorGraph["<<i<<"] pointer at convoluteFileGraphMap [" << dataKey << "] is zero";
    throw SPXGraphException(cn + mn + oss.str());
   } else {
    if(debug) std::cout << cn << mn << " numeratorGraph["<<i<<"]= "<<numeratorGraph[i]->GetName() << std::endl;
   }
  }
 } else if(ratioStyle.IsDataOverData()) {
  //Create keys
  std::string numDataKey = numeratorDataFile;
  std::string denDataKey = denominatorDataFile;

  if(!os.ContainsData()) {
   throw SPXGraphException(cn + mn + "Overlay Style does NOT contain \"data\", yet a ratio with data is specified: " + ratioStyle.ToString());
  }

  if(debug) {
   std::cout << cn << mn << "Numerator   Data Key = [" << numDataKey << "]" << std::endl;
   std::cout << cn << mn << "Denominator Data Key = [" << denDataKey << "]" << std::endl;
  }

  //Check for existence of numerator data key
  if(dataFileGraphMap->count(numDataKey) == 0) {
   PrintDataFileGraphMapKeys(std::cerr);
   std::ostringstream oss;
   oss << "dataFileGraphMap[" << numDataKey << "] was not found: Invalid key";
   std::cout<<cn<<mn<<oss.str()<<std::endl;
   throw SPXGraphException(cn + mn + oss.str());
  }

  //Check for existence of denominator data key
  if(dataFileGraphMap->count(denDataKey) == 0) {
   PrintDataFileGraphMapKeys(std::cerr);
   std::ostringstream oss;
   oss << "dataFileGraphMap[" << denDataKey << "] was not found: Invalid key";
   std::cout<<cn<<mn<<oss.str()<<std::endl;
   throw SPXGraphException(cn + mn + oss.str());
  }

  if(dataFileLabelMap->count(denDataKey) == 0) {
   PrintDataFileLabelMapKeys(std::cerr);
   std::ostringstream oss;
   oss << "dataFileLabelMap[" << denDataKey << "] was not found: Invalid key";
   std::cout<<cn<<mn<<oss.str()<<std::endl;
   throw SPXGraphException(cn + mn + oss.str());
  }

  if(dataFileLabelMap->count(numDataKey) == 0) {
   PrintDataFileLabelMapKeys(std::cerr);
   std::ostringstream oss;
   oss << "dataFileLabelMap[" << numDataKey << "] was not found: Invalid key";
   std::cout<<cn<<mn<<oss.str()<<std::endl;
   throw SPXGraphException(cn + mn + oss.str());
  }

  numeratorDataLabel=(*dataFileLabelMap)[numDataKey];
  denominatorDataLabel=(*dataFileLabelMap)[denDataKey];

  if (debug) {
   std::cout<<cn<<mn<<"numeratorDataLabel= "<<numeratorDataLabel<<std::endl;
   std::cout<<cn<<mn<<"denominatorDataLabel= "<<denominatorDataLabel<<std::endl;
  }

  //Keys exist, grab graphs

  // now check if map contains systematic uncertainties
  // PrintDataFileGraphMapKeys();

   if (DataFileGraphMapHasSystematics()) {
    for (StringGraphMap_T::iterator it = dataFileGraphMap->begin(); it != dataFileGraphMap->end(); ++it) {
     std::string systkey =  it->first;
     //if (debug) std::cout<<" denDataKey= "<< denDataKey <<" systkey= "<<systkey.c_str()<<std::endl;

     if (systkey!=denDataKey && systkey!=denDataKey+"_stat") {
      //if (debug) std::cout<<" Add numeratorGraph systkey= "<<systkey.c_str()<<std::endl;

      numeratorGraph.push_back((*dataFileGraphMap)[systkey]);
     }
    }
    denominatorGraph = (*dataFileGraphMap)[denDataKey];
   } else {
    numeratorGraph.push_back((*dataFileGraphMap)[numDataKey]);
    denominatorGraph = (*dataFileGraphMap)[denDataKey];
   }

  if(!denominatorGraph) {
   std::ostringstream oss; 
   oss << "TGraph pointer at dataFileGraphMap[" << denDataKey << "] is NULL";
   throw SPXGraphException(cn + mn + oss.str());
  }

  //Make sure graphs are valid
  if(numeratorGraph.size()==0) {
   std::ostringstream oss;
   oss << "TGraph numeratorGraph has zero size at dataFileGraphMap[" << denDataKey << "]";
   throw SPXGraphException(cn + mn + oss.str());
  }

  for (int i=0; i< numeratorGraph.size(); i++) {
   if (!numeratorGraph[i]) {
    std::ostringstream oss;
    oss << "TGraph numeratorGraph["<<i<<"] pointer at dataFileGraphMap[" << numDataKey << "] is zero";
    throw SPXGraphException(cn + mn + oss.str());
   }
  }
  //
  //if (!DataFileGraphMapHasSystematics()) {

   std::string dataKeystat = numDataKey+"_stat";
   //Check for existence of data key statistical only
   if(dataFileGraphMap->count(dataKeystat) == 0) {
    PrintDataFileGraphMapKeys(std::cerr);
    std::ostringstream oss;
    oss <<"dataFileGraphMap[" << dataKeystat << "] was not found: Invalid key";
    throw SPXGraphException(cn + mn + oss.str());
   } else {
    numeratorGraphstatonly.push_back((*dataFileGraphMap)[dataKeystat]);
   }

   dataKeystat = denDataKey+"_stat";
   if (dataFileLabelMap->count(denDataKey) == 0) {
    PrintDataFileLabelMapKeys(std::cerr);
    std::ostringstream oss;
    oss << "dataFileLabelMap[" << dataKeystat << "] was not found: Invalid key";
    throw SPXGraphException(cn + mn + oss.str());
   } else {
    if (debug) std::cout<<cn<<mn<<"Added stat "<<dataKeystat<<" graph "<<(*dataFileGraphMap)[dataKeystat]->GetName()<<std::endl;
    denominatorGraphstatonly=(*dataFileGraphMap)[dataKeystat];
   }  
  //}
  //  
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


void SPXRatio::Draw(std::string option, int statRatios, int totRatios, bool plotmarker, double xbox, double ybox) {
 std::string mn = "Draw: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if ( ratioGraph.size()==0)
  std::cout<<cn<<mn<<"WARNING: Ratio graph is empty "<<std::endl;

 //double xmin=0.25, ymin=0.3, boxsize=0.05;
 double xmin=xbox, ymin=ybox, boxsize=0.05;

  
 // plot data if requested
 if(IsDataStat() || IsDataTot()) {
  for (int igraph=0; igraph < ratioGraph.size(); igraph++) {
   TGraphAsymmErrors *graph = ratioGraph[igraph];
   if (!graph) {
    throw SPXGraphException(cn + mn + "graph not found");
   }
   //Incrementally darken the data_stat/data_tot graphs based on their order for increased visibility
   graph->SetFillColor(graph->GetFillColor() + (statRatios + totRatios));
   graph->Draw("E2");

   int mcolor=graph->GetFillColor();

   if(IsDataTot()) {
    std::string datatext="Data uncertainty";
    SPXDrawUtilities::BoxText(xmin, ymin, boxsize, boxsize, mcolor, datatext, mcolor, 1, 0.75*boxsize);
   }

   if(IsDataStat()) {
    SPXDrawUtilities::BoxText(xmin, ymin, boxsize, boxsize/2., mcolor,"", mcolor, 1, 0.75*boxsize);
   }
  }
 } else if ( IsDataOverData() ){
  
  if (!debug) std::cout<<"DataOverData!"<<std::endl;
  
  std::vector<TGraphAsymmErrors * > ratioGraphord=SPXUtilities::OrderLargestBinNumberGraphVector(ratioGraph);

  int isyst=0;
  for (std::vector<TGraphAsymmErrors *>::iterator igraph = ratioGraphord.begin();  igraph != ratioGraphord.end(); ++igraph ) { 
   TGraphAsymmErrors *graph = *igraph;
   if (!graph) std::cout<<"Graph not found !"<<std::endl;
   TString gname=graph->GetName();

   Color_t icolfill  =graph->GetFillColor();
   Style_t istyfill  =graph->GetFillStyle();
   Style_t istyline  =graph->GetLineStyle();
   Style_t icolline  =graph->GetLineColor();

   if (gname.Contains("syst_")) {
     option="e2";

     isyst++;

     TH1D *hedgelow =SPXGraphUtilities::GetEdgeHistogram(graph,true);
     TH1D *hedgehigh=SPXGraphUtilities::GetEdgeHistogram(graph,false);
     if (icolline<0) {
      hedgehigh->SetLineColor(-icolline);
      hedgelow ->SetLineColor(-icolline);
      graph->SetFillStyle(0);
      graph->SetFillColor(-icolline);
     }
     hedgehigh->SetLineStyle(istyline);
     hedgelow ->SetLineStyle(istyline);

     hedgehigh->SetFillStyle(0);
     hedgelow ->SetFillStyle(0);

     int linewidth=graph->GetLineWidth();
     hedgehigh->SetLineWidth(linewidth);
     hedgelow ->SetLineWidth(linewidth);

     //if (debug) {
     // SPXGraphUtilities::SPXPrintHistoProperties(hedgehigh);
     // SPXGraphUtilities::SPXPrintHistoProperties(hedgelow);
     //} 

     hedgehigh->Draw("][,same");
     hedgelow ->Draw("][,same");
   }

   if (debug) {
    double emax=SPXGraphUtilities::GetLargestRelativeError(graph);   
    std::cout<<cn<<mn<<"emax= "<<emax<<" Draw now gname= "<<gname.Data()<<" option= "<<option.c_str()
             <<" icolline= "<< icolline<<" icolfill= "<<icolfill
             <<" istyline= "<< istyline<<" istyfill= "<<istyfill
             <<std::endl;
    //SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)graph);
    //graph->Print();
   }

   if (icolline>0 && istyfill!=0) {
    graph->Draw(option.c_str());
   } else
    if (debug) std::cout<<cn<<mn<<"INFO Graph "<<graph->GetName()<<" not plotted istyfill= "<<istyfill<<" color= "<< icolline<<std::endl;
  }

  if (isyst<1) { 
  //
   if (debug) std::cout<<cn<<mn<<"Graph stat "<< ratioGraphstatonly.size()<<std::endl;    

   for (int igraphstat=0; igraphstat < ratioGraphstatonly.size(); igraphstat++) {
    TGraphAsymmErrors *graphstat = ratioGraphstatonly[igraphstat];
    if (!graphstat) std::cout<<"WARNING graphstat not found !"<<std::endl;
    TString gname=graphstat->GetName();

    if (debug) std::cout<<cn<<mn<<"Graph "<<graphstat->GetName()<<std::endl;

    // avoid plotting the vertical line for the x-error bars
    for (int ibin=0; ibin<graphstat->GetN(); ibin++) {
     graphstat->SetPointEXhigh(ibin,0.);
     graphstat->SetPointEXlow (ibin,0.);
    }
    option="||";
    if (debug) std::cout<<cn<<mn<<"Draw now gname= "<<gname.Data()<<" option= "<<option.c_str()<<std::endl;
    if (debug) graphstat->Print();
    graphstat->Draw(option.c_str());
   }
  } else if (debug) std::cout<<cn<<mn<<"statistical graphs not drawn, since isyst= "<<isyst<<std::endl;
  //
 } else {
  // Is a convolute 
  // look if detailed band is requested

  if(ratioStyle.IsDataOverConvolute()) {
   if (debug) std::cout<<cn<<mn<<" Is data over convolute number of ratio= "<< ratioGraph.size()<<std::endl;

   for (int igraph=0; igraph < ratioGraph.size(); igraph++) {
    TGraphAsymmErrors *graph = ratioGraph[igraph];
    if (!graph) std::cout<<"Graph not found !"<<std::endl;
    TString gname=graph->GetName();
    if (debug) std::cout<<cn<<mn<<"Draw now gname= "<<gname.Data()<<" option= "<<option.c_str()<<std::endl;
    if (debug) graph->Print();
   
    graph->Draw(option.c_str());
   }

   if (debug) {
    if (TString(option).Contains("Z")) std::cout<<cn<<mn<<"option does contain Z "    <<" option= "<<option.c_str()<<std::endl;
    else                               std::cout<<cn<<mn<<"option does not contain Z "<<" option= "<<option.c_str()<<std::endl;
    if (!TString(option).Contains("z"))std::cout<<cn<<mn<<"option does contain z "    <<" option= "<<option.c_str()<<std::endl;
    else                               std::cout<<cn<<mn<<"option does not contain z "<<" option= "<<option.c_str()<<std::endl;
   }

   if (!TString(option).Contains("Z") && !TString(option).Contains("z")) { // only plot if error ticks are ask for

    for (int igraphstat=0; igraphstat < ratioGraphstatonly.size(); igraphstat++) {
     TGraphAsymmErrors *graphstat = ratioGraphstatonly[igraphstat];
     if (!graphstat) std::cout<<"Graphstat not found !"<<std::endl;
     TString gname=graphstat->GetName();
     // avoid plotting the vertical line for the x-error bars
     for (int ibin=0; ibin<graphstat->GetN(); ibin++) {
      graphstat->SetPointEXhigh(ibin,0.);
      graphstat->SetPointEXlow (ibin,0.);
     }
     option="||";
     if (debug) std::cout<<cn<<mn<<"Draw now gname= "<<gname.Data()<<" option= "<<option.c_str()<<std::endl;
     if (debug) graphstat->Print();
     graphstat->Draw(option.c_str());
    }
   }
   return;
  }

  bool detailedband=false;
  for (int igraph1=0; igraph1 < ratioGraph.size(); igraph1++) {
   TGraphAsymmErrors *graph1 = ratioGraph[igraph1];
   TString gname=graph1->GetName();
   if (gname.Contains("_total_")) detailedband=true;
   if (gname.Contains("_scale_")) detailedband=true;
   if (gname.Contains("_pdf_"))   detailedband=true;
   if (gname.Contains("_alphas_"))detailedband=true;
   if (gname.Contains("_beamuncertainty_"))detailedband=true;
  }

  if (debug) if(!detailedband) std::cout<<cn<<mn<<"This is a simple band detailedband=FALSE "<<std::endl;
  if (debug) if (plotmarker)  std::cout<<cn<<mn<<"Asked to plot with markers "<<std::endl;
             else std::cout<<cn<<mn<<"plotmarker=FALSE "<<std::endl;

  if (!detailedband || plotmarker) {
   if (debug) if (plotmarker)  std::cout<<cn<<mn<<"Only plot total band"<<std::endl;
   for (int igraph1=0; igraph1 < ratioGraph.size(); igraph1++) {
    TGraphAsymmErrors *graph1 = ratioGraph[igraph1];
    TString gname=graph1->GetName();
    if (gname.Contains("_total_")) {
     if (debug) std::cout<<cn<<mn<<"Draw graph "<<graph1->GetName()<<std::endl;
     graph1->Draw(option.c_str());
     return;
    }
   }
  }

  if (debug) std::cout<<cn<<mn<<"Now order bands and plots"<<std::endl;

  // order bands for better plotting
  std::map<int, TGraphAsymmErrors * > bands=  SPXUtilities::OrderBandMap(ratioGraph);

  if (debug) std::cout<<cn<<mn<<"ordering done"<<std::endl;

  SPXPlotConfigurationInstance pci;

  if(ratioStyle.IsDataOverConvolute()) {
   if (debug) std::cout<<cn<<mn<<"Get plot configuration: "<<numeratorDataFile.c_str()<<std::endl;
   //pci=plotConfiguration.GetPlotConfigurationInstance(denominatorConvolutePDFFile);
   //pci=plotConfiguration.GetPlotConfigurationInstance(numeratorDataFile);
   throw SPXGraphException(cn+mn+"You should not come here, can not plot detailed band for data in numerator");
  } else {
   if (debug) std::cout<<cn<<mn<<"Get plot configuration: "<<numeratorConvolutePDFFile.c_str()<<std::endl;
   pci=plotConfiguration.GetPlotConfigurationInstance(numeratorConvolutePDFFile);
  }
  
  //if (!pci) std::cout<<cn<<mn<<"WARNING: pci not found "<<std::endl;

  // now plot graph: largest first
  //std::cout<<cn<<mn<<" \n iterate over map " <<std::endl;
  for(std::map<int, TGraphAsymmErrors *>::reverse_iterator it=bands.rbegin(); it!=bands.rend(); ++it) {
   std::cout<<cn<<mn<<it->first<<" "<<it->second->GetName()<<std::endl;

   TGraphAsymmErrors *graph = it->second;
   const int DEFAULT=-99;
   int fillcolor=DEFAULT, edgecolor=DEFAULT, edgestyle=DEFAULT, fillstyle=DEFAULT, markerstyle=DEFAULT;
   TString gname=graph->GetName();
   if (gname.Contains("_total_")) { 
    fillcolor  =pci.totalFillColor;
    edgecolor  =pci.totalEdgeColor;
    edgestyle  =pci.totalEdgeStyle;
    fillstyle  =pci.totalFillStyle;
    markerstyle=pci.totalMarkerStyle;
   }
   if (gname.Contains("_scale_")) { 
    fillcolor  =pci.scaleFillColor;
    edgecolor  =pci.scaleEdgeColor;
    edgestyle  =pci.scaleEdgeStyle;
    fillstyle  =pci.scaleFillStyle;
    markerstyle=pci.scaleMarkerStyle;
   }
   if (gname.Contains("_alphas_")) { 
    fillcolor  =pci.alphasFillColor;
    edgecolor  =pci.alphasEdgeColor;
    edgestyle  =pci.alphasEdgeStyle;
    fillstyle  =pci.alphasFillStyle;
    markerstyle=pci.alphasMarkerStyle;
   }
   if (gname.Contains("_pdf_")) { 
    fillcolor  =pci.pdfFillColor;
    edgecolor  =pci.pdfEdgeColor;
    fillstyle  =pci.pdfFillStyle;
    edgestyle  =pci.pdfEdgeStyle;
    markerstyle=pci.pdfMarkerStyle;
   }
   if (gname.Contains("_BeamUncertainty_")) { 
    fillcolor  =pci.beamuncertaintyFillColor;
    edgecolor  =pci.beamuncertaintyEdgeColor;
    fillstyle  =pci.beamuncertaintyFillStyle;
    edgestyle  =pci.beamuncertaintyEdgeStyle;
    markerstyle=pci.beamuncertaintyMarkerStyle;
   }
   if (gname.Contains("_corrections_")) { 
    fillcolor  =pci.correctionsFillColor;
    edgecolor  =pci.correctionsEdgeColor;
    fillstyle  =pci.correctionsFillStyle;
    edgestyle  =pci.correctionsEdgeStyle;
    markerstyle=pci.correctionsMarkerStyle;
   }

   if (fillcolor!=DEFAULT) {
    graph->SetFillColor(fillcolor);
   }
   if (fillstyle!=DEFAULT) {
    graph->SetFillStyle(fillstyle);
   }
   if (markerstyle!=DEFAULT) {
    graph->SetMarkerStyle(markerstyle);
   }

   if (edgecolor!=DEFAULT && edgecolor!=0) {
    TH1D *hedgelow =SPXGraphUtilities::GetEdgeHistogram(graph,true);
    TH1D *hedgehigh=SPXGraphUtilities::GetEdgeHistogram(graph,false);
    hedgelow ->SetLineColor(abs(edgecolor));
    hedgehigh->SetLineColor(abs(edgecolor));
    if (edgestyle!=DEFAULT) {
     hedgelow ->SetLineStyle(edgestyle);
     hedgehigh->SetLineStyle(edgestyle);

     //int linewidth=graph->GetLineWidth();
     //hedgehigh->SetLineWidth(linewidth);
     //hedgelow ->SetLineWidth(linewidth);
     hedgelow ->SetLineWidth(4);
     hedgehigh->SetLineWidth(4);
    }

    hedgelow ->SetFillStyle(0);
    hedgehigh->SetFillStyle(0);

    if (debug) std::cout<<cn<<mn<<"Call hedgelow->Draw() "<<hedgelow->GetName()<<"with option "<< option.c_str()  <<std::endl;
    hedgelow ->Draw("][,same");
    hedgehigh->Draw("][,same");
   }

   //graph->SetLineColor(edgecolor);
   if (edgecolor>=0) {
    if (debug) std::cout<<cn<<mn<<"Call graph->Draw "<<graph->GetName()<<"with option "<< option.c_str()  <<std::endl;
    graph->Draw(option.c_str());
   }

   if (debug) {
    std::cout<<cn<<mn<<"fillcolor= "  << fillcolor   <<std::endl;
    std::cout<<cn<<mn<<"fillstyle= "  << fillstyle   <<std::endl;
    std::cout<<cn<<mn<<"edgecolor= "  << edgecolor   <<std::endl;
    std::cout<<cn<<mn<<"edgestyle= "  << edgestyle   <<std::endl;
    std::cout<<cn<<mn<<"markerstyle= "<< markerstyle <<std::endl;

    std::cout << cn << mn << "Printing ratio name= "<<graph->GetName() << std::endl;

    graph->Print();
    std::cout << std::endl;
   }
  }
 }
}

void SPXRatio::PrintDataFileGraphMapKeys(std::ostream &out ) {
 out << "SPXRatio::PrintDataFileGraphMapKeys: " << std::endl;
 out << "\t Key Format: [Data]" << std::endl;
 out << "\t ============ KNOWN KEYS ============" << std::endl;
 for(StringGraphMap_T::iterator it = dataFileGraphMap->begin(); it != dataFileGraphMap->end(); ++it) {
  out << "\t [" << it->first << "]" << std::endl;
 }
 out << "\t ====================================" << std::endl << std::endl;
 return;
}

void SPXRatio::PrintDataFileLabelMapKeys(std::ostream &out ) {
 out << "SPXRatio::PrintDataFileLabelMapKeys: " << std::endl;
 out << "\t Key Format: [Data]" << std::endl;
 out << "\t ============ KNOWN KEYS ============" << std::endl;
 for(StringLabelMap_T::iterator it = dataFileLabelMap->begin(); it != dataFileLabelMap->end(); ++it) {
  out << "\t [" << it->first << "]" << std::endl;
 }
 out << "\t ====================================" << std::endl << std::endl;
 return;
}

void SPXRatio::PrintReferenceFileGraphMapKeys(std::ostream &out) {
 out << "SPXRatio::PrintReferenceFileGraphMapKeys: " << std::endl;
 out << "\t Key Format: [Grid, PDF]" << std::endl;
 out << "\t ============ KNOWN KEYS ============" << std::endl;
 for(StringPairGraphMap_T::iterator it = convoluteFileGraphMap->begin(); it != convoluteFileGraphMap->end(); ++it) {
  out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
 }
 out << "\t ====================================" << std::endl << std::endl;
 return;
}

void SPXRatio::PrintNominalFileGraphMapKeys(std::ostream &out) {
 out << "SPXRatio::PrintNominalFileGraphMapKeys: " << std::endl;
 out << "\t Key Format: [Grid, PDF]" << std::endl;
 out << "\t ============ KNOWN KEYS ============" << std::endl;
 for(StringPairGraphMap_T::iterator it = convoluteFileGraphMap->begin(); it != convoluteFileGraphMap->end(); ++it) {
  out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
 }
 out << "\t ====================================" << std::endl << std::endl;
 return;
}

void SPXRatio::PrintConvoluteFilePDFMapKeys(std::ostream &out) {
 out << "SPXRatio::PrintConvoluteFilePDFMapKeys: " << std::endl;
 out << "\t Key Format: [Grid, PDF]" << std::endl;
 out << "\t ============ KNOWN KEYS ============" << std::endl;
 for(StringPairPDFMap_T::iterator it = convoluteFilePDFMap->begin(); it != convoluteFilePDFMap->end(); ++it) {
  out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
 }
 out << "\t ====================================" << std::endl << std::endl;
 return;
}

void SPXRatio::PrintConvoluteFileGraphMapKeys(std::ostream &out) {
 out << "SPXRatio::PrintConvoluteFileGraphMapKeys: " << std::endl;
 out << "\t Key Format: [Grid, PDF]" << std::endl;
 out << "\t ============ KNOWN KEYS ============" << std::endl;
 for(StringPairGraphMap_T::iterator it = convoluteFileGraphMap->begin(); it != convoluteFileGraphMap->end(); ++it) {
  out << "\t [" << it->first.first << ", " << it->first.second << "]" << std::endl;
 }
 out << "\t ====================================" << std::endl << std::endl;
}

bool SPXRatio::DataFileGraphMapHasSystematics(){
 //if (debug) std::cout << "SPXRatio::DataFileGraphMapHasSystematics: " << std::endl;

 for(StringGraphMap_T::iterator it = dataFileGraphMap->begin(); it != dataFileGraphMap->end(); ++it) {
  //if (debug) std::cout << "\t [" << it->first << "]" << std::endl;
  TString testkey= it->first;
  if (testkey.Contains("syst_")) {
   return true;
  }
 }
 return false;
};
