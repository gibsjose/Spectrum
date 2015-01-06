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
#include <string.h> //memcpy

#include "SPXPlot.h"
#include "SPXUtilities.h"

const std::string cn = "SPXPlot::";

//Must define the static debug variable in the implementation
bool SPXPlot::debug;

//Initialize all plots
void SPXPlot::Initialize(void) {
	std::string mn = "Initialize: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Initializing Plot with ID id= " << id << std::endl;

	try {
		InitializeData();
                if (debug) std::cout << cn << mn << "finished InitializeData id= " << id << std::endl;

		InitializeCrossSections();
                if (debug) std::cout << cn << mn << "finished InitializeCrossSections id= " << id << std::endl;

		NormalizeCrossSections();
                if (debug) std::cout << cn << mn << "finished NormalizeCrossSections id= " << id << std::endl;

		MatchOverlayBinning();
                if (debug) std::cout << cn << mn << "finished MatchOverlayBinning id= " << id << std::endl;

		ApplyGridCorrections();
                if (debug) std::cout << cn << mn << "finished ApplyGridCorrections id= " << id << std::endl;

		InitializeRatios();
                if (debug) std::cout << cn << mn << "finished  	InitializeRatios id= " << id << std::endl;
	} catch(const SPXException &e) {
		throw;
	}
}

void SPXPlot::Plot(void) {
	std::string mn = "Plot: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Plotting Plot with ID " << id << std::endl;

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
	DrawLegend();
	UpdateCanvas();

	//Create a PNG of the canvas
	CanvasToPNG();
}

void SPXPlot::SetAxisLabels(void) {
	std::string mn = "SetAxisLabels: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	std::string xLabel = pc.GetPlotConfigurationInstance(0).dataSteeringFile.GetXLabel();	//0th instance is master
	std::string yLabelOverlay = pc.GetPlotConfigurationInstance(0).dataSteeringFile.GetYLabel();
	std::string yLabelRatio = (!pc.GetRatioTitle().empty() ? pc.GetRatioTitle() : "Ratio");

	//Set Overlay Y-Axis Label
	if(ds.ContainsOverlay()) {
		overlayFrameHisto->SetYTitle(yLabelOverlay.c_str());
	}

	//Set Ratio Y-Axis Label
	if(ds.ContainsRatio()) {
		ratioFrameHisto->SetYTitle(yLabelRatio.c_str());
	}

	//Set X-Axis Label
	if(ds.ContainsOverlay() && !ds.ContainsRatio()) {
		//Set Overlay X-Axis Label
		overlayFrameHisto->SetXTitle(xLabel.c_str());

	} else if(ds.ContainsRatio()){
		//Set Ratio X-Axis Label
		ratioFrameHisto->SetXTitle(xLabel.c_str());
	}

	if(ds.ContainsOverlay()) {
		overlayPad->cd();
		overlayFrameHisto->Draw();
	}

	if(ds.ContainsRatio()) {
		ratioPad->cd();
		ratioFrameHisto->Draw();
	}
}

void SPXPlot::ScaleAxes(void) {
	std::string mn = "ScaleAxes: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
	if(ds.ContainsOverlay()) {
		xAxisOverlay->SetTitleOffset(xTitleOffset);
		yAxisOverlay->SetTitleOffset(yTitleOffset);

		xAxisOverlay->SetLabelSize(xAxisOverlay->GetLabelSize() / scale);
		yAxisOverlay->SetLabelSize(yAxisOverlay->GetLabelSize() / scale);

		xAxisOverlay->SetTitleSize(xAxisOverlay->GetTitleSize() / scale);
		yAxisOverlay->SetTitleSize(yAxisOverlay->GetTitleSize() / scale);

		if(debug) std::cout << cn << mn << "Set Y Axis Overlay Title Offset to " << yTitleOffset << std::endl;
	}

	//Scale Ratio Axes
	if(ds.ContainsRatio()) {
		xAxisRatio->SetTitleOffset(xTitleOffset);

		//NOTE: Not sure why I have to do -0.25 to begin with... For some reason at 0.0 (total offset = 0.8 when below is false)
		// there is an additional offset relative to the overlay title. 0.55 (0.8 + (-0.25)) looks nice, however.

		double distScale = -0.25;		//Add an offset to y ratio title offset if only ratio is plotted
		if(!ds.ContainsOverlay() && ds.ContainsRatio()) {
			distScale = 0.5;
		}

		yAxisRatio->SetTitleOffset(yTitleOffset + distScale);

		if(debug) std::cout << cn << mn << "Set Y Axis Ratio Title Offset to " << yTitleOffset + distScale << std::endl;

		double rScale = 1.0;
		if(ds.ContainsOverlay()) {
			rScale = (0.4 - 0.0) / (1.0 - 0.4);
		}

		xAxisRatio->SetLabelSize(xAxisOverlay->GetLabelSize() / rScale);
		yAxisRatio->SetLabelSize(yAxisOverlay->GetLabelSize() / rScale);

		xAxisRatio->SetTitleSize(xAxisOverlay->GetTitleSize() / rScale);
		yAxisRatio->SetTitleSize(yAxisOverlay->GetTitleSize() / rScale);
	}

	if(ds.ContainsOverlay()) {
		overlayPad->cd();
		overlayFrameHisto->Draw();
	}

	if(ds.ContainsRatio()) {
		ratioPad->cd();
		ratioFrameHisto->Draw();
	}
}

void SPXPlot::CreateCanvas(void) {
	std::string mn = "CreateCanvas: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
	std::string mn = "DetermineOverlayFrameBounds: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	//Do nothing if overlay is not plotted
	if(!ds.ContainsOverlay()) {
		xMin = xMax = yMin = yMax = 0;
		return;
	}

	std::vector<TGraphAsymmErrors *> graphs;
	{
		//Data graphs
		//NOTE: Total should be the largest, but there could be a scenario in which
		//		one of the other errors is negative, and thus only the stat/syst could be
		//		slightly larger for some points since the other error is negative and
		//		would bring the total error down
		for(int i = 0; i < data.size(); i++) {
			graphs.push_back(data[i].GetStatisticalErrorGraph());
			graphs.push_back(data[i].GetSystematicErrorGraph());
			graphs.push_back(data[i].GetTotalErrorGraph());
		}

		//Cross sections
		for(int i = 0; i < crossSections.size(); i++) {
		  //graphs.push_back(crossSections[i].GetPDFBandResults());
                    graphs.push_back(crossSections[i].GetTotalBandResults());
		}

		xMin = SPXGraphUtilities::GetXMin(graphs);
		xMax = SPXGraphUtilities::GetXMax(graphs);
		yMin = SPXGraphUtilities::GetYMin(graphs);
		yMax = SPXGraphUtilities::GetYMax(graphs);

		//Sanity check
		if(xMin > xMax) {
			std::ostringstream oss;
			oss << cn << mn << "xMin (" << xMin << ") calculated to be larger than xMax (" << xMax << ")" << std::endl;
			throw SPXGraphException(oss.str());
		}

		if(yMin > yMax) {
			std::ostringstream oss;
			oss << cn << mn << "yMin (" << yMin << ") calculated to be larger than yMax (" << yMax << ")" << std::endl;
			throw SPXGraphException(oss.str());
		}
	}
}

