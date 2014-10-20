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

	//Create PNG Filename
	std::string pngFilename;

	//@TODO Spaces -> Underscores and handle duplicates
	//@TODO Should probably write a function: string CreatePNGFilename(string output dir, string desc);
	//pngFilename = PlotOutputDirectory + "/" + pc.GetDescription() + ".png";	//@TODO Implement passing plot output directory as arugment and defaulting to ./plots

	pngFilename = "./plots/" + pc.GetDescription() + "_plot_" + (ULong_t)id + ".png";

	if(debug) std::cout << cn << mn << "Creating Plot PNG with name = " << pngFilename << std::endl;

	//Draw PNG File
	canvas->Print(pngFilename.c_str());
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
	std::string mn = "NormalizeCrossSections: ";

	for(int i = 0; i < crossSections.size(); i++) {
		try {
			if(debug) std::cout << cn << mn << "Normalizing Cross Section at index " << i << std::endl;

			SPXPDFSteeringFile *psf = crossSections[i].GetPDFSteeringFile();
			SPXPlotConfigurationInstance *pci = crossSections[i].GetPlotConfigurationInstance();

			std::string masterXUnits = pci->dataSteeringFile.GetXUnits();
			std::string slaveXUnits = pci->gridSteeringFile.GetXUnits();
			std::string masterYUnits = pci->dataSteeringFile.GetYUnits();
			std::string slaveYUnits = pci->gridSteeringFile.GetYUnits();

			//Determine the scale from the unit difference between data and grid
			double xScale = SPXGraphUtilities::GetXUnitsScale(masterXUnits, slaveXUnits);
			double yScale = SPXGraphUtilities::GetYUnitsScale(masterYUnits, slaveYUnits);

			if(debug) {
				std::cout << cn << mn << "Scales determined from data/grid unit differential: " << std::endl;
				std::cout << "\t Data X Units: " << masterXUnits << std::endl;
				std::cout << "\t Grid X Units: " << slaveXUnits << std::endl << std::endl;
				std::cout << "\t Data Y Units: " << masterYUnits << std::endl;
				std::cout << "\t Grid Y Units: " << slaveYUnits << std::endl << std::endl;
				std::cout << "\t ---> X Unit Scale Determined: " << xScale << std::endl;
				std::cout << "\t ---> Y Unit Scale Determined: " << yScale << std::endl << std::endl;
			}

			//Also scale by the artificial scale from the plot configuration instance
			xScale *= pci->xScale;
			yScale *= pci->yScale;

			SPXGraphUtilities::Scale(crossSections[i].GetPDFBandResults(), xScale, yScale);

			if(debug) {
				std::cout << cn << mn << "Additional artificial scale for Cross Section: " << std::endl;
				std::cout << "\t X Scale: " << pci->xScale << std::endl;
				std::cout << "\t Y Scale: " << pci->yScale << std::endl << std::endl;
			}
			

			//Normalized to total sigma from the DATA steering file
			bool normalizeToTotalSigma = pci->dataSteeringFile.IsNormalizedToTotalSigma();
			bool dataDividedByBinWidth = pci->dataSteeringFile.IsDividedByBinWidth();
			bool gridDividedByBinWidth = pci->gridSteeringFile.IsDividedByBinWidth();
			
			//@TODO Change back to being initialized as false
			bool divideByBinWidth = true;

			if(dataDividedByBinWidth && !gridDividedByBinWidth) {
				if(debug) std::cout << cn << mn << "Data IS divided by bin width but the grid IS NOT. Will call Normalize with divideByBinWidth = true" << std::endl;
				divideByBinWidth = true;
			}

			double yBinWidthScale = 1.0;

			//If the data is divided by the bin width, then set the yBinWidthScale, which is the scaling of the Data's Y Bin Width Units to the Data's X Units
			if(dataDividedByBinWidth) {
				yBinWidthScale = SPXGraphUtilities::GetYBinWidthUnitsScale(pci->dataSteeringFile.GetXUnits(), pci->dataSteeringFile.GetYBinWidthUnits());
			}

			if(debug) std::cout << cn << mn << "Y Bin Width Scale = " << yBinWidthScale << std::endl;
			if(debug) std::cout << cn << mn << "Normalize to Total Sigma is " << (normalizeToTotalSigma ? "ON" : "OFF") << std::endl;
			if(debug) std::cout << cn << mn << "Divide by Bin Width is " << (divideByBinWidth ? "ON" : "OFF") << std::endl;

			//Normalize the cross section
			SPXGraphUtilities::Normalize(crossSections[i].GetPDFBandResults(), yBinWidthScale, normalizeToTotalSigma, divideByBinWidth);
			
			if(debug) std::cout << cn << mn << "Sucessfully normalized Cross Section " << i << std::endl;
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXGraphException("SPXPlot::NormalizeCrossSections: Unable to obtain X/Y Scale based on Data/Grid Units");
		}
		
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
		double xScale = pci.xScale;
		double yScale = pci.yScale;

		if(debug) {
			std::cout << cn << mn << "Scaling Data with: " << std::endl;
			std::cout << "\t X Scale = " << xScale << std::endl;
			std::cout << "\t Y Scale = " << yScale << std::endl;
		}

		SPXGraphUtilities::Scale(statGraph, xScale, yScale);
		SPXGraphUtilities::Scale(systGraph, xScale, yScale);

		//Modify Data Graph styles
		statGraph->SetMarkerStyle(pci.markerStyle);
		systGraph->SetMarkerStyle(pci.markerStyle);

		statGraph->SetMarkerColor(pci.markerColor);
		systGraph->SetMarkerColor(pci.markerColor);

		statGraph->SetMarkerSize(1.0);
		systGraph->SetMarkerSize(1.0);

		statGraph->SetLineColor(4);
		systGraph->SetLineColor(1);

		statGraph->SetLineWidth(1);
		systGraph->SetLineWidth(1);
	}
}