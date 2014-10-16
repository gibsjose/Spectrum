//************************************************************/
//
//	Plot Implementation
//
//	Implements the SPXPlot class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT objects
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		13.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <sstream>

#include "SPXPlot.h"
#include "SPXUtilities.h"

const std::string cn = "SPXPlot::";

//Must define the static debug variable in the implementation
bool SPXPlot::debug;

//Initialize all plots
void SPXPlot::Initialize(void) {
	try {
		InitializeData();
		InitializeCrossSections();
		NormalizeCrossSections();
	} catch(const SPXException &e) {
		throw;
	}
}

void SPXPlot::Plot(void) {
	std::string mn = "Plot:: ";

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);

	std::ostringstream oss;
	oss << "canvas" << id;
	std::string canvasID = oss.str();

	//@TODO Where should these come from? Or are they just initial values that are set later?
	int wtopx = 200;	//Window top x
	int wtopy = 10;		//Window top y
	int ww = 700;		//Window width
	int wh = 500;		//Window height

	canvas = new TCanvas(canvasID.c_str(), pc.GetDescription().c_str(), wtopx, wtopy, ww, wh);
	canvas->SetFillColor(0);
	canvas->SetGrid();

	if(debug) {
		std::cout << cn << mn << "Canvas (" << canvasID << ") created for Plot ID " << id << 
			" with dimensions: " << ww << " x " << wh << " and title: " << pc.GetDescription() << std::endl; 
	}

	//@TODO Where should these come from?
	double xMin; // = 0;
	double xMax; // = 3000;
	double yMin; // = 0;
	double yMax; // = 0.003;

	//Determine frame bounds by calculating the xmin, xmax, ymin, ymax from ALL graphs being drawn
	std::vector<TGraphAsymmErrors *> graphs;
	{	
		//Data graphs
		for(int i = 0; i < data.size(); i++) {
			graphs.push_back(data[i].GetStatisticalErrorGraph());
			graphs.push_back(data[i].GetSystematicErrorGraph());
		}

		//Cross sections
		for(int i = 0; i < crossSections.size(); i++) {
			graphs.push_back(crossSections[i].GetPDFBandResults());
		}

		xMin = SPXGraphUtilities::GetXMin(graphs);
		xMax = SPXGraphUtilities::GetXMax(graphs);
		yMin = SPXGraphUtilities::GetYMin(graphs);
		yMax = SPXGraphUtilities::GetYMax(graphs);

		//Sanity check
		if(xMin > xMax) {
			throw SPXGraphException("xMin calculated to be larger than xMax");
		}

		if(yMin > yMax) {
			throw SPXGraphException("yMin calculated to be larger than yMax");
		}
	}
	
	//Draw the frame (xmin, ymin, xmax, ymax)
	canvas->DrawFrame(xMin, yMin, xMax, yMax);

	if(debug) {
		std::cout << cn << mn << "Canvas (" << canvasID << ") frame drawn with dimensions: " << std::endl;
		std::cout << "\t xMin = " << xMin << std::endl;
		std::cout << "\t xMax = " << xMax << std::endl;
		std::cout << "\t yMin = " << yMin << std::endl;
		std::cout << "\t yMax = " << yMax << std::endl;
	}

	//Draw data graphs
	for(int i = 0; i < data.size(); i++) {
		data[i].GetSystematicErrorGraph()->Draw("P");
		data[i].GetStatisticalErrorGraph()->Draw("||");
	}

	//Draw cross sections
	for(int i = 0; i < crossSections.size(); i++) {
		crossSections[i].GetPDFBandResults()->Draw("P");
	}

	//Update canvas
	canvas->Update();
}