//Determine frame bounds by calculating the xmin, xmax, ymin, ymax from ALL graphs being drawn
void SPXPlot::DetermineRatioFrameBounds(double &xMin, double &xMax, double &yMin, double &yMax) {
	std::string mn = "DetermineRatioFrameBounds: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	//Do nothing if ratio is not plotted
	if(!ds.ContainsRatio()) {
		xMin = xMax = yMin = yMax = 0;
		return;
	}

	//Do nothing if ratio is specified but there are no ratios to plot
	if(!ratios.size()) {
		xMin = xMax = 0;
		yMin = 0.5;
		yMax = 1.5;
		return;
	}

	std::vector<TGraphAsymmErrors *> graphs;
	{
		for(int i = 0; i < ratios.size(); i++) {
                 //TC GetRatioGraph returns now vectors 
		 //graphs.push_back(ratios[i].GetRatioGraph());
		 std::vector<TGraphAsymmErrors *> ratiographs=ratios[i].GetRatioGraph();
  		 for(int j = 0; j < ratiographs.size(); j++) {
		  graphs.push_back(ratiographs[j]);
                 }
		}

		xMin = SPXGraphUtilities::GetXMin(graphs);
		xMax = SPXGraphUtilities::GetXMax(graphs);
		yMin = SPXGraphUtilities::GetYMin(graphs);
		yMax = SPXGraphUtilities::GetYMax(graphs);

		//Sanity check
		if(xMin > xMax) {
			std::ostringstream oss;
			oss << cn << mn << "xMin (" << xMin << ") calculated to be larger than xMax (" << xMax << ")" << std::endl;
			throw SPXGraphException(oss.str());
		}

		if(yMin > yMax) {
			std::ostringstream oss;
			oss << cn << mn << "yMin (" << yMin << ") calculated to be larger than yMax (" << yMax << ")" << std::endl;
			throw SPXGraphException(oss.str());
		}
	}
}

void SPXPlot::DivideCanvasIntoPads(void) {
	std::string mn = "DivideCanvasIntoPads: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
	overlayPad->SetTickx();
	overlayPad->SetTicky();
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
	ratioPad->SetTickx();
	ratioPad->SetTicky();
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
		//ratioPad->SetLogy();	//@TODO Should I have separate log flags in steering file for overlay/ratio?
	}
}

void SPXPlot::DrawOverlayPadFrame(void) {
	std::string mn = "DrawOverlayPadFrame: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(!overlayPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::ConfigurePads before drawing the pad frame");
	}

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	DetermineOverlayFrameBounds(xMinOverlay, xMaxOverlay, yMinOverlay, yMaxOverlay);

	//Force to steering file Y bounds if set
	if(steeringFile->GetYOverlayMin() != MIN_EMPTY) {
		yMinOverlay = steeringFile->GetYOverlayMin();
		if(debug) std::cout << cn << mn << "Forcing Overlay Y Axis Minimum to " << yMinOverlay << std::endl;
	}
	if(steeringFile->GetYOverlayMax() != MAX_EMPTY) {
		yMaxOverlay = steeringFile->GetYOverlayMax();
		if(debug) std::cout << cn << mn << "Forcing Overlay Y Axis Maxmimum to " << yMaxOverlay << std::endl;
	}

	//Force to non-negative if plotting logarithmic axis
	if(pc.IsXLog()) {
		if(xMinOverlay < 0) {
			xMinOverlay = 1e-10;
			if(debug) std::cerr << cn << mn << "WARNING: Forcing non-negative Overlay X Axis since it was specified as Logarithmic" << std::endl;
		}
	}
	if(pc.IsYLog()) {
		if(yMinOverlay < 0) {
			yMinOverlay = 1e-10;
			if(debug) std::cerr << cn << mn << "WARNING: Forcing non-negative Overlay Y Axis since it was specified as Logarithmic" << std::endl;
		}
	}

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
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(!ratioPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::ConfigurePads before drawing the pad frame");
	}

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	DetermineRatioFrameBounds(xMinRatio, xMaxRatio, yMinRatio, yMaxRatio);

	//Force to steering file Y bounds if set
	if(steeringFile->GetYRatioMin() != MIN_EMPTY) {
		yMinRatio = steeringFile->GetYRatioMin();
		if(debug) std::cout << cn << mn << "Forcing Ratio Y Axis Minimum to " << yMinRatio << std::endl;
	}
	if(steeringFile->GetYRatioMax() != MAX_EMPTY) {
		yMaxRatio = steeringFile->GetYRatioMax();
		if(debug) std::cout << cn << mn << "Forcing Ratio Y Axis Maxmimum to " << yMaxRatio << std::endl;
	}

	//Force Ratio X Min/Max to match Overlay, if plotted (should alread match anyway...)
	if(ds.ContainsOverlay()) {
		xMinRatio = xMinOverlay;
		xMaxRatio = xMaxOverlay;
	}

	//@TODO What to do here for forcing Y axis within logarithmic limits like I do for overlay? It doesn't matter now, since
	//			only overlay Y is logarithmic, and the X follows the overlay exactly (if it's plotted)

	//Force to non-negative if plotting logarithmic axis
	if(pc.IsXLog()) {
		if(xMinRatio < 0) {
			xMinRatio = 1e-10;
			if(debug) std::cerr << cn << mn << "WARNING: Forcing non-negative Ratio X Axis since it was specified as Logarithmic" << std::endl;
		}
	}

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

void SPXPlot::StaggerConvoluteOverlay(void) {
	std::string mn = "StaggerConvoluteOverlay: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Change this to alter the fraction of the error range in which the point is staggered
	const int FRAC_RANGE = 4;

	//Stagger convolutes in overlay
	for(int i = 0; i < crossSections.size(); i++) {

	      //TGraphAsymmErrors *graph = crossSections[i].GetPDFBandResults();
                TGraphAsymmErrors *graph = crossSections[i].GetTotalBandResults();

		for(int j = 0; j < graph->GetN(); j++) {
			//Loop over bins
			double x = 0;
			double y = 0;
			double range = 0;
			double error = 0;
			double xh = 0;
			double xl = 0;
			double dr = 0;
			double newX = 0;

			graph->GetPoint(j, x, y);
			error = graph->GetErrorX(j);
			range = error / FRAC_RANGE;
			dr = range / graph->GetN();
			newX = x + (pow(-1, (i + 1)) * (dr *  (i + 1)));

			graph->SetPoint(j, newX, y);
		}
	}
}

void SPXPlot::StaggerConvoluteRatio(void) {
	std::string mn = "StaggerConvoluteRatio: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Change this to alter the fraction of the error range in which the point is staggered
	const int FRAC_RANGE = 4;

	//Create local ratios array
	std::vector<SPXRatio> _ratios;

	//Remove all non-convolute ratios from array:
	// This is important to have the staggering match from the overlay, since the
	// staggering algorithm is dependent on the total number of ratios, and the ratio index
	for(int i = 0; i < ratios.size(); i++) {
		//Remove if ratio style is not a convolute ratio
		if(!ratios[i].HasConvolute()) {
			if(debug) std::cout << cn << mn << "Not staggering ratio at index " << i << ": " << ratios[i].GetRatioStyle().ToString() << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "Staggering ratio at index " << i << ": " << ratios[i].GetRatioStyle().ToString() << std::endl;
			_ratios.push_back(ratios[i]);
		}
	}

	//Stagger convolutes in ratio
	for(int i = 0; i < _ratios.size(); i++) {
	  // TC GetRatioGraph returns now vector of graphs
	  //TGraphAsymmErrors *graph = _ratios[i].GetRatioGraph();
	  std::vector<TGraphAsymmErrors *> ratiographs=ratios[i].GetRatioGraph();
  		 
          for (int igraph=0; igraph < ratiographs.size(); igraph++) {
	        TGraphAsymmErrors *graph = ratiographs[igraph];
		for(int j = 0; j < graph->GetN(); j++) {
			//Loop over bins
			double x = 0;
			double y = 0;
			graph->GetPoint(j, x, y);
			double error = graph->GetErrorX(j);
			double range = error / FRAC_RANGE;
			double dr = range / graph->GetN();
			double newX = x + (pow(-1, (i + 1)) * (dr *  (i + 1)));

			graph->SetPoint(j, newX, y);
		}
	  }
	}
}

