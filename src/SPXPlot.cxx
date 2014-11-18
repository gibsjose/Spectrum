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
	try {
		InitializeData();
		InitializeCrossSections();
		NormalizeCrossSections();
		InitializeRatios();
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
	DrawLegend();
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

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	//Do nothing if overlay is not plotted
	if(!ds.ContainsRatio()) {
		xMin = xMax = yMin = yMax = 0;
		return;
	}

	std::vector<TGraphAsymmErrors *> graphs;
	{
		for(int i = 0; i < ratios.size(); i++) {
			graphs.push_back(ratios[i].GetRatioGraph());
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

	std::cout << "TEST" << std::endl;

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

	if(!overlayPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::ConfigurePads before drawing the pad frame");
	}

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

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

	//Get the plot configuration and display style from steering file
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();

	DetermineRatioFrameBounds(xMinRatio, xMaxRatio, yMinRatio, yMaxRatio);

	//Force Ratio X Min/Max to match Overlay, if plotted (should alread match anyway...)
	if(ds.ContainsOverlay()) {
		xMinRatio = xMinOverlay;
		xMaxRatio = xMaxOverlay;
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

	//Change this to alter the fraction of the error range in which the point is staggered
	const int FRAC_RANGE = 4;

	//Stagger convolutes in overlay
	for(int i = 0; i < crossSections.size(); i++) {

		TGraphAsymmErrors *graph = crossSections[i].GetPDFBandResults();

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

		TGraphAsymmErrors *graph = _ratios[i].GetRatioGraph();

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

void SPXPlot::DrawOverlay(void) {
	std::string mn = "DrawOverlay: ";

	if(!overlayPad) {
		throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawOverlayPadFrame before drawing the overlay graphs");
	}

	//Do nothing if not drawing overlay
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXDisplayStyle &ds = pc.GetDisplayStyle();
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!ds.ContainsOverlay()) {
		return;
	}

	//Change to the overlay pad
	overlayPad->cd();

	//@TODO Reference Overlay

	//Check if convolute is to be plotted in the overlay section
	if(os.ContainsConvolute()) {

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
				SPXGraphUtilities::ClearXErrors(crossSections[i].GetPDFBandResults());
			}

			crossSections[i].GetPDFBandResults()->Draw(csOptions.c_str());

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
				SPXGraphUtilities::ClearXErrors(ratios[i].GetRatioGraph());
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

		if(ratios[i].IsDataStat() || ratios[i].IsDataTot()) {

			//Incrementally darken the data_stat/data_tot graphs based on their order for increased visibility
			{
				ratios[i].GetRatioGraph()->SetFillColor(ratios[i].GetRatioGraph()->GetFillColor() + (statRatios + totRatios));
			}

			ratios[i].GetRatioGraph()->Draw("E2");
			if(debug) {
				std::cout << cn << mn << "Successfully drew ratios[" << i << "] with options: " << "E2" << std::endl;
				std::cout << cn << mn << "Printing ratios[" << i << "]" << std::endl;
				ratios[i].GetRatioGraph()->Print();
				std::cout << std::endl;
			}
		} else {
			ratios[i].GetRatioGraph()->Draw(ratioOptions.c_str());
			if(debug) {
				std::cout << cn << mn << "Successfully drew ratios[" << i << "] with options: " << ratioOptions << std::endl;
				std::cout << cn << mn << "Printing ratios[" << i << "]" << std::endl;
				ratios[i].GetRatioGraph()->Print();
				std::cout << std::endl;
			}
		}
	}

	//Draw a line at 1, where ratios are relative to
	TLine *referenceLine = new TLine(xMinRatio, 1.0, xMaxRatio, 1.0);
	referenceLine->Draw();
}

void SPXPlot::DrawLegend(void) {
	return;
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

void SPXPlot::InitializeRatios(void) {
	std::string mn = "InitializeRatios: ";

	//Create a ratio for each ratio instance
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);

	for(int i = 0; i < pc.GetNumberOfRatios(); i++) {

		SPXRatioStyle ratioStyle = pc.GetRatioStyle(i);
		std::string ratioString = pc.GetRatio(i);

		try {
			SPXRatio ratioInstance = SPXRatio(pc, ratioStyle);
			ratioInstance.AddDataFileGraphMap(dataFileGraphMap);
			ratioInstance.AddReferenceFileGraphMap(referenceFileGraphMap);
			ratioInstance.AddConvoluteFileGraphMap(convoluteFileGraphMap);
			ratioInstance.SetDataDirectory(steeringFile->GetDataDirectory());
			ratioInstance.SetGridDirectory(steeringFile->GetGridDirectory());
			ratioInstance.SetPDFDirectory(steeringFile->GetPDFDirectory());
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

	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!os.ContainsConvolute()) {
		if(debug) std::cout << cn << mn << "Display style does not contain convolute: Do nothing" << std::endl;
		return;
	}

	std::vector<SPXPlotConfigurationInstance> pcis;

	//Create cross sections for each configuration instance
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
			SPXPDFSteeringFile &psf = pci.pdfSteeringFile;
			SPXCrossSection crossSectionInstance = SPXCrossSection(&psf, &pci);
			pcis.push_back(pci);
			crossSectionInstance.Create();
			crossSections.push_back(crossSectionInstance);
		} catch(const SPXException &e) {
			throw;
		}
	}

	for(int i = 0; i < crossSections.size(); i++) {
		SPXPlotConfigurationInstance &pci = pcis[i];

		//Update the Convolute File Map
		StringPair_T convolutePair = StringPair_T(pci.gridSteeringFile.GetFilename(), pci.pdfSteeringFile.GetFilename());
		TGraphAsymmErrors *graph = crossSections[i].GetPDFBandResults();
		convoluteFileGraphMap.insert(StringPairGraphPair_T(convolutePair, graph));

		//Style convolute graph
		graph->SetMarkerSize(1.2);
		graph->SetMarkerStyle(pci.pdfMarkerStyle);
		graph->SetMarkerColor(pci.pdfFillColor);
		graph->SetLineColor(pci.pdfFillColor);
		graph->SetFillStyle(pci.pdfFillStyle);
		graph->SetFillColor(pci.pdfFillColor);

		if(convoluteFileGraphMap.count(convolutePair)) {
			if(debug) {
				std::cout << cn << mn << "Added convolute pair to map: [" << convolutePair.first << ", " << convolutePair.second << "]" << std::endl;
			}
		} else {
			std::cerr << "---> Warning: Unable to add convolute pair to map: [" << convolutePair.first << ", " << convolutePair.second << "]" << std::endl;
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

			//@TODO Change back to being initialized as false: Why does this not work???
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
		TGraphAsymmErrors *totGraph = data[i].GetTotalErrorGraph();

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
		SPXGraphUtilities::Scale(totGraph, xScale, yScale);

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
