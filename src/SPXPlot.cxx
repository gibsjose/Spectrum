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

	//Perform plotting
	CreateCanvas();
	DivideCanvasIntoPads();
	ConfigurePads();
	DrawOverlayPadFrame();
	DrawRatioPadFrame();
	SetAxisLabels();
	ScaleAxes();
	DrawOverlay();
	DrawRatio();
	//DrawLegend();
	UpdateCanvas();

	//Create a PNG of the canvas
	CanvasToPNG();
}

void SPXPlot::SetAxisLabels(void) {
	std::string mn = "SetAxisLabels: ";

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	std::string xLabel = pc.GetPlotConfigurationInstance(0).dataSteeringFile.GetXLabel();	//0th instance is master
	std::string yLabelOverlay = pc.GetPlotConfigurationInstance(0).dataSteeringFile.GetYLabel();
	std::string yLabelRatio = (!pc.GetRatioTitle().empty() ? pc.GetRatioTitle() : "Ratio");

	//Set Overlay Y-Axis Label
	overlayFrameHisto->SetYTitle(yLabelOverlay.c_str());

	//Set Ratio Y-Axis Label
	ratioFrameHisto->SetYTitle(yLabelRatio.c_str());

	//Set X-Axis Label
	if(ds.ContainsOverlay() && !ds.ContainsRatio()) {
		//Set Overlay X-Axis Label
		overlayFrameHisto->SetXTitle(xLabel.c_str());

	} else {
		//Set Ratio X-Axis Label
		ratioFrameHisto->SetXTitle(xLabel.c_str());
	}

	overlayPad->cd();
	overlayFrameHisto->Draw();

	ratioPad->cd();
	ratioFrameHisto->Draw();
}

void SPXPlot::ScaleAxes(void) {
	std::string mn = "ScaleAxes: ";

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	double scale = 1.0;

	//Change scale to 1.5 if only 1 are plotted
	if(ds.ContainsOverlay() ^ ds.ContainsRatio()) {
		scale = 1.5;
	}

	double xTitleOffset, yTitleOffset;

	xTitleOffset = 1.1;
	yTitleOffset = 0.8;

	//Scale Overlay Axes
	xAxisOverlay->SetTitleOffset(xTitleOffset);
	yAxisOverlay->SetTitleOffset(yTitleOffset);

	xAxisOverlay->SetLabelSize(xAxisOverlay->GetLabelSize() / scale);
	yAxisOverlay->SetLabelSize(yAxisOverlay->GetLabelSize() / scale);

	xAxisOverlay->SetTitleSize(xAxisOverlay->GetTitleSize() / scale);
	yAxisOverlay->SetTitleSize(yAxisOverlay->GetTitleSize() / scale);

	//Scale Ratio Axes
	xAxisRatio->SetTitleOffset(xTitleOffset);

	double distScale = 0.0;		//Add an offset to y ratio title offset if only ratio is plotted
	if(!ds.ContainsOverlay() && ds.ContainsRatio()) {
		distScale = 0.5;
	}
	yAxisRatio->SetTitleOffset(yTitleOffset + distScale);

	double rScale = 1.0;
	if(ds.ContainsOverlay()) {
		rScale = (0.4 - 0.0) / (1.0 - 0.4);
	}

	xAxisRatio->SetLabelSize(xAxisOverlay->GetLabelSize() / rScale);
	yAxisRatio->SetLabelSize(yAxisOverlay->GetLabelSize() / rScale);

	xAxisRatio->SetTitleSize(xAxisOverlay->GetTitleSize() / rScale);
	yAxisRatio->SetTitleSize(yAxisOverlay->GetTitleSize() / rScale);

	overlayPad->cd();
	overlayFrameHisto->Draw();

	ratioPad->cd();
	ratioFrameHisto->Draw();
}

void SPXPlot::CreateCanvas(void) {
	std::string mn = "CreateCanvas: ";

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);

	std::ostringstream oss;
	oss << "canvas_" << id;
	std::string canvasID = oss.str();

	//@TODO Where should these come from? Or are they just initial values that are set later?
	int wtopx = 400;	//Window top x
	int wtopy = 0;		//Window top y
	int ww = 1000;		//Window width
	int wh = 1000;		//Window height

	canvas = new TCanvas(canvasID.c_str(), pc.GetDescription().c_str(), wtopx, wtopy, ww, wh);
	canvas->SetFillColor(0);
	canvas->SetGrid();
	canvas->SetLeftMargin(0.2);
	canvas->SetRightMargin(0.1);
	canvas->SetBottomMargin(0.15);
	canvas->cd();

	if(debug) {
		std::cout << cn << mn << "Canvas (" << canvasID << ") created for Plot ID " << id <<
			" with dimensions: " << ww << " x " << wh << " and title: " << pc.GetDescription() << std::endl;
	}
}