//Matches the overlay binning if the match_binning flag is set
//NOTE: Ratios are automatically matched, since they must align in order
//		to use the SPXGraphUtilities::Divide function
void SPXPlot::MatchOverlayBinning(void) {
	std::string mn = "MatchOverlayBinning: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXPlotType &pt = pc.GetPlotType();
	SPXDisplayStyle &ds = pc.GetDisplayStyle();
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	//Do nothing if not drawing overlay
	if(!ds.ContainsOverlay()) {
		return;
	}

	//Match binning of all graphs within each PCI, if matchBinning set
	if(steeringFile->GetMatchBinning()) {

		TGraphAsymmErrors *master;
		TGraphAsymmErrors *slave;

		//Match all convolutes to it's data (if data is plotted)
		if(os.ContainsConvolute() && os.ContainsData()) {

			if(debug) std::cout << cn << mn << "Matching overlay convolutes to data master" << std::endl;

			for(int i = 0; i < steeringFile->GetNumberOfConfigurationInstances(id); i++) {
				SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);

				std::string dataKey = pci.dataSteeringFile.GetFilename();
				std::string gridKey = pci.gridSteeringFile.GetFilename();
				std::string pdfKey  = pci.pdfSteeringFile.GetFilename();

				StringPair_T convoluteKey(gridKey, pdfKey);

         			if(debug) std::cout << cn << mn << "dataKey= "<<dataKey
                                                    <<"gridKey= "<<gridKey <<" pdfKey= "<<pdfKey<< std::endl;

				master = dataFileGraphMap[dataKey];
                                // loop over map instead
 				//slave = convoluteFileGraphMap[convoluteKey];
				//Check if data master is divided by bin width
				bool dividedByBinWidth = false;
				if(pci.dataSteeringFile.IsDividedByBinWidth() && !pci.gridSteeringFile.IsGridDividedByBinWidth()) {
					dividedByBinWidth = true;
                                }
                                //
                                // loop over all graphs in Map
                                // TC>>>
                                for(StringPairGraphMap_T::const_iterator it = convoluteFileGraphMap.begin(); it !=  convoluteFileGraphMap.end(); ++it) 
                                {
				 TGraphAsymmErrors *slave=it->second;
				 if (!slave) throw SPXParseException("slave graph not found ! ");                                
				 if (debug) std::cout << cn <<mn<<"match binning for graph "<<slave->GetName()<<std::endl;
				 SPXGraphUtilities::MatchBinning(master, slave, dividedByBinWidth);
                                }
			}
		}
	}
}

void SPXPlot::ApplyGridCorrections(void) {
 std::string mn = "ApplyGridCorrections: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if(steeringFile->GetGridCorr()) {
  for(int i = 0; i < crossSections.size(); i++) {
    std::cout<<i<<" fix ApplyCorrections()"<<std::endl;
    //crossSections.at(i).ApplyCorrections();
  }
 }
}

void SPXPlot::DrawOverlay(void) {
	std::string mn = "DrawOverlay: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(!overlayPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawOverlayPadFrame before drawing the overlay graphs");
	}

	//Do nothing if not drawing overlay
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXPlotType &pt = pc.GetPlotType();
	SPXDisplayStyle &ds = pc.GetDisplayStyle();
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!ds.ContainsOverlay()) {
		return;
	}

	//Change to the overlay pad
	overlayPad->cd();

	//Check if convolute is to be plotted in the overlay section
	if(os.ContainsConvolute() || os.ContainsPDF()) {

		//Stagger overlay convolute points if requested
		if(steeringFile->GetPlotStaggered() && !steeringFile->GetPlotBand()) {
			StaggerConvoluteOverlay();
		}

		//Draw cross sections on Overlay Pad
		for(int i = 0; i < crossSections.size(); i++) {

			std::string csOptions;

			if(steeringFile->GetPlotBand()) {
				csOptions = "E2";
			}

			if(steeringFile->GetPlotMarker() && !steeringFile->GetPlotBand()) {
				csOptions = "P";
			}

			//@TODO Never plot error ticks on convolute graph right now... At some point might plot when alphas uncertainties exist
			//if(!steeringFile->GetPlotErrorTicks() && !steeringFile->GetPlotBand()) {
				csOptions += "Z";
			//}

			//Set cross section X errors to 0 if not plotting band
			if(!steeringFile->GetPlotBand()) {
			      //SPXGraphUtilities::ClearXErrors(crossSections.at(i).GetPDFBandResults());
                                SPXGraphUtilities::ClearXErrors(crossSections.at(i).GetTotalBandResults());
				SPXGraphUtilities::ClearXErrors(crossSections.at(i).GetNominal());
			}

			//Warn user that the number of bins in data does not match the number in convolute, if that's the case
			if(os.ContainsData()) {
			      //unsigned int cbins = crossSections.at(i).GetPDFBandResults()->GetN();
                                unsigned int cbins = crossSections.at(i).GetTotalBandResults()->GetN();

				unsigned int dbins = data.at(0).GetTotalErrorGraph()->GetN();

				if(cbins != dbins) {
					std::cerr << cn << mn << "WARNING: The number of convolute bins (" << cbins << ") does not match the number of master data bins (" << dbins << ")" << std::endl;
					std::cerr << "\t\t\t You can enable bin matching with the \"match_binning = true\" flag in the steering file, if you would like to do so" << std::endl;
				}

				//Same for theory
				unsigned int pbins = crossSections.at(i).GetNominal()->GetN();

				if(pbins != dbins) {
					std::cerr << cn << mn << "WARNING: The number of theory cross section bins (" << pbins << ") does not match the number of master data bins (" << dbins << ")" << std::endl;
					std::cerr << "\t\t\t You can enable bin matching with the \"match_binning = true\" flag in the steering file, if you would like to do so" << std::endl;
				}
			}

			//Draw theory Cross Section 
			if(os.ContainsConvolute()) {
			  //crossSections.at(i).GetPDFBandResults()->Draw(csOptions.c_str());
                            crossSections.at(i).GetTotalBandResults()->Draw(csOptions.c_str());
			}

			//Draw PDF Nominal, if requested
			if(os.ContainsPDF()) {

				//Modify style for theory, if both convolute and theory are drawn
				if(os.ContainsConvolute()) {
				      //TGraphAsymmErrors *cs = crossSections.at(i).GetPDFBandResults();
                                        TGraphAsymmErrors *cs = crossSections.at(i).GetTotalBandResults();
					TGraphAsymmErrors *nom= crossSections.at(i).GetNominal();
					nom->SetFillStyle(3017);
					nom->SetLineStyle(2);
					nom->SetFillColor(cs->GetFillColor() + 2);
					nom->SetMarkerColor(cs->GetMarkerColor() + 2);
				}

				crossSections.at(i).GetNominal()->Draw(csOptions.c_str());
			}

                        //
                        // loop over bands (pdf, alphas, scale etc) as set in SPXPDF
                        //
                        int nbands=(crossSections[i].GetPDF())->GetNBands();
                        if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;
                        for (int iband=0; iband<nbands; iband++) {
                          SPXPDF * pdf=crossSections[i].GetPDF();
			  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
			  if (!gband) {
                           std::ostringstream oss;
                           oss << cn <<mn<<"GetBands:"<<"Band "<<iband<<" not found at index "<<i;
                           throw SPXParseException(oss.str());
			  }else {
			  if (debug) std::cout << cn << mn << "Draw band " << gband->GetName() << std::endl;  
   			  //Draw
                          gband->Draw("2");
                         }
                        }


			/*
			TGraphAsymmErrors *total= crossSections[i].GetTotalBandResults();
                        if (!total) throw SPXGraphException(cn + mn + "Total Band not found!");

			TGraphAsymmErrors *pdfb= crossSections[i].GetPDFBandResults();
                        //if (!pdfb) throw SPXGraphException(cn + mn + "PDF Band not found!");

			TGraphAsymmErrors *asb = crossSections[i].GetAlphaSBandResults();
                        //if (!asb)  throw SPXGraphException(cn + mn + "Alphas Band not found!");

			TGraphAsymmErrors *scb = crossSections[i].GetScaleBandResults();
                        //if (!scb) throw SPXGraphException(cn + mn + "Total Band not found!");

			//Print graphs
			if(debug) {
			 std::cout << cn << mn << "Printing Total Band Graph" << std::endl;
			 total->Print();
			 std::cout << std::endl;

			 if (pdfb) {
			  std::cout << cn << mn << "Printing PDF Band Graph" << std::endl;
			  pdfb->Print();
			  std::cout << std::endl;
                         }

			 if (asb) {
			  std::cout << cn << mn << "Printing Alpha S Band Graph" << std::endl;
			  asb->Print();
			  std::cout << std::endl;
                         }
              
                         if (scb) {
			  std::cout << cn << mn << "Printing Scale Band Graph" << std::endl;
			  scb->Print();
			  std::cout << std::endl;
                         }
			}

			//Draw
                        total->Draw("2");
			if (pdfb)pdfb->Draw("2");
			if (asb) asb ->Draw("2");
			if (scb) scb ->Draw("2");
*/
			if(debug) std::cout << cn << mn << "Sucessfully drew cross section for Plot " << id << " cross section " << i << \
				" with options = " << csOptions << std::endl;
		}
	}

	//Check if data is to be plotted in the overlay
	if(os.ContainsData()) {
		//Draw data graphs on Overlay Pad
		for(int i = 0; i < data.size(); i++) {

			std::string statOptions;
			std::string totOptions;

			if(steeringFile->GetPlotMarker()) {
				totOptions = "PZ";	//Never plot ticks on tot error graph
			}

			if(steeringFile->GetPlotErrorTicks()) {
				statOptions = "||";
			} else {
				statOptions = "Z";
			}

			data[i].GetTotalErrorGraph()->Draw(totOptions.c_str());
			data[i].GetStatisticalErrorGraph()->Draw(statOptions.c_str());

			if(debug) std::cout << cn << mn << "Sucessfully drew data for Plot " << id << " data " << i << " with Syst options = " \
				<< totOptions << " Stat options = " << statOptions << std::endl;
		}
	}
}

