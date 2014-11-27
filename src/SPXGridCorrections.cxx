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

    unsigned int numberOfCorrectionFiles = pci.gridSteeringFile.GetNumberOfCorrectionFiles();
    numberOfBins = 0;

    //Total correction scales
    std::vector<double> t_x;
    std::vector<double> t_exl;
    std::vector<double> t_exh;
    std::vector<double> tot_y;
    std::vector<double> tot_eyl;
    std::vector<double> tot_eyh;

    for(int i = 0; i < numberOfCorrectionFiles; i++) {
        std::string filename = pci.gridSteeringFile.GetCorrectionFile(i);

        if(debug) std::cout << cn << mn << "Beginning to parse correction file: " << filename << std::endl;

        //Open the file
        OpenCorrectionFile(filename);
        if(!(*correctionFile)) {
            throw SPXFileIOException("Something went awry with the correctionFile ifstream...");
        }

        std::string line;
        std::vector<double> x;
        std::vector<double> exl;
        std::vector<double> exh;
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

                    if(debug) std::cout << "LINE: " << line << std::endl;

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

                        std::cout << cn << mn << "WARNING: The remaining bins MUST also have exactly " << numberOfColumns << " columns" << std::endl;
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
                    exl.push_back(tmp[1]);
                    exh.push_back(tmp[2]);

                    if(numberOfColumns == 5) {
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
                tot_eyl.push_back(1.0);
                tot_eyh.push_back(1.0);
            }
        }

        //Check vector sizes
        try {
            CheckVectorSize(x, "x", numberOfBins);
            CheckVectorSize(exl, "exl", numberOfBins);
            CheckVectorSize(exh, "exh", numberOfBins);
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
        m.insert(StringDoubleVectorPair_T("exl", exl));
        m.insert(StringDoubleVectorPair_T("exh", exh));
        m.insert(StringDoubleVectorPair_T("y", y));
        m.insert(StringDoubleVectorPair_T("eyl", eyl));
        m.insert(StringDoubleVectorPair_T("eyh", eyh));

        //Insert it into the map
        corrections.insert(CorrectionsPair_T(filename, m));

        //Set the total corrections x, exl, exh vectors based on the first file
        if(i == 0) {
            t_x = x;
            t_exl = exl;
            t_exh = exh;
        }

        //Multiply total vectors by scale to maintain total scaling
        for(int j = 0; j < numberOfBins; j++) {
            tot_y[j] *= y[j];
            tot_eyl[j] *= eyl[j];
            tot_eyh[j] *= eyh[j];
        }

        if(debug) std::cout << cn << mn << " ---> Successfully added correction to map" << std::endl;

        //Close the file
        CloseCorrectionFile();
    }

    //Insert the x, exl, exh, and total vectors into the totalCorrections map
    totalCorrections.insert(StringDoubleVectorPair_T("x", t_x));
    totalCorrections.insert(StringDoubleVectorPair_T("exl", t_exl));
    totalCorrections.insert(StringDoubleVectorPair_T("exh", t_exh));
    totalCorrections.insert(StringDoubleVectorPair_T("y", tot_y));
    totalCorrections.insert(StringDoubleVectorPair_T("eyl", tot_eyl));
    totalCorrections.insert(StringDoubleVectorPair_T("eyh", tot_eyh));

    if(debug) std::cout << cn << mn << "Successfully added all corrections to map" << std::endl;
}

void SPXGridCorrections::PrintMap(std::string &s, StringDoubleVectorMap_T &m) {
    std::string mn = "PrintMap: ";

    std::cout << cn << mn << "Printing " << s << " map: " << std::endl;
    std::cout << "===============================================================================" << std::endl;
    std::cout << "|          x |        exl |        exh |          y |        eyl |        eyh |" << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;

    //Iterate over map
    for(int i = 0; i < numberOfBins; i++) {
        //Show 4 decimal places
        std::cout << std::fixed;
        std::cout.precision(4);

        std::cout << "| ";
        std::cout.width(10);
        std::cout << m["x"][i];
        std::cout << " | ";
        std::cout.width(10); std::cout << m["exl"][i];
        std::cout << " | ";
        std::cout.width(10); std::cout << m["exh"][i];
        std::cout << " | ";
        std::cout.width(10); std::cout << m["y"][i];
        std::cout << " | ";
        std::cout.width(10); std::cout << m["eyl"][i];
        std::cout << " | ";
        std::cout.width(10); std::cout << m["eyh"][i];
        std::cout << " |" << std::endl;
    }

    std::cout << "============================================================================================" << std::endl;
    std::cout << std::endl << std::endl;
}

//Print all individual corrections plus the total corrections
void SPXGridCorrections::Print(void) {
    unsigned int numberOfCorrectionFiles = pci.gridSteeringFile.GetNumberOfCorrectionFiles();

    for(int i = 0; i < numberOfCorrectionFiles; i++) {
        std::string filename = pci.gridSteeringFile.GetCorrectionFile(i);

        PrintMap(filename, corrections[filename]);
    }

    std::cout << "============================================================================================" << std::endl;
    std::cout << "============================================================================================" << std::endl;
    std::cout << std::endl;

    std::string s = "Total Corrections";
    PrintMap(s, totalCorrections);
}
