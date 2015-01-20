//************************************************************/
//
//	Grid Corrections Implementation
//
//	Implements the SPXGridCorrections class, which stores the
//  corrections for each grid in a String-Double Vector map
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		26.11.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXGridCorrections.h"

//Class name for debug statements
const std::string cn = "SPXGridCorrections::";

//Must define the static debug variable in the implementation
bool SPXGridCorrections::debug;

void SPXGridCorrections::Parse(void) {
    std::string mn = "Parse: ";
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

    unsigned int numberOfCorrectionFiles = pci.gridSteeringFile.GetNumberOfCorrectionFiles();
    numberOfBins = 0;

    //Total correction scales
    std::vector<double> t_x;
    std::vector<double> t_xmin;
    std::vector<double> t_xmax;
    std::vector<double> tot_y;
    std::vector<double> tot_eyl;
    std::vector<double> tot_eyh;

    if(debug) std::cout << cn << mn << "Number of correction files: " << numberOfCorrectionFiles  << std::endl;
    if(debug) std::cout << cn << mn << "Grid directory= "<<pci.gridDirectory<<std::endl;

    for(int i = 0; i < numberOfCorrectionFiles; i++) {
        std::string filename = pci.gridSteeringFile.GetCorrectionFile(i);
      
        filename=pci.gridDirectory+'/'+filename;

        if(debug) std::cout << std::endl;
        if(debug) std::cout << cn << mn << "Beginning to parse correction file: " << filename << std::endl;

        //Open the file
        OpenCorrectionFile(filename);
        if(!(*correctionFile)) {
            throw SPXFileIOException("Something went awry with the correctionFile ifstream...");
        }

        std::string line;
        std::vector<double> x;
        std::vector<double> xmin;
        std::vector<double> xmax;
        std::vector<double> y;
        std::vector<double> eyl;
        std::vector<double> eyh;
        unsigned int bin_count = 0;
        unsigned int numberOfColumns = 0;

        try {
            //Process the file
            while(correctionFile->good()) {
                std::getline(*correctionFile, line);

                //Skip empty lines
                if(line.empty()) {
                    continue;
                }

                //Read in line if it starts with a digit
                if(isdigit((int)SPXStringUtilities::LeftTrim(line).at(0))) {

                    if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

                    //Parse line into vector
                    //Convert all tabs to spaces
                    std::string formatted_line = SPXStringUtilities::ReplaceAll(line, "\t", " ");
                    std::vector<double> tmp = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

                    if(tmp.size() < 4) {
                        throw SPXParseException(cn + mn + "There must be at least 4 correction columns (xm, xlow, xhigh, sigma corr)");
                    }

                    //Set number of columns to the size of the 0th bin
                    if(!bin_count) {
                        //Set number of columns
                        numberOfColumns = tmp.size();

                        //Only 2 options: 4 columns or 6 columns (without eyl/eyh or with)
                        if((numberOfColumns != 4) && (numberOfColumns != 6)) {
                            throw SPXParseException(cn + mn + "There can only be either 4 or 6 columns in correction file");
                        }

                        if (debug) std::cout << cn << mn << "The remaining bins MUST also have exactly " << numberOfColumns << " columns" << std::endl;
                    }

                    //After the 0th bin, make sure all other bins have the exact same number of columns
                    else {
                        if(tmp.size() != numberOfColumns) {
                            std::ostringstream oss;
                            oss << cn << mn << "Number of columns for bin " << bin_count + 1<< " (" << tmp.size() << \
                            ") does NOT match expected (" << numberOfColumns << ")" << std::endl;
                            throw SPXParseException(oss.str());

                        }
                    }

                    //Increment bin count
                    bin_count++;

                    //Obtain the correction type (SPXGridCorrectionType???)
                    //Build the matrix based on the xm, dx-, dx+, sigma, dsigma-, dsigma+
                    x.push_back(tmp[0]);
                    xmin.push_back(tmp[1]);
                    xmax.push_back(tmp[2]);

                    if(numberOfColumns == 4) {
                        y.push_back(tmp[3]);
                        eyl.push_back(tmp[3]);
                        eyh.push_back(tmp[3]);
                    } else if(numberOfColumns == 6) {
                        y.push_back(tmp[3]);
                        eyl.push_back(tmp[4]);
                        eyh.push_back(tmp[5]);
                    }
                }
            }
        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;
            throw SPXParseException(cn + mn + "Unable to parse corrections file: " + filename);
        }

        //Set number of bins to match the first corrections vector's xm size
        if(!numberOfBins) {
            numberOfBins = x.size();
            if(debug) std::cout << cn << mn << "Setting the number of bins to first correction's x size: " << numberOfBins << std::endl;

            //Fill total vectors with '1.0'
            for(int j = 0; j < numberOfBins; j++) {
                tot_y.push_back(1.0);
                tot_eyl.push_back(0.0);
                tot_eyh.push_back(0.0);
            }
        }

        //Check vector sizes
        try {
            CheckVectorSize(x, "x", numberOfBins);
            CheckVectorSize(xmin, "xmin", numberOfBins);
            CheckVectorSize(xmax, "xmax", numberOfBins);
            CheckVectorSize(y, "y", numberOfBins);
            CheckVectorSize(eyl, "eyl", numberOfBins);
            CheckVectorSize(eyh, "eyh", numberOfBins);
        } catch(const SPXException &e) {
            std::cerr << e.what() << std::endl;

            throw SPXParseException(cn + mn + "Unable to parse corrections file: " + filename);
        }

        //Create the double vector map based on the 6 vectors
        StringDoubleVectorMap_T m;
        m.insert(StringDoubleVectorPair_T("x", x));
        m.insert(StringDoubleVectorPair_T("xmin", xmin));
        m.insert(StringDoubleVectorPair_T("xmax", xmax));
        m.insert(StringDoubleVectorPair_T("y", y));
        m.insert(StringDoubleVectorPair_T("eyl", eyl));
        m.insert(StringDoubleVectorPair_T("eyh", eyh));

        //Insert it into the map
        corrections.insert(CorrectionsPair_T(filename, m));

        //Set the total corrections x, xmin, xmax vectors based on the first file
        // 
        if(i == 0) {
            t_x = x;
            t_xmin = xmin;
            t_xmax = xmax;
        }

        //Multiply total vectors by scale to maintain total scaling
        for(int j = 0; j < numberOfBins; j++) {
            tot_y[j] *= y[j];
            tot_eyl[j] += eyl[j]*eyl[j]/(y[j]*y[j]);
            tot_eyh[j] += eyh[j]*eyh[j]/(y[j]*y[j]);
        }

        if(debug) std::cout << cn << mn <<" i= "<<i<< " ---> Successfully added correction to map" << std::endl;

        //Close the file
        CloseCorrectionFile();
    }

    // add up relative errors in quadrature
    for (int j=0; j<tot_y.size(); j++) {
     if (tot_y[j]!=0.) {
      tot_eyl[j] =sqrt(tot_eyl[j])*tot_y[j];
      tot_eyh[j] =sqrt(tot_eyh[j])*tot_y[j];
     } else {
      tot_eyl[j] =0.;
      tot_eyh[j] =0.;
     }
    }

    //Insert the x, xmin, xmax, and total vectors into the totalCorrections map
    totalCorrections.insert(StringDoubleVectorPair_T("x", t_x));
    totalCorrections.insert(StringDoubleVectorPair_T("xmin", t_xmin));
    totalCorrections.insert(StringDoubleVectorPair_T("xmax", t_xmax));
    totalCorrections.insert(StringDoubleVectorPair_T("y", tot_y));
    totalCorrections.insert(StringDoubleVectorPair_T("eyl", tot_eyl));
    totalCorrections.insert(StringDoubleVectorPair_T("eyh", tot_eyh));

    if(debug) {
     std::cout << cn << mn << "Successfully added all corrections to map" << std::endl;
     //std::cout << cn << mn << "Print map" << std::endl;
     //string test="testing map: ";
     //PrintMap(test,totalCorrections);
    }
    
}