void SPXPlot::DrawRatio(void) {
	std::string mn = "DrawRatio: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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

	//Stagger ratio convolute points if requested
	if(steeringFile->GetPlotStaggered() && !steeringFile->GetPlotBand()) {
		StaggerConvoluteRatio();
	}

	//Counters for number of stat/tot ratios: Used for darkening stat/tot ratios and also
	// for sanity checking (data/stat graphs covering up other ratios)
	unsigned int statRatios = 0;
	unsigned int totRatios = 0;

	for(int i = 0; i < pc.GetNumberOfRatios(); i++) {

		std::string ratioOptions;

		if(steeringFile->GetPlotBand()) {
			ratioOptions = "E2";
		}

		if(steeringFile->GetPlotMarker() && !steeringFile->GetPlotBand()) {
			ratioOptions = "P";
		}

		if(!steeringFile->GetPlotErrorTicks() && !steeringFile->GetPlotBand()) {
			ratioOptions += "Z";
		}

		//Set x errors to zero if ratio involves convolute AND is not plot band
		if(ratios[i].HasConvolute() && !steeringFile->GetPlotBand()) {

			//Never clear X errors for DataStat or DataTot (extra check: HasConvolute() should already rule out stat/tot...)
			if(!ratios[i].IsDataStat() && !ratios[i].IsDataTot()) {
 	                        // TC GetRatioGraph returns now vector of graphs
			        //SPXGraphUtilities::ClearXErrors(ratios[i].GetRatioGraph());
	                        std::vector<TGraphAsymmErrors *> ratiographs=ratios[i].GetRatioGraph();
                                for (int igraph=0; igraph < ratiographs.size(); igraph++) {
	                         TGraphAsymmErrors *graph = ratiographs[igraph];
                                 SPXGraphUtilities::ClearXErrors(graph);
                                }
				if(debug) std::cout << cn << mn << "Set X errors to zero for ratios[" << i << "]" << std::endl;
			}
		}

		if(ratios[i].IsDataStat()) {
			statRatios++;

			//Warn if not the first graph AND first graph is not data_tot: Will possibly cover up points
			if((i != 0) && !ratios[0].IsDataTot()) {
				std::cerr << cn << mn << "WARNING: Data Stat band could possibly hide other bands/points plotted underneath it: Move data_stat to ratio_0?" << std::endl;
			}
		}

		if(ratios[i].IsDataTot()) {
			totRatios++;

			//Warn if not the first graph: Will possibly cover up points
			if(i != 0) {
				std::cerr << cn << mn << "WARNING: Data Tot band could possibly hide other bands/points plotted underneath it: Move data_to to ratio_0?" << std::endl;
			}
		}

 	       // TC GetRatioGraph returns now vector of graphs
 	       std::vector<TGraphAsymmErrors *> ratiographs=ratios[i].GetRatioGraph();	 
               for (int igraph=0; igraph < ratiographs.size(); igraph++) {
	        TGraphAsymmErrors *graph = ratiographs[igraph];

		if(ratios[i].IsDataStat() || ratios[i].IsDataTot()) {
		 //Incrementally darken the data_stat/data_tot graphs based on their order for increased visibility
 	         // TC GetRatioGraph returns now vector of graphs
	         //ratios[i].GetRatioGraph()->SetFillColor(ratios[i].GetRatioGraph()->GetFillColor() + (statRatios + totRatios));
	         graph->SetFillColor(graph->GetFillColor() + (statRatios + totRatios));
  		 //TC ratios[i].GetRatioGraph()->Draw("E2");
                 graph->Draw("E2");
		} else {
		 //ratios[i].GetRatioGraph()->Draw(ratioOptions.c_str());
                 graph->Draw(ratioOptions.c_str());
                }
		if(debug) {
		 std::cout << cn << mn << "Successfully drew ratios[" << i << "] with options: " << ratioOptions << std::endl;
		 std::cout << cn << mn << "Printing ratios[" << i << "] name= "<<graph->GetName() << std::endl;
		 std::cout << cn << mn << "fillcolor "<<graph->GetFillColor() << std::endl;
		 std::cout << cn << mn << "fillstyle "<<graph->GetFillStyle() << std::endl;
		 //ratios[i].GetRatioGraph()->Print();
                 graph->Print();
		 std::cout << std::endl;
		}
	       }
	}

	//Draw a line at 1, where ratios are relative to
	TLine *referenceLine = new TLine(xMinRatio, 1.0, xMaxRatio, 1.0);
	referenceLine->Draw();
}

void SPXPlot::DrawLegend(void) {
	std::string mn = "DrawLegend: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	return;
}

