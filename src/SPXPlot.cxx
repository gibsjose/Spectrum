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
		//InitializeCrossSections();
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
	int xMin = 0;
	int xMax = 3000;
	int yMin = -1;
	int yMax = 4;

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

	//@TODO Draw other stuff...

	//Update canvas
	canvas->Update();
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