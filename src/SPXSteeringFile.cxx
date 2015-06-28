//************************************************************/
//
//	Steering File Class
//
//	Implements the SPXSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections within Spectrum.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		19.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/
#include <set>

#include "SPXSteeringFile.h"
#include "SPXUtilities.h"

//Class name for debug statements
const std::string cn = "SPXSteeringFile::";

const int DEFAULT_DATA_MARKER_STYLE = 20;	//Circle
const int DEFAULT_DATA_MARKER_COLOR = 1;	//Black

const int DEFAULT_GRID_MARKER_STYLE = 20;	
const int DEFAULT_GRID_FILL_STYLE = 1001;	
const int DEFAULT_GRID_MARKER_COLOR = 1;	

void SPXSteeringFile::SetDefaults(void) {
	std::string mn = "SetDefaults: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Setting default Steering File data" << std::endl;

	plotBand = false;
	if(debug) std::cout << cn << mn << "plotBand set to default: \"false\"" << std::endl;

        addonLegendNLOProgramName= false;
	if(debug) std::cout << cn << mn << " addonLegendNLOProgramName set to default: \"false\"" << std::endl;

	plotErrorTicks = true;
	if(debug) std::cout << cn << mn << "plotErrorTicks set to default: \"true\"" << std::endl;

	plotMarker = true;
	if(debug) std::cout << cn << mn << "plotMarker set to default: \"true\"" << std::endl;

	BandwithPDF    = true;
	if(debug) std::cout << cn << mn << "BandwithPDF set to default: \"true\"" << std::endl;

	BandwithAlphaS = false;
	if(debug) std::cout << cn << mn << "BandwithAlphaS set to default: \"false\"" << std::endl;

	BandwithScales = true;
	if(debug) std::cout << cn << mn << "BandwithScales set to default: \"false\"" << std::endl;

	BandTotal      = true;
	if(debug) std::cout << cn << mn << "BandwithTotal set to default: \"true\"" << std::endl;

        gridcorrections.clear();

        RenScales.clear();
        FacScales.clear();

	plotStaggered = false;
	if(debug) std::cout << cn << mn << "plotStaggered set to default: \"false\"" << std::endl;

	showIndividualSystematics = 0.;
	if(debug) std::cout << cn << mn << "showIndividualSystematics set to default: \"0\"" << std::endl;

	ordersystematiccolorbyalphabeth = false;
	if(debug) std::cout << cn << mn << "ordersystematiccolorbyalphabeth set to default: \"False\"" << std::endl;

	matchBinning = true;
	if(debug) std::cout << cn << mn << "matchBinning set to default: \"true\"" << std::endl;

	TakeSignforTotalError = true;
	if(debug) std::cout << cn << mn << "TakeSignforTotalError set to default: \"true\"" << std::endl;

	RemoveXbins.clear(); 
	DataCutXmin.clear();
	DataCutXmax.clear();

        set_grid_styles=false;    
	if(debug) std::cout << cn << mn << "set_grid_styles set to default: \"false\"" << std::endl;

        printTotalSigma=false;
	if(debug) std::cout << cn << mn << " printTotalSigma set to default: \"false\"" << std::endl;

	gridCorr = false;
	if(debug) std::cout << cn << mn << "gridCorr set to default: \"false\"" << std::endl;

	labelSqrtS = false;
	if(debug) std::cout << cn << mn << "labelSqrtS set to default: \"false\"" << std::endl;

	xLegend = 0.8;
	if(debug) std::cout << cn << mn << "xLegend set to default: \"0.75\"" << std::endl;

	yLegend = 0.93;
	if(debug) std::cout << cn << mn << "yLegend set to default: \"0.93\"" << std::endl;

	xInfoLegend = 0.4;
	if(debug) std::cout << cn << mn << "xInfoLegend set to default: \"0.4\"" << std::endl;

	yInfoLegend = 0.3;
	if(debug) std::cout << cn << mn << "yInfoLegend set to default: \"0.3\"" << std::endl;

	InfoLegendLabel = "";
	if(debug) std::cout << cn << mn << "InfoLegendLabel set to default: empty string" << std::endl;

	XDataBoxLabel = 0.25;
	if(debug) std::cout << cn << mn << "XDataBoxLabel set to default: \"0.25\"" << std::endl;

	YDataBoxLabel = 0.3;
	if(debug) std::cout << cn << mn << "XDataBoxLabel set to default: \"0.3\"" << std::endl;

	xOverlayMin = MIN_EMPTY;
	if(debug) std::cout << cn << mn << "xOverlayMin set to default: \"" << MIN_EMPTY << "\"" << std::endl;

	xOverlayMax = MAX_EMPTY;
	if(debug) std::cout << cn << mn << "xOverlayMax set to default: \"" << MAX_EMPTY << "\"" << std::endl;

	yOverlayMin = MIN_EMPTY;
	if(debug) std::cout << cn << mn << "yOverlayMin set to default: \"" << MIN_EMPTY << "\"" << std::endl;

	yOverlayMax = MAX_EMPTY;
	if(debug) std::cout << cn << mn << "yOverlayMax set to default: \"" << MAX_EMPTY << "\"" << std::endl;

	yRatioMin = MIN_EMPTY;
	if(debug) std::cout << cn << mn << "yRatioMin set to default: \"" << MIN_EMPTY << "\"" << std::endl;

	yRatioMax = MAX_EMPTY;
	if(debug) std::cout << cn << mn << "yRatioMax set to default: \"" << MAX_EMPTY << "\"" << std::endl;

	CalculateChi2 = 0;
	if(debug) std::cout << cn << mn << "CalculateChi2 set to default: OFF" << std::endl;

	DumpTables = 0;
	if(debug) std::cout << cn << mn << "DumpTables set to default: 0" << std::endl;

        AddLumi=false;
	if(debug) std::cout << cn << mn << "Addlumi set to default: OFF" << std::endl;

        AddJournal=false;
	if(debug) std::cout << cn << mn << "AddJournal set to default: OFF" << std::endl;

        AddJournalYear=false;
	if(debug) std::cout << cn << mn << "AddJournalYear set to default: OFF" << std::endl;

}

void SPXSteeringFile::PrintAll(void) {
	this->Print();
	this->PrintPDFSteeringFiles();
	this->PrintDataSteeringFiles();
	this->PrintGridSteeringFiles();
}

void SPXSteeringFile::ParseAll(bool print) {
	std::string mn = "ParseAll: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	try {
		if(debug) std::cout << cn << mn << "Parsing Steering File" << std::endl;
		this->Parse();
		if(print) this->Print();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing Steering File");
	}

	try {
		if(debug) std::cout << cn << mn << "Parsing PDF Steering Files" << std::endl;
		this->ParsePDFSteeringFiles();
		if(print) this->PrintPDFSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing PDF Steering Files");
	}

	try {
		if(debug) std::cout << cn << mn << "Parsing Data Steering Files" << std::endl;
		this->ParseDataSteeringFiles();
		if(print) this->PrintDataSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing Data Steering Files");
	}

	try {
		if(debug) std::cout << cn << mn << "Parsing Grid Steering Files" << std::endl;
		this->ParseGridSteeringFiles();
		if(print) this->PrintGridSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		throw SPXParseException("Error parsing Grid Steering Files");
	}
}

void SPXSteeringFile::PrintDataMetadata(void) {
	std::string mn = "PrintDataMetadata: ";

	std::string filename = "./metadata/data.txt";
	std::ofstream file(filename.c_str(), std::ios::trunc);

	//Make sure file is open
	if(!file.is_open()) {
		throw SPXFileIOException(filename, "Unable to open Data Metadata file");
	}

	std::set<std::string>metadataSet;

	//Loop over each instance of Data and print Metadata
	for(int i = 0; i < this->GetNumberOfPlotConfigurations(); i++) {
		for(int j = 0; j < this->GetNumberOfConfigurationInstances(i); j++) {
			SPXDataSteeringFile &dsf = this->GetDataSteeringFile(i, j);

			//Only show the metadata once for each data file
			if(metadataSet.count(dsf.GetFilename()) != 0) {
				continue;
			}

			metadataSet.insert(dsf.GetFilename());

			file << "Data: " << dsf.GetName() << std::endl;
			file << "File: " << dsf.GetFilename() << std::endl;
			file << "Experiment: " << dsf.GetExperiment() << std::endl;
			file << "Reaction: " << dsf.GetReaction() << std::endl;
			file << "Dataset Year: " << dsf.GetDatasetYear() << std::endl;
			file << "Dataset Luminosity: " << dsf.GetDatasetLumi() << std::endl;
			file << "Publication Status: " << dsf.GetPublicationStatus() << std::endl;
			file << "Reference Journal Name: " << dsf.GetReferenceJournalName() << ", " << dsf.GetReferenceArXivNumber() << std::endl;
			file << "Reference Journal Year: " << dsf.GetReferenceJournalYear() << std::endl;
			file << "Comments: " << dsf.GetComments() << std::endl;
			file << std::endl;
		}
	}

	//Close the file
	file.close();
}