//Determine frame bounds by calculating the xmin, xmax, ymin, ymax from ALL graphs being drawn
void SPXPlot::DetermineOverlayFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax) {

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
}

//Determine frame bounds by calculating the xmin, xmax, ymin, ymax from ALL graphs being drawn
void SPXPlot::DetermineRatioFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax) {

	//@TODO Implement determining ratio frame bounds
	/*
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
	*/

	xMin = 0;
	xMax = 0;
	yMin = 0;
	yMax = 0;
}

void SPXPlot::DivideCanvasIntoPads(void) {
	std::string mn = "DivideCanvasIntoPads: ";

	if(!canvas) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::CreateCanvas before creating the pads");
	}

	//Divide the TCanvas and get pointers to TPads
	canvas->Divide(1, 2);
	std::string canvasID = canvas->GetName();
	overlayPad = (TPad *)canvas->GetListOfPrimitives()->FindObject(TString(canvasID + "_1"));
	ratioPad = (TPad *)canvas->GetListOfPrimitives()->FindObject(TString(canvasID + "_2"));

	if(!overlayPad || !ratioPad) {
		throw SPXROOTException(cn + mn + "There was an error while getting the TPads from the divided TCanvas");
	}
}

void SPXPlot::ConfigurePads(void) {
	std::string mn = "ConfigurePads: ";

	if(!overlayPad || !ratioPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DivideCanvasIntoPads before configuring the pads");
	}

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	//xLow/Up and yLow/Up: The TPad bounds for the Overlay and Ratio TPads
	double xLowOverlay, xUpOverlay, yLowOverlay, yUpOverlay;
	double xLowRatio, xUpRatio, yLowRatio, yUpRatio;

	//Margins
	double leftMargin, rightMargin, bottomMargin, topMargin;

	//Set defaults for TPad bounds
	xLowOverlay = 0.0;
	xUpOverlay = 1.0;
	yLowOverlay = 0.4;
	yUpOverlay = 1.0;

	xLowRatio = 0.0;
	xUpRatio = 1.0;
	yLowRatio = 0.0;
	yUpRatio = 0.0;

	//Set defaults for margins
	leftMargin = 0.15;
	rightMargin = 0.05;
	bottomMargin = 0.001;
	topMargin = 0.001;

	if(ds.ContainsOverlay() && ds.ContainsRatio()) {
		if(debug) std::cout << cn << mn << "Plotting Overlay and Ratio" << std::endl;

		yUpRatio = 0.4;

		//Only set top/bottom margins for ratio pad when both are plotted
		ratioPad->SetBottomMargin(0.25);
		ratioPad->SetTopMargin(topMargin);

	} else if(ds.ContainsOverlay() && !ds.ContainsRatio()) {
		if(debug) std::cout << cn << mn << "Only plotting Overlay" << std::endl;

		yLowOverlay = 0.0;
		bottomMargin = 0.15;
		yUpRatio = 0.0;

	} else if(!ds.ContainsOverlay() && ds.ContainsRatio()) {
		if(debug) std::cout << cn << mn << "Only plotting Ratio" << std::endl;

		yLowOverlay = 1.0;
		yUpRatio = 1.0;
	}

	//Resize and style overlay pad
	overlayPad->SetPad(xLowOverlay, yLowOverlay, xUpOverlay, yUpOverlay);
	overlayPad->SetFillColor(0);
	overlayPad->SetLeftMargin(leftMargin);
	overlayPad->SetRightMargin(rightMargin);
	overlayPad->SetBottomMargin(bottomMargin);
	overlayPad->Draw();

	if(debug) {
		std::cout << cn << mn << "Configured Overlay Pad:" << std::endl;
		std::cout << "\t xLow = " << xLowOverlay << std::endl;
		std::cout << "\t xUp  = " << xUpOverlay << std::endl;
		std::cout << "\t yLow = " << yLowOverlay << std::endl;
		std::cout << "\t yUp  = " << yUpOverlay << std::endl;
		std::cout << "\t Fill Color = " << 0 << std::endl;
		std::cout << "\t Left Margin = " << overlayPad->GetLeftMargin() << std::endl;
		std::cout << "\t Right Margin = " << overlayPad->GetRightMargin() << std::endl;
		std::cout << "\t Top Margin = " << overlayPad->GetTopMargin() << std::endl;
		std::cout << "\t Bottom Margin = " << overlayPad->GetBottomMargin() << std::endl;
 	}

	//Resize and style ratio pad
	ratioPad->SetPad(xLowRatio, yLowRatio, xUpRatio, yUpRatio);
	ratioPad->SetFillColor(0);
	ratioPad->SetLeftMargin(leftMargin);
	ratioPad->SetRightMargin(rightMargin);
	ratioPad->Draw();

	if(debug) {
		std::cout << cn << mn << "Configured Ratio Pad:" << std::endl;
		std::cout << "\t xLow = " << xLowRatio << std::endl;
		std::cout << "\t xUp  = " << xUpRatio << std::endl;
		std::cout << "\t yLow = " << yLowRatio << std::endl;
		std::cout << "\t yUp  = " << yUpRatio << std::endl;
		std::cout << "\t Fill Color = " << 0 << std::endl;
		std::cout << "\t Left Margin = " << ratioPad->GetLeftMargin() << std::endl;
		std::cout << "\t Right Margin = " << ratioPad->GetRightMargin() << std::endl;
		std::cout << "\t Top Margin = " << ratioPad->GetTopMargin() << std::endl;
		std::cout << "\t Bottom Margin = " << ratioPad->GetBottomMargin() << std::endl;
	}

	//Set linear/logarithmic axes
	if(pc.IsXLog()) {
		if(debug) std::cout << cn << mn << "Setting X Axis to Logarithmic Scale for Plot " << id << std::endl;
		overlayPad->SetLogx();
		ratioPad->SetLogx();
	}

	if(pc.IsYLog()) {
		if(debug) std::cout << cn << mn << "Setting Y Axis to Logarithmic Scale for Plot " << id << std::endl;
		overlayPad->SetLogy();
		//ratioPad->SetLogy();	//@TODO How to handle this in SF? What to do for negatives?
	}
}