void SPXPlot::UpdateCanvas(void) {
	std::string mn = "UpdateCanvas: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(!canvas) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::CreateCanvas before updating the canvas");
	}

	canvas->Modified();
	canvas->Update();
}

void SPXPlot::CanvasToPNG(void) {
	std::string mn = "CanvasToPNG: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

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
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	std::string filename;

	//Fill in dummy description if empty
	if(desc.empty()) {
		desc = std::string("general");
	}

	filename = "./plots/" + desc + "_plot_" + (ULong_t)id + ".png";

	if(debug) std::cout << cn << mn << "Created PNG Filename: " << filename << std::endl;

	return filename;
}

void SPXPlot::InitializeRatios(void) {
	std::string mn = "InitializeRatios: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Create a ratio for each ratio instance
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);

	for(int i = 0; i < pc.GetNumberOfRatios(); i++) {

		SPXRatioStyle ratioStyle = pc.GetRatioStyle(i);
		std::string ratioString  = pc.GetRatio(i);

		try {
			SPXRatio ratioInstance = SPXRatio(pc, ratioStyle);
			ratioInstance.AddDataFileGraphMap(dataFileGraphMap);
			ratioInstance.AddReferenceFileGraphMap(referenceFileGraphMap);
			ratioInstance.AddNominalFileGraphMap(nominalFileGraphMap);
			ratioInstance.AddConvoluteFileGraphMap(convoluteFileGraphMap);
			ratioInstance.Parse(ratioString);
			ratioInstance.GetGraphs();
			ratioInstance.Divide();

			ratios.push_back(ratioInstance);

		} catch(const SPXException &e) {
			throw;
		}
	}
}

void SPXPlot::InitializeCrossSections(void) {
	std::string mn = "InitializeCrossSections: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!os.ContainsConvolute()) {
		if(debug) std::cout << cn << mn << "Display style does not contain convolute: Do nothing" << std::endl;
		return;
	}

	std::vector<SPXPlotConfigurationInstance> pcis;

	//Create cross sections for each configuration instance
	if(debug) std::cout << cn << mn << "Create cross sections for each configuration instance" << std::endl;
	for(int i = 0; i < steeringFile->GetNumberOfConfigurationInstances(id); i++) {
		SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);

		//Don't add the cross section to the cross section vector if there is already a cross section with
		// the same exact grid and pdf file...
		StringPair_T key = StringPair_T(pci.gridSteeringFile.GetFilename(), pci.pdfSteeringFile.GetFilename());

		if(debug) std::cout << cn << mn << "Checking for prior existence of convolute with key = [" << key.first << \
			", " << key.second << "]" << std::endl;

		if(crossSectionSet.count(key) != 0) {
			if(debug) std::cout << cn << mn << "Convolute with grid filename \"" << key.first << "\" and pdf filename \"" \
				<< key.second << "\" has already been processed: Will not process duplicate" << std::endl;

			continue;
		}

		crossSectionSet.insert(key);
		if(debug) std::cout << cn << mn << "Added convolute with key = [" << key.first << ", " << key.second << "] to crossSectionsSet" << std::endl;

		try {
                        // TC changed this grid steering is in plot configuration
			//SPXPDFSteeringFile &psf = pci.pdfSteeringFile;
			//SPXCrossSection crossSectionInstance = SPXCrossSection(&psf, &pci);
 		        SPXCrossSection crossSectionInstance = SPXCrossSection(&pci);
			pcis.push_back(pci);
			crossSectionInstance.Create(steeringFile);

			if(steeringFile->GetGridCorr()) {
				crossSectionInstance.ParseCorrections();
			}

			crossSections.push_back(crossSectionInstance);
		} catch(const SPXException &e) {
			throw;
		}
	}

	if(debug) std::cout << cn << mn << "Loop over cross section size=" << crossSections.size() <<std::endl;

	for(int i = 0; i < crossSections.size(); i++) {
		SPXPlotConfigurationInstance &pci = pcis[i];
	       /*
    	        if(debug) std::cout << cn << mn << i<<" Get graphPDF" <<std::endl;
		TGraphAsymmErrors *graphPDF = crossSections[i].GetPDFBandResults();
                if (!graphPDF) std::cout << cn << mn << i<<" graphPDF not found !" <<std::endl;

    	        if(debug) std::cout << cn << mn << i<<" Get graphAlphaS" <<std::endl;
		TGraphAsymmErrors *graphAlphaS = crossSections[i].GetAlphaSBandResults();
                if (!graphAlphaS) std::cout << cn << mn << i<<" graphAlphaS not found !" <<std::endl;

    	        if(debug) std::cout << cn << mn << i<<" Get graphScale" <<std::endl;
		TGraphAsymmErrors *graphScale = crossSections[i].GetScaleBandResults();
                if (!graphScale) std::cout << cn << mn << i<<" graphScale not found !" <<std::endl;

		StringPair_T convolutePair = StringPair_T(pci.gridSteeringFile.GetFilename(), pci.pdfSteeringFile.GetFilename());
                // XXX need to think what to do here, for the moment take graphPDF is there
		TGraphAsymmErrors *graph = 0;
                if (graphScale)  graph=graphScale;
                if (graphAlphaS) graph=graphAlphaS;
                if (graphPDF)    graph=graphPDF;
               */         		
               int nbands=(crossSections[i].GetPDF())->GetNBands();
               if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;
               for (int iband=0; iband<nbands; iband++) {
                SPXPDF * pdf=crossSections[i].GetPDF();
		TGraphAsymmErrors * gband   =pdf->GetBand(iband);
		string              gtype   =pdf->GetBandType(iband);
		if (!gband) {
                 std::ostringstream oss;
                 oss << cn <<mn<<"GetBands:"<<"Band "<<iband<<" not found at index "<<i;
                 throw SPXParseException(oss.str());
                }
                if (debug) cout << cn <<mn<<"Band "<<gband->GetName()<<" type= "<<gtype.c_str()<<endl;
		//Update the Convolute File Map
		//string theoryname=pci.pdfSteeringFile.GetFilename()+gband->GetName();
                string theoryname=gband->GetName();
   	        StringPair_T convolutePair = StringPair_T(pci.gridSteeringFile.GetFilename(), theoryname);
                convoluteFileGraphMap.insert(StringPairGraphPair_T(convolutePair, gband));
                // would be better to introduce a map
                int markerstyle, fillcolor,fillstyle;
                if (gtype.compare(string("pdf"))==0){
		 //if (debug) std::cout << cn << mn <<" matched pdf "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.pdfMarkerStyle;
                 fillcolor  =pci.pdfFillColor;
                 fillstyle  =pci.pdfFillStyle;
                } 
                if (gtype.compare(string("alphas"))==0){
		  //if (debug) std::cout << cn << mn <<" matched alphas "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.alphasMarkerStyle;
                 fillcolor  =pci.alphasFillColor;
                 fillstyle  =pci.alphasFillStyle;
                } 
                if (gtype.compare(string("scale"))==0){
		 //if (debug) std::cout << cn << mn <<" matched scale "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.scaleMarkerStyle;
                 fillcolor  =pci.scaleFillColor;
                 fillstyle  =pci.scaleFillStyle;
                } 
                if (gtype.compare(string("total"))==0){
		 //if (debug) std::cout << cn << mn <<" matched total "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.totalMarkerStyle;
                 fillcolor  =pci.totalFillColor;
                 fillstyle  =pci.totalFillStyle;
                } 

                if (debug)
		  std::cout << cn << mn <<"gband: "<< gband->GetName()<<" Setting: \n"
                                        <<" fillcolor= "<< fillcolor
                                        <<" fillstyle= "<<fillstyle
                                        <<" markerstyle= "<<markerstyle <<std::endl;

		gband->SetMarkerSize(1.2);
		gband->SetMarkerStyle(markerstyle);
		gband->SetMarkerColor(fillcolor);
		gband->SetLineColor  (fillcolor);
		gband->SetFillStyle  (fillstyle);
		gband->SetFillColor  (fillcolor);
	       }
               StringPair_T convolutePair = StringPair_T(pci.gridSteeringFile.GetFilename(), pci.pdfSteeringFile.GetFilename());
	       //Update the Reference File Map
    	       if(debug) std::cout << cn << mn << i<<" Get refGraph" <<std::endl;
	       TGraphAsymmErrors *refGraph = crossSections[i].GetGridReference();
               if (!refGraph) std::cout << cn << mn << i<<" refGraph not found !" <<std::endl;
	       referenceFileGraphMap.insert(StringPairGraphPair_T(convolutePair, refGraph));

	       //Update the Nominal File Map
    	       if(debug) std::cout << cn << mn << i<<" Get nomGraph" <<std::endl;
	       TGraphAsymmErrors *nomGraph = crossSections[i].GetNominal();
               if (!nomGraph) std::cout << cn << mn << i<<" nomGraph not found !" <<std::endl;
	       nominalFileGraphMap.insert(StringPairGraphPair_T(convolutePair, nomGraph));

	       /*
		//Style convolute/reference/nominal graph
                if (graphPDF) {
         	 if(debug) std::cout << cn << mn << i<<" Set graphPDF" <<std::endl;
   		 graphPDF->SetMarkerSize(1.2);
		 graphPDF->SetMarkerStyle(pci.pdfMarkerStyle);
		 graphPDF->SetMarkerColor(pci.pdfFillColor);
		 graphPDF->SetLineColor(pci.pdfFillColor);
		 graphPDF->SetFillStyle(pci.pdfFillStyle);
		 graphPDF->SetFillColor(pci.pdfFillColor);
                }

                if (graphAlphaS) {
      	         if(debug) std::cout << cn << mn << i<<" Set graphAlphas" <<std::endl;
         	 graphAlphaS->SetMarkerSize(1.2);
		 graphAlphaS->SetMarkerStyle(pci.alphasMarkerStyle);
		 graphAlphaS->SetMarkerColor(pci.alphasFillColor);
		 graphAlphaS->SetLineColor(pci.alphasFillColor);
		 graphAlphaS->SetFillStyle(pci.alphasFillStyle);
		 graphAlphaS->SetFillColor(pci.alphasFillColor);
                }

                if (graphScale) {
    	         if(debug) std::cout << cn << mn << i<<" Set graphScale" <<std::endl;
		 graphScale->SetMarkerSize(1.2);
		 graphScale->SetMarkerStyle(pci.scaleMarkerStyle);
		 graphScale->SetMarkerColor(pci.scaleFillColor);
		 graphScale->SetLineColor(pci.scaleFillColor);
		 graphScale->SetFillStyle(pci.scaleFillStyle);
		 graphScale->SetFillColor(pci.scaleFillColor);
                }
	       */
               if (refGraph) {
     	        if(debug) std::cout << cn << mn << i<<" Set refGraph" <<std::endl;
	        refGraph->SetMarkerSize(1.2);
	        refGraph->SetMarkerStyle(pci.pdfMarkerStyle);
	        refGraph->SetMarkerColor(pci.pdfFillColor);
	        refGraph->SetLineColor(pci.pdfFillColor);
	        refGraph->SetFillStyle(pci.pdfFillStyle);
	        refGraph->SetFillColor(pci.pdfFillColor);
               }

               if (nomGraph) {
    	        if(debug) std::cout << cn << mn << i<<" Set nomGraph" <<std::endl;
	        nomGraph->SetMarkerSize(1.2);
	        nomGraph->SetMarkerStyle(pci.pdfMarkerStyle);
	        nomGraph->SetMarkerColor(pci.pdfFillColor);
	        nomGraph->SetLineColor(pci.pdfFillColor);
	        nomGraph->SetFillStyle(pci.pdfFillStyle);
	        nomGraph->SetFillColor(pci.pdfFillColor);
               }

    	       if(debug) std::cout << cn << mn << i<<" ...finished GraphSetting" <<std::endl;
	       if(convoluteFileGraphMap.count(convolutePair)) {
			if(debug) {
				std::cout << cn << mn << "Added convolute pair to map: [" << convolutePair.first << ", " << convolutePair.second << "]" << std::endl;
			}
		} else {
			std::cerr << "---> Warning: Unable to add convolute pair to map: [" << convolutePair.first << ", " << convolutePair.second << "]" << std::endl;
		}
	}
        if(debug) std::cout << cn << mn <<" ...finished !" <<std::endl;
}