void SPXSteeringFile::PrintGridMetadata(void) {
	std::string mn = "PrintGridMetadata: ";

	std::string filename = "./metadata/grids.txt";
	std::ofstream file(filename.c_str(), std::ios::trunc);

	//Make sure file is open
	if(!file.is_open()) {
		throw SPXFileIOException(filename, "Unable to open Grid Metadata file");
	}

	std::set<std::string>metadataSet;

	//Loop over each instance of Data and print Metadata
	for(int i = 0; i < this->GetNumberOfPlotConfigurations(); i++) {
		for(int j = 0; j < this->GetNumberOfConfigurationInstances(i); j++) {
			SPXGridSteeringFile &gsf = this->GetGridSteeringFile(i, j);

			//Only show the metadata once for each grid file
			if(metadataSet.count(gsf.GetFilename()) != 0) {
				continue;
			}

			metadataSet.insert(gsf.GetFilename());

			file << "Grid: " << gsf.GetName() << std::endl;
			file << "File: " << gsf.GetFilename() << std::endl;
			file << "Author: " << gsf.GetAuthor() << std::endl;
			file << "Renomalization Scale: " << gsf.GetScale() << std::endl;
			file << "NLO Program Name: " << gsf.GetNLOProgramName() << std::endl;
			file << "Grid Program Name: " << gsf.GetGridProgramName() << std::endl;
			file << "Comments: " << gsf.GetComments() << std::endl;
			file << std::endl;
		}
	}

	//Close the file
	file.close();
}

