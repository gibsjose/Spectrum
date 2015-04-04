//************************************************************/
//
//	Data Header
//
//	Outlines the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXDATA_H
#define SPXDATA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>

#include "SPXROOT.h"
#include "TMatrixTLazy.h"
#include "TMatrixT.h"

#include "SPXPlotConfiguration.h"
#include "SPXException.h"
#include "SPXUtilities.h"

class SPXData {

public:

        SPXData(const SPXPlotConfigurationInstance &pci);

	void Parse(void);
	void Print(void);
	void Draw(void);

	void CreateGraphs(void);

	void ReadCorrelation();

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	const SPXDataFormat & GetDataFormat(void) {
		return pci.dataSteeringFile.GetDataFormat();
	}

        const std::string & GetLegendLabel(void) {
	 return pci.dataSteeringFile.GetLegendLabel();
        }

        const std::string & GetJournalLegendLabel(void) {
	 return pci.dataSteeringFile.GetReferenceJournalName();
        }

        const std::string & GetJournalYear(void) {
	 return pci.dataSteeringFile.GetReferenceJournalYear();
        }


        const double GetSqrtS(void) {
	 return pci.dataSteeringFile.GetSqrtS();
        }

        const std::string GetJetAlgorithmLabel(void) {
	 return pci.dataSteeringFile.GetJetAlgorithmLabel();
        }

        const int GetJetAlgorithmRadius(void) {
	 return pci.dataSteeringFile.GetJetAlgorithmRadius();
        }

        const std::string GetDoubleBinVariableName(void) {
	  return pci.dataSteeringFile.GetDoubleBinVariableName();
        }

        const double GetDoubleBinValueMin(void) {
	 return pci.dataSteeringFile.GetDoubleBinValueMin();
        }

	const bool IsDividedByDoubleDiffBinWidth(void) const {
	  return pci.dataSteeringFile.IsDividedByDoubleDiffBinWidth();
	}

        const double GetDoubleBinValueMax(void) {
	 return pci.dataSteeringFile.GetDoubleBinValueMax();
        }

        const double GetDoubleBinValueWidth(void) {
	 return pci.dataSteeringFile.GetDoubleBinValueWidth();
        }

        const std::string & GetDatasetYear(void) {
	 return pci.dataSteeringFile.GetDatasetYear();
        }

        const std::string & GetDatasetLumi(void) {
	 return pci.dataSteeringFile.GetDatasetLumi();
        }

	unsigned int GetNumberOfBins(void) {
		return numberOfBins;
	}

	const bool IsDividedByBinWidth(void) const {
		return dividedByBinWidth;
	}

	unsigned int GetNumberOfIndividualSystematicErrors(void) const {
	 return individualSystematics.size();
        }

	StringDoubleVectorMap_T GetIndividualSystematics(){
	 return individualSystematics;
        };

	std::vector<double> GetDataColumn(const std::string &column) {
		return data[column];
	}

	std::map<std::string, std::vector<double> > & GetDataMap(void) {
		return data;
	}

	std::vector<double> & GetXmVector(void) {
		return data["xm"];
	}

	std::vector<double> & GetXlowVector(void) {
		return data["xlow"];
	}

	std::vector<double> & GetXhighVector(void) {
		return data["xhigh"];
	}

	std::vector<double> & GetSigmaVector(void) {
		return data["sigma"];
	}

	std::vector<double> & GetStatisticalErrorVector(void) {
		return data["stat"];
	}



	//NOTE: Returns average of postive and negative error
	//std::vector<double> & GetSystematicErrorVector(void) {
	//  return 0.5*(data["syst_p"]+data["syst_n"]);
	//}

	std::vector<double> & GetPositiveSystematicErrorVector(void) {
		return data["syst_p"];
	}

	std::vector<double> & GetNegativeSystematicErrorVector(void) {
		return data["syst_n"];
	}

	std::vector <TGraphAsymmErrors *> GetSystematicsErrorGraphs(void);

	TGraphAsymmErrors * GetStatisticalErrorGraph(void) {

	if(!this->statisticalErrorGraph) {
	 throw SPXGraphException("Statistical Error Graph pointer is NULL: You MUST call ::CreateGraphs() before obtaining the graph");
	 }
         return this->statisticalErrorGraph;
	}

