//************************************************************/
//
//	Steering File Header
//
//	Outlines the SPXSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		19.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <map>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "INIReader.h"

#include "SPXPlotType.h"
#include "SPXRatioStyle.h"
#include "SPXDisplayStyle.h"
#include "SPXOverlayStyle.h"
#include "SPXPlotConfiguration.h"

#include "SPXPDFSteeringFile.h"
#include "SPXDataSteeringFile.h"
#include "SPXGridSteeringFile.h"

#include "SPXException.h"

#ifndef SPXSTEERINGFILE_H
#define SPXSTEERINGFILE_H

const int STYLE_EMPTY = -1;
const int COLOR_EMPTY = -1;

//const long long MAX_EMPTY = -1e10;
//const long long MIN_EMPTY = 1e10;

const double MAX_EMPTY = -HUGE_VAL;
const double MIN_EMPTY =  HUGE_VAL;

class SPXSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;						//Flag to indicate debug mode

        bool  OutputRootfile; // Flag to write out rootfile with all objects
	std::string OutputGraphicFormat; // string specifying graphic format of figures

	//[GRAPH]
        bool addonLegendNLOProgramName; // Flag to indicate that NLO program name should be added in Legend
	bool plotBand;		//Flag to indicate that error bands should be plotted
	bool plotErrorTicks;	//Flag to indicate that tick marks should be used at the end of error bars
	bool plotMarker;	//Flag to indicate that data point markers should be plotted
	bool plotStaggered;	//Flag to indicate that points should be staggered horizontally for readability
	bool matchBinning;	//Flag to indicate that the binning of all graphs on the plot should be matched
        bool TakeSignforTotalError; // Flag to calculate total data uncertainty
                                    // when true all positive/negative uncertainty are added to total negative/positive total error
                                    //      false uncertainties are taken as such
      	bool set_grid_styles;       // Flag to indicate, if grid properties are set 

	std::vector <bool> RemoveXbins;           // if ON points below/above DataCutXmin/DataCutXmax are removed
	std::vector <double> DataCutXmin;         // Value below which data points are removed if  RemoveXbins=true
	std::vector <double> DataCutXmax;         // Value above which data points are removed if  RemoveXbins=true
        
        bool printTotalSigma;   // Flag to indicate that, if total integrated cross section should be printed

	bool gridCorr;		//Flag if corrections should be applied to grid/convolute
	std::string ContainGridCorr; // only keep correction that contain this string
	bool nomCorr;		//Flag if correction should be applied to nominal histogram
	bool labelSqrtS;	//Flag to indicate that the Sqrt(s) value should be shown in the legend

	bool labelDate;	        //Flag to add Data beside label

	bool   showIndividualSystematicswithName; //Flag to show individual systematics data uncertainties that contain name
                                          // given in steering
	double showIndividualSystematics; //Flag to show individual systematics data uncertainties
	double showTotalSystematics; //Flag to show total systematics data uncertainties
	double showIndividualSystematicsAsLine; //Flag to show individual systematics data uncertainties
        bool ordersystematiccolorbyalphabeth; // Flag if color should be ordered according to systematic names

        double XDataBoxLabel;    // X-coordinate of data box label
        double YDataBoxLabel;    // Y-coordinate of data box label

        double 	BeamUncertainty; // Introduce beam uncertainty to band with value 	BeamUncertainty

        bool BandwithPDF;        // show the Uncertainty band with the PDF uncertainties
	bool BandwithAlphaS;     // show the Uncertainty band with alphas  uncertainties
	bool BandwithScales;     // show the Uncertainty band with ren&fac scales uncertainties
	bool BandwithScalesAlternativeScaleChoice;     // show the Uncertainty band with ren&fac scales alternative scale choice uncertainties
	bool BandTotal;          // show the Total uncertainty band

        std::vector<bool> gridcorrections; // flags if grid correction go into band

        std::vector<std::string> systematicsclasses;      // groups to display systematics
        std::vector<int>         systematicsclassescolor; // groups to display systematics fill colors
        std::vector<int>         systematicsclassesedgecolor; // groups to display systematics edge colors
        std::vector<int>         systematicsclassesedgestyle; // groups to display systematics edge style
        std::vector<int>         systematicsclassesedgewidth; // groups to display systematics edge line width

        std::vector<std::string> removesystematicsclasses;      // groups to remove systematics
        std::vector<std::string> containsystematicsclasses;     // groups to contain systematics

        std::vector<double> RenScales; // vector for renormalisation scale variations
        std::vector<double> FacScales; // vector for factorisation scale variations
 
        // from Graph field
	double xLegend;		    //X Position of the main legend, based on top-right corner
	double yLegend;		    //Y Position of the main legend, based on top-right corner
	double xInfoLegend;	    //X Position of the information legend, based on  corner
	double yInfoLegend;	    //Y Position of the information legend, based on top-right corner
	std::string InfoLegendLabel;// Additional information legend label
	double yOverlayMin;	    //Force Y-Axis minimum for overlay section
	double yOverlayMax;	    //Force Y-Axis maximum for overlay section
	double xOverlayMin;	    //Force X-Axis minimum for overlay section
	double xOverlayMax;	    //Force X-Axis maximum for overlay section
	double yRatioMin;	    //Force Y-Axis minimum for ratio section
	double yRatioMax;	    //Force Y-Axis maximum for ratio section

        // from Plot field (will take priority on Graph field)
	double xLegendPlot;	
	double yLegendPlot;	
	double xInfoLegendPlot;	    
	double yInfoLegendPlot;	    
	std::string InfoLegendLabelPlot;	
	double yOverlayMinPlot;	
	double yOverlayMaxPlot;	    
	double xOverlayMinPlot;	    
	double xOverlayMaxPlot;	    
	double yRatioMinPlot;	    
	double yRatioMaxPlot;	    


        int CalculateChi2;      // Calculate Chi2
        int DumpTables;      // Dump Latex tables

        bool AddScaleFunctionalForm; // add functional form of scale in info legend
        bool AddLumi;             // add luminosity label in info legend
        bool AddJournal;          // add journal label in legend
        bool AddJournalYear;      // add journal year  in legend

	//[FRAME_n]
	std::vector<SPXPlotConfiguration> plotConfigurations;	//Options for each plot
	void SetDefaultScales(void);
	void SetDefaults(void);
	unsigned int ParseNumberOfPlots(void);
	unsigned int ParseNumberOfRatios(unsigned int plotNumber);
	void ParsePlotConfigurations();