void SPXPlot::DrawOverlayPadFrame(void) {
	std::string mn = "DrawOverlayPadFrame: ";

	if(!overlayPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::ConfigurePads before drawing the pad frame");
	}

	DetermineOverlayFrameBounds(xMinOverlay, xMaxOverlay, yMinOverlay, yMaxOverlay);

	overlayPad->cd();
	overlayFrameHisto = overlayPad->DrawFrame(xMinOverlay, yMinOverlay, xMaxOverlay, yMaxOverlay);
	xAxisOverlay = overlayFrameHisto->GetXaxis();
	yAxisOverlay = overlayFrameHisto->GetYaxis();

	if(debug) {
		std::cout << cn << mn << "Overlay Pad frame drawn with dimensions: " << std::endl;
		std::cout << "\t xMin = " << xMinOverlay << std::endl;
		std::cout << "\t xMax = " << xMaxOverlay << std::endl;
		std::cout << "\t yMin = " << yMinOverlay << std::endl;
		std::cout << "\t yMax = " << yMaxOverlay << std::endl;
	}
}

void SPXPlot::DrawRatioPadFrame(void) {
	std::string mn = "DrawRatioPadFrame: ";

	if(!ratioPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::ConfigurePads before drawing the pad frame");
	}

	DetermineRatioFrameBounds(xMinRatio, xMaxRatio, yMinRatio, yMaxRatio);

	//@TODO DEBUG! Set properly...
	yMinRatio = 0.9;
	yMaxRatio = 1.1;

	//Force Ratio X Min/Max to match Overlay (should alread match anyway...)
	xMinRatio = xMinOverlay;
	xMaxRatio = xMaxOverlay;

	ratioPad->cd();
	ratioFrameHisto = ratioPad->DrawFrame(xMinRatio, yMinRatio, xMaxRatio, yMaxRatio);
	xAxisRatio = ratioFrameHisto->GetXaxis();
	yAxisRatio = ratioFrameHisto->GetYaxis();

	if(debug) {
		std::cout << cn << mn << "Ratio Pad frame drawn with dimensions: " << std::endl;
		std::cout << "\t xMin = " << xMinRatio << std::endl;
		std::cout << "\t xMax = " << xMaxRatio << std::endl;
		std::cout << "\t yMin = " << yMinRatio << std::endl;
		std::cout << "\t yMax = " << yMaxRatio << std::endl;
	}
}