void SPXPlot::NormalizeCrossSections(void) {
	std::string mn = "NormalizeCrossSections: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!os.ContainsConvolute()) {
		if(debug) std::cout << cn << mn << "Display style does not contain convolute: Do nothing" << std::endl;
		return;
	}

	for(int i = 0; i < crossSections.size(); i++) {
		try {
			if(debug) std::cout << cn << mn << "Normalizing Cross Section at index " << i << std::endl;

			SPXPDFSteeringFile *psf = crossSections[i].GetPDFSteeringFile();
			SPXPlotConfigurationInstance *pci = crossSections[i].GetPlotConfigurationInstance();

			std::string masterXUnits=pci->dataSteeringFile.GetXUnits();
			std::string slaveXUnits =pci->gridSteeringFile.GetXUnits();
			std::string masterYUnits=pci->dataSteeringFile.GetYUnits();
			std::string slaveYUnits =pci->gridSteeringFile.GetYUnits();

			/*                       		       
			TGraphAsymmErrors * gTotal   = crossSections[i].GetTotalBandResults();
                        if (!gTotal) { 
                         std::ostringstream oss;
                         oss << cn << mn << "ERRORTotal graph not found at index "<<i; 
                         throw SPXParseException(oss.str());
                        }

			TGraphAsymmErrors * gPDF   = crossSections[i].GetPDFBandResults();
                        if (!gPDF) std::cout << cn << mn << "PDF graph not found at index " << i << std::endl;

			TGraphAsymmErrors * gAlphaS= crossSections[i].GetAlphaSBandResults();
                        if (!gAlphaS) std::cout << cn << mn << "AlphaS graph not found at index " << i << std::endl;

			TGraphAsymmErrors * gScale = crossSections[i].GetScaleBandResults();
                        if (!gScale) std::cout << cn << mn << "Scale graph not found at index " << i << std::endl;
			*/
                       
			TGraphAsymmErrors * gNom = crossSections[i].GetNominal();
                        if (!gNom) std::cout << cn << mn << "gNom graph not found at index " << i << std::endl;

			TGraphAsymmErrors * gRef = crossSections[i].GetGridReference();
                        if (!gRef) std::cout << cn << mn << "gRef graph not found at index " << i << std::endl;

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
			if(debug) {
			 std::cout << cn << mn << "Additional artificial scale for Cross Section/Reference: " << std::endl;
			 std::cout << "\t X Scale: " << pci->xScale << std::endl;
			 std::cout << "\t Y Scale: " << pci->yScale << std::endl << std::endl;
			}
			SPXGraphUtilities::Scale(gNom, xScale, yScale);
			SPXGraphUtilities::Scale(gRef, xScale, yScale);
			//Also scale by the artificial grid scale from the grid steering file
			xScale = 1.0;
			yScale = pci->gridSteeringFile.GetYScale();
                        //
                        // loop over bands (pdf, alphas, scale etc) as set in SPXPDF
                        //
                        int nbands=(crossSections[i].GetPDF())->GetNBands();
                        if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;
                        for (int iband=0; iband<nbands; iband++) {
                          SPXPDF * pdf=crossSections[i].GetPDF();
			  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
			  if (!gband) {
                           std::ostringstream oss;
                           oss << cn <<mn<<"Band "<<iband<<" not found at index "<<i;
                           throw SPXParseException(oss.str());
			  } else {
			  if (debug) std::cout << cn << mn << "scale band " << gband->GetName() << std::endl;  
                          SPXGraphUtilities::Scale(gband , xScale, yScale);
                         }
                        }

			//if (gTotal)  SPXGraphUtilities::Scale(gTotal, xScale, yScale);
			//if (gPDF)    SPXGraphUtilities::Scale(gPDF,   xScale, yScale);
			//if (gAlphaS) SPXGraphUtilities::Scale(gAlphaS,xScale, yScale);
			//if (gScale)  SPXGraphUtilities::Scale(gScale, xScale, yScale);

			//Check if data/grid are/are not normalized by total sigma or bin width
			bool normalizeToTotalSigma = pci->dataSteeringFile.IsNormalizedToTotalSigma();
			bool dataDividedByBinWidth = pci->dataSteeringFile.IsDividedByBinWidth();
			bool gridDividedByBinWidth = pci->gridSteeringFile.IsGridDividedByBinWidth();
			bool referenceDividedByBinWidth = pci->gridSteeringFile.IsReferenceDividedByBinWidth();

			if(debug) {
			 std::cout << cn << mn << "normalizeToTotalSigma is " << (normalizeToTotalSigma ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "dataDividedByBinWidth is " << (dataDividedByBinWidth ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "gridDividedByBinWidth is " << (gridDividedByBinWidth ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "referenceDividedByBinWidth is " << (referenceDividedByBinWidth ? "ON" : "OFF") << std::endl;
			}

			if(!dataDividedByBinWidth && gridDividedByBinWidth) {
			 throw SPXGraphException(cn + mn + "Grid IS divided by the bin with but the data IS NOT: Not supported");
			}
			/*
			double totalSigma=0.;
			double totalSigmaPDF=0.;
			double totalSigmaAlphaS=0.;
			double totalSigmaScale=0.;
                        if (gTotal)  totalSigma=SPXGraphUtilities::GetTotalSigma(gTotal, gridDividedByBinWidth);
                        if (gPDF)    totalSigmaPDF   =SPXGraphUtilities::GetTotalSigma(gPDF, gridDividedByBinWidth);
                        if (gAlphaS) totalSigmaAlphaS=SPXGraphUtilities::GetTotalSigma(gAlphaS, gridDividedByBinWidth);
                        if (gScale) totalSigmaScale=SPXGraphUtilities::GetTotalSigma(gScale, gridDividedByBinWidth);
			double totalSigmaNom = SPXGraphUtilities::GetTotalSigma(gNom, gridDividedByBinWidth);
			double totalSigmaRef = SPXGraphUtilities::GetTotalSigma(gRef, referenceDividedByBinWidth);

			if(debug) {
			  std::cout<<cn<<mn<< "Total Sigma    = "      << totalSigma      << std::endl;
			  std::cout<<cn<<mn<< "Total SigmaPDF = "      << totalSigmaPDF   << std::endl;
			  std::cout<<cn<<mn<< "Total SigmaAlphaS= "    << totalSigmaAlphaS << std::endl;
			  std::cout<<cn<<mn<< "Total SigmaScale = "    << totalSigmaScale << std::endl;
			  std::cout<<cn<<mn<< "Nominal Total Sigma = " << totalSigmaNom   << std::endl;
			  std::cout<<cn<<mn<< "Reference Total Sigma = "<< totalSigmaRef  << std::endl;
			}
			*/
			double totalSigmaNom = SPXGraphUtilities::GetTotalSigma(gNom, gridDividedByBinWidth);
			double totalSigmaRef = SPXGraphUtilities::GetTotalSigma(gRef, referenceDividedByBinWidth);
			if(debug) {
			  std::cout<<cn<<mn<< "Nominal Total Sigma = " << totalSigmaNom   << std::endl;
			  std::cout<<cn<<mn<< "Reference Total Sigma = "<< totalSigmaRef  << std::endl;
			}
			//First divide the cross section by the bin width if it needs to be
			//@TODO Do I need to do this also for the Alpha S and Scale Uncertainty bands?
			if(dataDividedByBinWidth && !gridDividedByBinWidth) {
			 if(debug) std::cout << cn << mn << "Dividing Cross Section by the Bin Width" << std::endl;
			 //	if (gTotal)  SPXGraphUtilities::DivideByBinWidth(gTotal);
			 //	if (gPDF)    SPXGraphUtilities::DivideByBinWidth(gPDF);
			 //	if (gAlphaS) SPXGraphUtilities::DivideByBinWidth(gAlphaS);
			 //	if (gScale)  SPXGraphUtilities::DivideByBinWidth(gScale);
			 SPXGraphUtilities::DivideByBinWidth(gNom);
                        }
			if(dataDividedByBinWidth && !referenceDividedByBinWidth) {
			  if(debug) std::cout << cn << mn << "Dividing Grid Reference by the Bin Width" << std::endl;
			  SPXGraphUtilities::DivideByBinWidth(gRef);
			}

			//Set the yBinWidthScale, which is the scaling of the data's Y Bin Width Units to the data's X Units
			double yBinWidthScale = SPXGraphUtilities::GetYBinWidthUnitsScale(pci->dataSteeringFile.GetXUnits(), pci->dataSteeringFile.GetYBinWidthUnits());

			SPXGraphUtilities::Scale(gNom, 1.0, (1.0 / yBinWidthScale));
			SPXGraphUtilities::Scale(gRef, 1.0, (1.0 / yBinWidthScale));

                        //
                        // loop over bands (pdf, alphas, scale etc) as set in SPXPDF
                        //
                        if (debug) std::cout << cn << mn <<"Normalise bands Number of bands= " <<nbands<< std::endl;
                        for (int iband=0; iband<nbands; iband++) {
                          SPXPDF * pdf=crossSections[i].GetPDF();
			  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
			  if (!gband) {
                           std::ostringstream oss;
                           oss << cn <<mn<<"Band "<<iband<<" not found at index "<<i;
                           throw SPXParseException(oss.str());
			  }else {
			  if (debug) std::cout << cn << mn << "Normalise band " << gband->GetName() << std::endl;  
                          double totalSigma=SPXGraphUtilities::GetTotalSigma(gband, gridDividedByBinWidth);
    			  if(dataDividedByBinWidth && !gridDividedByBinWidth) {
			  if(debug) std::cout << cn << mn << "Divide by binwidth "<< std::endl;
                           SPXGraphUtilities::DivideByBinWidth(gband);
                          }

			  if(debug) std::cout << cn << mn << "Scaling by 1 / Y Bin Width Scale: " << (1.0 / yBinWidthScale) << std::endl;
                          SPXGraphUtilities::Scale(gband, 1.0, (1.0 / yBinWidthScale));
   			  if(normalizeToTotalSigma) {
			   if(totalSigma== 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma is zero");
			   if(debug) std::cout << cn << mn << "Scaling by 1 / total sigma: " << std::scientific << (1.0 / totalSigma) << std::endl;
			   SPXGraphUtilities::Scale(gband, 1.0, (1.0 / totalSigma));
                          }
                         }
                        }
			//}
			/*
			if (gTotal)  SPXGraphUtilities::Scale(gTotal, 1.0, (1.0 / yBinWidthScale));
                        if (gPDF)    SPXGraphUtilities::Scale(gPDF, 1.0, (1.0 / yBinWidthScale));
                        if (gAlphaS) SPXGraphUtilities::Scale(gAlphaS, 1.0, (1.0 / yBinWidthScale));
                        if (gScale)  SPXGraphUtilities::Scale(gScale, 1.0, (1.0 / yBinWidthScale));

			if(normalizeToTotalSigma) {
                                if (gTotal) {
				 if(totalSigma== 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma is zero");

				if(debug) std::cout << cn << mn << "Scaling by 1 / total sigma: " << std::scientific << (1.0 / totalSigma) << std::endl;
				 SPXGraphUtilities::Scale(gTotal, 1.0, (1.0 / totalSigma));
                                }

                                if (gPDF) {
				 if(totalSigmaPDF == 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma PDF is zero");

				if(debug) std::cout << cn << mn << "Scaling by 1 / total sigmaPDF: " << std::scientific << (1.0 / totalSigmaPDF) << std::endl;
				 SPXGraphUtilities::Scale(gPDF, 1.0, (1.0 / totalSigmaPDF));
                                }
                                if (gAlphaS) {
				 if(totalSigmaAlphaS == 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma Alphas is zero");

				if(debug) std::cout << cn << mn << "Scaling by 1 / total sigmaAlphaS: " << std::scientific << (1.0 / totalSigmaAlphaS) << std::endl;
				 SPXGraphUtilities::Scale(gAlphaS, 1.0, (1.0 / totalSigmaAlphaS));
                                }
                                if (gScale) {
				 if(totalSigmaScale == 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma Scale is zero");

				if(debug) std::cout << cn << mn << "Scaling by 1 / total sigmaScale: " << std::scientific << (1.0 / totalSigmaScale) << std::endl;
				 SPXGraphUtilities::Scale(gScale, 1.0, (1.0 / totalSigmaScale));
                                }

				SPXGraphUtilities::Scale(gNom, 1.0, (1.0 / totalSigmaNom));
				SPXGraphUtilities::Scale(gRef, 1.0, (1.0 / totalSigmaRef));
			}

			if(debug) std::cout << cn << mn << "Sucessfully normalized Cross Section i= " << i << std::endl;

			//Print cross section
			if(gTotal&&debug) {
				std::cout << cn << mn << "Printing Cross Section Total i= " << i << std::endl;
				gTotal->Print();
				std::cout << std::endl;
			}

			if(gPDF&&debug) {
				std::cout << cn << mn << "Printing Cross Section PDF i= " << i << std::endl;
				gPDF->Print();
				std::cout << std::endl;
			}

			if(gAlphaS&&debug) {
				std::cout << cn << mn << "Printing Cross Section AlphaS i= " << i << std::endl;
				gAlphaS->Print();
				std::cout << std::endl;
			}

			if(gScale&&debug) {
				std::cout << cn << mn << "Printing Cross Section Scale i= " << i << std::endl;
				gScale->Print();
				std::cout << std::endl;
			}

			//Print PDF nominal
			if(debug) {
				std::cout << cn << mn << "Printing Cross Section Nominal " << i << std::endl;
				gNom->Print();
				std::cout << std::endl;
			}

			//Print Grid Reference
			if(debug) {
				std::cout << cn << mn << "Printing Grid Reference " << i << std::endl;
				gRef->Print();
				std::cout << std::endl;
			}
			*/

		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXGraphException("SPXPlot::NormalizeCrossSections: Unable to obtain X/Y Scale based on Data/Grid Units");
		}
	}
}

void SPXPlot::InitializeData(void) {
	std::string mn = "InitializeData: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!os.ContainsData()) {
		if(debug) std::cout << cn << mn << "Display style does not contain convolute: Do nothing" << std::endl;
		return;
	}

	//Create data objects for each configuration instance of this plot and add them to vector
	for(int i = 0; i < steeringFile->GetNumberOfConfigurationInstances(id); i++) {
		SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);

		std::string key = pci.dataSteeringFile.GetFilename();

		if(debug) std::cout << cn << mn << "Checking for prior existence of data with key = [" << key << "]" << std::endl;

		//Check if data with the same steering file has already been added to the data vector (same data...)
		// Don't add to vector if it already exists
		if(dataSet.count(key) != 0) {
			if(debug) std::cout << cn << mn << "Data with filename \"" << pci.dataSteeringFile.GetFilename() << \
				"\" has already been processed: Will not process duplicate" << std::endl;

			continue;
		}

		//Add data steering file to data set
		dataSet.insert(key);

		if(debug) std::cout << cn << mn << "Added data with key = [" << key << "] to dataSet" << std::endl;

		SPXData dataInstance = SPXData(pci);

		//@DEBUG
		std::cout  << cn << mn << pci.dataDirectory << std::endl;
		std::cout  << cn << mn << pci.dataSteeringFile.GetFilename() << std::endl;

		try {
			dataInstance.Parse();
			dataInstance.Print();
			data.push_back(dataInstance);
		} catch(const SPXException &e) {
			throw;
		}
	}

	if(debug) std::cout << cn << mn << " Loop over data with size" <<data.size()<< std::endl;
	//Go through data objects and create graphs, modify settings, etc.
	for(int i = 0; i < data.size(); i++) {

		//Create graphs: once this function is called, it is safe to manipulate graphs
		data[i].CreateGraphs();

		//Obtain the graphs
		TGraphAsymmErrors *statGraph = data[i].GetStatisticalErrorGraph();
		TGraphAsymmErrors *systGraph = data[i].GetSystematicErrorGraph();
		TGraphAsymmErrors *totGraph  = data[i].GetTotalErrorGraph();

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
		SPXGraphUtilities::Scale(totGraph,  xScale, yScale);

		//Modify Data Graph styles
		statGraph->SetMarkerStyle(pci.dataMarkerStyle);
		systGraph->SetMarkerStyle(pci.dataMarkerStyle);
		totGraph->SetMarkerStyle(pci.dataMarkerStyle);

		statGraph->SetMarkerColor(pci.dataMarkerColor);
		systGraph->SetMarkerColor(pci.dataMarkerColor);
		totGraph->SetMarkerColor(pci.dataMarkerColor);

		statGraph->SetMarkerSize(1.0);
		systGraph->SetMarkerSize(1.0);
		totGraph->SetMarkerSize(1.0);

		statGraph->SetLineColor(pci.dataMarkerColor);
		systGraph->SetLineColor(pci.dataMarkerColor);
		totGraph->SetLineColor(pci.dataMarkerColor);

		statGraph->SetLineWidth(1);
		systGraph->SetLineWidth(1);
		totGraph->SetLineWidth(1);

		//Add total error graph and stat error graph to dataFileGraphMap
		dataFileGraphMap.insert(StringGraphPair_T(pci.dataSteeringFile.GetFilename(), totGraph));
		dataFileGraphMap.insert(StringGraphPair_T(pci.dataSteeringFile.GetFilename() + "_stat", statGraph));

		if(dataFileGraphMap.count(pci.dataSteeringFile.GetFilename())) {
			if(debug) {
				std::cout << cn << mn << "Added data to map: [" << pci.dataSteeringFile.GetFilename() << "]" << std::endl;
			}
		} else {
			std::cerr << "---> Warning: Unable to add data to map: [" << pci.dataSteeringFile.GetFilename() << "]" << std::endl;
		}
	}
}
