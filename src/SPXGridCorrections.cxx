//************************************************************/
//
//	Grid Corrections Implementation
//
//	Implements the SPXGridCorrections class, which stores the
//  corrections for each grid in a String-Double Vector map
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 unsigned int numberOfCorrectionFiles = pci.gridSteeringFile.GetNumberOfCorrectionFiles();
 numberOfBins = 0;

 //Total correction scales
 std::vector<double> t_x;
 std::vector<double> t_xmin;
 std::vector<double> t_xmax;
 std::vector<double> tot_y;
 std::vector<double> tot_eyl;
 std::vector<double> tot_eyh;

 if (debug) std::cout<<cn<<mn<<"Number of correction files: " << numberOfCorrectionFiles  << std::endl;
 if (debug) std::cout<<cn<<mn<<"Grid directory= "<<pci.gridDirectory<<std::endl;

 for (int i = 0; i < numberOfCorrectionFiles; i++) {
  std::string filename = pci.gridSteeringFile.GetCorrectionFile(i);
  filename=pci.gridDirectory+'/'+filename;

  if (debug) std::cout << std::endl;
  if (debug) std::cout << cn << mn << "Beginning to parse correction file: " << filename << std::endl;

  //Open the file
  OpenCorrectionFile(filename);
  if (!(*correctionFile)) {
   throw SPXFileIOException("Something went wrong with the correctionFile ifstream...");
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

  name="";
  comment="";
  errortype="relative";
  xbinformat="normal";

   try {
    //Process the file
    while (correctionFile->good()) {
     std::getline(*correctionFile, line);

     if (line.empty() || (line[0] == ';')) {
      continue;
     } 
     if (isalpha((int)SPXStringUtilities::LeftTrim(line).at(0))) {
      std::vector<std::string> vtmp=SPXStringUtilities::ParseString(line,'=');
      if (vtmp.size()!=2) {
       std::cout<<cn<<mn<<"Vector should have exactly size= 2 but is "<<vtmp.size()<<" line= "<<line<<std::endl;
      }

      if (TString(vtmp.at(0)).Contains("name")) { 
       name    =vtmp.at(1);
       if (debug) std::cout<<cn<<mn<<"name= "<<name.c_str()<<std::endl;
      }

      if (TString(vtmp.at(0)).Contains("comment")) { 
       comment  =vtmp.at(1);
       if (debug) std::cout<<cn<<mn<<"comment= "<<comment.c_str()<<std::endl;
      }

      if (TString(vtmp.at(0)).Contains("errortype")) {
       errortype=vtmp.at(1);
       if (debug) std::cout<<cn<<mn<<"errortype= "<<errortype.c_str()<<std::endl;
      }

      if (TString(vtmp.at(0)).Contains("xbinformat")) {
       xbinformat=vtmp.at(1);
       if (debug) std::cout<<cn<<mn<<"xbinformat= "<<xbinformat.c_str()<<std::endl;
      }

     }

     //Read in line if it starts with a digit
     if (isdigit((int)SPXStringUtilities::LeftTrim(line).at(0))) {

      if (debug) std::cout << cn << mn << "Line: " << line << std::endl;

      // Parse line into vector
      // Convert all tabs to spaces
      std::string formatted_line = SPXStringUtilities::ReplaceAll(line, "\t", " ");
      std::vector<double> tmp = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

      if (tmp.size() < 3) {
       throw SPXParseException(cn + mn + "There must be at least 3 correction columns");
      }

      // Set number of columns to the size of the 0th bin
      if (!bin_count) {
       //Set number of columns
       numberOfColumns = tmp.size();

       int ncol=0;
       if (xbinformat=="xminxmaxonly") ncol=2;       // only xmin and xmax is given
       else                            ncol=3;       // central value, xmin and xmax are given
       if (errortype=="noerror")       ncol=ncol+1;  // only central value no error
       else ncol=ncol+3;                             // central value and errors



       // Only 2 options: 4 columns or 6 columns (without eyl/eyh or with)
       //if ((numberOfColumns != 4) && (numberOfColumns != 6)) {
       if (numberOfColumns != ncol) {
        std::ostringstream oss;
        oss<<cn<<mn<<"There should be "<<ncol<<" columns xbinformat="<<xbinformat<<" errortype="<<errortype;
        throw SPXParseException(oss.str());
       }

       if (debug) std::cout << cn << mn << "The remaining bins MUST also have exactly " << numberOfColumns << " columns" << std::endl;
      }

      //After the 0th bin, make sure all other bins have the exact same number of columns
      else {
       if (tmp.size() != numberOfColumns) {
        std::ostringstream oss;
        oss << cn << mn << "Number of columns for bin " << bin_count + 1<< " (" << tmp.size() << ") does NOT match expected (" << numberOfColumns << ")" << std::endl;
        throw SPXParseException(oss.str());

       }
      }

      // Increment bin count
      bin_count++;

     //Obtain the correction type (SPXGridCorrectionType???)
     //Build the matrix based on the xm, dx-, dx+, sigma, dsigma-, dsigma+
     int ioff=0;
     if (xbinformat=="xminxmaxonly") {
      ioff=-1;
      double xm=(tmp[ioff+1]+tmp[ioff+2])/2.;
      x.push_back(xm);
     } else {
      x.push_back(tmp[0]);
     }
     xmin.push_back(tmp[ioff+1]);
     xmax.push_back(tmp[ioff+2]);

     //if (debug) std::cout<<cn<<mn<<"ioff= "<<ioff<<std::endl;

     //std::cout<<cn<<mn<<" Number of bins= "<<numberOfBins<<std::endl;

     if (numberOfColumns == ioff+4) { // no uncertainties given
      y.push_back(tmp[ioff+3]);
      eyl.push_back(0.);
      eyh.push_back(0.);
     } else if(numberOfColumns == ioff+6) { // uncertainties are given
      y.push_back(tmp[ioff+3]);
      if (errortype.compare("relative")==0) {

       eyh.push_back(tmp[ioff+3]-tmp[ioff+4]*tmp[ioff+3]);
       eyl.push_back(tmp[ioff+5]*tmp[ioff+3]-tmp[ioff+3]);
      } else if (errortype.compare("absolute")==0) {
       eyh.push_back(tmp[ioff+4]);
       eyl.push_back(tmp[ioff+5]);
      } else {
       std::cout<<cn<<mn<<"Do not know what to do errortype= "<<errortype.c_str()<<std::endl;
       throw SPXParseException(cn+mn+"Do not know what to do errortype= "+errortype);
      }
      if (debug) {
	std::cout<<cn<<mn<<" y= "<<y.back()<<" eyl= "<<eyl.back()<<" eyh= "<<eyh.back()<<std::endl;
      }
     } else {
      std::ostringstream oss;
      oss<<cn<<mn<<"Do not know what to do numberOfColumns="<<numberOfColumns;
      throw SPXParseException(oss.str());
     }
    }
   }
  } catch(const SPXException &e) {
   std::cerr << e.what() << std::endl;
   throw SPXParseException(cn + mn + "Unable to parse corrections file: " + filename);
  }

  if (debug) {
    std::cout<<cn<<mn<<"Number of bins= "<<numberOfBins<<" xsize= "<< x.size()<<std::endl;
  }

  // Set number of bins to match the first corrections vector's xm size
  if (!numberOfBins) {
   numberOfBins = x.size();
   if (debug) std::cout << cn << mn << "Setting the number of bins to first correction's x size: " << numberOfBins << std::endl;

   // Fill total vectors with '1.0'
   for (int j = 0; j < numberOfBins; j++) {
    tot_y.push_back(1.0);
    tot_eyl.push_back(0.0);
    tot_eyh.push_back(0.0);
   }
  }

  // Check vector sizes

  if (debug) {
    std::cout<<cn<<mn<<"Number of bins= "<<numberOfBins<<" xsize= "<< x.size()<<" check if corrections files have different sizes "<<std::endl;

  }

  try {
   CheckVectorSize(x,   "x",    numberOfBins);
   CheckVectorSize(xmin,"xmin", numberOfBins);
   CheckVectorSize(xmax,"xmax", numberOfBins);
   CheckVectorSize(y,   "y",    numberOfBins);
   CheckVectorSize(eyl, "eyl",  numberOfBins);
   CheckVectorSize(eyh, "eyh",  numberOfBins);
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
  if (i == 0) {
   t_x = x;
   t_xmin = xmin;
   t_xmax = xmax;
  }

  // Calculate total uncertainty
  for (int j = 0; j < numberOfBins; j++) {
   tot_y[j] *= y[j];
   // relative error add up in quadrature
   tot_eyl[j] += eyl[j]*eyl[j]/(y[j]*y[j]);
   tot_eyh[j] += eyh[j]*eyh[j]/(y[j]*y[j]);

   if (debug) std::cout<<cn<<mn<<" i= "<<i<<" j= "<<j
                       << " y= "<<y[j]<<" eyl= "<<eyl[j] <<" eyh= "<<eyh[j] 
                       << " tot_y= "<<tot_y[j]<<" tot_eyl= "<<tot_eyl[j] <<" tot_eyh= "<<tot_eyh[j]<< std::endl;
  }

  if (debug) std::cout << cn << mn <<" i= "<<i<< " ---> Successfully added total correction to correction map" << std::endl;

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

 if (debug) {
  std::cout << cn << mn << "Successfully added all corrections to map" << std::endl;

  //std::cout<<cn<<mn<<"name= "<<name.c_str() << std::endl;
  //std::cout<<cn<<mn<<"comment= "<<comment.c_str() << std::endl;
  //std::cout<<cn<<mn<<"errortype= "<<errortype.c_str() << std::endl;

  //this->Print();
  //PrintMap(test,totalCorrections);
  //string test="testing map: ";
  //PrintMap(test,m);
  //std::cout<<cn<<mn<<"finished " << std::endl;
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
 std::string mn = "GetCorrectionGraph: ";

 TGraphAsymmErrors* gcorr= new TGraphAsymmErrors();
 if (!gcorr) throw SPXGraphException(cn + mn + "ERROR creating graph ");

 gcorr->SetName(TString("corrections_"+filename));

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
  std::cout<<cn<<mn<<"numberOfBins= "<<numberOfBins<<std::endl;
 }

 for (int i=0; i<numberOfBins; i++) {

  double c_x, c_xmin, c_xmax, c_y, c_eyl, c_eyh;

  if (corrections[filename].count("x")==0)
   std::cout<<cn<<mn<<"WARNING: element x not found in map "<<std::endl;
  else
   c_x   = corrections[filename]["x"][i];

  if (corrections[filename].count("xmin")==0)
   std::cout<<cn<<mn<<"WARNING: element xmin not found in map "<<std::endl;
  else
   c_xmin= corrections[filename]["xmin"][i];

  if (corrections[filename].count("xmax")==0)
   std::cout<<cn<<mn<<"WARNING: element xmax not found in map "<<std::endl;
  else
   c_xmax= corrections[filename]["xmax"][i];

  if (corrections[filename].count("y")==0)
   std::cout<<cn<<mn<<"WARNING: element y not found in map "<<std::endl;
  else
   c_y   = corrections[filename]["y"][i];

  if (corrections[filename].count("eyl")==0)
   std::cout<<cn<<mn<<"WARNING: element eyl not found in map "<<std::endl;
  else
   c_eyl = corrections[filename]["eyl"][i];

  if (corrections[filename].count("eyh")==0)
   std::cout<<cn<<mn<<"WARNING: element eyh not found in map "<<std::endl;
  else
   c_eyh = corrections[filename]["eyh"][i];

  // Note, text file contain relative uncertainty
  gcorr->SetPoint(i, c_x,c_y);

  double eyl=0., eyh=0.;
  double exl=c_x-c_xmin;
  double exh=c_xmax-c_x;

  gcorr->SetPointError(i,exl,exh,c_eyl,c_eyh);

 }

 if (debug) {
  std::cout<<cn<<mn<<"Print graph gcorr= "<<gcorr->GetName()<<std::endl;
  gcorr->Print("all");
 }

 return gcorr;
}