void SPXPlot::DrawOverlay(void) {
	std::string mn = "DrawOverlay: ";

	if(!overlayPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawOverlayPadFrame before drawing the overlay graphs");
	}

	//Do nothing if not drawing overlay
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	if(!ds.ContainsOverlay()) {
		return;
	}

	//Change to the overlay pad
	overlayPad->cd();

	//Draw data graphs on Overlay Pad
	for(int i = 0; i < data.size(); i++) {
		data[i].GetSystematicErrorGraph()->Draw("P");
		data[i].GetStatisticalErrorGraph()->Draw("||");

		if(debug) std::cout << cn << mn << "Sucessfully drew data for Plot " << id << " data " << i << std::endl;
	}

	//Draw cross sections on Overlay Pad
	for(int i = 0; i < crossSections.size(); i++) {
		crossSections[i].GetPDFBandResults()->Draw("P");

		if(debug) std::cout << cn << mn << "Sucessfully drew cross section for Plot " << id << " cross section " << i << std::endl;
	}
}

void SPXPlot::DrawRatio(void) {
	std::string mn = "DrawRatio: ";

	if(!ratioPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawRatioPadFrame before drawing the ratio graphs");
	}

	//Do nothing if not drawing ratio
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	if(!ds.ContainsRatio()) {
		return;
	}

	//Change to the ratio pad
	ratioPad->cd();

	//Draw a line at 1, where ratios are relative to
	TLine *referenceLine = new TLine(xMinRatio, 1.0, xMaxRatio, 1.0);
	referenceLine->Draw();

	//@TODO Create and draw ratio graphs
}

void SPXPlot::UpdateCanvas(void) {
	std::string mn = "UpdateCanvas: ";

	if(!canvas) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::CreateCanvas before updating the canvas");
	}

	canvas->Modified();
	canvas->Update();
}

void SPXPlot::CanvasToPNG(void) {
	std::string mn = "CanvasToPNG: ";

	if(!canvas) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::CreateCanvas before printing the canvas as a PNG");
	}

	//Get the plot configuration
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);

	//Create/get the PNG filename
	std::string filename;
	filename = GetPNGFilename(pc.GetDescription());

	//Draw PNG File
	canvas->Print(filename.c_str());
}

std::string SPXPlot::GetPNGFilename(std::string desc) {
	std::string mn = "GetPNGFilename: ";

	std::string filename;

	//Fill in dummy description if empty
	if(desc.empty()) {
		desc = std::string("general");
	}

	filename = "./plots/" + desc + "_plot_" + (ULong_t)id + ".png";

	if(debug) std::cout << cn << mn << "Created PNG Filename: " << filename << std::endl;

	return filename;
}

void SPXPlot::InitializeCrossSections(void) {
	std::string mn = "InitializeCrossSections: ";

	//Create cross sections for each configuration instance
	for(int i = 0; i < steeringFile->GetNumberOfConfigurationInstances(id); i++) {
		SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);
		SPXPDFSteeringFile &psf = pci.pdfSteeringFile;

		SPXCrossSection crossSectionInstance = SPXCrossSection(&psf, &pci);

		try {
			crossSectionInstance.Create();
			crossSections.push_back(crossSectionInstance);
		} catch(const SPXException &e) {
			throw;
		}
	}
}

//@TODO See MyCrossSection.cxx:287-298
/*
void SPXPlot::ChangeDefaultPDFHistogramNames(void) {
	std::string mn = "ChangeDefaultPDFHistogramNames: ";

	for(int i = 0; i < crossSections.size(); i++) {
		;
	}
}
*/

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
				if(debug) std::cout << cn << mn << "Data IS divided by bin width but the grid IS NOT. Will call Normalize "\
					"with divideByBinWidth = true" << std::endl;
				divideByBinWidth = true;
			}

			double yBinWidthScale = 1.0;

			//If the data is divided by the bin width, then set the yBinWidthScale, which is the scaling of the
			// Data's Y Bin Width Units to the Data's X Units
			if(dataDividedByBinWidth) {
				yBinWidthScale = SPXGraphUtilities::GetYBinWidthUnitsScale(pci->dataSteeringFile.GetXUnits(), \
					pci->dataSteeringFile.GetYBinWidthUnits());
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
		statGraph->SetMarkerStyle(pci.dataMarkerStyle);
		systGraph->SetMarkerStyle(pci.dataMarkerStyle);

		statGraph->SetMarkerColor(pci.dataMarkerColor);
		systGraph->SetMarkerColor(pci.dataMarkerColor);

		statGraph->SetMarkerSize(1.0);
		systGraph->SetMarkerSize(1.0);

		statGraph->SetLineColor(4);
		systGraph->SetLineColor(1);

		statGraph->SetLineWidth(1);
		systGraph->SetLineWidth(1);
	}
}