	TGraphAsymmErrors * GetSystematicErrorGraph(int i);

	TGraphAsymmErrors * GetSystematicErrorGraph(void) {

	 if(!this->systematicErrorGraph) {
	  throw SPXGraphException("Systematic Error Graph pointer is NULL: You MUST call ::CreateGraphs() before obtaining the graph");
	  }
	  return this->systematicErrorGraph;
	}


	TGraphAsymmErrors * GetTotalErrorGraph(void) {
	 if(!this->totalErrorGraph) {
	  throw SPXGraphException("Total Error Graph pointer is NULL: You MUST call ::CreateGraphs() before obtaining the graph");
	  }
	  return this->totalErrorGraph;
	}

        TMatrixT <double> * GetDataTotalCovarianceMatrix() {return cov_matrixtot; };
        TMatrixT <double> * GetDataStatCovarianceMatrix()  {return cov_matrixstat;};
        TMatrixT <double> * GetDataSystCovarianceMatrix()  {return cov_matrixsyst;};

private:
	static bool debug;				//Flag indicating debug mode
	std::ifstream *dataFile;			//Must declare as pointer... ifstream's copy constructor is private
	SPXPlotConfigurationInstance pci;		//Frame options instance which contains the data steering file as well as the plot options
	SPXDataFormat dataFormat;			//Format of this data: Included directly simply to cut down on syntax in implementation
	bool dividedByBinWidth;				//Flag indicating whether the initial data was already divided by the bin width

	//Number of bins in data map
	unsigned int numberOfBins;

	//Individual Systematic Errors
	StringDoubleVectorMap_T individualSystematics;

	//Actual data map
	StringDoubleVectorMap_T data;

	//Graph for plotting statistical error ticks
	TGraphAsymmErrors *statisticalErrorGraph;

	//Graph containing systematic errors
	TGraphAsymmErrors *systematicErrorGraph;

	std::vector <TGraphAsymmErrors *> individualsystematicErrorGraph;

	//Actual data graph containing total errors
	TGraphAsymmErrors *totalErrorGraph;

        // stuff related to covariance matrix

        TMatrixT<double>  *cov_matrixtot;
        TMatrixT<double>  *cov_matrixstat;
        TMatrixT<double>  *cov_matrixsyst;

        TMatrixT<double>  *corr_matrixtot;
        TMatrixT<double>  *corr_matrixstat;
        TMatrixT<double>  *corr_matrixsyst;

        void ReadCorrelationMatrix(std::string filename);
        void CalculateSystematicCovarianceMatrix();

	std::string GetCorrespondingSystematicName(std::string systename);

        StringDoubleVectorMap_T SymmetrizeSystemicUncertaintiesMatrix(StringDoubleVectorMap_T syst);


        // parsing and print methods

	void ParseSpectrum(void);
	//void ParseHERAFitter(void);

	void PrintSpectrum(void);
	//void PrintHERAFitter(void);

        void PrintSystematics(StringDoubleVectorMap_T syst);

	void OpenDataFile(void) {
	 std::string filepath = pci.dataSteeringFile.GetDataFile();

	 if(filepath.empty()) {
	  throw SPXFileIOException("Data Filepath is empty");
	 }

	 try {
	  dataFile = new std::ifstream();
	  dataFile->open(filepath.c_str());

	  if(*dataFile) {
	   if(debug) std::cout << "SPXData::OpenDataFile: Successfully opened data file: " << filepath << std::endl;
	  } else {
	   throw SPXFileIOException(filepath, "Unable to open data file");
	  }
	 } catch(const std::exception &e) {
	  throw;
	 }
	}

	void CheckVectorSize(const std::vector<double> & vector, const std::string & name, unsigned int masterSize) {
	 if(vector.size() != masterSize) {
	  std::ostringstream oss;
	  oss << "Size error: \"" << name << "\" vector has different size (" << vector.size() << ") than master size (" << masterSize << ")" << std::endl;
	  throw SPXParseException(pci.dataSteeringFile.GetDataFile(), oss.str());
	 } else {
	  if(debug) std::cout << "SPXData::" << "CheckVectorSize: " << "\t -->  Success: \"" << name << "\" vector size matches master size" << std::endl;
	 }
	}
};

#endif