//Print the Steering File Data in a nice format
void SPXSteeringFile::Print(void) {
	std::cout << "Steering File: " << filename << std::endl;
	std::cout << "\t General configurations [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl;
	std::cout << "\t Graphing configurations [GRAPH]" << std::endl;
	std::cout << "\t\t Plot Band is: " << (plotBand ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot cross section Uncertainties with PDF uncertainties: " << (BandwithPDF ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot cross section Uncertainty with AlphaS uncertainties: " << (BandwithAlphaS ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot cross section Uncertainty with ren&fac scale uncertainties: " << (BandwithScales ? "ON" : "OFF") << std::endl;    std::cout << "\t\t Plot Cross section Uncertainties with total uncertainties: " << (BandTotal ? "ON" : "OFF") << std::endl;

	std::cout << "\t\t Add NLO program name on Legend: " << ( addonLegendNLOProgramName? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Error Ticks is: " << (plotErrorTicks ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Marker is: " << (plotMarker ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Set grid properties " << (set_grid_styles ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Plot Staggered is: " << (plotStaggered ? "ON" : "OFF") << std::endl;

	std::cout << "set_grid_styles set to default: \"false\"" << std::endl;

	std::cout << "\t\t Match Binning is: " << (matchBinning ? "ON" : "OFF") << std::endl;

        if (CalculateChi2==0) std::cout << "\t\t Calculate Chi2: OFF" << std::endl;
        if (CalculateChi2==1) std::cout << "\t\t Calculate Simple Chi2" << std::endl;

        if (DumpTables==0) std::cout << "\t\t Dump Latex tables ON" << std::endl;

	if (printTotalSigma) std::cout << "\t\t Print total cross section "  << std::endl;

	std::cout << "\t\t Grid Corrections are: " << (gridCorr ? "ON" : "OFF") << std::endl;
	std::cout << "\t\t Take sign when adding total error: " << (TakeSignforTotalError? "ON" : "OFF") << std::endl;

	std::cout << "\t\t Label Sqrt(s) on Legend: " << (labelSqrtS ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Add luminosity label on Legend: " << (AddLumi ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Add journal label on Legend: " << (AddJournal ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Add journal year on Legend: " << (AddJournalYear ? "YES" : "NO") << std::endl;
        if (showIndividualSystematics==0)
	 std::cout << "\t\t Show NO individual systematics: " << std::endl;
        if (showIndividualSystematics>0)
	 std::cout << "\t\t Show individual systematics with one bin above "<<showIndividualSystematics << std::endl;

        if (ordersystematiccolorbyalphabeth)
	  std::cout << "\t\t Order systematics color by alphabethetical order of names "
                    <<(ordersystematiccolorbyalphabeth ? "ON" : "OFF") << std::endl;

	std::cout << "\t\t X main Legend: " << xLegend << std::endl;
	std::cout << "\t\t Y main Legend: " << yLegend << std::endl;
	std::cout << "\t\t X information Legend: " << xInfoLegend << std::endl;
	std::cout << "\t\t Y information Legend: " << yInfoLegend << std::endl;
	std::cout << "\t\t X Data uncertainty box: " <<  XDataBoxLabel << std::endl;
	std::cout << "\t\t Y Data uncertainty box: " <<  YDataBoxLabel << std::endl;

        if (yOverlayMin!=HUGE_VAL)
	 std::cout << "\t\t Y Overlay Min: " << yOverlayMin << std::endl;
        if (yOverlayMax!=HUGE_VAL)
	 std::cout << "\t\t Y Overlay Max: " << yOverlayMax << std::endl;
        if (xOverlayMin!=HUGE_VAL)
	 std::cout << "\t\t X Overlay Min: " << xOverlayMin << std::endl;
        if (xOverlayMax!=HUGE_VAL)
	 std::cout << "\t\t X Overlay Max: " << xOverlayMax << std::endl;

	std::cout << "\t\t Y Ratio Min: "   << yRatioMin << std::endl;
	std::cout << "\t\t Y Ratio Max: "   << yRatioMax << std::endl << std::endl;

        //if (debug) std::cout<<cn<<"RemoveXbins.size() = "<<RemoveXbins.size()<<std::endl;
        //if (debug) std::cout<<cn<<"DataCutXmax..size()= "<<DataCutXmax.size()<<std::endl;
        //if (debug) std::cout<<cn<<"DataCutXmin..size()= "<<DataCutXmin.size()<<std::endl;

        for (int i=0; i<RemoveXbins.size(); i++) {
	 if (RemoveXbins.at(i)) {
	   std::cout << "\t\t Remove bins IS ON  "<<std::endl;
           if (i<DataCutXmin.size())
	    std::cout << "\t\t DataCutXmin= "<<DataCutXmin.at(i)<<std::endl;
           if (i<DataCutXmax.size())
	    std::cout << "\t\t DataCutXmax= "<<DataCutXmax.at(i)<<std::endl;
         } else {
          std::cout << "\t\t Remove no bins "<<std::endl;
         }
          std::cout << "  "<<std::endl;
        }

        std::cout<<"\t Number of Scale variations:"<<RenScales.size()<<std::endl;
        if (RenScales.size()!=FacScales.size()) {
         std::ostringstream oss;
         oss << cn << "Print ERROR Same number of renormalisation and factorisation scales are needed ! RenScales.size() = "<<RenScales.size()<<" FacScales.size() = "<<FacScales.size();
         throw SPXParseException(oss.str());
        }
        for (int i=0; i<RenScales.size(); i++) {
         std::cout<<"\t\t RenScales["<<i<<"]= "<<RenScales[i]<<std::endl;
        }
        for (int i=0; i<FacScales.size(); i++) {
         std::cout<<"\t\t FacScales["<<i<<"]= "<<FacScales[i]<<std::endl;
        }


	std::cout << "\n \t Plot Configurations" << std::endl;
	std::cout << "\t\t Number of Plots: " << GetNumberOfPlotConfigurations() << std::endl << std::endl;
        if (GetNumberOfPlotConfigurations()==0) {
          std::ostringstream oss;
          oss << cn << "Please, specify either a data_steering_file or a mc_Steering_file " << std::endl << std::endl;

          throw SPXParseException(oss.str());
        }

	for(int i = 0; i < GetNumberOfPlotConfigurations(); i++) {
		std::cout << "\t\t Plot " << i << " Configuration [PLOT_" << i << "]" << std::endl;
 		std::cout << "\t\t Description: " << plotConfigurations[i].GetDescription() << std::endl;

      	        //if (hasdata.at(i)) std::cout << "\n \t Data steering file found !" << std::endl;
      	        //else               std::cout << "\n \t No data steering file found !" << std::endl;
                
	        //if (hasmc.at(i))   std::cout << "\t MC steering file found ! \n"   << std::endl;
	        //else               std::cout << "\t No MC steering file found ! \n"   << std::endl;

		std::cout << "\t\t Plot Type: " << plotConfigurations[i].GetPlotType().ToString() << std::endl;
		std::cout << "\t\t Display Style: " << plotConfigurations[i].GetDisplayStyle().ToString() << std::endl;
		std::cout << "\t\t Overlay Style: " << plotConfigurations[i].GetOverlayStyle().ToString() << std::endl;
		std::cout << "\t\t X Log: " << (plotConfigurations[i].IsXLog() ? "YES" : "NO") << std::endl;
		std::cout << "\t\t Y Log: " << (plotConfigurations[i].IsYLog() ? "YES" : "NO") << std::endl;
		std::cout << "\t\t Ratio Title: " << plotConfigurations[i].GetRatioTitle() << std::endl;
		std::cout << "\t\t Number of Ratios: " << plotConfigurations[i].GetNumberOfRatios() << std::endl;
		for(int j = 0; j < plotConfigurations[i].GetNumberOfRatios(); j++) {
			std::cout << "\t\t\t Ratio Style " << j << ": " << plotConfigurations[i].GetRatioStyle(j).ToString() << std::endl;
			std::cout << "\t\t\t Ratio " << j << ": " << plotConfigurations[i].GetRatio(j) << std::endl;
		}
		std::cout << std::endl;

		std::cout << "\t\t Number of Configuration Instances for Plot " << i << ": " << \
			plotConfigurations[i].GetNumberOfConfigurationInstances() << std::endl << std::endl;

		for(int j = 0; j < plotConfigurations[i].GetNumberOfConfigurationInstances(); j++) {
			SPXPlotConfigurationInstance tmp;
			tmp = plotConfigurations[i].GetPlotConfigurationInstance(j);

			std::cout << "\t\t\t PDF Directory: " << tmp.pdfDirectory << std::endl;
			std::cout << "\t\t\t Data Directory: " << tmp.dataDirectory << std::endl;
			std::cout << "\t\t\t Grid Directory: " << tmp.gridDirectory << std::endl << std::endl;
			std::cout << "\t\t\t Data Steering File " << j << ": " << tmp.dataSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t Grid Steering File " << j << ": " << tmp.gridSteeringFile.GetFilename() << std::endl;
			std::cout << "\t\t\t PDF Steering File "  << j << ": " << tmp.pdfSteeringFile.GetFilename() << std::endl;

                        std::cout <<" " << std::endl;
			std::cout << "\t\t\t Data Marker Style "  << j << ": " << tmp.dataMarkerStyle << std::endl;
			std::cout << "\t\t\t Data Marker Color "  << j << ": " << tmp.dataMarkerColor << std::endl;

                        std::cout <<" " << std::endl;
			std::cout << "\t\t\t Grid Marker Style "  << j << ": " << tmp.gridMarkerStyle << std::endl;
			std::cout << "\t\t\t Grid Fill Style "  << j << ": "   << tmp.gridFillStyle << std::endl;
			std::cout << "\t\t\t Grid Marker Color "  << j << ": " << tmp.gridMarkerColor << std::endl;

                        std::cout <<" " << std::endl;
			std::cout << "\t\t\t PDF Fill Style "     << j << ": " << tmp.pdfFillStyle << std::endl;
			std::cout << "\t\t\t PDF Fill Color "     << j << ": " << tmp.pdfFillColor << std::endl;
			std::cout << "\t\t\t PDF Marker Style "   << j << ": " << tmp.pdfMarkerStyle << std::endl;

			std::cout << "\t\t\t Scale Fill Style "   << j << ": " << tmp.scaleFillStyle << std::endl;
			std::cout << "\t\t\t Scale Fill Color "   << j << ": " << tmp.scaleFillColor << std::endl;
			std::cout << "\t\t\t Scale Marker Style " << j << ": " << tmp.scaleMarkerStyle << std::endl;

			std::cout << "\t\t\t AlphaS Fill Style "  << j << ": " << tmp.alphasFillStyle << std::endl;
			std::cout << "\t\t\t AlphaS Fill Color "  << j << ": " << tmp.alphasFillColor << std::endl;
			std::cout << "\t\t\t AlphaS Marker Style "<< j << ": " << tmp.alphasMarkerStyle << std::endl;

                        std::cout <<" " << std::endl;
			std::cout << "\t\t\t Corrections Fill Style "  << j << ": " << tmp.correctionsFillStyle << std::endl;
			std::cout << "\t\t\t Corrections Fill Color "  << j << ": " << tmp.correctionsFillColor << std::endl;
			std::cout << "\t\t\t Corrections Marker Style "<< j << ": " << tmp.correctionsMarkerStyle << std::endl;
			std::cout << "\t\t\t X Scale " << j << ": " << tmp.xScale << std::endl;
			std::cout << "\t\t\t Y Scale " << j << ": " << tmp.yScale << std::endl << std::endl;
		}
	}
}

unsigned int SPXSteeringFile::ParseNumberOfPlots(void) {
	std::string mn = "ParseNumberOfPlots: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	unsigned int plotNumber = 0;
	bool noMorePlots = false;
	std::string tmp;
	std::string plotSection;

	if(debug) std::cout << cn << mn << "Starting to look for plot sections..." << std::endl;

	do {
		std::ostringstream intStream;
		intStream << "PLOT_" << plotNumber;
		plotSection = intStream.str();
		if(debug) std::cout << cn << mn << "Formed plot section string: " << plotSection << std::endl;
		if(debug) std::cout << cn << mn << "Checking for existence of plot section: " << plotSection << std::endl;

		tmp = reader->Get(plotSection, "data_steering_files", "EMPTY");

		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "plot section: " << plotSection << " was not found. Number of plots found: " << plotNumber << std::endl;
			noMorePlots = true;
			break;
    		} else {
			plotNumber++;
  		        //hasdata.push_back(plotNumber);
			if(debug) std::cout << cn << mn << "plot section: " << plotSection << " found. Current number of plots: " << plotNumber << std::endl;
		}

	} while(!noMorePlots);

	if(debug) std::cout << cn << mn << "Done searching for plots: " << plotNumber << " plots found" << std::endl;

	return plotNumber;
}

unsigned int SPXSteeringFile::ParseNumberOfRatios(unsigned int plotNumber) {
	std::string mn = "ParseNumberOfRatios: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	unsigned int ratioNumber = 0;
	bool noMoreRatios = false;
	std::string tmpStyle;
	std::string tmp;
	std::string plotSection;
	std::string ratioStyle;
	std::string ratio;

	std::ostringstream pss;
	pss << "PLOT_" << plotNumber;
	plotSection = pss.str();

	if(debug) std::cout << cn << mn << "Looking for ratios under plot [" << plotSection << "]" << std::endl;

	do {
		std::ostringstream rsss;
		rsss << "ratio_style_" << ratioNumber;
		ratioStyle = rsss.str();

		std::ostringstream rss;
		rss << "ratio_" << ratioNumber;
		ratio = rss.str();

		if(debug) std::cout << cn << mn << "Checking existence of " << ratioStyle << " AND " << ratio << std::endl;

		tmpStyle = reader->Get(plotSection, ratioStyle, "EMPTY");
		tmp = reader->Get(plotSection, ratio, "EMPTY");

		if(!tmpStyle.compare("EMPTY") || !tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "Ratio " << ratio << " or Ratio Style " << ratioStyle << " was not found. Number of ratios found: " << ratioNumber << std::endl;
			noMoreRatios = true;
			break;
		} else {
			ratioNumber++;
			if(debug) std::cout << cn << mn << "Ratio " << ratio << " found. Current number of ratios: " << ratioNumber << std::endl;
		}

	} while(!noMoreRatios);

	if(debug) std::cout << cn << mn << "Done searching for ratios for plot [" << plotSection << "]: " << ratioNumber << " ratios found" << std::endl;

	return ratioNumber;
}

void SPXSteeringFile::ParsePlotConfigurations(void) {
	std::string mn = "ParsePlotConfigurations: ";

	std::string tmp;
	bool xLog = false;
	bool yLog = false;

	std::string plotSection;
	std::vector<std::string> tmpVector;
	std::map<std::string, std::vector<std::string> > configurations;

	unsigned int numPlots = ParseNumberOfPlots();

	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);


	//Create plot configurations object for all plots found
	for(int i = 0; i < numPlots; i++) {

		tmp.clear();
		tmpVector.clear();
		configurations.clear();

		std::ostringstream intStream;

		//Form the section name from the plot number
		intStream << "plot_" << i;
		plotSection = intStream.str();
		if(debug) std::cout << cn << mn << "Formed plot section string: " << plotSection << std::endl;

		//Get the plot_type
		tmp = reader->Get(plotSection, "plot_type", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "plot_type", "You MUST specify the plot_type");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("plot_type", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[plot_type] = " << configurations["plot_type"].at(0) << std::endl;
		}

		//Get the data_directory
		tmp = reader->Get(plotSection, "data_directory", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No directory specified: Defaulting to \".\"" << std::endl;
			tmp = ".";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				if(debug) {
					std::cout << cn << mn << "data_directory configuration string: " << tmp << " parsed into:" << std::endl;
					for(int j = 0; j < tmpVector.size(); j++) {
						std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
					}
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_directory", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[data_directory] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_directory"]) << std::endl;
		}

		//Get the grid_directory
		tmp = reader->Get(plotSection, "grid_directory", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No directory specified: Defaulting to \".\"" << std::endl;
			tmp = ".";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				if(debug) {
					std::cout << cn << mn << "grid_directory configuration string: " << tmp << " parsed into:" << std::endl;
					for(int j = 0; j < tmpVector.size(); j++) {
						std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
					}
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_directory", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[grid_directory] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_directory"]) << std::endl;
		}

		//Get the pdf_directory
		tmp = reader->Get(plotSection, "pdf_directory", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No directory specified: Defaulting to \".\"" << std::endl;
			tmp = ".";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				if(debug) {
					std::cout << cn << mn << "pdf_directory configuration string: " << tmp << " parsed into:" << std::endl;
					for(int j = 0; j < tmpVector.size(); j++) {
						std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
					}
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_directory", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[pdf_directory] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_directory"]) << std::endl;
		}

		//Get the data_steering_files
		tmp = reader->Get(plotSection, "data_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		  //throw SPXINIParseException(plotSection, "data_steering_files", "You MUST specify the data_steering_files");
		 std::cout<<"INFO no data_Steering_file found "<<std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_steering_files", tmpVector));

			if(debug) std::cout << cn << mn << "configurations[data_steering_files] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_steering_files"]) << std::endl;
		}

		//Get the grid_steering_files
		tmp = reader->Get(plotSection, "grid_steering_files", "EMPTY");

		if(!tmp.compare("EMPTY") ) {
		  //throw SPXINIParseException(plotSection, "grid_steering_files", "You MUST specify the grid_steering_files");
		  std::cout<<cn<<mn<<"INFO no grid_steering_file found "<<std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "grid_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
                        //if (configurations.count("grid_steering_files")>0) {
                        // std::cout<<cn<<mn<<
                        //} else {
			 configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_steering_files", tmpVector));
			 //}
			if(debug) std::cout << cn << mn << "configurations[grid_steering_files] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_steering_files"]) << std::endl;
		}

		//Get the pdf_steering_files
		tmp = reader->Get(plotSection, "pdf_steering_files", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		  //throw SPXINIParseException(plotSection, "pdf_steering_files", "You MUST specify the pdf_steering_files");
		  std::cout<<cn<<mn<<"INFO no pdf_steering_file found "<<std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_steering_files configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_steering_files", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_steering_files] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_steering_files"]) << std::endl;
		}


       	        if(debug) std::cout << cn << mn << "Start parsing grid_fill_style " << std::endl;
		//Get the grid_fill_style
		tmp = reader->Get(plotSection, "grid_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			std::cout << cn << mn << "INFO: No plot option for grid_fill_style found: Defaulting to pre-defined settings (" << DEFAULT_GRID_FILL_STYLE << ")" << std::endl;
		} else {
			//Parse into vector
      		        set_grid_styles=true;
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "grid_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_fill_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[grid_fill_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_fill_style"]) << std::endl;
		}

       	        if(debug) std::cout << cn << mn << "Start parsing grid_marker_style " << std::endl;
		//Get the grid_marker_style
		tmp = reader->Get(plotSection, "grid_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			std::cout << cn << mn << "INFO: No plot option for grid_marker_style found: Defaulting to pre-defined settings (" << DEFAULT_GRID_MARKER_STYLE << ")" << std::endl;
		} else {
			//Parse into vector
      		        set_grid_styles=true;
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "grid_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_marker_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[grid_marker_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_marker_style"]) << std::endl;
		}

       	        if(debug) std::cout << cn << mn << "Start parsing grid_marker_color " << std::endl;
		//Get the grid_marker_color
		tmp = reader->Get(plotSection, "grid_marker_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			std::cout << cn << mn << "INFO: No plot option for grid_marker_color found: Defaulting to pre-defined settings (" << DEFAULT_GRID_MARKER_STYLE << ")" << std::endl;
		} else {
			//Parse into vector
      		        set_grid_styles=true;
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "grid_marker_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("grid_marker_color", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[grid_marker_color] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["grid_marker_color"]) << std::endl;
		}

                // Read in cuts on xmin and xmax in data  
		// 

       	        if(debug) std::cout << cn << mn << "Start parsing data_marker_style " << std::endl;
		//Get the data_marker_style
		tmp = reader->Get(plotSection, "data_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			std::cout << cn << mn << "INFO: No plot option for data_marker_style found: Defaulting to pre-defined settings (" << DEFAULT_DATA_MARKER_STYLE << ")" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_marker_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[data_marker_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_marker_style"]) << std::endl;
		}

                // Read in cuts on xmin and xmax in data  
		// 

       	        if(debug) std::cout<<cn<<mn<<"Start parsing data_cut_xmin " << std::endl;

		tmp = reader->Get(plotSection, "data_cut_xmin", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout<<cn<<mn<<"INFO: No plot option for data_cut_xmin found"<< std::endl;
		} else {
		 //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		 if(debug) std::cout<<cn<<mn<<"data_cut_xmin configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		  DataCutXmin.push_back(atoi(tmpVector.at(j).c_str()));
		  if (debug) std::cout<<cn<<mn<< "\t" << tmpVector.at(j).c_str() << std::endl;
		 }
		}

       	        if(debug) std::cout<<cn<<mn<<"Start parsing data_cut_xmax " << std::endl;

		tmp = reader->Get(plotSection, "data_cut_xmax", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout<<cn<<mn<<"INFO: No plot option for data_cut_xmax found"<< std::endl;
		} else {
		 //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		 if(debug) std::cout<<cn<<mn<<"data_cut_xmax configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		  DataCutXmax.push_back(atoi(tmpVector.at(j).c_str()));
		  if (debug) std::cout<<cn<<mn<< "\t" << tmpVector.at(j).c_str() << std::endl;
		 }
		}

		if (DataCutXmax.size()!=DataCutXmin.size() && !(DataCutXmax.size()==0||DataCutXmin.size()==0)) {
		    std::cout<<cn<<mn<<" DataCutXmax.size()= "<<DataCutXmax.size() 
                                     <<" DataCutXmin.size()= "<<DataCutXmin.size() 
			             <<" should not be different unless one of them is zero "
                                     << std::endl;
                }
                if (DataCutXmax.size()==0) {
		 for(int j = 0; j < DataCutXmin.size(); j++) {
		  RemoveXbins.push_back(true);
		 }
                }
                if (DataCutXmin.size()==0) {
		 for(int j = 0; j < DataCutXmax.size(); j++) {
		  RemoveXbins.push_back(true);
		 }
		}
                if (DataCutXmin.size()==DataCutXmin.size()) {
		 for(int j = 0; j < DataCutXmax.size(); j++) {
		  RemoveXbins.push_back(true);
		 }
                }
	        
                // 
		//Get the systematic class
		//

       	        if(debug) std::cout<<cn<<mn<<"Start parsing display_systematic_group " << std::endl;

		tmp = reader->Get(plotSection, "display_systematic_group", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout<<cn<<mn<<"INFO: No plot option for display_systematic_group found"<< std::endl;
		} else {
		 //Parse into vector
		 systematicsclasses = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		 if(debug) {
		  std::cout<<cn<<mn<<"display_systematic_group configuration string: " << tmp << " parsed into:" << std::endl;
		  for(int j = 0; j < systematicsclasses.size(); j++) {
		   std::cout<<cn<<mn<< "\t" << systematicsclasses[j] << std::endl;
		  }
		 }
		}

                if(debug) std::cout<<cn<<mn<<"Start parsing display_systematic_group_color " << std::endl;
		tmp = reader->Get(plotSection, "display_systematic_group_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout<<cn<<mn<<"INFO: No plot option for display_systematic_group_color found"<< std::endl;
		} else {	       
		 //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		 if(debug) {
		  std::cout<<cn<<mn<<"display_systematic_group_color configuration string: " << tmp << " parsed into:" << std::endl;
                 }
		  for(int j = 0; j < tmpVector.size(); j++) {
		   if (debug) std::cout<<cn<<mn<< "\t" << tmpVector[j] << std::endl;
                   systematicsclassescolor.push_back( atoi(tmpVector.at(j).c_str()));	       
		 }
		}


		// 
		//Get the data_marker_color
       	        if(debug) std::cout << cn << mn << "Start parsing data_marker_color " << std::endl;
		tmp = reader->Get(plotSection, "data_marker_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for data_marker_color found: Defaulting to pre-defined settings (" << DEFAULT_DATA_MARKER_COLOR << ")" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "data_marker_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("data_marker_color", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[data_marker_color] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["data_marker_color"]) << std::endl;
		}

		//Get the pdf_fill_style
       	        if(debug) std::cout << cn << mn << "Start parsing pdf_fill_style " << std::endl;
		tmp = reader->Get(plotSection, "pdf_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 if(debug) std::cout << cn << mn << "No plot option for pdf_fill_style found using defaults " << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_fill_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_fill_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_fill_style"]) << std::endl;
		}

		//Get the pdf_fill_color
       	        if(debug) std::cout << cn << mn << "Start parsing pdf_fill_color " << std::endl;
		tmp = reader->Get(plotSection, "pdf_fill_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 if(debug) std::cout << cn << mn << "No plot options for pdf_fill_color found" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_fill_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_fill_color", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_fill_color] = " << \
			SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_fill_color"]) << std::endl;

		}

		//Get the pdf_edge_color
       	        if(debug) std::cout << cn << mn << "Start parsing pdf_edge_color " << std::endl;
		tmp = reader->Get(plotSection, "pdf_edge_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 if(debug) std::cout << cn << mn << "No plot options for pdf_edge_color found" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_edge_color configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_edge_color", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_edge_color] = " << \
			SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_edge_color"]) << std::endl;

		}

		//Get the pdf_edge_style
       	        if(debug) std::cout << cn << mn << "Start parsing pdf_edge_style " << std::endl;
		tmp = reader->Get(plotSection, "pdf_edge_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 if(debug) std::cout << cn << mn << "No plot options for pdf_edge_style found" << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_edge_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_edge_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_edge_style] = " << \
			SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_edge_style"]) << std::endl;

		}

		//Get the pdf_marker_style
       	        if(debug) std::cout << cn << mn << "Start parsing pdf_marker_style " << std::endl;
		tmp = reader->Get(plotSection, "pdf_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 if(debug) std::cout << cn << mn << "No plot option for pdf_marker_style found using defaults " << std::endl;
		} else {
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "pdf_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}


			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("pdf_marker_style", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[pdf_marker_style] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["pdf_marker_style"]) << std::endl;
		}

		//Get the total_fill_style
       	        if(debug) std::cout << cn << mn << "Start parsing total_fill_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "total_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for total_fill_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_style" << std::endl;

                 if (configurations.count("pdf_fill_style")!=0)
		  tmpVector = configurations["pdf_fill_style"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "total_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}
		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("total_fill_style", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[total_fill_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["total_fill_style"]) << std::endl;
                }

		//Get the total_fill_color
       	        if(debug) std::cout << cn << mn << "Start parsing total_fill_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "total_fill_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for total_fill_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_color" << std::endl;

                 if (configurations.count("pdf_fill_color")!=0)
		  tmpVector = configurations["pdf_fill_color"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "total_fill_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("total_fill_color", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[total_fill_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["total_fill_color"]) << std::endl;
                }

		//Get the total_edge_style
       	        if(debug) std::cout << cn << mn << "Start parsing total_edge_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "total_edge_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for total_edge_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_style" << std::endl;

                 if (configurations.count("pdf_edge_style")!=0)
		  tmpVector = configurations["pdf_edge_style"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "total_edge_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}
		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("total_edge_style", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[total_edge_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["total_edge_style"]) << std::endl;
                }

		//Get the total_edge_color
       	        if(debug) std::cout << cn << mn << "Start parsing total_edge_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "total_edge_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for total_edge_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_edge_color" << std::endl;

                 if (configurations.count("pdf_edge_color")!=0)
		  tmpVector = configurations["pdf_edge_color"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "total_edge_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("total_edge_color", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[total_edge_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["total_edge_color"]) << std::endl;
                }

		//Get the total_marker_style
       	        if(debug) std::cout << cn << mn << "Start parsing total_marker_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "total_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for total_marker_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_marker_style" << std::endl;


		 if (configurations.count("pdf_marker_style")!=0)
		  tmpVector = configurations["pdf_marker_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "total_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
                if (tmpVector.size()!=0) {		
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("total_marker_style", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[total_marker_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["total_marker_style"]) << std::endl;		
                }

		//-----
		//Get the scale_fill_color
       	        if(debug) std::cout << cn << mn << "Start parsing scale_edge_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "scale_edge_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for scale_edge_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_edge_color" << std::endl;

		if (configurations.count("pdf_edge_color")!=0)
		 tmpVector = configurations["pdf_edge_color"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "scale_edge_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("scale_edge_color", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[scale_edge_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["scale_edge_color"]) << std::endl;
                }

		//Get the scale_edge_style
       	        if(debug) std::cout << cn << mn << "Start parsing scale_edge_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "scale_edge_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for scale_edge_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_edge_style" << std::endl;

		if (configurations.count("pdf_edge_style")!=0)
		 tmpVector = configurations["pdf_edge_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "scale_edge_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}
 
		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("scale_edge_style", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[scale_edge_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["scale_edge_style"]) << std::endl;
                }
		//-------

		//Get the scale_fill_color
       	        if(debug) std::cout << cn << mn << "Start parsing scale_fill_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "scale_fill_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for scale_fill_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_color" << std::endl;

		if (configurations.count("pdf_fill_color")!=0)
		 tmpVector = configurations["pdf_fill_color"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "scale_fill_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("scale_fill_color", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[scale_fill_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["scale_fill_color"]) << std::endl;
                }

		//Get the scale_fill_style
       	        if(debug) std::cout << cn << mn << "Start parsing scale_fill_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "scale_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for scale_fill_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_color" << std::endl;

		if (configurations.count("pdf_fill_style")!=0)
		 tmpVector = configurations["pdf_fill_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "scale_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}
 
		//Add to configurations map
                if (tmpVector.size()!=0) {
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("scale_fill_style", tmpVector));
		 if(debug) std::cout << cn << mn << "configurations[scale_fill_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["scale_fill_style"]) << std::endl;
                }


		//Get the scale_marker_style
       	        if(debug) std::cout << cn << mn << "Start parsing scale_marker_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "scale_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for scale_marker_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_marker_style" << std::endl;

		if (configurations.count("pdf_marker_style")!=0)
		 tmpVector = configurations["pdf_marker_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "scale_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("scale_marker_style", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[scale_marker_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["scale_marker_style"]) << std::endl;

		//Get the alphas_fill_style
       	        if(debug) std::cout << cn << mn << "Start parsing alphas_fill_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "alphas_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for alphas_fill_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_style" << std::endl;

		if (configurations.count("pdf_fill_style")!=0)
		 tmpVector = configurations["pdf_fill_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "alphas_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}
 
		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("alphas_fill_style", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[alphas_fill_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["alphas_fill_style"]) << std::endl;

		//------

		//Get the alphas_edge_color
       	        if(debug) std::cout << cn << mn << "Start parsing alphas_edge_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "alphas_edge_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for alphas_edge_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_edge_color" << std::endl;

		if (configurations.count("pdf_edge_color")!=0)
		 tmpVector = configurations["pdf_edge_color"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "alphas_edge_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("alphas_edge_color", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[alphas_edge_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["alphas_edge_color"]) << std::endl;


		//------

		//Get the alphas_fill_color
       	        if(debug) std::cout << cn << mn << "Start parsing alphas_fill_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "alphas_fill_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for alphas_fill_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_color" << std::endl;

		if (configurations.count("pdf_fill_color")!=0)
		 tmpVector = configurations["pdf_fill_color"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "alphas_fill_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("alphas_fill_color", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[alphas_fill_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["alphas_fill_color"]) << std::endl;

		//Get the alphas_marker_style
       	        if(debug) std::cout << cn << mn << "Start parsing alphas_marker_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "alphas_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for alphas_marker_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_marker_style" << std::endl;

		if (configurations.count("pdf_marker_style")!=0)
		 tmpVector = configurations["pdf_marker_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "alphas_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
  		 configurations.insert(std::pair<std::string, std::vector<std::string> >("alphas_marker_style", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[alphas_marker_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["alphas_marker_style"]) << std::endl;


		//Get the corrections_fill_style
       	        if(debug) std::cout << cn << mn << "Start parsing corrections_fill_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "corrections_fill_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for corrections_fill_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_style" << std::endl;

		if (configurations.count("pdf_fill_style")!=0)
		 tmpVector = configurations["pdf_fill_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "corrections_fill_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}
 
		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("corrections_fill_style", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[corrections_fill_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["corrections_fill_style"]) << std::endl;


		//------
		//Get the corrections_edge_color
       	        if(debug) std::cout << cn << mn << "Start parsing corrections_edge_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "corrections_edge_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for corrections_edge_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_edge_color" << std::endl;

		if (configurations.count("pdf_edge_color")!=0)
		 tmpVector = configurations["pdf_edge_color"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "corrections_edge_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("corrections_edge_color", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[corrections_edge_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["corrections_edge_color"]) << std::endl;

		//------

		//Get the corrections_fill_color
       	        if(debug) std::cout << cn << mn << "Start parsing corrections_fill_color " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "corrections_fill_color", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		 std::cout << cn << mn << "INFO: No plot option for corrections_fill_color found, but plot_band = true: Defaulting to pdf steering file settings pdf_fill_color" << std::endl;

		if (configurations.count("pdf_fill_color")!=0)
		 tmpVector = configurations["pdf_fill_color"];
		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "corrections_fill_color configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
		 configurations.insert(std::pair<std::string, std::vector<std::string> >("corrections_fill_color", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[corrections_fill_color] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["corrections_fill_color"]) << std::endl;

		//Get the corrections_marker_style
       	        if(debug) std::cout << cn << mn << "Start parsing corrections_marker_style " << std::endl;
		tmpVector.clear();
		tmp = reader->Get(plotSection, "corrections_marker_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
		  std::cout << cn << mn << "INFO: No plot option for corrections_marker_style found, but plot_band = true: Defaulting to pdf steering file settings pdf_marker_style "<< std::endl;

		if (configurations.count("pdf_marker_style")!=0)
		 tmpVector = configurations["pdf_marker_style"];

		} else {
		   //Parse into vector
		 tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
		}

		if(debug) {
		 std::cout << cn << mn << "corrections_marker_style configuration string: " << tmp << " parsed into:" << std::endl;
		 for(int j = 0; j < tmpVector.size(); j++) {
		   std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
		 }
		}

		//Add to configurations map
		if (tmpVector.size()!=0)
  		 configurations.insert(std::pair<std::string, std::vector<std::string> >("corrections_marker_style", tmpVector));
		if(debug) std::cout << cn << mn << "configurations[corrections_marker_style] = " << SPXStringUtilities::VectorToCommaSeparatedList(configurations["corrections_marker_style"]) << std::endl;
	       

		//Get the x_scale
       	        if(debug) std::cout << cn << mn << "Start parsing x_scale " << std::endl;
		tmp = reader->Get(plotSection, "x_scale", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No plot option for x_scale was specified" << std::endl;
			tmp = "1.0";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "x_scale configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("x_scale", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[x_scale] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["x_scale"]) << std::endl;
		}

///---------------------------


///---------------------------

		//Get the y_scale
       	        if(debug) std::cout << cn << mn << "Start parsing y_scale " << std::endl;
		tmp = reader->Get(plotSection, "y_scale", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "No plot option for y_scale was specified" << std::endl;
			tmp = "1.0";
		}
		{
			//Parse into vector
			tmpVector = SPXStringUtilities::CommaSeparatedListToVector(tmp);
			if(debug) {
				std::cout << cn << mn << "y_scale configuration string: " << tmp << " parsed into:" << std::endl;
				for(int j = 0; j < tmpVector.size(); j++) {
					std::cout << cn << mn << "\t" << tmpVector[j] << std::endl;
				}
			}

			if(Test::TestFeatures) std::cin.get();

			//Add to configurations map
			configurations.insert(std::pair<std::string, std::vector<std::string> >("y_scale", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[y_scale] = " << \
				SPXStringUtilities::VectorToCommaSeparatedList(configurations["y_scale"]) << std::endl;
		}

		//Get the x_log
       	        if(debug) std::cout << cn << mn << "Start parsing x_log " << std::endl;
		tmp = reader->Get(plotSection, "x_log", "EMPTY");
		if(tmp.compare("EMPTY") != 0) {
			xLog = reader->GetBoolean(plotSection, "x_log", false);
			if(debug) std::cout << cn << mn << "X Log successfully parsed: " << (xLog ? "ON" : "OFF") << std::endl;
		} else {
			xLog = false;
			if(debug) std::cout << cn << mn << "X Log not specified: defaulting to OFF" << std::endl;
		}
		{
			if(xLog) {
				tmp = std::string("true");
			} else {
				tmp = std::string("false");
			}

			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("x_log", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[x_log] = " << configurations["x_log"].at(0) << std::endl;
		}

		//Get the y_log
		tmp = reader->Get(plotSection, "y_log", "EMPTY");
		if(tmp.compare("EMPTY") != 0) {
			yLog = reader->GetBoolean(plotSection, "y_log", false);
			if(debug) std::cout << cn << mn << "Y Log successfully parsed: " << (yLog ? "ON" : "OFF") << std::endl;
		} else {
			yLog = false;
			if(debug) std::cout << cn << mn << "Y Log not specified: defaulting to OFF" << std::endl;
		}
		{
			if(yLog) {
				tmp = std::string("true");
			} else {
				tmp = std::string("false");
			}

			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("y_log", tmpVector));
			if(debug) std::cout << cn << mn << "configurations[y_log] = " << configurations["y_log"].at(0) << std::endl;
		}

		//Get the desc
		tmp = reader->Get(plotSection, "desc", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "desc", "You MUST specify the desc");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("desc", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[desc] = " << configurations["desc"].at(0) << std::endl;
		}

		//Get the display_style
       	        if(debug) std::cout << cn << mn << "Start parsing display_style " << std::endl;
		tmp = reader->Get(plotSection, "display_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "display_style", "You MUST specify the display_style");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("display_style", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[display_style] = " << configurations["display_style"].at(0) << std::endl;
		}

		//Get the overlay_style
       	        if(debug) std::cout << cn << mn << "Start parsing overlay_style " << std::endl;
		tmp = reader->Get(plotSection, "overlay_style", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			throw SPXINIParseException(plotSection, "overlay_style", "You MUST specify the overlay_style");
		} else {
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("overlay_style", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[overlay_style] = " << configurations["overlay_style"].at(0) << std::endl;
		}

		//Get the ratio_title
		tmp = reader->Get(plotSection, "ratio_title", "EMPTY");
		if(!tmp.compare("EMPTY")) {
			if(debug) std::cout << cn << mn << "ratio_title was not specified: Defaulting to \"Ratio\"" << std::endl;
			tmp = std::string("Ratio");
		}
		{
			//Add to configurations map
			tmpVector.clear();
			tmpVector.push_back(tmp);
			configurations.insert(std::pair<std::string, std::vector<std::string> >("ratio_title", tmpVector));
			tmpVector.clear();
			if(debug) std::cout << cn << mn << "configurations[ratio_title] = " << configurations["ratio_title"].at(0) << std::endl;
		}

		//Get the ratio styles and ratios
		unsigned int numberOfRatios = ParseNumberOfRatios(i);
		std::string ratioStyle;
		std::string ratio;
		std::vector<std::string> rsVector;
		std::vector<std::string> rVector;

		if(debug) std::cout << cn << mn << "numberOfRatios = " << numberOfRatios << std::endl;

		for(int j = 0; j < numberOfRatios; j++) {

			std::ostringstream rsss;
			std::ostringstream rss;
			rsss << "ratio_style_" << j;
			rss << "ratio_" << j;
			ratioStyle = rsss.str();
			ratio = rss.str();

			if(debug) std::cout << cn << mn << "Getting [" << plotSection << "]: " << ratioStyle << std::endl;
			tmp = reader->Get(plotSection, ratioStyle, "EMPTY");
			if(debug) std::cout << "\t Found: " << tmp << std::endl;
			rsVector.push_back(tmp);

			if(debug) std::cout << cn << mn << "Getting [" << plotSection << "]: " << ratio << std::endl;
			tmp = reader->Get(plotSection, ratio, "EMPTY");
			if(debug) std::cout << "\t Found: " << tmp << std::endl;
			rVector.push_back(tmp);
		}

		//Insert the ratio and ratio styles vectors into the map
		configurations.insert(std::pair<std::string, std::vector<std::string> >("ratio_style", rsVector));
		configurations.insert(std::pair<std::string, std::vector<std::string> >("ratio", rVector));

		//Create a new plotConfigurations object with configurations vector (plotconfigurations constructor will parse it)
		try {
			SPXPlotConfiguration pc = SPXPlotConfiguration(configurations, i);

			//Make sure it is valid and non-empty, and add it to plot vector
			if(!pc.IsEmpty() && pc.IsValid()) {
				plotConfigurations.push_back(pc);
				if(debug) std::cout << cn << mn << "Successfully added " << plotSection << " to the plotConfigurations vector" << std::endl;
			} else {
				throw SPXINIParseException(plotSection, "SPXPlotConfiguration object is empty or invalid");
			}
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			std::ostringstream oss;
			oss << "[PLOT_" << i << "]";
			throw SPXParseException("Unable to parse plot configuration: " + oss.str());
		}
	}

	if(debug) std::cout << cn << mn << "Successfully parsed and added " << numPlots << " plot configurations" << std::endl;
}

void SPXSteeringFile::Parse(void) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(filename.empty()) {
	 //throw SPXFileIOException(filename, "Empty file string \"\" was given");
	 std::cout<<cn<<mn<<"WARNING: no steering file given, do not know what to do, return "<<std::endl;
         return;
	}

	//Initialize reader
	reader = new INIReader(filename);

	if(reader->ParseError() < 0) {
		delete reader;

    	throw SPXFileIOException(filename, "INIReader::INIReader(): ParseError generated when parsing file");
	}

	if(debug) std::cout << cn << mn << " INIReader done " << std::endl;

	std::string tmp;

	//General configurations [GEN]
	debug = reader->GetBoolean("GEN", "debug", debug);

	//Set Defaults
	this->SetDefaults();

	//Enable all debug configurations if debug is on
	if(debug) {
		std::cout << cn << mn << "Debug is ON" << std::endl;
		SPXPlotType::SetDebug(true);
		SPXPlotConfiguration::SetDebug(true);
		SPXRatioStyle::SetDebug(true);
		SPXOverlayStyle::SetDebug(true);
		SPXDisplayStyle::SetDebug(true);
		SPXPDFBandType::SetDebug(true);
		SPXPDFErrorType::SetDebug(true);
		SPXPDFErrorSize::SetDebug(true);
	}

	//Graphing configurations [GRAPH]
	plotBand = reader->GetBoolean("GRAPH", "plot_band", plotBand);

	BandwithPDF    = reader->GetBoolean("GRAPH", "band_with_pdf", BandwithPDF);
	if (debug&& BandwithPDF) std::cout << cn << mn << "BandwithPDF is ON" << std::endl;

	BandwithAlphaS = reader->GetBoolean("GRAPH", "band_with_alphas", BandwithAlphaS);
	if (debug&& BandwithAlphaS) std::cout << cn << mn << "BandwithAlphaS is ON" << std::endl;

	BandwithScales = reader->GetBoolean("GRAPH", "band_with_scale", BandwithScales);
	if (debug&& BandwithScales) std::cout << cn << mn << "BandwithScales is ON" << std::endl;


	BandTotal      = reader->GetBoolean("GRAPH", "band_total", BandTotal);
	if (debug&& BandTotal) std::cout << cn << mn << "BandTotal is ON" << std::endl;

	//Get grid correction flags

	tmp = reader->Get("GRAPH","band_with_gridcorrection", "EMPTY");
	if(!tmp.compare("EMPTY")) {
	 if(debug) std::cout << cn << mn << "No grid correction included in band " << std::endl;       
	}else{
	 //Parse into vector
         gridcorrections.clear();
         gridcorrections=SPXStringUtilities::ParseStringToBooleanVector(tmp,',');
        }

	if(debug) {
	 if(gridcorrections.size()>0) {
	  std::cout << cn << mn << "band_with_correction number=" << gridcorrections.size() << " parsed into:" << std::endl;
	  for(int j = 0; j < gridcorrections.size(); j++) {
	   std::cout << cn << mn << "\t" << gridcorrections[j] << std::endl;
	  }
         }
	}

	//Get the renormalisation scale
	tmp = reader->Get("GRAPH","ren_scales", "EMPTY");
	if(!tmp.compare("EMPTY")) {
	 if(debug) std::cout << cn << mn << "No ren_scales given using defaults " << std::endl;
         SetDefaultScales();         
	}else{
	 //Parse into vector
         RenScales.clear();
         //std::vector<std::string> tmpRenScales;
	 //tmpRenScales = SPXStringUtilities::CommaSeparatedListToVector(tmp);
         RenScales=SPXStringUtilities::ParseStringToDoubleVector(tmp,',');
        }

	if(debug) {
	 std::cout << cn << mn << "RenScales: number=" << RenScales.size() << " parsed into:" << std::endl;
	 for(int j = 0; j < RenScales.size(); j++) {
	  std::cout << cn << mn << "\t" << RenScales[j] << std::endl;
	 }
	}

	tmp = reader->Get("GRAPH", "fac_scales", "EMPTY");
	if(!tmp.compare("EMPTY")) {
	 if(debug) std::cout << cn << mn << "No fac_scales given using defaults " << std::endl;
         SetDefaultScales();         
	}else{
	 //Parse into vector
         FacScales.clear();
         //std::vector<std::string> tmpFacScales;
	 //tmpFacScales = SPXStringUtilities::CommaSeparatedListToVector(tmp);
         FacScales=SPXStringUtilities::ParseStringToDoubleVector(tmp,',');

         if (RenScales.size()!=FacScales.size()) {
          std::ostringstream oss;
          oss << cn<< mn << "ERROR Same number of renormalisation and factorisation scales are needed ! RenScales.size() = "<<RenScales.size()<<" FacScales.size() = "<<FacScales.size();
          throw SPXParseException(oss.str());
         }
	}

	if(debug) {
	 std::cout << cn << mn << "FacScales: number= " << FacScales.size() << " parsed into:" << std::endl;
	 for(int j = 0; j < FacScales.size(); j++) {
	  std::cout << cn << mn << "\t" << FacScales[j] << std::endl;
	 }
        }

	addonLegendNLOProgramName = reader->GetBoolean("GRAPH", "add_onlegend_nlo_program_name", addonLegendNLOProgramName);

	plotErrorTicks = reader->GetBoolean("GRAPH", "plot_error_ticks", plotErrorTicks);
	plotMarker     = reader->GetBoolean("GRAPH", "plot_marker", plotMarker);
	plotStaggered  = reader->GetBoolean("GRAPH", "plot_staggered", plotStaggered);
	matchBinning   = reader->GetBoolean("GRAPH", "match_binning", matchBinning);
	TakeSignforTotalError = reader->GetBoolean("GRAPH", "take_sign_intoaccount_for_total_error", TakeSignforTotalError);

	printTotalSigma = reader->GetBoolean("GRAPH", "print_total_sigma", printTotalSigma);

	gridCorr       = reader->GetBoolean("GRAPH", "apply_grid_corr", gridCorr);
	labelSqrtS     = reader->GetBoolean("GRAPH", "label_sqrt_s", labelSqrtS);

        showIndividualSystematics = reader->GetReal("GRAPH", "show_individual_systematics", showIndividualSystematics);

        ordersystematiccolorbyalphabeth = reader->GetBoolean("GRAPH", "order_systematic_colorbyalphabeth", ordersystematiccolorbyalphabeth);

	xLegend = reader->GetReal("GRAPH", "x_legend", xLegend);
	yLegend = reader->GetReal("GRAPH", "y_legend", yLegend);

	xInfoLegend = reader->GetReal("GRAPH", "x_info_legend", xInfoLegend);
	yInfoLegend = reader->GetReal("GRAPH", "y_info_legend", yInfoLegend);

	XDataBoxLabel = reader->GetReal("GRAPH", "x_databox", XDataBoxLabel);
	YDataBoxLabel = reader->GetReal("GRAPH", "y_databox", YDataBoxLabel);


	xOverlayMin = reader->GetReal("GRAPH", "x_overlay_min", xOverlayMin);
	xOverlayMax = reader->GetReal("GRAPH", "x_overlay_max", xOverlayMax);

	yOverlayMin = reader->GetReal("GRAPH", "y_overlay_min", yOverlayMin);
	yOverlayMax = reader->GetReal("GRAPH", "y_overlay_max", yOverlayMax);
	yRatioMin   = reader->GetReal("GRAPH", "y_ratio_min", yRatioMin);
	yRatioMax   = reader->GetReal("GRAPH", "y_ratio_max", yRatioMax);


	CalculateChi2 = reader->GetInteger("GRAPH", "calculate_chi2", CalculateChi2);

	DumpTables = reader->GetInteger("GRAPH", "dump_latex_tables", DumpTables);

	AddLumi       = reader->GetBoolean("GRAPH", "label_lumi", AddLumi);
	AddJournal    = reader->GetBoolean("GRAPH", "label_journalreference", AddJournal);
	AddJournalYear= reader->GetBoolean("GRAPH", "label_journalyear", AddJournalYear);

	InfoLegendLabel= reader->Get("GRAPH", "label_informationlegend",InfoLegendLabel);

	if(debug) std::cout << cn << mn << "Start with ParsePlotConfigurations " << std::endl;

	//Attempt to parse plot configurations
	try {
		this->ParsePlotConfigurations();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXParseException("Could not parse plots: Verify correct plot configuration syntax");
	}
}

void SPXSteeringFile::PrintDataSteeringFiles(void) {
        std::cout<<" "<<std::endl;
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {
			SPXDataSteeringFile &dataSteeringFile = plotConfigurations.at(i).GetPlotConfigurationInstance(j).dataSteeringFile;
			dataSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParseDataSteeringFiles(void) {
	std::string mn = "ParseDataSteeringFiles: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Loop through all plot configurations instances for each plot option
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {

			SPXPlotConfigurationInstance &pci = plotConfigurations.at(i).GetPlotConfigurationInstance(j);
			SPXDataSteeringFile &dataSteeringFile = pci.dataSteeringFile;

			//Attempt to parse the Data Steering File
			try {
				dataSteeringFile.Parse();

				//Prepend the data directory onto the steering file's data path
				dataSteeringFile.PrependDataFile(pci.dataDirectory);
				if(debug) std::cout << cn << mn << "Successfully prepended directory \"" << pci.dataDirectory << "\" onto data file" << std::endl;
				if(debug) std::cout << cn << mn << "Resulting data filepath: \"" << dataSteeringFile.GetDataFile() << "\"" << std::endl;

				//Use default marker style or marker color if currently empty
				if(pci.gridMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						    " Grid Marker Style was empty: Defaulting to " << DEFAULT_GRID_MARKER_STYLE << std::endl;
					pci.gridMarkerStyle = DEFAULT_GRID_MARKER_STYLE;	
				}

				if(pci.gridMarkerColor == PC_EMPTY_STYLE) {
				  if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << " Grid Marker Color was empty: Defaulting to " << DEFAULT_GRID_MARKER_COLOR << std::endl;
					pci.gridMarkerColor = DEFAULT_GRID_MARKER_COLOR;	
				}

				//Use default fill style 
				if(pci.gridFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						    " Grid Marker Style was empty: Defaulting to" <<DEFAULT_GRID_FILL_STYLE << std::endl;
					pci.gridFillStyle = DEFAULT_GRID_FILL_STYLE;	
				}


				//Use default marker style or marker color if currently empty
				if(pci.dataMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Data Marker Style was empty: Defaulting to Circle (20)" << std::endl;
					pci.dataMarkerStyle = DEFAULT_DATA_MARKER_STYLE;	//DEFAULT TO 20 (circle)
				}

				if(pci.dataMarkerColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Data Marker Color was empty: Defaulting to Black (1)" << std::endl;
					pci.dataMarkerColor = DEFAULT_DATA_MARKER_COLOR;	//DEFAULT TO BLACK
				}

			//Update PCI with new data
			SPXPlotConfigurationInstance &pcim = plotConfigurations.at(i).GetPlotConfigurationInstance(dataSteeringFile.GetFilename());

			pcim.gridMarkerStyle = pci.gridMarkerStyle;
			pcim.gridMarkerColor = pci.gridMarkerColor;
			pcim.gridFillStyle = pci.gridFillStyle;

			pcim.dataMarkerStyle = pci.dataMarkerStyle;
			pcim.dataMarkerColor = pci.dataMarkerColor;

			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse the Data Steering File: " << dataSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}
}

void SPXSteeringFile::PrintGridSteeringFiles(void) {
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {
			SPXGridSteeringFile &gridSteeringFile = plotConfigurations.at(i).GetPlotConfigurationInstance(j).gridSteeringFile;
			gridSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParseGridSteeringFiles(void) {
	std::string mn = "ParseGridSteeringFiles: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Loop through all plot configurations instances for each plot option
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {

			SPXPlotConfigurationInstance &pci = plotConfigurations.at(i).GetPlotConfigurationInstance(j);
			SPXGridSteeringFile &gridSteeringFile = pci.gridSteeringFile;

			//Attempt to parse the Grid Steering File
			try {
				gridSteeringFile.Parse();

				gridSteeringFile.PrependGridFilepath(pci.gridDirectory);
				if(debug) std::cout << cn << mn << "Successfully prepended directory \"" << pci.gridDirectory << "\" onto grid file" << std::endl;
				if(debug) std::cout << cn << mn << "Resulting grid filepath: \"" << gridSteeringFile.GetGridFilepath() << "\"" << std::endl;
			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse the Grid Steering File: " << gridSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}
}

void SPXSteeringFile::PrintPDFSteeringFiles(void) {
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {
			SPXPDFSteeringFile &pdfSteeringFile = plotConfigurations.at(i).GetPlotConfigurationInstance(j).pdfSteeringFile;
			pdfSteeringFile.Print();
		}
	}
}

void SPXSteeringFile::ParsePDFSteeringFiles(void) {
	std::string mn = "ParsePDFSteeringFiles: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Loop through all plot configurations instances for each plot option
	for(int i = 0; i < plotConfigurations.size(); i++) {
		for(int j = 0; j < plotConfigurations.at(i).GetNumberOfConfigurationInstances(); j++) {

			SPXPlotConfigurationInstance &pci = plotConfigurations.at(i).GetPlotConfigurationInstance(j);
			SPXPDFSteeringFile &pdfSteeringFile = pci.pdfSteeringFile;

			//Attempt to parse the PDF Steering File
			try {
				pdfSteeringFile.Parse();

				//Use default pdfFillStyle, pdfFillColor, and pdfMarkerStyle if currently empty
				if(pci.totalFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Total Fill Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillStyle() << std::endl;
					pci.totalFillStyle = pdfSteeringFile.GetFillStyle();
				}

				if(pci.totalFillColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Total Fill Color was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillColor() << std::endl;
					pci.totalFillColor = pdfSteeringFile.GetFillColor();
				}

				if(pci.totalMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Total Marker Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetMarkerStyle() << std::endl;
					pci.totalMarkerStyle = pdfSteeringFile.GetMarkerStyle();
				}

				//Use default pdfFillStyle, pdfFillColor, and pdfMarkerStyle if currently empty
				if(pci.pdfFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" PDF Fill Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillStyle() << std::endl;
					pci.pdfFillStyle = pdfSteeringFile.GetFillStyle();
				}

				// if(!pci.pdfErrorType.IsValid()) {
				// 	//User did not specify PDF Error Type in Top-Level Steering, use default from PDF Steering File
				// }

				if(pci.pdfFillColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" PDF Fill Color was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillColor() << std::endl;
					pci.pdfFillColor = pdfSteeringFile.GetFillColor();
				}

				if(pci.pdfMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" PDF Marker Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetMarkerStyle() << std::endl;
					pci.pdfMarkerStyle = pdfSteeringFile.GetMarkerStyle();
				}

				if(pci.scaleFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Scale Fill Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillStyle() << std::endl;
					pci.scaleFillStyle = pdfSteeringFile.GetFillStyle();
				}

				if(pci.scaleFillColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Scale Fill Color was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillColor() << std::endl;
					pci.scaleFillColor = pdfSteeringFile.GetFillColor();
				}

				if(pci.scaleMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Scale Marker Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetMarkerStyle() << std::endl;
					pci.scaleMarkerStyle = pdfSteeringFile.GetMarkerStyle();
				}

				if(pci.alphasFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" AlphaS Fill Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillStyle() << std::endl;
					pci.alphasFillStyle = pdfSteeringFile.GetFillStyle();
				}

				if(pci.alphasFillColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" AlphaS Fill Color was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillColor() << std::endl;
					pci.alphasFillColor = pdfSteeringFile.GetFillColor();
				}

				if(pci.alphasMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Alphas Marker Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetMarkerStyle() << std::endl;
					pci.alphasMarkerStyle = pdfSteeringFile.GetMarkerStyle();
				}

				if(pci.correctionsFillStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Corrections Fill Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillStyle() << std::endl;
					pci.correctionsFillStyle = pdfSteeringFile.GetFillStyle();
				}

				if(pci.correctionsFillColor == PC_EMPTY_COLOR) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Correction Fill Color was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetFillColor() << std::endl;
					pci.correctionsFillColor = pdfSteeringFile.GetFillColor();
				}

				if(debug) std::cout << cn << mn << " MarkerStyle=  " << pci.correctionsMarkerStyle<< " Default= "<<PC_EMPTY_STYLE << std::endl;

				if(pci.correctionsMarkerStyle == PC_EMPTY_STYLE) {
					if(debug) std::cout << cn << mn << "Plot Configuration Instance " << j << \
						" Corrections Marker Style was empty: Defaulting to PDF Steering file: " << pdfSteeringFile.GetMarkerStyle() << std::endl;
					pci.correctionsMarkerStyle = pdfSteeringFile.GetMarkerStyle();
				}


				//Update PCI with new data
				SPXPlotConfigurationInstance &pcim = plotConfigurations.at(i).GetPlotConfigurationInstance(pdfSteeringFile.GetFilename());

				pcim.totalFillStyle = pci.totalFillStyle;
				pcim.totalFillColor = pci.totalFillColor;
				pcim.totalMarkerStyle=pci.totalMarkerStyle;

				pcim.pdfFillStyle = pci.pdfFillStyle;
				pcim.pdfFillColor = pci.pdfFillColor;
				pcim.pdfMarkerStyle=pci.pdfMarkerStyle;

				pcim.scaleFillStyle = pci.scaleFillStyle;
				pcim.scaleFillColor = pci.scaleFillColor;
				pcim.scaleMarkerStyle=pci.scaleMarkerStyle;

				pcim.alphasFillStyle = pci.alphasFillStyle;
				pcim.alphasFillColor = pci.alphasFillColor;
				pcim.alphasMarkerStyle=pci.alphasMarkerStyle;

				pcim.correctionsFillStyle = pci.correctionsFillStyle;
				pcim.correctionsFillColor = pci.correctionsFillColor;
				pcim.correctionsMarkerStyle=pci.correctionsMarkerStyle;

			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse the PDF Steering File: " << pdfSteeringFile.GetFilename() << ": Aborting further parsing of remaining files";
				throw SPXParseException(oss.str());
			}
		}
	}
}

void SPXSteeringFile::SetDefaultScales(void) {
  //
  // Set default renormalisation and factorisation scales
  // Note, that values can be overwritten in steering ren_scales and fac_scales
  //
         RenScales.clear();
         FacScales.clear();
         RenScales.push_back(2.0);
         FacScales.push_back(2.0);

         RenScales.push_back(2.0);
         FacScales.push_back(1.0);

         RenScales.push_back(1.0);
         FacScales.push_back(2.0);

         RenScales.push_back(0.5);
         FacScales.push_back(0.5);

         RenScales.push_back(1.0);
         FacScales.push_back(0.5);

         RenScales.push_back(0.5);
         FacScales.push_back(1.0);
}

double SPXSteeringFile::GetDataCutXmin(int i){
 std::string mn = "GetDataCutXmin: ";
 if (DataCutXmin.size()==0) {
  if (debug) std::cout<<cn<<mn<<"GetDataCutXmin.size()==0 return -HUGE_VAL "<<std::endl;
  return -HUGE_VAL;
 }

 if (i>DataCutXmin.size()) {
  std::cout<<cn<<mn<<" something is wrong i= "<<i<<" size= "<<DataCutXmin.size()<<std::endl;
  std::cerr<<"SPXDataSteeringFile::GetDataCutXmin: something is wrong i= "<<i<<" size= "<<DataCutXmin.size()<<std::endl;
  return false;
 }

 if (!RemoveXbins.at(i)) std::cout<<cn<<mn<<"WARNING: RemoveXbin flag is not ON "<<std::endl;

 return DataCutXmin.at(i);
}

double SPXSteeringFile::GetDataCutXmax(int i){
 std::string mn = "GetDataCutXmax: ";
 if (DataCutXmax.size()==0) {
   if (debug) std::cout<<cn<<mn<<"GetDataCutXmax.size()==0 return HUGE_VAL "<<std::endl;
   return HUGE_VAL;
  }

  if (i>DataCutXmax.size()) {
   std::cout<<cn<<mn<<"Something is wrong i= "<<i<<" size= "<<DataCutXmax.size()<<std::endl;
   std::cerr<<cn<<mn<<"Something is wrong i= "<<i<<" size= "<<DataCutXmax.size()<<std::endl;
   return HUGE_VAL;
  }

  if (!RemoveXbins.at(i)) std::cout<<cn<<mn<<"WARNING: RemoveXbin flag is not ON "<<std::endl;

  return DataCutXmax.at(i);
}

bool SPXSteeringFile::GetDataRemoveXbinsFlag(int i) {
 std::string mn = "GetDataRemoveXbinsFlag: ";
 if (debug) std::cout<<cn<<mn<<" i= "<<i<<std::endl;

 if (RemoveXbins.size()==0) {
   if (debug) std::cout<<cn<<mn<<"RemoveXbins.size()==0 return false "<<std::endl;
  return false;
  }

 if (i>RemoveXbins.size()) std::cout<<cn<<mn<<"Something is wrong i= "<<i<<" size= "<<RemoveXbins.size()<<std::endl;
  return RemoveXbins.at(i);
}
