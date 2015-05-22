//************************************************************/
//
//	Grid Corrections Header
//
//	Outlines the SPXGridCorrections class, which stores the
//  corrections for each grid in a String-Double Vector map
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		26.11.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXGRIDCORRECTIONS_H
#define SPXGRIDCORRECTIONS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>

#include "SPXPlotConfiguration.h"
#include "SPXException.h"
#include "SPXUtilities.h"

typedef std::map<std::string, StringDoubleVectorMap_T >  CorrectionsMap_T;
typedef std::pair<std::string, StringDoubleVectorMap_T >  CorrectionsPair_T;

class SPXGridCorrections {

public:
    explicit SPXGridCorrections(const SPXPlotConfigurationInstance &pci) {
        this->pci = pci;
        correctionFile = new std::ifstream();
    }

    void Parse(void);
    void PrintMap(std::string &s, StringDoubleVectorMap_T &m);
    void Print(void);

    static bool GetDebug(void) {
        return debug;
    }

    static void SetDebug(bool b) {
        debug = b;
    }

    unsigned int GetNumberOfBins(void) {
        return numberOfBins;
    }

    CorrectionsMap_T & GetCorrectionsMap(void) {
        return corrections;
    }

    StringDoubleVectorMap_T & GetTotalCorrectionsMap(void) {
        return totalCorrections;
    }

    TGraphAsymmErrors * GetCorrectionGraph(std::string &filename);
    TGraphAsymmErrors * GetTotalCorrectionGraph(std::string &filename);

    std::vector<double> & GetX(std::string &filename) {
        return corrections[filename]["x"];
    }

    std::vector<double> & GetEXL(std::string &filename) {
        return corrections[filename]["exl"];
    }

    std::vector<double> & GetEXH(std::string &filename) {
        return corrections[filename]["exh"];
    }

    std::vector<double> & GetY(std::string &filename) {
        return corrections[filename]["y"];
    }

    std::vector<double> & GetEYL(std::string &filename) {
        return corrections[filename]["eyl"];
    }

    std::vector<double> & GetEYH(std::string &filename) {
        return corrections[filename]["eyh"];
    }

    std::vector<double> & GetTotalX(void) {
        return totalCorrections["x"];
    }

    std::vector<double> & GetTotalEXL(void) {
        return totalCorrections["exl"];
    }

    std::vector<double> & GetTotalEXH(void) {
        return totalCorrections["exh"];
    }

    std::vector<double> & GetTotalYCorrections(void) {
        return totalCorrections["y"];
    }

    std::vector<double> & GetTotalEYLCorrections(void) {
        return totalCorrections["eyl"];
    }

    std::vector<double> & GetTotalEYHCorrections(void) {
        return totalCorrections["eyh"];
    }

private:
    static bool debug;								//Flag indicating debug mode
    std::ifstream *correctionFile;					//Must declare as pointer... ifstream's copy constructor is private
    SPXPlotConfigurationInstance pci;				//Plot configuration instance which contains the grid steering file

    //Number of bins in correction
    unsigned int numberOfBins;

    //Actual corrections map: Keys are correction filenames, values are Nx6 matrices (6 columns, N rows) where the
    //  columns correspond to x, eyl, eyh, y, eyl, and eyh corrections, and the rows correspond to each bin
    CorrectionsMap_T corrections;

    std::string name;
    std::string comment;
    std::string errortype;

    //Total correction map: These are the product of all scales from all the individual corrections. The format for the map is:
    //  Keys: "x", "exl", "exh", "y", "eyl", and "eyh", Values: Vector of corresponding x or corrections, with length N (N = # of Bins)
    StringDoubleVectorMap_T totalCorrections;

    void OpenCorrectionFile(std::string filepath) {

        if(filepath.empty()) {
            throw SPXFileIOException("Grid Correction Filepath is empty");
        }

        try {
            correctionFile->open(filepath.c_str());

            if(*correctionFile) {
                if(debug) std::cout << "SPXGridCorrections::OpenCorrectionFile: Successfully opened correction file: " << filepath << std::endl;
            } else {
                throw SPXFileIOException(filepath, "Unable to open correction file");
            }
        } catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;

            throw SPXFileIOException(filepath, "Unable to open correction file");
        }
    }

    void CloseCorrectionFile(void) {
        correctionFile->close();
    }

    void CheckVectorSize(const std::vector<double> & vector, const std::string & name, unsigned int masterSize) {
        if(vector.size() != masterSize) {
            std::ostringstream oss;
            oss << "Size error: \"" << name << "\" vector has different size (" << vector.size() << ") than master size (" << masterSize << ")" << std::endl;
            throw SPXParseException(oss.str());
        } else {
            if(debug) std::cout << "SPXGridCorrections::" << "CheckVectorSize: " << "\t -->  Success: \"" << name << "\" vector size matches master size" << std::endl;
        }
    }
};

#endif