//@TODO Where do I normalize cross section???
void SPXPlot::InitializeCrossSections(void) {
	std::string mn = "InitializeCrossSections: ";

	//Create cross sections for each configuration instance (and each PDF)
	for(int i = 0; i < steeringFile->GetNumberOfConfigurationInstances(id); i++) {
		for(int j = 0; j < steeringFile->GetNumberOfPDFSteeringFiles(); j++) {
			SPXPDFSteeringFile &psf = steeringFile->GetPDFSteeringFile(j);
			SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);

			SPXCrossSection crossSectionInstance = SPXCrossSection(&psf, &pci);

			try {
				crossSectionInstance.Create();
				crossSections.push_back(crossSectionInstance);
			} catch(const SPXException &e) {
				throw;
			}
		}
	}
}

void SPXPlot::NormalizeCrossSections(void) {
	for(int i = 0; i < crossSections.size(); i++) {
		SPXPDFSteeringFile *psf = crossSections[i].GetPDFSteeringFile();
		SPXPlotConfigurationInstance *pci = crossSections[i].GetPlotConfigurationInstance();

		//@TODO WHERE DO I GET THE CS SCALE FROM????
		double xScale = pci->dataSteeringFile.GetXScale();
		double yScale = pci->dataSteeringFile.GetYScale();
		bool normalizedToTotalSigma = false; //@TODO Where is this for the PDF/Grid/CS?
		bool dividedByBinWidth = pci->gridSteeringFile.IsDividedByBinWidth();

		SPXGraphUtilities::Normalize(crossSections[i].GetPDFBandResults(), xScale, yScale, true, dividedByBinWidth);
	}
}

void SPXPlot::InitializeData(void) {
	std::string mn = "InitializeData: ";

	//Create data objects for each configuration instance of this plot and add them to vector
	for(int i = 0; i < steeringFile->GetNumberOfConfigurationInstances(id); i++) {
		SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);
		
		SPXData dataInstance = SPXData(pci);
	
		try {
			dataInstance.Parse();
			dataInstance.Print();
			data.push_back(dataInstance);
		} catch(const SPXException &e) {
			throw;
		}
	}

	//Go through data objects and create graphs, modify settings, etc.
	for(int i = 0; i < data.size(); i++) {

		//Create graphs: once this function is called, it is safe to manipulate graphs
		data[i].CreateGraphs();

		//Obtain the graphs
		TGraphAsymmErrors *statGraph = data[i].GetStatisticalErrorGraph();
		TGraphAsymmErrors *systGraph = data[i].GetSystematicErrorGraph();

		//Get settings
		SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);

		//Normalize the graphs based on the settings
		double xScale = pci.dataSteeringFile.GetXScale();
		double yScale = pci.dataSteeringFile.GetYScale();
		bool normalizedToTotalSigma = pci.dataSteeringFile.IsNormalizedToTotalSigma();
		bool dividedByBinWidth = pci.dataSteeringFile.IsDividedByBinWidth();

		if(debug) {
			std::cout << cn << mn << "Normalizing with: " << std::endl;
			std::cout << "\t X Scale = " << xScale << std::endl;
			std::cout << "\t Y Scale = " << yScale << std::endl;
			std::cout << "\t Normalized To Total Sigma? " << (normalizedToTotalSigma ? "YES" : "NO") << std::endl;
			std::cout << "\t Divided By Bin Width? " << (dividedByBinWidth ? "YES" : "NO") << std::endl;
		}

		SPXGraphUtilities::Normalize(statGraph, xScale, yScale, normalizedToTotalSigma, dividedByBinWidth);
		SPXGraphUtilities::Normalize(systGraph, xScale, yScale, normalizedToTotalSigma, dividedByBinWidth);

		//Modify Data Graph styles
		statGraph->SetMarkerStyle(pci.markerStyle);
		systGraph->SetMarkerStyle(pci.markerStyle);

		statGraph->SetMarkerColor(pci.markerColor);
		systGraph->SetMarkerColor(pci.markerColor);

		//@TODO Why is this not in the data steering file? Why is one larger?
		statGraph->SetMarkerSize(1.0);
		systGraph->SetMarkerSize(1.0);

		statGraph->SetLineColor(4);
		systGraph->SetLineColor(1);

		statGraph->SetLineWidth(1);
		systGraph->SetLineWidth(1);
	}
}