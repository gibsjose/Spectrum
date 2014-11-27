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

        //Process the file
        while(correctionFile->good()) {
            std::getline(*correctionFile, line);

            //Skip empty lines
            if(line.empty()) {
                continue;
            }

            if(SPXStringUtilities::Trim(line))

            std::istringstream iss(line);

            std::cout << "LINE: " << line << std::endl;

            //Obtain the correction type (SPXGridCorrectionType???)
            //Build the matrix based on the xm, dx-, dx+, sigma, dsigma-, dsigma+
            x.push_back(1.0);
            exl.push_back(1.0);
            exh.push_back(1.0);
            y.push_back(1.0);
            eyl.push_back(1.0);
            eyh.push_back(1.0);

            //MAKE SURE TO CONVERT XLOW/XHIGH TO DELTA INSTEAD OF ABSOLUTE (e.g. 100, 8, 6 instead of 100, 92, 106)
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
        correctionsMap.insert(CorrectionsPair_T(filename, m));

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
    totalCorrections.insert(StringDoubleVectorPair_T("x", x));
    totalCorrections.insert(StringDoubleVectorPair_T("exl", exl));
    totalCorrections.insert(StringDoubleVectorPair_T("exh", exh));
    totalCorrections.insert(StringDoubleVectorPair_T("tot_y", tot_y));
    totalCorrections.insert(StringDoubleVectorPair_T("tot_eyl", tot_eyl));
    totalCorrections.insert(StringDoubleVectorPair_T("tot_eyh", tot_eyh));

    if(debug) std::cout << cn << mn << "Successfully added all corrections to map" << std::endl;

    //Print everything...
    //if(debug) Print();
}

void SPXGridCorrections::PrintMap(std::string &s, StringDoubleMap_T &m) {
    std::string mn = "PrintMap: ";

    std::cout cn << mn << "Printing " << s << " map: " << std::endl << std::endl;
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

    std::cout << std::endl;
    std::cout << "============================================================================================" << std::endl;
    std::cout << "============================================================================================" << std::endl;
    std::cout << std::endl;

    PrintMap("Total Corrections", totalCorrections);
}