//@TODO Move this to some sort of utilities library? SPXStandardUtilities/SPXGeneralUtilities???
void SPXGridCorrections::PrintMap(std::string &s, StringDoubleVectorMap_T &m) {
    std::string mn = "PrintMap: ";
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

    if (debug) std::cout<<cn<<mn<<"numberOfBins= "<< numberOfBins <<" Mapsize= "<< m.size() <<std::endl;

    std::cout<<cn<<mn<<"Printing map: " << s << std::endl;

    if (m.size()==0)
     throw SPXParseException(cn + mn + "Map is empty ");

    std::cout << "===============================================================================" << std::endl;
    std::cout << "|   x        |    xmin     |    xmax     |     y      |    eyl     |    eyh     |" << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;

    //Iterate over map
    for(int i = 0; i < numberOfBins; i++) {
        //Show 4 decimal places
        std::cout << std::fixed;
        std::cout.precision(4);

        //if (m["x"].count())
 	// std:cout<<"Something worng with map m[x] not found !"<<endl;
 
        std::cout << "| ";
        std::cout.width(10);
        if (m.count("x")>0)
         std::cout << m["x"].at(i);
        std::cout << " | ";
        if (m.count("xmin")>0)
         std::cout.width(10); std::cout << m["xmin"].at(i);
        std::cout << " | ";
        if (m.count("xmax")>0)
         std::cout.width(10); std::cout << m["xmax"].at(i);
        std::cout << " | ";
        if (m.count("y")>0)
        std::cout.width(10); std::cout << m["y"].at(i);
        std::cout << " | ";
        if (m.count("eyl")>0)
        std::cout.width(10); std::cout << m["eyl"].at(i);
        std::cout << " | ";
        if (m.count("eyh")>0)
        std::cout.width(10); std::cout << m["eyh"].at(i);
        std::cout << " |" << std::endl;
    }

    std::cout << "===============================================================================" << std::endl;
    std::cout << std::endl;
}