public:

	//These functions simplify the main file's interface to parsing
	// and printing the Steering Files
        //vector <bool> hasdata;
        //vector <bool> hasmc;
 
	void PrintAll(void);
	void ParseAll(bool print);

	void Print(void);
	void PrintPDFSteeringFiles(void);
	void PrintDataSteeringFiles(void);
	void PrintGridSteeringFiles(void);
	void PrintDataMetadata(void);
	void PrintGridMetadata(void);

	void Parse(void);
	void ParsePDFSteeringFiles(void);
	void ParseDataSteeringFiles(void);
	void ParseGridSteeringFiles(void);

	explicit SPXSteeringFile(const std::string &filename) : debug(false){
		//Set filename
		this->filename = filename;
	}

	const std::string & GetFilename(void) const {
		return this->filename;
	}

	void SetDebug(bool b) {
		debug = b;
	}

	bool GetDebug(void) const {
		return this->debug;
	}

	bool GetOutputRootfile(void) const {
		return this->OutputRootfile;
	}

	std::string GetOutputGraphicFormat(void) const {
		return this->OutputGraphicFormat;
	}

	bool GetPlotBand(void) const {
		return this->plotBand;
	}

	bool GetAddonLegendNLOProgramName(void) const {
		return this->addonLegendNLOProgramName;
	}

	bool GetPlotErrorTicks(void) const {
		return this->plotErrorTicks;
	}

	bool GetPlotMarker(void) const {
		return this->plotMarker;
	}

	bool GetPlotStaggered(void) const {
		return this->plotStaggered;
	}

	bool GetMatchBinning(void) const {
		return this->matchBinning;
	}

	bool GetTakeSignforTotalError(void) const {
	  return this-> TakeSignforTotalError;
	}


	bool PrintTotalCrossSection(void) const {
		return this->printTotalSigma;
	}

	bool ApplyGridCorr(void) const {
		return this->gridCorr;
	}

	std::string KeepGridCorr(void) const {
		return this->ContainGridCorr;
	}

	bool ApplyNominalCorr(void) const {
		return this->nomCorr;
	}


	bool GetLabelSqrtS(void) const {
		return this->labelSqrtS;
	}

	bool GetLabelDate(void) const {
		return this->labelDate;
	}

        double ShowIndividualSystematics(){
	 return showIndividualSystematics; 
        }

        int ShowTotalSystematics(){
	 return showTotalSystematics; 
        }

        double ShowIndividualSystematicsAsLine(){
	 return showIndividualSystematicsAsLine; 
        }

        bool OrderColorSystematicNamesByAlphabeth(){
	 return ordersystematiccolorbyalphabeth; 
        }

	double GetXDataBoxLabel(void) const {
	  return this->XDataBoxLabel;
	}

	double GetYDataBoxLabel(void) const {
	  return this->YDataBoxLabel;
	}


	double GetXLegend(void) const {
		return this->xLegend;
	}

	double GetYLegend(void) const {
		return this->yLegend;
	}

	double GetXInfoLegend(void) const {
		return this->xInfoLegend;
	}

	double GetYInfoLegend(void) const {
		return this->yInfoLegend;
	}

	std::string GetInfoLegendLabel(void) const {
		return this->InfoLegendLabel;
	}

	double GetYOverlayMin(void) const {
		return this->yOverlayMin;
	}

	double GetYOverlayMax(void) const {
		return this->yOverlayMax;
	}

	double GetXOverlayMin(void) const {
		return this->xOverlayMin;
	}

	double GetXOverlayMax(void) const {
		return this->xOverlayMax;
	}

	double GetYRatioMin(void) const {
		return this->yRatioMin;
	}

	double GetYRatioMax(void) const {
		return this->yRatioMax;
	}

	bool GetAddLumiLabel(void) const {
		return this->AddLumi;
	}

	bool GetScaleFunctionalFormLabel(void) const {
                return AddScaleFunctionalForm; 
        }

	bool GetAddJournalLabel(void) const {
		return this->AddJournal;
	}

	bool GetAddJournalYear(void) const {
		return this->AddJournalYear;
	}

	int GetCalculateChi2(void) const {
		return this->CalculateChi2;
	}

	void SetCalculateChi2(int newchi2) {
		CalculateChi2= newchi2;
	}

	int GetDumpTables(void) const {
		return this->DumpTables;
	}

	void SetDumpTables(int newdump) {
		DumpTables= newdump;
	}

	double GetBeamUncertainty(void) const {
		return BeamUncertainty;
	}

	bool GetBandwithPDF(void) const {
		return this->BandwithPDF;
	}

	bool GetBandwithAlphaS(void) const {
		return this->BandwithAlphaS;
	}

	bool GetBandwithScales(void) const {
		return this->BandwithScales;
	}

	bool GetBandwithAlternativeScaleChoice(void) const {
		return this->BandwithScalesAlternativeScaleChoice;
	}

	bool GetBandTotal(void) const {
		return this->BandTotal;
	}

	
	int GetNumberofCorrectionToBand() {
	 //std::cout<<" gridcorrections.size()= "<<gridcorrections.size()<<std::endl;
	 return gridcorrections.size(); 
        }

	bool GetGridCorrectionToBand(int i) {
	 if (i>=gridcorrections.size()){  
	   //std::ostringstream oss;
	   //oss<<"SPXSteering::GetBandGridCorrection: gridcorrectionsize= "<<gridcorrections.size()<<" but i= "<<i;
	   //throw SPXParseException(oss.str());
           return false;
         } 
	 return gridcorrections.at(i);
	}

        std::vector<std::string> GetSystematicClasses(){
         // groups to display systematics
         return systematicsclasses;      
        }

        std::vector<int> GetSystematicClassesColor(){         
         // groups to display systematics colors
         return systematicsclassescolor; 
        }

        std::vector<int> GetSystematicClassesEdgeColor(){         
         return systematicsclassesedgecolor; 
        }

        std::vector<int> GetSystematicClassesEdgeWidth(){         
         return systematicsclassesedgewidth; 
        }

        std::vector<int> GetSystematicClassesEdgeStyle(){         
         return systematicsclassesedgestyle; 
        }

        std::vector<std::string> GetSystematicClassesToRemove(){
         // groups to display systematics
         return removesystematicsclasses;      
        }

        std::vector<std::string> GetSystematicClassesToKeep(){
         // groups to display systematics
         return containsystematicsclasses;      
        }

        bool GetSetGridStyleFlag(){
      	 return set_grid_styles;      
        }

        //bool HasData(int iplot) {
	// if (iplot>hasdata.size()) {
	// std::ostringstream oss;
	//  oss << "SPXSteering:: Something is wrong iplot= "<<iplot<<" but hasdata vector size= "<<hasdata.size();
	// throw SPXParseException(oss.str());
	// };
	// return hasdata.at(iplot);
	//};

        //bool HasMC(int iplot) {
	// if (iplot>hasmc.size()) {
        //  std::ostringstream oss;
	//  oss << "SPXSteering:: Something is wrong iplot= "<<iplot<<" but hasmc vector size= "<<hasmc.size();
	//  throw SPXParseException(oss.str());
        // }
        // return hasmc.at(iplot);
        //};

	unsigned int GetNumberOfPlotConfigurations(void) const {
		return this->plotConfigurations.size();
	}

	std::vector<double> & GetRenScales(void) {
		return this->RenScales;
	}

	std::vector<double> & GetFacScales(void) {
		return this->FacScales;
	}

	std::vector<SPXPlotConfiguration> & GetPlotConfigurationsVector(void) {
		return this->plotConfigurations;
	}

	SPXPlotConfiguration & GetPlotConfiguration(unsigned int index) {

		if((index + 1) > plotConfigurations.size()) {
			int top = plotConfigurations.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXSteeringFile::GetPlotConfiguration: Index out of range");
		}

		return plotConfigurations.at(index);
	}

	unsigned int GetNumberOfConfigurationInstances(unsigned int index) {
		return plotConfigurations.at(index).GetNumberOfConfigurationInstances();
	}

	SPXPlotConfigurationInstance & GetPlotConfigurationInstance(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii);
	}

	SPXDataSteeringFile & GetDataSteeringFile(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii).dataSteeringFile;
	}

	SPXGridSteeringFile & GetGridSteeringFile(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii).gridSteeringFile;
	}

	SPXPDFSteeringFile & GetPDFSteeringFile(unsigned int pci, unsigned int pcii) {
		return plotConfigurations.at(pci).GetPlotConfigurationInstance(pcii).pdfSteeringFile;
	}

        double GetDataCutXmin(int i);
        double GetDataCutXmax(int i);
        bool GetDataRemoveXbinsFlag(int i);

};

#endif