//Print all individual corrections plus the total corrections
void SPXGridCorrections::Print(void) {
    std::string mn = "Print: ";
    if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

    unsigned int numberOfCorrectionFiles = pci.gridSteeringFile.GetNumberOfCorrectionFiles();

    for(int i = 0; i < numberOfCorrectionFiles; i++) {
        std::string filename = pci.gridSteeringFile.GetCorrectionFile(i);
        std::string corrLabel = pci.gridSteeringFile.GetCorrectionFileLabel(i);
        if (debug) std::cout<<cn<<mn<<"Correction file label= "<<corrLabel.c_str()<<std::endl;   
     
        std::string griddir = pci.gridDirectory;
        filename=griddir+'/'+filename;

        if (debug) std::cout<<cn<<mn<<"print Correction file "<<filename.c_str()<<std::endl;   
        PrintMap(filename, corrections[filename]);
    }

    std::cout << "===============================================================================" << std::endl;
    std::cout << "===============================================================================" << std::endl;
    std::cout << std::endl;

    //std::string s = "Total Corrections";
    //PrintMap(s, totalCorrections);
}


TGraphAsymmErrors * SPXGridCorrections::GetCorrectionGraph(std::string &filename){
 std::string mn = "GetCorrectionGraph ";

 TGraphAsymmErrors* gcorr= new TGraphAsymmErrors();
 if (!gcorr) throw SPXGraphException(cn + mn + "ERROR creating graph ");
 gcorr->SetName(TString(filename));

 if (corrections.empty())
  throw SPXGraphException(cn + mn + "ERROR correction map is empty ! ");
 else
  if(debug) std::cout<<cn<<mn<<"correction map found "<<filename.c_str()<<std::endl;

 if (corrections.count(filename)==0) {
  throw SPXGraphException(cn + mn + "ERROR correction map not found ");
 } else {
  if (debug) {
   std::cout<<cn<<mn<<"Print corrections "<<filename.c_str()<<std::endl;
   PrintMap(filename,corrections[filename]);
  }
 }

 if (debug) {
  std::cout<<cn<<mn<<" numberOfBins= "<<numberOfBins<<std::endl;
 }

 for (int i=0; i<numberOfBins; i++) {

  double c_x, c_xmin, c_xmax, c_y, c_eyl, c_eyh;

  if (corrections[filename].count("x")==0)
   std::cout<<cn<<mn<<"WARNING element x not found in map "<<std::endl;
  else
   c_x   = corrections[filename]["x"][i];

  if (corrections[filename].count("xmin")==0)
   std::cout<<cn<<mn<<"WARNING element xmin not found in map "<<std::endl;
  else
   c_xmin= corrections[filename]["xmin"][i];

  if (corrections[filename].count("xmax")==0)
   std::cout<<cn<<mn<<"WARNING element xmax not found in map "<<std::endl;
  else
   c_xmax= corrections[filename]["xmax"][i];

  if (corrections[filename].count("y")==0)
   std::cout<<cn<<mn<<"WARNING element y not found in map "<<std::endl;
  else
   c_y   = corrections[filename]["y"][i];

  if (corrections[filename].count("eyl")==0)
   std::cout<<cn<<mn<<"WARNING element eyl not found in map "<<std::endl;
  else
   c_eyl = corrections[filename]["eyl"][i];

  if (corrections[filename].count("eyh")==0)
   std::cout<<cn<<mn<<"WARNING element eyh not found in map "<<std::endl;
  else
   c_eyh = corrections[filename]["eyh"][i];

  // Note, text file contain relative uncertainty
  gcorr->SetPoint(i, c_x,c_y);
  gcorr->SetPointError(i,c_x-c_xmin,c_xmax-c_x,c_eyl*c_y,c_eyh*c_y);
 }

 if (debug) {
  std::cout<<cn<<mn<<" print graph"<<std::endl;
  gcorr->Print("all");
 }

 return gcorr;
}
