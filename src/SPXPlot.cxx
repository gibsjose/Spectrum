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

const std::string cn = "SPXPlot::";

//Must define the static debug variable in the implementation
bool SPXPlot::debug;

//SPXPlot::~SPXPlot(){
// std::cout<<cn<<" destructor delete canvas "<<std::endl;
//
// if (canvas) delete canvas;
//}
#ifdef TIMER
class quick_timer { 
public:
  quick_timer() : _timer(appl_timer_start()) {  }
  double time()     { return appl_timer_stop(_timer); }
private:
  struct timeval _timer;
};
#endif
//Initialize all plots
void SPXPlot::Initialize(void) {
	std::string mn = "Initialize: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Initializing Plot with ID id= " << id << std::endl;

	try {

#ifdef TIMER     
                std::cout<<cn<<mn<<"TIMER InitializeData " << std::endl;
                quick_timer t0;
#endif
		InitializeData();
                if (debug) std::cout << cn << mn << "finished InitializeData id= " << id << std::endl;
#ifdef TIMER     
                std::cout<<cn<<mn<<"TIMER InitializeData to= "<< t0.time()<<" [ms]"<< std::endl;
                quick_timer t1;
#endif
		InitializeCrossSections();
                if (debug) std::cout << cn << mn << "finished InitializeCrossSections id= " << id << std::endl;

#ifdef TIMER     
                std::cout<<cn<<mn<<"TIMER InitializeCrossSections t1= "<< t1.time()<<" [ms]"<< std::endl;
                quick_timer t2;
#endif
		NormalizeCrossSections();
                if (debug) std::cout << cn << mn << "finished NormalizeCrossSections id= " << id << std::endl;

#ifdef TIMER     
                std::cout<<cn<<mn<<"TIMER NormalizeCrossSections t2= "<< t2.time()<<" [ms]"<< std::endl;
                quick_timer t3;
#endif
		InitializeRatios();
                if (debug) std::cout << cn << mn << "finished InitializeRatios id= " << id << std::endl;

#ifdef TIMER     
                std::cout<<cn<<mn<<"TIMER InitializeRatios t4= "<<  t3.time()<<" [ms]"<< std::endl;
#endif
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

#ifdef DEVELOP
        //if (debug) {
	// std::cout<<" "<<std::endl;
	// std::cout<<cn<<mn<<"Now call pValue calculation "<<std::endl;
	// std::cout<<cn<<mn<<"Data size "<<data.size()<<std::endl;
	// std::cout<<cn<<mn<<"CrossSection size "<<crossSections.size()<<std::endl;
	// std::cout<<cn<<mn<<"id= "<<id<<std::endl;
        //}
	SPXpValue* pvalue= new SPXpValue(data,crossSections, steeringFile);
        pvalue->SetPlotNumber(id); 
#endif      
	//if (debug) std::cout<<cn<<mn<<"Dump Latex table "<<steeringFile->GetDumpTables()<<std::endl;         
        if (steeringFile->GetDumpTables()!=0) {
	 SPXLatexTable *ltable= new SPXLatexTable(data,crossSections, steeringFile);
        }

        if (steeringFile->PrintTotalCrossSection()!=0) {
	 std::cout<<cn<<mn<<"Print total cross sections "<<std::endl;
	 //std::cout<<cn<<mn<<"Data: "<<std::endl;
         for (int i=0; i<data.size(); i++)
	   data.at(i)->PrintTotalCrossSection();

	 //std::cout<<cn<<mn<<"Theory: "<<std::endl;
         for (int i=0; i< crossSections.size(); i++)
	  crossSections.at(i).PrintTotalCrossSection();
 
        }


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
	 if (debug) std::cout<<cn<<mn<<"Set yLabelOverlay "<<yLabelOverlay.c_str()<<std::endl;
         if (!overlayFrameHisto) throw SPXGeneralException(cn+mn+"overlayFrameHisto not found !");

	  overlayFrameHisto->SetYTitle(yLabelOverlay.c_str());
	}

	//Set Ratio Y-Axis Label
	if(ds.ContainsRatio()) {
	  if (debug) std::cout<<cn<<mn<<"Set yLabelRatio= "<<yLabelRatio.c_str()<<std::endl;
          if (!ratioFrameHisto) throw SPXGeneralException(cn+mn+"ratioFrameHisto not found !");
	  ratioFrameHisto->SetYTitle(yLabelRatio.c_str());
	}

	//Set X-Axis Label
	if(ds.ContainsOverlay() && !ds.ContainsRatio()) {
	 if (debug) std::cout<<cn<<mn<<"Set xLabel= "<<xLabel.c_str()<<std::endl;
         if (!overlayFrameHisto) throw SPXGeneralException(cn+mn+"overlayFrameHisto not found !");
	  //Set Overlay X-Axis Label
	  overlayFrameHisto->SetXTitle(xLabel.c_str());

	} else if(ds.ContainsRatio()){
	  if (debug) std::cout<<cn<<mn<<"Set xLabel= "<<xLabel.c_str()<<std::endl;
          if (!ratioFrameHisto) throw SPXGeneralException(cn+mn+"ratioFrameHisto not found !");
	  //Set Ratio X-Axis Label
	  ratioFrameHisto->SetXTitle(xLabel.c_str());
	}

	if(ds.ContainsOverlay()) {
	 if (debug) std::cout<<cn<<mn<<"Draw overlayFramHisto"<<xLabel.c_str()<<std::endl;
         if (!overlayFrameHisto) throw SPXGeneralException(cn+mn+"overlayFrameHisto not found !");
	 overlayPad->cd();
	 overlayFrameHisto->Draw();
	}

	if(ds.ContainsRatio()) {
	  if (debug) std::cout<<cn<<mn<<"Draw ratioFrameHisto "<<std::endl;
          if (!ratioFrameHisto) throw SPXGeneralException(cn+mn+"ratioFrameHisto not found !");
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

	xTitleOffset = 1.0;
	yTitleOffset = 1.0;

        if (pc.IsYLog()) {
         yTitleOffset = 1.25; 
        }

	if (pc.IsXLog()) {
	 xTitleOffset = 1.25; 
        }

	if(debug) std::cout << cn << mn << "Set X Axis Overlay Title Offset to " << xTitleOffset << std::endl;
 	if(debug) std::cout << cn << mn << "Set Y Axis Overlay Title Offset to " << yTitleOffset << std::endl;

	//Scale Overlay Axes
	if(ds.ContainsOverlay()) {
		if(debug) std::cout << cn << mn << "Contains Overlay " << std::endl;

                if (!yAxisOverlay) throw SPXGeneralException(cn+mn+"yAxisOverlay not found !");
                if (!xAxisOverlay) throw SPXGeneralException(cn+mn+"xAxisOverlay not found !");

		xAxisOverlay->SetTitleOffset(xTitleOffset);
		yAxisOverlay->SetTitleOffset(yTitleOffset);

		xAxisOverlay->SetLabelSize(xAxisOverlay->GetLabelSize() / scale);
		yAxisOverlay->SetLabelSize(yAxisOverlay->GetLabelSize() / scale);

		xAxisOverlay->SetTitleSize(xAxisOverlay->GetTitleSize() / scale);
		yAxisOverlay->SetTitleSize(yAxisOverlay->GetTitleSize() / scale);


	}

	//Scale Ratio Axes
	if(ds.ContainsRatio()) {

		if(debug) std::cout << cn << mn << "Contains Ratio " << std::endl;

                if (!xAxisRatio) throw SPXGeneralException(cn+mn+"xAxisRatio not found !");
                if (!yAxisRatio) throw SPXGeneralException(cn+mn+"yAxisRatio not found !");

		if(debug) std::cout << cn << mn << "Set X Axis Ratio Title Offset to " << xTitleOffset << std::endl;
		xAxisRatio->SetTitleOffset(xTitleOffset);

		double rScale = 1.0;
		if(ds.ContainsOverlay()) {
		 rScale = (0.4 - 0.0) / (1.0 - 0.4);

  		 xAxisRatio->SetLabelSize(xAxisOverlay->GetLabelSize() / rScale);
 		 yAxisRatio->SetLabelSize(yAxisOverlay->GetLabelSize() / rScale);


		}


		//NOTE: Not sure why I have to do -0.25 to begin with... For some reason at 0.0 (total offset = 0.8 when below is false)
		// there is an additional offset relative to the overlay title. 0.55 (0.8 + (-0.25)) looks nice, however.

		//double distScale = -0.25;		//Add an offset to y ratio title offset if only ratio is plotted
		//if(!ds.ContainsOverlay() && ds.ContainsRatio()) {
		//	distScale = 0.5;
		//}
		//yAxisRatio->SetTitleOffset(yTitleOffset + distScale);
                yTitleOffset*=rScale;
		if(debug) std::cout << cn << mn << "Set Y Axis Ratio Title Offset to " << yTitleOffset << std::endl;
                yAxisRatio->SetTitleOffset(yTitleOffset);


     		if(ds.ContainsRatio()) {
      
		 xAxisRatio->SetTitleSize(xAxisRatio->GetTitleSize() / rScale);
		 yAxisRatio->SetTitleSize(yAxisRatio->GetTitleSize() / rScale);
                }

 		if(debug) std::cout << cn << mn << "After rescaleing titlesize rScale= " << rScale << std::endl;

	}

	if(ds.ContainsOverlay()) {
		overlayPad->cd();
		if(debug) {
                 std::cout << cn << mn << "Draw overlayFrameHisto "  << std::endl;
                 //overlayFrameHisto->Print("all");
                }
		overlayFrameHisto->Draw();
	}

	if(ds.ContainsRatio()) {
		ratioPad->cd();
		if(debug) {
                 std::cout << cn << mn << "Draw ratio FrameHisto "  << std::endl;
                 //ratioFrameHisto->Print("all");
                }
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
			graphs.push_back(data[i]->GetStatisticalErrorGraph());
			graphs.push_back(data[i]->GetSystematicErrorGraph());
			graphs.push_back(data[i]->GetTotalErrorGraph());
		}

		//Cross sections
		for(int i = 0; i < crossSections.size(); i++) {
                 SPXPDF * pdf=crossSections[i].GetPDF();
                 int nbands=pdf->GetNBands();
                 if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;
                 for (int iband=0; iband<nbands; iband++) {
		  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
		  if (!gband) {
                   std::ostringstream oss;
                   oss << cn <<mn<<"Band iband= "<<iband<<" not found at index "<<iband;
                   throw SPXGeneralException(oss.str());
                  }
                  graphs.push_back(gband);
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
	ratioPad   = (TPad *)canvas->GetListOfPrimitives()->FindObject(TString(canvasID + "_2"));

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
		std::cout << "\t Left Margin = "   << ratioPad->GetLeftMargin() << std::endl;
		std::cout << "\t Right Margin = "  << ratioPad->GetRightMargin() << std::endl;
		std::cout << "\t Top Margin = "    << ratioPad->GetTopMargin() << std::endl;
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

	if(steeringFile->GetXOverlayMin() != MIN_EMPTY) {
		xMinOverlay = steeringFile->GetXOverlayMin();
		if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Minimum to " << xMinOverlay << std::endl;
	}

	/* commenting this is can lead to a corrupted eps file ! .. no idea why
	if(steeringFile->GetXOverlayMax() != MAX_EMPTY) {
	  xMaxOverlay = steeringFile->GetXOverlayMax();
		if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Maxmimum to " << xMaxOverlay << std::endl;
	}
	*/

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

        if (xMinOverlay==0 && yMinOverlay==0 && xMaxOverlay==0 && yMaxOverlay==0) {
	 if (debug) std::cout << cn << mn << "No Frame plotted all bounds are zero ! " << std::endl;
          overlayFrameHisto=0;
          xAxisOverlay=0;
          yAxisOverlay=0;
	 } else {
        
 	 overlayFrameHisto = overlayPad->DrawFrame(xMinOverlay, yMinOverlay, xMaxOverlay, yMaxOverlay);
	 xAxisOverlay = overlayFrameHisto->GetXaxis();
	 yAxisOverlay = overlayFrameHisto->GetYaxis();

         xAxisOverlay->SetNdivisions(5);
	 xAxisOverlay->SetMoreLogLabels(true);

	 if(debug) {
	  std::cout << cn << mn << "Overlay Pad frame drawn with dimensions: " << std::endl;
	  std::cout << "\t xMin = " << xMinOverlay << std::endl;
	  std::cout << "\t xMax = " << xMaxOverlay << std::endl;
	  std::cout << "\t yMin = " << yMinOverlay << std::endl;
	  std::cout << "\t yMax = " << yMaxOverlay << std::endl;
	 }
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

	//Force Ratio X Min/Max to match Overlay, if plotted (should already match anyway...)
	if(ds.ContainsOverlay()) {
	 xMinRatio = xMinOverlay;
	 xMaxRatio = xMaxOverlay;
	} else { // otherwise read in values from Steering
	
	 if(steeringFile->GetXOverlayMin() != MIN_EMPTY) {
		xMinRatio = steeringFile->GetXOverlayMin();
		if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Minimum to " << xMinRatio << std::endl;
	 }
	 if(steeringFile->GetXOverlayMax() != MAX_EMPTY) {
	   xMaxRatio = steeringFile->GetXOverlayMax();
		if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Maxmimum to " << xMaxRatio << std::endl;
	 }
      
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
        
	if(ratios.size()>0) {

	 ratioFrameHisto = ratioPad->DrawFrame(xMinRatio, yMinRatio, xMaxRatio, yMaxRatio);
	 xAxisRatio = ratioFrameHisto->GetXaxis();
	 yAxisRatio = ratioFrameHisto->GetYaxis();
     
         //xAxisRatio->SetNdivisions(m_nDivY);

	 if(debug) {
	  std::cout << cn << mn << "Ratio Pad frame drawn with dimensions: " << std::endl;
	  std::cout << "\t xMin = " << xMinRatio << std::endl;
	  std::cout << "\t xMax = " << xMaxRatio << std::endl;
	  std::cout << "\t yMin = " << yMinRatio << std::endl;
	  std::cout << "\t yMax = " << yMaxRatio << std::endl;
	 }
	} else {
	  ratioFrameHisto=0;
          xAxisRatio=0;
          yAxisRatio=0;
	  if (debug) std::cout << cn << mn << "No ratios asked for -> no DrawFrame for ratioPad " << std::endl;
	}
}

void SPXPlot::StaggerConvoluteOverlay(void) {
	std::string mn = "StaggerConvoluteOverlay: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Change this to alter the fraction of the error range in which the point is staggered
	const int FRAC_RANGE = 4;

	//Stagger convolutes in overlay
        int ncross=crossSections.size();
	for(int i = 0; i < ncross; i++) {

	      //double dx= pow(-1,i) * int((i+1)/2.);
	      //if (debug) std::cout << cn << mn <<i<<" dx= " <<dx<< std::endl;

              SPXPDF * pdf=crossSections[i].GetPDF();
              int nbands=pdf->GetNBands();
              if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;
              for (int iband=0; iband<nbands; iband++) {
	       TGraphAsymmErrors * graph   =pdf->GetBand(iband);
	       if (!graph) {
                std::ostringstream oss;
                oss << cn <<mn<<"get graphs "<<"Band "<<iband<<" not found at index "<<i;
                throw SPXParseException(oss.str());
               }
               if (debug) std::cout << cn << mn <<"Stagger Graph " <<graph->GetName()<< std::endl;
	       SPXGraphUtilities::StaggerGraph(i, ncross, graph);
              }
	}
}

void SPXPlot::StaggerConvoluteRatio(void) {
	std::string mn = "StaggerConvoluteRatio: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Change this to alter the fraction of the error range in which the point is staggered
	//const int FRAC_RANGE = 4;

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
        int nratio=_ratios.size();
	for(int i = 0; i < nratio; i++) {
	 std::vector<TGraphAsymmErrors *> ratiographs=_ratios[i].GetRatioGraph();

         for (int igraph=0; igraph < ratiographs.size(); igraph++) {
	  TGraphAsymmErrors *graph = ratiographs[igraph];
          if (!graph) throw SPXGraphException(cn+mn+"Graph not found !");
	  if(debug) std::cout << cn << mn << "Staggering ratio at index " << i << ": " << graph->GetName() << std::endl;  		 
	  SPXGraphUtilities::StaggerGraph(i, nratio, graph);
         }


	 std::vector<TGraphAsymmErrors *> ratiographsstat=_ratios[i].GetRatioGraphStat();

         for (int igraph=0; igraph < ratiographsstat.size(); igraph++) {
	  TGraphAsymmErrors *graph = ratiographsstat[igraph];
          if (!graph) throw SPXGraphException(cn+mn+"Graph not found !");
	  if(debug) std::cout << cn << mn << "Staggering ratio at index " << i << ": " << graph->GetName() << std::endl;  		 
	  SPXGraphUtilities::StaggerGraph(i, nratio, graph);
         }
        }

}

//Matches the overlay binning if the match_binning flag is set
//NOTE: Ratios are automatically matched, since they must align in order
//		to use the SPXGraphUtilities::Divide function

void SPXPlot::DrawOverlay(void) {
	std::string mn = "DrawOverlay: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(!overlayPad) {
	 throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawOverlayPadFrame before drawing the overlay graphs");
	}

	//Do nothing if not drawing overlay
	SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
	SPXPlotType     &pt = pc.GetPlotType();
	SPXDisplayStyle &ds = pc.GetDisplayStyle();
	SPXOverlayStyle &os = pc.GetOverlayStyle();

	if(!ds.ContainsOverlay()) {
		return;
	}

        if (debug) {
	 if (steeringFile->GetPlotBand())   std::cout<<cn<<mn<<" plotBand ON ! "<<std::endl;
	 else                               std::cout<<cn<<mn<<" plotBand OFF ! "<<std::endl;
         if (steeringFile->GetPlotMarker()) std::cout<<cn<<mn<<" plotMarker ON ! "<<std::endl;
         else                               std::cout<<cn<<mn<<" plotMarker OFF ! "<<std::endl;
	}

        if (steeringFile->GetPlotBand() &&steeringFile->GetPlotMarker()){
	  //throw SPXROOTException(cn + mn + "plot_band and plot_marker should not be both on !");
          std::cerr<<cn<<mn<<"Warning: plot_band and plot_marker should not be both on !"<<std::endl;
          //steeringFile->SetPlotBandOff();
	}

	if (!steeringFile->GetPlotBand() && !steeringFile->GetPlotMarker()){
	 throw SPXROOTException(cn + mn + "plot_band and plot_marker should not be both off !");
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

			//Warn user that the number of bins in data does not match the number in convolute, if that's the case
			if(os.ContainsData()) {
                              SPXPDF * pdf=crossSections[i].GetPDF();
                              int nbands=pdf->GetNBands();
                              if (nbands<1) throw SPXGeneralException(cn+mn+"No band found !");
                              // there should e also a band
			      unsigned int cbins = pdf->GetBand(0)->GetN();
                              unsigned int dbins = data.at(0)->GetTotalErrorGraph()->GetN();

			      if(cbins != dbins) {
			       std::cerr << cn << mn << "WARNING: The number of convolute bins (" << cbins << ") does not match the number of master data bins (" << dbins << ")" << std::endl;
			       std::cerr << "\t\t\t This might be fixed by calling Matchbinning later !" << std::endl;
			       //std::cerr << "\t\t\t You can enable bin matching with the \"match_binning = true\" flag in the steering file, if you would like to do so" << std::endl;
			       }

			       //Same for theory
			       unsigned int pbins = crossSections.at(i).GetNominal()->GetN();

			       if(pbins != dbins) {
			        std::cerr << cn << mn << "WARNING: The number of theory cross section bins (" << pbins << ") does not match the number of master data bins (" << dbins << ")" << std::endl;
				//std::cerr << "\t\t\t You can enable bin matching with the \"match_binning = true\" flag in the steering file, if you would like to do so" << std::endl;
				std::cerr << "\t\t\t This might be fixed by calling MatchBinning later..." << std::endl;
			       }
			}

			//Draw theory Cross Section 
			if(os.ContainsConvolute()) {

			 SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(id, i);
                         SPXPDF * pdf=crossSections[i].GetPDF();
     		         this->DrawBand(pdf, csOptions.c_str(), pci);
			}

			//Draw PDF Nominal, if requested
			if(os.ContainsPDF()) {

				//Modify style for theory, if both convolute and theory are drawn
				if(os.ContainsConvolute()) {
				      SPXPDF * pdf=crossSections.at(i).GetPDF();
                                      int nbands=pdf->GetNBands();
                                      if (nbands<1) throw SPXGeneralException(cn+mn+"no band found !");
                                      TGraphAsymmErrors *cs = pdf->GetBand(0);
				      TGraphAsymmErrors *nom= crossSections.at(i).GetNominal();
				      nom->SetFillStyle(3017);
				      nom->SetLineStyle(2);
				      nom->SetFillColor(cs->GetFillColor() + 2);
				      nom->SetMarkerColor(cs->GetMarkerColor() + 2);
				}

				crossSections.at(i).GetNominal()->Draw(csOptions.c_str());

                                if (debug) {
 			         std::cout<< cn << mn <<" ContainsPDF Drew "<<crossSections.at(i).GetNominal()->GetName()
                                                << " with options "<<csOptions.c_str()
                                                << " fillcolor= " << crossSections.at(i).GetNominal()->GetFillColor() 
                                                << " fillstyle= " << crossSections.at(i).GetNominal()->GetFillStyle() 
                                                << " markerstyle= " << crossSections.at(i).GetNominal()->GetMarkerStyle()<< std::endl;          
                                }
			}

		}
	}

	//Check if data is to be plotted in the overlay
	if(os.ContainsData()) {
		//Draw data graphs on Overlay Pad
		for(int i = 0; i < data.size(); i++) {

			std::string statOptions;
			std::string totOptions;

			//if(steeringFile->GetPlotMarker()) {
			//	totOptions = "PZ";	//Never plot ticks on tot error graph
			//}

			totOptions = "PZ";	// data always marker

			if(steeringFile->GetPlotErrorTicks()) {
				statOptions = "||";
			} else {
				statOptions = "Z";
			}

			//data[i].GetTotalErrorGraph()->Draw(totOptions.c_str());
			//data[i].GetStatisticalErrorGraph()->Draw(statOptions.c_str());

			data[i]->GetTotalErrorGraph()->Draw(totOptions.c_str());
			data[i]->GetStatisticalErrorGraph()->Draw(statOptions.c_str());

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

 for(int iratio = 0; iratio < pc.GetNumberOfRatios(); iratio++) {
  std::string ratioOptions;
  if(steeringFile->GetPlotBand()) 
   ratioOptions = "E2";

  if(steeringFile->GetPlotMarker() && !steeringFile->GetPlotBand())
   ratioOptions = "P";

  if(!steeringFile->GetPlotErrorTicks() && !steeringFile->GetPlotBand()) 
   ratioOptions += "Z";
		
  //Set x errors to zero if ratio involves convolute AND is not plot band
  if(ratios.at(iratio).HasConvolute() && !steeringFile->GetPlotBand()) {
   //Never clear X errors for DataStat or DataTot (extra check: HasConvolute() should already rule out stat/tot...)
   // if(!ratios[i].IsDataStat() && !ratios[i].IsDataTot()) {
   //  std::vector<TGraphAsymmErrors *> ratiographs=ratios[i].GetRatioGraph();
   //  for (int igraph=0; igraph < ratiographs.size(); igraph++) {
   //   TGraphAsymmErrors *graph = ratiographs[igraph];
   //  }
   // if(debug) std::cout << cn << mn << "Set X errors to zero for ratios[" << i << "]" << std::endl;
  }

  if(ratios.at(iratio).IsDataStat()) {
   statRatios++;

   //Warn if not the first graph AND first graph is not data_tot: Will possibly cover up points
   if((iratio != 0) && !ratios.at(0).IsDataTot()) {
    std::cerr << cn << mn << "WARNING: Data Stat band could possibly hide other bands/points plotted underneath it: Move data_stat to ratio_0?" << std::endl;
   }
  }

  if(ratios.at(iratio).IsDataTot()) {
   totRatios++;
   //Warn if not the first graph: Will possibly cover up points
   if(iratio != 0) {
    std::cerr << cn << mn << "WARNING: Data Tot band could possibly hide other bands/points plotted underneath it: Move data_to to ratio_0?" << std::endl;
   }
  }
   
  bool plotmarker=steeringFile->GetPlotMarker();
  double xbox    =steeringFile->GetXDataBoxLabel();
  double ybox    =steeringFile->GetYDataBoxLabel();
  if (debug) {
   std::cout<<cn<<mn<<"Draw ratio with xbox= "<<xbox<<" ybox= "<<ybox<<std::endl;
   if (plotmarker) std::cout<<cn<<mn<<"plotmarker TRUE "<<std::endl;
   std::cout<<cn<<mn<<"Draw ratio with options= "<<ratioOptions.c_str()<<std::endl;
  }
  ratios.at(iratio).Draw(ratioOptions.c_str(),statRatios, totRatios ,plotmarker,xbox,ybox);
 
 }

 //Draw a line at 1, where ratios are relative to
 TLine *referenceLine = new TLine(xMinRatio, 1.0, xMaxRatio, 1.0);
 referenceLine->Draw();
}

void SPXPlot::DrawLegend(void) {
 std::string mn = "DrawLegend: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
 //

 SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
 //SPXPlotType        &pt = pc.GetPlotType();
 SPXDisplayStyle      &ds = pc.GetDisplayStyle();
 SPXOverlayStyle      &os = pc.GetOverlayStyle();

 // Why zero here, Where to the the PlotConfiguriotninstance ??
 //std::cout<<cn<<mn<<"HuHu fix me pc.GetPlotConfigurationInstance(0) is not good ! "<<std::endl;
 SPXPlotConfigurationInstance pci=pc.GetPlotConfigurationInstance(0);
 SPXGridSteeringFile gridsteeringFile=pci.gridSteeringFile;

 bool overlay=ds.ContainsOverlay();  
 bool ratioonly=ds.ContainsRatio()&&!overlay;

 if(!overlay&&!ratioonly){
  return;
 }

 if(overlay&&!overlayPad) {
  throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawRatioPadFrame before drawing the overlay graphs");
 }

 if(ratioonly&&!ratioPad) {
  throw SPXROOTException(cn + mn + "You MUST call SPXPlot::DrawRatioPadFrame before drawing the ratio graphs");
 }

 if (overlay) overlayPad->cd();
 if (ratioonly) ratioPad->cd();

 if (debug&&ratioonly) std::cout << cn << mn <<"Is ratio only "<< std::endl;

 // Analyse what data are there...
 //
 // eta-scan is when all data have same name, but ymin and y max are all different
 bool etascan=false; bool onedataset=true; bool oneeta=true;
 double binminold=-99999;  double binmaxold=-99999;
 TString datalabelold="";
 int ietabins=0;
 double sqrtsval = -1., sqrtsvalold = -1., jetR    = -1., jetRold = -1.;
 std::string lumiold ="NOVALUE";
 bool onlysyst=false;
 
 bool differentsqrts=false, differentR=false, differentetabin=false, differentlumi=false;

 if(os.ContainsData()) {
  if (debug) std::cout << cn << mn <<"Contains data "<< std::endl;

  if (data.size()==0)
   throw SPXGeneralException(cn+mn+"No data object found !");

  if (debug) std::cout<<cn<<mn<<"Number of data objects= "<< data.size()<<std::endl;

  for(int idata = 0; idata < data.size(); idata++) {                 

   TString datalabel=data.at(idata)->GetLegendLabel();
   if (datalabel.EqualTo(datalabelold)) {
     if (debug) std::cout<<cn<<mn<<"datalabel= "<<datalabel.Data()<<" equal to old "<<std::endl;
   } else {
     if (debug) std::cout<<cn<<mn<<"datalabel= "<<datalabel.Data()<<" changed ! old= "<<datalabelold.Data()<<std::endl;
    datalabelold=datalabel;
    if (idata>0) onedataset=false;
   }

   if (steeringFile->GetAddLumiLabel()) {
    std::string lumi = data.at(idata)-> GetDatasetLumi();
    if (lumi!=lumiold) {
     if (lumiold!=std::string("NOVALUE")) {
      if (debug) std::cout<<cn<<mn<<"idata= "<<idata<<" lumi changed: lumi= "<<lumi.c_str()<<" lumiold= "<<lumiold.c_str()<<std::endl;
      differentlumi=true;
     }
     lumiold=lumi; 
    }
   }

   if (steeringFile->GetLabelSqrtS()) {
    sqrtsval = data.at(idata)->GetSqrtS();
    if (sqrtsval!=sqrtsvalold) {
     if (sqrtsvalold!=-1) {
      if (debug) std::cout<<cn<<mn<<"idata= "<<idata<<" sqrt changed: sqrtsval= "<<sqrtsval<<" sqrtsvalold= "<<sqrtsvalold<<std::endl;
      differentsqrts=true;
     }
     sqrtsvalold=sqrtsval;
    }
   }

   if (data.at(idata)->GetJetAlgorithmLabel().size()>0){
    jetR=data.at(idata)->GetJetAlgorithmRadius();
    if (jetR!=jetRold) {
     if (jetRold!=-1) {
      if (debug) std::cout<<cn<<mn<<"idata= "<<idata<<" jet R changed: jetR= "<<jetR<<" jetRold= "<<jetRold<<std::endl;
      differentR=true;
     }
     jetRold=jetR;

    }
   }

   if (data.at(idata)->GetDoubleBinVariableName().size()>0) {
     if (debug) std::cout<<cn<<mn<<"Double differential variable ON for idata= "<<idata <<std::endl;

    double binmin = data.at(idata)->GetDoubleBinValueMin();
    double binmax = data.at(idata)->GetDoubleBinValueMax();
    //std::cout<<cn<<mn<<"binmin= "<<binmin<<" binmax= "<<binmax<<std::endl;
    if (binmin!=binminold || binmax!=binmaxold) {
     if (binminold!=-99999) {
     if (debug) std::cout<<cn<<mn<<"binmin= "<<binmin<<" binmax= "<<binmax<<" ietabins= " << ietabins <<std::endl;
      differentetabin=true;
     }
     binminold =  binmin;
     binmaxold  = binmax;
     ietabins++;
    }
   }
  }
 }

 if (ietabins==data.size() && ietabins>1) etascan=true;
 if (ietabins==1) oneeta=true;

 if (debug) {
   std::cout << cn << mn <<" "<< std::endl;
   std::cout<<cn<<mn<<"Number of different eta bins " << ietabins <<" Number of data= "<<data.size()<<std::endl;
  if (etascan) std::cout<<cn<<mn<<"etascan is ON ! " <<std::endl;
  else         std::cout<<cn<<mn<<"etascan is OFF ! " <<std::endl;
  if (onedataset)  std::cout<<cn<<mn<<"Only one data-set ! " <<std::endl;
  else             std::cout<<cn<<mn<<"More than one data-set ! " <<std::endl;

  if (steeringFile->GetLabelSqrtS()) {
   if (differentsqrts) std::cout<<cn<<mn<<"Data-sets with different sqrts values ! " <<std::endl;
   else                std::cout<<cn<<mn<<"Data-sets have same sqrts values ! " <<std::endl;
  }

  if (differentR)     std::cout<<cn<<mn<<"Data-sets with different jet R values ! " <<std::endl;
  else                std::cout<<cn<<mn<<"Data-sets have same jet R values ! " <<std::endl;

  if (differentetabin)std::cout<<cn<<mn<<"Data-sets with different eta bins ! " <<std::endl;
  else                std::cout<<cn<<mn<<"Data-sets with same eta bins ! " <<std::endl;

  if (steeringFile->GetAddLumiLabel()) {
   if (differentlumi)  std::cout<<cn<<mn<<"Data-sets with different lumi ! " <<std::endl;
   else                std::cout<<cn<<mn<<"Data-sets with same lumi ! " <<std::endl;
  }
 }

 int namesize=5;    
 int leginfomax=0;    
 double charactersize=0.04;    
 if (etascan) {
  charactersize=0.03;
  if (debug) std::cout<<cn<<mn<<"Set charactersize= "<< charactersize <<std::endl;
 }

 float linesize=0.07; 
 TLegend *leg = 0;

 leg = new TLegend();
 leg->SetBorderSize(0);
 leg->SetFillColor(0);
 //leg->SetFillColorAlpha(kYellow,0.);
 leg->SetFillStyle(0);
 leg->SetMargin(0.2);


 // Look first, if properties of bands are different
 int old_fill_style=-999, old_fill_color=-999;
 int old_marker_style=-999, old_marker_color=-999;
 bool bandsdifferent=false;
 bool hadcorronly=false;
 
 for(int icross = 0; icross < crossSections.size(); icross++) {
  SPXPDF * pdf=crossSections[icross].GetPDF();

  if (debug) {
   std::cout<<cn<<mn<<" "<< std::endl;
   std::cout<<cn<<mn<<"icross= "<<icross<<" Test band properties "<< std::endl;
  }

  bool bscale  =pdf->HasBandofType("_scale_");
  bool bpdf    =pdf->HasBandofType("_pdf_");
  bool balphas =pdf->HasBandofType("_alphas_");
  bool bhadcorr=pdf->HasBandofType("_corrections_");

  if (bhadcorr && !(bscale&&bpdf&&balphas)) hadcorronly=true;

  bandsdifferent=pdf->BandsHaveDifferentProperties();
  if (debug) {
   if (bandsdifferent) std::cout<<cn<<mn<<"icross= "<<icross<<" Bands have different properties !"<< std::endl;
   else                std::cout<<cn<<mn<<"icross= "<<icross<<" Bands have same properties ! "<< std::endl;

   if (bscale)  std::cout<<cn<<mn<<"Bands contains scale uncertainty"<< std::endl;
   if (bpdf)    std::cout<<cn<<mn<<"Bands contains PDF uncertainty"<< std::endl;
   if (balphas) std::cout<<cn<<mn<<"Bands contains AlphaS uncertainty"<< std::endl;
   if (bhadcorr)std::cout<<cn<<mn<<"Bands contains corrections uncertainty"<< std::endl;
   if (hadcorronly)std::cout<<cn<<mn<<"Bands contains ONLY corrections uncertainty"<< std::endl;
  }
 }

 if (debug) {
  if (bandsdifferent) std::cout<<cn<<mn<<"One Cross section with Bands with have different properties !"<< std::endl;
  else                std::cout<<cn<<mn<<"All cross section have bands with have same properties ! "<< std::endl;
 }

 if (debug) std::cout<<cn<<mn<<" "<< std::endl;

  // Now analyse ratio

 if (ratioonly && os.ContainsData() && !os.ContainsConvolute()) { 
  if (debug) std::cout<<cn<<mn<<"Now loop over ratio to see if systematics is plotted"<<std::endl;
  for (int iratio=0; iratio<ratios.size(); iratio++) {
   std::vector<TGraphAsymmErrors *> ratiographs=ratios.at(iratio).GetRatioGraph();
   int isyst=0;
   for(int igraph = 0; igraph < ratiographs.size(); igraph++) {
    TString gname=ratiographs.at(igraph)->GetName();
    //if (debug) std::cout<<cn<<mn<<"iratio= "<<iratio<<" graph name= "<<gname.Data()<<std::endl;
    if (gname.Contains("syst_")) {
     isyst++;
    }
   }

   if (isyst==ratiographs.size()) onlysyst=true;
   if (debug) std::cout<<cn<<mn<<"iratio= "<<iratio<<" isyst= "<<isyst<<(onlysyst? " onlysyst is ON" : " onlysyst is OFF")<<std::endl;

   if (onlysyst) {
    std::vector<TGraphAsymmErrors * > ratiographsordered=SPXUtilities::OrderLargestRelativeErrorGraphVector(ratiographs);
    for(int igraph = 0; igraph < ratiographsordered.size(); igraph++) {
     TString gname=ratiographsordered.at(igraph)->GetName();
     gname.ReplaceAll("syst_","");
     //double emax=SPXGraphUtilities::GetLargestRelativeError(ratiographs.at(igraph));
     //gname+=Form(" %3.2f",emax*100);
     if (debug) std::cout<<cn<<mn<<"Add in legend gname= "<<gname.Data()<<" color= "<<ratiographsordered.at(igraph)->GetLineColor()<<" size= "<<TString(gname).Sizeof()<<std::endl;

     double scale=1.0;
     TString gname2=gname;
     if (TString(gname).Sizeof()>16) {
      scale=0.9;
      if (TString(gname).Sizeof()>20) scale=0.85;
      if (TString(gname).Sizeof()>35) scale=0.70;
      gname.Form("#scale[%3.2f]{%s}",scale,gname2.Data());
     }
     if (TString(gname2).Sizeof()*scale>namesize) namesize=TString(gname2).Sizeof()*scale;
     if (debug) std::cout<<cn<<mn<<"scale= "<<scale<<" namesize= "<<namesize<<std::endl;

     if (ratiographsordered.at(igraph)->GetLineColor()>0)
      leg->AddEntry(ratiographsordered.at(igraph), gname, "F");
     else {
      SPXGraphUtilities::SetColors(ratiographsordered.at(igraph),-ratiographsordered.at(igraph)->GetLineColor());
      leg->AddEntry(ratiographsordered.at(igraph), gname, "L");
     }
    }
   }
  }
 }

 // if (debug) std::cout<<cn<<mn<<"A namesize= "<<namesize<<std::endl;

 if (debug&&onlysyst) {
  std::cout<<cn<<mn<<" "<< std::endl;
  std::cout<<cn<<mn<<"Only systematics to legend, data label in information legend "<< std::endl;
 }

 if (os.ContainsData()) {
  if (debug) { 
   std::cout<<cn<<mn<<" "<< std::endl;
   std::cout<<cn<<mn<<"Contains data "<< std::endl;
  }

  if (data.size()==0)
   throw SPXGeneralException(cn+mn+"No data object found !");

  if (debug) std::cout<<cn<<mn<<"Number of data objects= "<< data.size()<<std::endl;

  for (int idata = 0; idata < data.size(); idata++) {                 

   TString datalabel=data.at(idata)->GetLegendLabel();
   //datalabel.ReplaceAll("high-mu","");

   if (!etascan) {

    if (differentsqrts) {
     datalabel+=" ";
     double sqrtsval = data.at(idata)->GetSqrtS();
     datalabel+=this->FormatSqrtLabel(sqrtsval);
    }
 
    if (differentR) {
     datalabel+=" ";
     double jetR=data.at(idata)->GetJetAlgorithmRadius();
     datalabel+=this->FormatjetRLabel(jetR);
    }
 
    if (differentetabin){
     datalabel+=" ";
     double binmin = data.at(idata)->GetDoubleBinValueMin();
     double binmax = data.at(idata)->GetDoubleBinValueMax();
     TString varname=data.at(idata)->GetDoubleBinVariableName();
     datalabel+=this->FormatetabinLabel(varname, binmin,binmax);
    }

    if (differentlumi){
     datalabel+=" ";
     datalabel+= data.at(idata)-> GetDatasetLumi();
    }

    if (steeringFile->GetAddJournalLabel()) {
     datalabel+=" "+data.at(idata)->GetJournalLegendLabel();
     if (debug) std::cout<<cn<<mn<<"Add journal label datalabel  "<<datalabel.Data() <<std::endl;
    }

    if (steeringFile->GetAddJournalYear()) {
     if (debug) std::cout<<" Add journal year= "<<data.at(idata)->GetJournalYear()<<std::endl;
     datalabel+=" "+data.at(idata)->GetJournalYear();
     //datalabel=" #splitline{"+datalabel+"}{";
     //datalabel+=data.at(idata)->GetJournalYear();
     //datalabel+="}";
    }

    //if (!ratioonly && data.size()>0) { // ratioonly figures have data in the ratio, no separate label
    if (data.size()>0) { 
     if (!onlysyst) {
      if (TString(datalabel).Sizeof()>namesize) namesize=TString(datalabel).Sizeof();
      if (debug) std::cout<<cn<<mn<<"Add to legend Data Label: "<<datalabel.Data()<<" namesize= "<<namesize<<std::endl;
      if (!hadcorronly) {
       leg->AddEntry(data.at(idata)->GetTotalErrorGraph(), datalabel, "P");
      } else {
       if (debug) std::cout<<cn<<mn<<"correction only does not need marker "<<std::endl;
       leg->AddEntry((TObject*)0, datalabel, "P");
      }
     }
    } else 
     if (debug) std::cout<<cn<<mn<<"Ratio only or data.size==0  "<<std::endl;

    //if (debug) std::cout<<cn<<mn<<"B namesize= "<<namesize<<std::endl;

   } else { // This is for the eta-scan
    if (onedataset) {
     if (idata==0) {
      TString datalabel=data.at(idata)->GetLegendLabel();
      if (TString(datalabel).Sizeof()>namesize) namesize=TString(datalabel).Sizeof();
      if (debug) std::cout<<cn<<mn<<"Add data label to legend: "<<datalabel.Data()<< " namesize= "<<namesize<<std::endl;
      leg->AddEntry((TObject*)0, datalabel, "");
      // leg->AddEntry(data.at(idata)->GetTotalErrorGraph(), datalabel, "P");
     }

     if (debug) std::cout<<cn<<mn<<" onlysyst= "<< (onlysyst ? "ON" : "OFF")<<std::endl;

     if (!onlysyst) { // for systematics only move Data label to leginfo
      double binmin = data.at(idata)->GetDoubleBinValueMin();
      double binmax = data.at(idata)->GetDoubleBinValueMax();
      TString varname=data.at(idata)->GetDoubleBinVariableName();
      TString datalabel;
      if (binmin!=0) {
       datalabel.Form(" %3.2f ",binmin); 
      } else {
       varname.ReplaceAll("#leq","");
       varname.ReplaceAll("","");
      }
      datalabel+=varname;
      datalabel+=Form(" %3.2f ",binmax);

      SPXPlotConfigurationInstance mypci=pc.GetPlotConfigurationInstance(idata);
      double yScale = mypci.yScale;
      std::cout<<cn<<mn<<" etascan= "<< (etascan ? "ON" : "OFF")<<" yScale= "<<yScale<<std::endl;
      if (etascan&&yScale!=1) {
       /*
        int exp; double x;
        SPXMathUtilities::frexp10(yScale, exp, x);
        if (debug) std::cout<<cn<<mn<<"value= "<<yScale<<" x= "<<x<<" 10^"<<exp<<std::endl;
        //datalabel+=Form(" #fontsize{0.02}{ (#times %1.1f 10^{%d})}",x,exp);
        //datalabel+=Form("#font[8]{ (#times %1.1f 10^{%d}) }",x,exp);
        datalabel+=Form("(#times %1.1f 10^{%d}) ",x,exp);
       */
       datalabel+="(#times";
       datalabel+=SPXDrawUtilities::FormatwithExp(yScale);
       datalabel+=")";
      }

      if (TString(datalabel).Sizeof()>namesize) namesize=TString(datalabel).Sizeof();
      if (debug) std::cout<<cn<<mn<<"Add to legend data label: "<<datalabel.Data()<< " namesize= "<<namesize<<std::endl;
      leg->AddEntry(data.at(idata)->GetTotalErrorGraph(), datalabel, "P");
     } 
    } else {
     if (debug) std::cout<<cn<<mn<<"WARNING: etascan label is only supported for one data-set"<< std::endl;
    }
   }
  }
 }

 //if (debug) leg->Print();

 //if (debug) std::cout<<cn<<mn<<"C namesize= "<<namesize<<std::endl;

 bool nlolabel=true;
 SPXRatioStyle ratioStyle = pc.GetRatioStyle(0);
 //if (ratioStyle.IsDataOverConvolute()) nlolabel=false;

 if (os.ContainsConvolute()) {
  if (debug) std::cout<<cn<<mn<<"Contains convolute "<< std::endl;
  std::vector<TString> vlabel;

  int npdf=0; int iold=-1;
  for (int icross = 0; icross < crossSections.size(); icross++) {
   // Now analyze PDF-object
   // Get PDF object
   SPXPDF * pdf=crossSections[icross].GetPDF();
 
   string nloprogamename;
   if (steeringFile->GetAddonLegendNLOProgramName() ) {
    SPXGrid *grid=crossSections[icross].GetGrid();
    nloprogamename=grid->GetNLOProgramNameName();
    if (debug) std::cout<<cn<<mn<<"Add NLO Program name= "<<nloprogamename.c_str()<<std::endl;      
   }       

   int nbands=pdf->GetNBands();
   TString pdftype = pdf->GetPDFtype();
   if (nbands<1) throw SPXGeneralException(cn+mn+"No band found, but PDF object exist !"); 
   bool pdffound=false;
   bool nlouncertainty=false;
   bool gridcorrectionfound=false;

   for (int iband=0; iband<nbands; iband++) {
    std::string gtype = pdf->GetBandType(iband);
    TGraphAsymmErrors * gband = pdf->GetBand(iband);
    if (gtype.compare(std::string("alphas"))==0) nlouncertainty=true;
    if (gtype.compare(std::string("scale"))==0)  nlouncertainty=true;
    if (gtype.compare(std::string("pdf"))==0) {
     pdffound=true; nlouncertainty=true;
    }
    if(steeringFile->ApplyGridCorr()) {
     std::vector<std::string> corrlabel=crossSections[icross].GetCorrectionLabels();
     for(int ic = 0; ic < corrlabel.size(); ic++) {
      if (gtype.compare(corrlabel[ic])==0) gridcorrectionfound=true;	   
     }
    }
   }

   if (debug) {
    if (gridcorrectionfound) std::cout << cn << mn <<"Grid corrections found !"<< std::endl;
    else                     std::cout << cn << mn <<"No Grid corrections found !"<< std::endl;

    if (nlouncertainty)      std::cout << cn << mn <<"Band contains NLO uncertainty !"<< std::endl;
    else                     std::cout << cn << mn <<"No NLO uncertainty found !"<< std::endl;

    if (pdffound)            std::cout << cn << mn <<"Band contains PDF !"<< std::endl;
    else                     std::cout << cn << mn <<"No PDF band found !"<< std::endl;

    if (nlolabel)            std::cout << cn << mn <<"Plot NLO labels "<< std::endl;
    else                     std::cout << cn << mn <<"No NLO labels "<< std::endl;
   }

   if (!bandsdifferent) {
    if (nlolabel) {
     TString text="";
     if (icross==0) { //only Draw text for the first cross section
      text+="NLO QCD";
      if(steeringFile->ApplyGridCorr()) {
	std::vector<std::string> corrlabel=crossSections[icross].GetCorrectionLabels();
       if (debug) std::cout<<cn<<mn<<"Number of corrections= "<<corrlabel.size()<<std::endl;           
       for(int ic = 0; ic < corrlabel.size(); ic++) {
	std::string label=" #otimes "+corrlabel[ic];
        //if (debug) std::cout<<cn<<mn<<"grid correction label= "<<label.c_str()<<std::endl;
        text+=TString(label); // do this better
       }
        //std::cout<<cn<<mn<<text.Data()<<" namesize= "<<namesize<<std::endl;
      }
  
      text+=" with:";
      // 
      if (gridcorrectionfound&&!nlouncertainty) text="";
      if (nlouncertainty) leg->AddEntry((TObject*)0, text, "");
     }
    }
   } else {
    if (nlouncertainty) {
     TString label="NLO QCD Uncertainties";
     if (label.Sizeof()>namesize) namesize=label.Sizeof();
     if (debug) std::cout << cn << mn <<"Add data label to legend: "<<label.Data()<< " namesize= "<<namesize<<std::endl;
 
     leg->AddEntry((TObject*)0,label, "");
    }
    if (gridcorrectionfound&&!nlouncertainty)
     leg->AddEntry((TObject*)0,"Uncertainties", "");
   }

   if (bandsdifferent) { // uncertainty bands have different properties
    for (int iband=0; iband<nbands; iband++) {
     TGraphAsymmErrors * gband   =pdf->GetBand(iband);
     std::string         gtype   =pdf->GetBandType(iband);

     if (debug) std::cout << cn << mn <<"Different properties iband= "<<iband<<" gtype= "<< gtype.c_str()<< std::endl;

     TString label="";
     TString bandtype=TString(gtype);
     bandtype.ToUpper();
     if (bandtype.Contains("ALPHAS")) bandtype="#alpha_{s}";
     if (bandtype.Contains("SCALE"))  bandtype="Scale";
     if (bandtype.Contains("TOTAL"))  bandtype="Total";
     if (gtype.compare(std::string("pdf"))==0)
      label=pdftype+" "+bandtype;
     else
      label=bandtype;

     if (steeringFile->GetPlotMarker()) { 
      //std::cout<<cn<<mn<<" Plot marker "<<std::endl;
      if (debug) std::cout<<cn<<mn<<"0 add in legend iband= "<<iband<<" gband= "<<gband->GetName()
			  <<" label= "<<label.Data()<<std::endl;
      if (label.Sizeof()>namesize) namesize=label.Sizeof();
      if (debug) std::cout << cn << mn <<"Add data label to legend: "<<label.Data()<< " namesize= "<<namesize<<std::endl;
      leg->AddEntry(gband, label, "P");
     } else if (steeringFile->GetPlotBand()) {
      if (debug) std::cout<<cn<<mn<<"Plot type "<<bandtype<<std::endl;
      if (debug) std::cout<<cn<<mn<<"gband Name= "<<gband->GetName()<<std::endl;
      std::string opt="LF";
      int edgecolor=-1; 
      TString gname=gband->GetName();
     
      if(gname.Contains("_alphas_")) edgecolor=pci.alphasEdgeColor;
      if(gname.Contains("_scale_"))  edgecolor=pci.scaleEdgeColor;
      if(gname.Contains("_pdf_"))    edgecolor=pci.pdfEdgeColor;
      if(gname.Contains("_total_"))  edgecolor=pci.totalEdgeColor;
      if(gname.Contains("_corrections_")) edgecolor=pci.correctionsEdgeColor;

      //int edgestyle  =pci.correctionsEdgeStyle;
      std::cout<<cn<<mn<<" edgecolor= "<<edgecolor<<std::endl;      
      if (edgecolor<0) {
       opt="L";
       TString hname="h";
       hname+=gband->GetName();
       hname+="LowEdge";
       TH1D *hedge=(TH1D*)gDirectory->Get(hname);
       if (hedge) {
	if (debug) std::cout<<cn<<mn<<" hedge line= "<<hedge->GetLineWidth()<<std::endl;      
        if (label.Sizeof()>namesize) namesize=label.Sizeof();
        if (debug) std::cout<<cn<<mn<<"Add label to legend: "<<label.Data()<< " namesize= "<<namesize<<std::endl;
        leg->AddEntry(hedge, label, TString(opt));
       } else {
        if (label.Sizeof()>namesize) namesize=label.Sizeof();
	if (debug) std::cout<<cn<<mn<<" hedge= "<<hname<<" not found. Fill band "<<std::endl;      
        if (debug) std::cout<<cn<<mn<<"Add in legend iband= "<<iband<<" gband= "<<gband->GetName()<<" namesize= "<<namesize<<std::endl;
        leg->AddEntry(gband, label, TString(opt));
       }
      } else {
       if (label.Sizeof()>namesize) namesize=label.Sizeof();
       if (debug) std::cout<<cn<<mn<<"Add in legend iband= "<<iband<<" gband= "<<gband->GetName()<<" namesize= "<<namesize<<std::endl;
       leg->AddEntry(gband, label, TString(opt));
      }
     } else
      std::cout<<cn<<mn<<"WARNING: do not know what to do not plotMarker, not plotBand"<< std::endl;
    }
   } else { // uncertainty bands have the same properties

    if (debug) std::cout<<cn<<mn<<"All bands have same properties !"<< std::endl;

    for (int iband=0; iband<nbands; iband++) {
     TGraphAsymmErrors * gband   =pdf->GetBand(iband);
     std::string         gtype   =pdf->GetBandType(iband);

     if (debug) std::cout<<cn<<mn<<"iband= "<<iband<<" gtype= "<< gtype.c_str()<< std::endl;
     
     if (gtype.compare(std::string("pdf"))==0){
      if (debug) std::cout<<cn<<mn<<"Band is of type PDF"<< std::endl;
      pdffound=true; npdf++;
      if (nlolabel) {
      
       TString labelname=pdftype;
       //int pdfcount=std::count (vpdf.begin(), vpdf.end(), pdftype);

       if (steeringFile->GetAddonLegendNLOProgramName() ) {
  	if (debug) std::cout<<cn<<mn<<"Add NLO Program name to Legend "<<std::endl;      
        labelname+=" ";
        labelname+=nloprogamename;
       }       

       int labelcount=std::count (vlabel.begin(), vlabel.end(), labelname);
       if (debug) std::cout<<cn<<mn<<"icross= "<<icross<<" label= "<<labelname.Data()<<" labelcount= "<<labelcount<<std::endl;

      /*
       if (steeringFile->GetPlotMarker()) {
        if (pdfcount<1) {
         if (debug) std::cout<<cn<<mn<<"in PDF Plot marker add in legend iband= "<<iband<<" gband= "<<gband->GetName()<<std::endl;
  
         if (pdftype.Sizeof()>namesize) namesize=pdftype.Sizeof();
         TString opt="PE";
         if (ratioStyle.IsDataOverConvolute()) opt=""; // for Data over convolute do not plot marker
	 if (debug) std::cout<<cn<<mn<<"Add legend pdftype= "<<pdftype.Data()<<" gband= "<<gband->GetName()<<" opt= "<<opt.Data()<<std::endl;        

         leg->AddEntry(gband, pdftype,opt);
         vpdf.push_back(pdftype);
        }
       } else if (steeringFile->GetPlotBand()) {
	if (debug) std::cout<<cn<<mn<<"Add in legend iband= "<<iband
			    <<" gband= "<<gband->GetName()<<" pdfcount= "<<pdfcount<<std::endl;
        if (pdfcount<1) {
  	 if (debug) std::cout<<cn<<mn<<"Add legend pdftype= "<<pdftype.Data()<<std::endl;      
         if (pdftype.Sizeof()>namesize) namesize=pdftype.Sizeof();
         leg->AddEntry(gband, pdftype, "LF");
         vpdf.push_back(pdftype);
        }
       } else
        std::cout << cn << mn <<"WARNING: do not know what to do not plotMarker, not plotBand"<< std::endl;
     */
       // only plot label once 
       if (labelcount<1) {

        TString opt="";
        if (steeringFile->GetPlotMarker()) {
         opt="PE";
         if (ratioStyle.IsDataOverConvolute()) opt=""; // for Data over convolute do not plot marker
        } else if (steeringFile->GetPlotBand()) {
         opt="LF";
        } else std::cout << cn << mn <<"WARNING: do not know what to do not plotMarker, not plotBand"<< std::endl;

	if (debug) std::cout<<cn<<mn<<"Add in legend iband= "<<iband
			     <<" gband= "<<gband->GetName()<<" labelcount= "<<labelcount<<" opt= "<<opt.Data()<<std::endl;

  	if (debug) std::cout<<cn<<mn<<"Add legend label= "<<labelname.Data()<<std::endl;      
        if (labelname.Sizeof()>namesize) namesize=labelname.Sizeof();
        leg->AddEntry(gband, labelname, opt);
        vlabel.push_back(labelname);
       }
      }
     }

     if (nbands==1) { // band has same properties because there is only one uncertainty
       if (debug) std::cout<<cn<<mn<<"icross= "<<icross<<" nbands==1"<<std::endl;
       if (vlabel.size()!=1) {
	if (debug) std::cout<<cn<<mn<<" Add legend gband= "<<gband->GetName()<<" gtype= "<<gtype.c_str()<<std::endl;
        leg->AddEntry(gband, TString(gtype), "LF");
       }
     } else {
      //if (debug) std::cout << cn << mn <<"npdf= "<<npdf<< std::endl;
      //if (pdffound) {
      // if (debug) std::cout << cn << mn <<"PDF found "<< pdftype.Data()<< std::endl;
      //} else {
      // if (debug) std::cout << cn << mn <<"No PDF band found "<< std::endl;
      //
      if (pdffound) {
       if (debug) std::cout << cn << mn <<"Only plot corrections legend when no PDF "<< std::endl;
      } else {
       if (debug) std::cout << cn << mn <<"No PDF -> plot correction legend "<< std::endl;
       TString gname=gband->GetName();
       if (gname.Contains("_corrections_")) {
	std::vector<std::string> corrlabel=crossSections[icross].GetCorrectionLabels();
        if (debug) std::cout<<cn<<mn<<"Number of corrections= "<<corrlabel.size()<<std::endl;           
        for(int ic = 0; ic < corrlabel.size(); ic++) {
	 if (debug) std::cout<<cn<<mn<<"add in legend ic= "<<ic<<" label= "<<corrlabel[ic].c_str()<<std::endl;       
         leg->AddEntry(gband, TString(corrlabel[ic]), "LF");
        }
       } else if (debug) std::cout << cn << mn <<"No grid corrections specified "<< std::endl;
      }
     }
    }
   }
  }

  //gPad->Update();
  gPad->RedrawAxis();
  //TLine l;
  //l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
  //l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());
 }

 double x1=0., y1=0., x2=0., y2=0.;
 double xlegend=steeringFile->GetXLegend();
 double ylegend=steeringFile->GetYLegend();

 int nraw=leg->GetNRows();
 if (debug) { 
   std::cout << cn << mn <<"nraw= "<<nraw<<" namesize= "<< namesize
             << " linesize= "<<linesize<<" charactersize= "<< charactersize<<std::endl;
 }

 double csize=charactersize; 
 if (onlysyst) {
  if (nraw>5)  csize =charactersize*0.5; 
  if (nraw>8)  csize =charactersize*0.4; 
  //if (nraw>10) csize =charactersize*0.2; 
 }
 leg->SetTextSize(csize); 

 if (onlysyst) {
  if (nraw>5)  leg->SetNColumns(2);
  if (nraw>8)  leg->SetNColumns(3);
  if (nraw>12) leg->SetNColumns(4);
 }

 double fac=0.50, xfac=1.0;
 //if (namesize<20) fac=0.4;
 //if (namesize>30) fac=0.2;
 if (onlysyst) {
  //if (nraw>5) {fac*=1.2; xfac*=1.0;}
  //if (nraw>8) {fac*=1.75; xfac*=1.1;}
  //else if (nraw>10) {fac*=1.0; xfac*=1.1;}
  if (leg->GetNColumns()>3)      {fac*=1.0; xfac*=1.5;}
  else if (leg->GetNColumns()>2) {fac*=0.9; xfac*=1.4;}

 }

 double bwidth=fac*namesize*charactersize; 
 x1 = xlegend-bwidth; x2=xfac*xlegend;
 double xmin=0.18;
 if (debug) {
  std::cout<<cn<<mn<<"fac= "<<fac<<" xfac= "<<xfac<<std::endl;
  std::cout<<cn<<mn<<"x1= "<<x1<<" y1= "<<y1<<std::endl;
  std::cout<<cn<<mn<<"x2= "<<x2<<" y2= "<<y2<<std::endl;
 }
 if (x1<xmin) {x1=xmin; x2+=xmin;}

 int lsize=nraw;

 if (nraw>3)  lsize*=0.6;
 if (onlysyst) {
   //if (nraw>8)  lsize*=0.4;
   //if (nraw>20) lsize*=0.25;
   if (leg->GetNRows()>10)     {lsize*=0.15;}
   else if (leg->GetNRows()>6) {lsize*=0.2;}
   else if (leg->GetNRows()>4) {lsize*=0.3;}
   else if (leg->GetNRows()>3) {lsize*=0.4;}

 } 
 y1 = ylegend-(lsize*linesize);  y2=ylegend;
  
 if (debug) { 
  std::cout<<cn<<mn<<"xlegend= "<<xlegend<<" ylegend= "<<ylegend<<std::endl;
  std::cout<<cn<<mn<<"x1= "<<x1<<" y1= "<<y1<<std::endl;
  std::cout<<cn<<mn<<"x2= "<<x2<<" y2= "<<y2<<std::endl;
 }

 leg->SetX1NDC(x1);
 leg->SetX2NDC(x2);
 leg->SetY1NDC(y1);
 leg->SetY2NDC(y2);

 //if (debug) leg->Print();
 leg->Draw();

 // Now build second Legend with info

 TLegend *leginfo = new TLegend();
 leginfo->SetBorderSize(0);
 leginfo->SetFillColor(0);
 leginfo->SetFillStyle(0);
 leginfo->SetMargin(0.2);

 sqrtsval = -1.; sqrtsvalold = -1.; jetR    = -1.; jetRold = -1.;
 lumiold ="NOVALUE";

 double doublebinminold = -999.;
 double doublebinmaxold = -999.;
 
 for(int idata = 0; idata < data.size(); idata++) {                 

  if (etascan&&onedataset && idata>0) break;
  //TString infolabel = "#font[9]{";
  TString infolabel = "";

  if (onlysyst) { // for systematic only move data label to leginfo
   TString datalabel=data.at(idata)->GetLegendLabel();
   //leginfo->AddEntry(data.at(idata)->GetTotalErrorGraph(), datalabel, "P");
   leginfo->AddEntry((TObject*)0, datalabel, "");
  }

  //if (!etascan&&steeringFile->GetInfoLegendLabel().size()>0) {
  if (steeringFile->GetInfoLegendLabel().size()>0) {
    if (idata==0) { // Draw info lable only once
    TString label=steeringFile->GetInfoLegendLabel();
    if (debug) std::cout<<cn<<mn<<"Add to info legend idata= "<<idata<<" add info legend label "<<label.Data()<<std::endl;
    if (label.Sizeof()>leginfomax) leginfomax=label.Sizeof();
    leginfo->AddEntry((TObject*)0, label,"");
   }
  }

  if (!differentsqrts && steeringFile->GetLabelSqrtS()) {
   sqrtsval = data.at(idata)->GetSqrtS();
   //std:cout<<cn<<mn<<"idata= "<<idata<<" sqrtsval= "<<sqrtsval<<" sqrtsvalold= "<<sqrtsvalold<<std::endl;

   if (sqrtsval!=sqrtsvalold) {
    sqrtsvalold=sqrtsval;

    infolabel=this->FormatSqrtLabel(sqrtsval);
    //if (TString(infolabel).Sizeof()>leginfomax) leginfomax=TString(infolabel).Sizeof();
    if (infolabel.Sizeof()>leginfomax) leginfomax=infolabel.Sizeof();
    if (debug) std::cout<<cn<<mn<<"Add to info legend idata= "<<idata<<" add sqrts label "<<infolabel.Data()<<std::endl;
    leginfo->AddEntry((TObject*)0, infolabel,"");
   }
  }
  //sqrtsval = data.at(i)->GetSqrtS();
  //std::cout<<cn<<mn<<" jet algorithm= "<< data.at(i)->GetJetAlgorithmLabel()<<" R= "<< data.at(i)->GetJetAlgorithmRadius()<<std::endl;
  //std::cout<<cn<<mn<< " min= "<<data.at(i)->GetDoubleBinValueMin()
  //                 <<" double bin name= "<<data.at(i)->GetDoubleBinVariableName()
  //                 << " max= "<<data.at(i)->GetDoubleBinValueMax()
  //	          << std::endl;

  // if (data.at(i)->IsDividedByDoubleDiffBinWidth())  std::cout<<cn<<mn<<" Data are divided by bin width of "
  //                                                          <<data.at(i)->GetDoubleBinValueWidth()<<std::endl;
  //else std::cout<<cn<<mn<<"Data are divided by bin width of double differential variable "<<std::endl;

  if (!differentR && data.at(idata)->GetJetAlgorithmLabel().size()>0){
   jetR=data.at(idata)->GetJetAlgorithmRadius();
   if (jetR!=jetRold) {
    jetRold=jetR;
    infolabel=data.at(idata)->GetJetAlgorithmLabel();
    //    if (jetR<10)
    // infolabel+=" R= 0.";
    //else
    // infolabel+=" R= ";
    infolabel+=this->FormatjetRLabel(jetR);
    if (infolabel.Sizeof()>leginfomax) leginfomax=infolabel.Sizeof();
    if (debug) std::cout<<cn<<mn<<"Add to info legend idata= "<<idata<<" add R label "<<infolabel.Data()<<std::endl;
    leginfo->AddEntry((TObject*)0, infolabel,"");
   }
   //if (debug) std::cout<<cn<<mn<<" infolabel= "<<infolabel.Data()<<std::endl;
  }

  if (!differentetabin && !etascan) { // for eta-scan this information is in the other legend
   if (data.at(idata)->GetDoubleBinVariableName().size()>0) {
    double binmin = data.at(idata)->GetDoubleBinValueMin();
    double binmax = data.at(idata)->GetDoubleBinValueMax();
    if (binmin!=doublebinminold && binmax!=doublebinmaxold) {

     doublebinminold =  binmin;
     doublebinmaxold  = binmax;

     TString varname=data.at(idata)->GetDoubleBinVariableName();
     infolabel=this->FormatetabinLabel(varname, binmin,binmax);

     if (infolabel.Sizeof()>leginfomax) leginfomax=infolabel.Sizeof();
     if (debug) std::cout<<cn<<mn<<"Add to info legend infolabel= "<<infolabel.Data()<<std::endl;
     leginfo->AddEntry((TObject*)0, infolabel,"");
    }
   }
  }

  if (steeringFile->GetAddLumiLabel()) {
   std::string lumi = data.at(idata)-> GetDatasetLumi();
   //std::cout<<"data set lumi= "<<lumi.c_str()<<std::endl;
   if (etascan) {
    if (idata==0) {// for eta scan only for the first data-set
     if (TString(lumi).Sizeof()>leginfomax) leginfomax=TString(lumi).Sizeof();
     leginfo->AddEntry((TObject*)0, TString(lumi),"");
    }
   } else {
    if (lumi!=lumiold) {
     lumiold=lumi; 
     if (debug) std::cout<<cn<<mn<<"Add to info legend dataset lumi= "<<lumi.c_str()<<std::endl;
     if (TString(lumi).Sizeof()>leginfomax) leginfomax=TString(lumi).Sizeof();
     leginfo->AddEntry((TObject*)0, TString(lumi),"");
    }
   }
  }
   
  if (onlysyst) {
   if (steeringFile->GetSystematicClasses().size()==0) {
    double xcut=steeringFile->ShowIndividualSystematics();
    TString mypercent="%";
    TString label="";
    if (xcut!=0) {
     label="All syst>";
     int exp=0; double x=0.;
     SPXMathUtilities::frexp10(xcut, exp, x);
     if (exp>2 || exp<-2) {
      label+=SPXDrawUtilities::FormatwithExp(xcut);
     } else {
      label+=Form("%2.1f",xcut);
     }
     label+=mypercent;
    } else { 
     // Plot all systematics if no cut or no systematic classes ask for
     //std::cout<<cn<<mn<<"Number of systematic classes "<< steeringFile->GetSystematicClasses().size()<<std::endl;
     if (steeringFile->GetSystematicClasses().size()==0) label="All systematics"; 
    }
    if (debug) std::cout<<cn<<mn<<"Add to info legend label= "<<label.Data()<<std::endl;
    if (TString(label).Sizeof()>leginfomax) leginfomax=TString(label).Sizeof();
    leginfo->AddEntry((TObject*)0, TString(label),"");
   }
  }
 }

 nraw=leginfo->GetNRows();
 xfac=0.25;
 double yfac=0.60;
 if (nraw>3) yfac=0.5;
 if (nraw>10) yfac=0.1;
 double x2info=steeringFile->GetXInfoLegend();
 double y2info=steeringFile->GetYInfoLegend();
 if (nraw>3) y2info*=1.1;
 if (nraw>4) y2info*=1.15;

  //if (nraw>10) y2info*=1.15;

 double x1info=x2info-xfac*charactersize*leginfomax;
 double y1info=y2info-yfac*linesize*leginfo->GetNRows();

 if (debug) { 
  std::cout<<cn<<mn<<"leginfomax= "<<leginfomax<<std::endl;
  std::cout<<cn<<mn<<"x1info= "<<x1info<<" y1info= "<<y1info<<std::endl;
  std::cout<<cn<<mn<<"x2info= "<<x2info<<" y2info= "<<y2info<<std::endl;
 }

 csize=charactersize;
 if (onlysyst)
  if (nraw>5) csize=0.8*charactersize;
 leginfo->SetTextSize(csize);

 leginfo->SetX1NDC(x1info);
 leginfo->SetX2NDC(x2info);
 leginfo->SetY1NDC(y1info);
 leginfo->SetY2NDC(y2info);

 //if (debug) leginfo->Print();
 leginfo->Draw();

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

        //Draw PDF file
        TString epsname=filename;
        epsname.ReplaceAll("_plot_0","");
        epsname.ReplaceAll("_plot_1","");
        epsname.ReplaceAll("png","eps");
	canvas->Print(epsname);

        //Save *.C
        TString cname=filename;
        cname.ReplaceAll("_plot_0","");
        cname.ReplaceAll("_plot_1","");
        cname.ReplaceAll("png","pdf");
	canvas->Print(cname);

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
   ratioInstance.AddConvoluteFilePDFMap(convoluteFilePDFMap);
   ratioInstance.Parse(ratioString);

   bool matchbin=steeringFile->GetMatchBinning();
   if (matchbin)
    std::cout<<cn<<mn<<"Set Matchbinning to ON "<<std::endl;
   else
    std::cout<<cn<<mn<<"Set Matchbinning to OFF "<<std::endl;
   ratioInstance.SetMatchBinning(matchbin);

   if (debug) std::cout<<cn<<mn<<"i= "<<i<<" call GetGraphs "<<std::endl; 
   ratioInstance.GetGraphs();

   if (debug) std::cout<<cn<<mn<<"i= "<<i<<" call Divide "<<std::endl; 
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
		std::string keyname=pci.gridSteeringFile.GetFilename();
                //keyname=SPXUtilities::ReplaceAll(keyame,".txt","");
		StringPair_T key = StringPair_T(keyname, pci.pdfSteeringFile.GetFilename());

		if(debug) std::cout << cn << mn << "Checking for prior existence of convolute with key = [" << key.first << \
			", " << key.second << "]" << std::endl;

		if(crossSectionSet.count(key) != 0) {
		  //	if(debug) std::cout << cn << mn << "Convolute with grid filename \"" << key.first << "\" and pdf filename \"" \
		  //		<< key.second << "\" has already been processed: Will not process duplicate" << std::endl;

		  //				continue;
                  std::cout<<cn<<mn<<" is ok to process duplicates -> to be checked "<<std::endl;
 
                  std::stringstream newname;
                  newname << keyname<<"_"<< i;
		  std::cout<<cn<<mn<<"enter this grid with name= "<<newname.str()<<std::endl;
		  key = StringPair_T(newname.str(), pci.pdfSteeringFile.GetFilename());
	       }

                
		crossSectionSet.insert(key);

		if(debug) std::cout << cn << mn << "Added convolute with key = [" << key.first << ", " << key.second << "] to crossSectionsSet" << std::endl;

		try {

 		        SPXCrossSection crossSectionInstance = SPXCrossSection(&pci);
			pcis.push_back(pci);

			crossSectionInstance.SetGridName(key.first);

			crossSectionInstance.Create(steeringFile);

			crossSectionInstance.MatchBinning(dataFileGraphMap);

			crossSectionInstance.ApplyCorrections();

			crossSectionInstance.UpdateBand();

			crossSections.push_back(crossSectionInstance);

		} catch(const SPXException &e) {
			throw;
		}
	}

	if (debug) std::cout<<cn<<mn<<"Loop over cross section size=" << crossSections.size() <<std::endl;

	for (int i = 0; i < crossSections.size(); i++) {
	      SPXPlotConfigurationInstance &pci = pcis[i];

              SPXPDF *pdf=crossSections[i].GetPDF();
              if (pdf==0) {
               throw SPXParseException(cn+mn+"pdf object not found !");
              }
              //int nbands=(crossSections[i].GetPDF())->GetNBands();
              int nbands=pdf->GetNBands();

              if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;

              if (nbands==0) {
               throw SPXParseException(cn+mn+"No bands found in pdf");
              }


              //StringPair_T convolutePair = StringPair_T(pci.gridSteeringFile.GetFilename(), pci.pdfSteeringFile.GetFilename());
	      std::string keyname=pci.gridSteeringFile.GetFilename();
              StringPair_T convolutePair = StringPair_T(keyname, pci.pdfSteeringFile.GetFilename());
              if (convoluteFilePDFMap.count(convolutePair)>0) {
	       if (debug) std::cout<<cn<<mn<<"Already in map key= " <<keyname.c_str()<< std::endl;
               std::stringstream newname;
               newname << keyname<<"_"<< i;
	       std::cout<<cn<<mn<<"enter this grid with name= "<<newname.str()<<std::endl;
	       convolutePair = StringPair_T(newname.str(), pci.pdfSteeringFile.GetFilename());
              } 

              convoluteFilePDFMap.insert(StringPairPDFPair_T(convolutePair, pdf));

              int markerstyle=-99, fillcolor=-99,fillstyle=-99, edgecolor=-99, edgestyle;
              for (int iband=0; iband<nbands; iband++) {

		TGraphAsymmErrors * gband   =pdf->GetBand(iband);
		std::string         gtype   =pdf->GetBandType(iband);
		if (!gband) {
                 std::ostringstream oss;
                 oss<<cn<<mn<<"GetBands:"<<"Band "<<iband<<" not found at index "<<i;
                 throw SPXParseException(oss.str());
                }
                if (debug) std::cout << cn <<mn<<"Band "<<gband->GetName()<<" type= "<<gtype.c_str()<<std::endl;
		//Update the Convolute File Map
		//string theoryname=pci.pdfSteeringFile.GetFilename()+gband->GetName();
		std::string theoryname=gband->GetName();
                // 

                if (gtype.compare(std::string("pdf"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.pdfMarkerStyle;
                 fillcolor  =pci.pdfFillColor;
                 fillstyle  =pci.pdfFillStyle;
                 edgecolor  =pci.pdfEdgeColor;
                 edgestyle  =pci.pdfEdgeStyle;
                } 
                if (gtype.compare(std::string("scale"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.scaleMarkerStyle;
                 fillcolor  =pci.scaleFillColor;
                 fillstyle  =pci.scaleFillStyle;
                 edgecolor  =pci.scaleEdgeColor;
                 edgestyle  =pci.scaleEdgeStyle;
                } 
                if (gtype.compare(std::string("alphas"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.alphasMarkerStyle;
                 fillcolor  =pci.alphasFillColor;
                 fillstyle  =pci.alphasFillStyle;
                 edgecolor  =pci.alphasEdgeColor;
                 edgestyle  =pci.alphasEdgeStyle;
                }

                int ncorr=pci.gridSteeringFile.GetNumberOfCorrectionFiles();
                for (int icorr=0; icorr<ncorr; icorr++) {
		  std::string corrlabel=pci.gridSteeringFile.GetCorrectionFileLabel(icorr);
                 if (gtype.compare(corrlabel)==0){
	 	 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                  markerstyle=pci.correctionsMarkerStyle;
                  fillcolor  =pci.correctionsFillColor;
                  fillstyle  =pci.correctionsFillStyle;
                  edgecolor  =pci.correctionsEdgeColor;
                  edgestyle  =pci.correctionsEdgeStyle;
                 } 
                }
                if (gtype.compare(std::string("total"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.totalMarkerStyle;
                 fillcolor  =pci.totalFillColor;
                 fillstyle  =pci.totalFillStyle;
                 edgecolor  =pci.totalEdgeColor;
                 edgestyle  =pci.totalEdgeStyle;
                } 

                if (debug) {
		 std::cout << cn << mn <<" "<<std::endl;
		 std::cout << cn << mn <<"gband: "<< gband->GetName()<<" Setting: \n"
                                        <<"\t \t \t \t fillcolor= "<< fillcolor<<" fillstyle= "<<fillstyle
                                        <<" edgecolor= "<< edgecolor<<" edgestyle= "<<edgestyle
                                        <<" markerstyle= "<<markerstyle <<std::endl;
		 std::cout << cn << mn <<" "<<std::endl;
                }

		gband->SetMarkerStyle(markerstyle);
		gband->SetFillStyle  (fillstyle);
                SPXGraphUtilities::SetColors(gband,fillcolor);

                if (debug) gband->Print();               
	       }

	       // If bands have same properties use the properties from the grid_marker_color etc

	       if (!pdf->BandsHaveDifferentProperties()){
                if (debug) std::cout<<cn<<mn<<"Bands have same properties use the grid_marker_color etc from steering "<<std::endl;
                if (steeringFile->GetSetGridStyleFlag()){
                 fillcolor  =pci.gridMarkerColor;
                 markerstyle=pci.gridMarkerStyle;
                 fillstyle  =pci.gridFillStyle;

                 if (debug) {
		  std::cout<<cn<<mn<<"fillcolor= "<<fillcolor<<" markerstyle= "<<markerstyle<<" fillstyle= "<<fillstyle<<std::endl;
                 }

                 for (int iband=0; iband<nbands; iband++) {

		  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
		  std::string         gtype   =pdf->GetBandType(iband);

                  if (debug) {
		    std::cout<<cn<<mn<<"Set Band Properties "<<gband->GetName()<<" type= "<<gtype.c_str()
                             <<"\n to fillcolor= "<<fillcolor<<" markerstyle= "<<markerstyle<<" fillstyle= "<<fillstyle<<std::endl;
                  }

		  edgecolor=fillcolor;
		  gband->SetMarkerStyle(markerstyle);
		  gband->SetFillStyle  (fillstyle);
                  SPXGraphUtilities::SetColors(gband,fillcolor);
                 }
                }
   	       }

	       //StringPair_T convolutePair = StringPair_T(pci.gridSteeringFile.GetFilename(), pci.pdfSteeringFile.GetFilename());
	       //Update the Reference File Map
    	       if(debug) std::cout << cn << mn << i<<" Get reference Graph" <<std::endl;
	       TGraphAsymmErrors *refGraph = crossSections[i].GetGridReference();
               if (!refGraph) std::cout<<cn<<mn<<i<<" reference Graph not found !" <<std::endl;
	       referenceFileGraphMap.insert(StringPairGraphPair_T(convolutePair, refGraph));

	       //Update the Nominal File Map
    	       if(debug) std::cout << cn << mn << i<<" Get nomGraph" <<std::endl;
	       TGraphAsymmErrors *nomGraph = crossSections[i].GetNominal();
               if (!nomGraph) std::cout << cn << mn << i<<" nomGraph not found !" <<std::endl;
	       nominalFileGraphMap.insert(StringPairGraphPair_T(convolutePair, nomGraph));


               if (refGraph) {
     	        if(debug) std::cout << cn << mn << i<<" Set refGraph" <<std::endl;
	        //refGraph->SetMarkerSize(1.2);
	        refGraph->SetMarkerStyle(pci.pdfMarkerStyle);
	        refGraph->SetFillStyle(pci.pdfFillStyle);
	        //refGraph->SetMarkerColor(pci.pdfFillColor);
	        //refGraph->SetLineColor(pci.pdfFillColor);
	        //refGraph->SetFillColor(pci.pdfFillColor);
                SPXGraphUtilities::SetColors(refGraph,pci.pdfFillColor);
               }

               if (nomGraph) {
    	        if(debug) std::cout << cn << mn << i<<" Set nomGraph" <<std::endl;
	        //nomGraph->SetMarkerSize(1.2);
	        nomGraph->SetMarkerStyle(pci.pdfMarkerStyle);
	        nomGraph->SetFillStyle(pci.pdfFillStyle);
	        //nomGraph->SetMarkerColor(pci.pdfFillColor);
	        //nomGraph->SetLineColor(pci.pdfFillColor);
	        //nomGraph->SetFillColor(pci.pdfFillColor);
                SPXGraphUtilities::SetColors(nomGraph,pci.pdfFillColor);
               }

	      /*
    	       if(debug) std::cout << cn << mn << i<<" ...finished GraphSetting" <<std::endl;
	       if(convoluteFileGraphMap.count(convolutePair)>0) {
		if(debug) {
		 std::cout << cn << mn << "convolute pair in map: [" << convolutePair.first << ", " << convolutePair.second << "]" << std::endl;
		}
	       } else {
		 std::cerr <<cn<<mn<< "---> Warning: Unable to add convolute pair to map: [" << convolutePair.first << ", " << convolutePair.second << "]" << std::endl;
	       }
	      */
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

			//Also scale by the artificial grid scale from the grid steering file
			double xScale2 = 1.0;
			double yScale2 = pci->gridSteeringFile.GetYScale();
			if (debug) std::cout << cn << mn << "Scaling Bands from Grid steering factor xScale= "
                                             <<xScale<<" yScale= "<<yScale<<" yscale2= "<<yScale2<< std::endl;  

                        double myscaley=yScale*yScale2;
                        double myscalex=xScale*xScale2;

                        int nbands=(crossSections[i].GetPDF())->GetNBands();
                        if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;

                        if (myscaley!=1. || myscalex!=1.) {
  			 SPXGraphUtilities::Scale(gNom, xScale, yScale);
 			 SPXGraphUtilities::Scale(gRef, xScale, yScale);
                         //
                         // loop over bands (pdf, alphas, scale etc) as set in SPXPDF
                         //
                         for (int iband=0; iband<nbands; iband++) {
                          SPXPDF * pdf=crossSections[i].GetPDF();
			  TGraphAsymmErrors * gband=pdf->GetBand(iband);
			  if (!gband) {
                           std::ostringstream oss;
                           oss << cn <<mn<<"Band "<<iband<<" not found at index "<<i;
                           throw SPXParseException(oss.str());
			  } else {
			   if (debug) std::cout << cn << mn << "Scaling band: " << gband->GetName() <<" with yScale= "<<myscaley<<" xScale= "<<myscalex<< std::endl;  
                           SPXGraphUtilities::Scale(gband , myscalex, myscaley);
                          }
                         }
                        }
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

			double totalSigmaNom = SPXGraphUtilities::GetTotalSigma(gNom, gridDividedByBinWidth);
			double totalSigmaRef = SPXGraphUtilities::GetTotalSigma(gRef, referenceDividedByBinWidth);
			if(debug) {
			  std::cout<<cn<<mn<< "Nominal Total Sigma = " << totalSigmaNom   << std::endl;
			  std::cout<<cn<<mn<< "Reference Total Sigma = "<< totalSigmaRef  << std::endl;
			}
			//First divide the cross section by the bin width if it needs to be
			//
			if(dataDividedByBinWidth && !gridDividedByBinWidth) {
			 if(debug) std::cout << cn << mn << "Dividing Cross Section by the Bin Width" << std::endl;
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
			  } else {
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

		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXGraphException(cn+mn+"Unable to obtain X/Y Scale based on Data/Grid Units");
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

		//SPXData dataInstance = SPXData(pci);

                SPXData *dataInstance = new SPXData(pci);
                if (!dataInstance) throw SPXGeneralException(cn+mn+"Problem to create dataInstance");

                if (steeringFile->GetTakeSignforTotalError()) {
                 dataInstance->SetTakeSignforTotalError(true);
		}

		if(debug) std::cout<<cn<<mn<<"Call GetDataRemoveFlag for i= "<<i << std::endl;
 
                if (steeringFile->GetDataRemoveXbinsFlag(i)) {
           
		 double xmax=steeringFile->GetDataCutXmax(i);
		 double xmin=steeringFile->GetDataCutXmin(i);

                 if (debug) std::cout<<cn<<mn<<"Remove bins from data with value > "<<xmax<<" and < "<<xmin<<std::endl;

                 dataInstance->SetDataCutXmin(xmin);
                 dataInstance->SetDataCutXmax(xmax);

		}

                dataInstance->Parse();

                dataInstance->CreateGraphs();

   		if(debug) std::cout << cn << mn << "Chi2 calculations" << steeringFile->GetCalculateChi2() << std::endl;

                if (steeringFile->GetCalculateChi2()>0) {
   		 if(debug) std::cout << cn << mn << "Read-in Chi2 calculations" << std::endl;
                 dataInstance->ReadCorrelation();
                }

		//@DEBUG
		std::cout  << cn << mn << pci.dataDirectory << std::endl;
		std::cout  << cn << mn << pci.dataSteeringFile.GetFilename() << std::endl;

		try {
		        //dataInstance.Parse();
			//dataInstance.Print();
                        dataInstance->Print();
			data.push_back(dataInstance);
		} catch(const SPXException &e) {
			throw;
		}
	}

	if(debug) std::cout<<cn<<mn<<"Loop over data with size= " <<data.size()<< std::endl;
	//Go through data objects and create graphs, modify settings, etc.
	for(int i = 0; i < data.size(); i++) {

		//Create graphs: once this function is called, it is safe to manipulate graphs
		//data[i].CreateGraphs();

		//Obtain the graphs

		TGraphAsymmErrors *statGraph = data[i]->GetStatisticalErrorGraph();
		TGraphAsymmErrors *systGraph = data[i]->GetSystematicErrorGraph();
		TGraphAsymmErrors *totGraph  = data[i]->GetTotalErrorGraph();

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
		totGraph ->SetMarkerStyle(pci.dataMarkerStyle);

                SPXGraphUtilities::SetColors(statGraph,pci.dataMarkerColor);
                SPXGraphUtilities::SetColors(systGraph,pci.dataMarkerColor);
                SPXGraphUtilities::SetColors(totGraph ,pci.dataMarkerColor);

		statGraph->SetLineWidth(1);
		systGraph->SetLineWidth(1);
		totGraph ->SetLineWidth(1);

		//Add total error graph and stat error graph to dataFileGraphMap
		dataFileGraphMap.insert(StringGraphPair_T(pci.dataSteeringFile.GetFilename(), totGraph));
		dataFileGraphMap.insert(StringGraphPair_T(pci.dataSteeringFile.GetFilename() + "_stat", statGraph));

		std::vector <TGraphAsymmErrors *> vsyst=data[i]->GetSystematicsErrorGraphs();
		if (debug) std::cout<<cn<<mn<<"Number of systematic found= " << vsyst.size() <<std::endl;

                if (steeringFile->ShowIndividualSystematics()!=0) {

                  int icountsyst=0;
                  for (int isyst=0; isyst<vsyst.size(); isyst++) {
		   std::string systname=vsyst.at(isyst)->GetName();
		   if(dataFileGraphMap.count(systname)>0) {
		    std::cout<<cn<<mn<<"WARNING: systematics"<<systname.c_str()<<" already in map "<< std::endl; 
                    continue;
                   }

                   double emax=SPXGraphUtilities::GetLargestRelativeError(vsyst.at(isyst))*100; // in percent
                   if (debug) std::cout<<cn<<mn<<systname.c_str()<<" emax= "<<emax<<"% above "<<std::endl;
		   if (fabs(emax)>steeringFile->ShowIndividualSystematics()) {
		    if (debug) {
		     std::cout<<cn<<mn<<"Add systematic "<<systname.c_str()<<" to dataFileGraphMap"<< std::endl;
                     //vsyst.at(isyst)->Print(); 
                     std::cout<<cn<<mn<<systname.c_str()<<" Largest systematics "<<emax<<"% above "<<steeringFile->ShowIndividualSystematics()<<"%"<<std::endl;
                    }
		    icountsyst++;

                    Color_t icol=SPXUtilities::ICol(icountsyst);
                    vsyst.at(isyst)->SetFillStyle(3002);
                    SPXGraphUtilities::SetColors(vsyst.at(isyst),icol);

 		    dataFileGraphMap.insert(StringGraphPair_T(systname, vsyst.at(isyst)));
		   } else {
		    //if (debug) std::cout<<cn<<mn<<"systematics "<<systname.c_str()<<" too small ! Largest systematics= "<<emax<<" < " << steeringFile->ShowIndividualSystematics() <<std::endl;
                   }
                  }
                } else {
		 //
		 // displaying groups only make sense if all systematics should be shown 
                 // otherwise it is better to show the individual components
                 std::vector<std::string>  systematicsgroups      = steeringFile->GetSystematicClasses();
		 std::vector<int>          systematicsgroupscolor = steeringFile->GetSystematicClassesColor();
                 if (systematicsgroups.size()!=systematicsgroupscolor.size()) {
                  std::ostringstream oss;
                  oss<<cn<<mn<<"Systematics group vectors have not the same size vsystgroups= "
                             <<systematicsgroups.size()<<" systematicsgroupscolor= "<<systematicsgroupscolor.size();
		  throw SPXGraphException(oss.str());
                 }
                 if (debug) {
                  std::cout<<cn<<mn<<"Number of systematic groups= " <<systematicsgroups.size() <<std::endl;                  
                  for (int igroup=0; igroup<systematicsgroups.size(); igroup++) {
                   std::cout<<cn<<mn<<" " <<std::endl;                  
                   std::cout<<cn<<mn<<igroup<<" systematic group= " <<systematicsgroups.at(igroup)
                                            <<" color= "<< systematicsgroupscolor.at(igroup) <<std::endl;
                  }
	         }

                 //
                 // Merge systematics according to groups
                 //
		 std::vector <TGraphAsymmErrors *> vsystgroups; vsystgroups.clear();
                 std::map<std::string,int> systmap;
                 int inotingroup=0;                    
                 for (int isyst=0; isyst<vsyst.size(); isyst++) {
		  TString systname=vsyst.at(isyst)->GetName();
                  bool ingroup=false; 
                  for (int igroup=0; igroup<systematicsgroups.size(); igroup++) {
		    //if (debug) std::cout<<cn<<mn<<"Test if isyst= "<<isyst<<" contains "<<systematicsgroups.at(igroup)
                    //         <<" igroup= "<<igroup<<" systname= "<<systname.Data()<<std::endl;
		    //if (debug) {
                    //for(std::map<std::string,int>::const_iterator it = systmap.begin(); it != systmap.end(); ++it) {
                    // std::cout <<cn<<mn<< "systmap["<<it->first<<"]="<< " index " << it->second << std::endl;
                    //}
		    //}
		   if (systname.Contains(systematicsgroups.at(igroup),TString::kIgnoreCase)) {
		    //if (debug) std::cout<<cn<<mn<<"Contains "<<systematicsgroups.at(igroup)<<std::endl;
                    ingroup=true;
		    if (systmap.count(systematicsgroups.at(igroup))==0) { 
		     //if (debug) std::cout<<cn<<mn<<"Create vector "<<std::endl;
		     vsystgroups.push_back(vsyst.at(isyst));
                     TString sname="syst_";
                     sname+=systematicsgroups.at(igroup);
                     vsystgroups.back()->SetName(sname);
		     Color_t icol=systematicsgroupscolor.at(igroup);     
                     SPXGraphUtilities::SetColors( vsystgroups.back(),icol);
                     systmap[systematicsgroups.at(igroup)]=vsystgroups.size()-1;
		     //if (debug) std::cout<<cn<<mn<<"Created vector "<<vsystgroups.back()->GetName()<<" with vector "<<vsyst.at(isyst)->GetName()<<std::endl;
                    } else {
		     //if (debug) std::cout<<cn<<mn<<"Add vector "<<std::endl;
		     //if (debug) std::cout<<cn<<mn<<systematicsgroups.at(igroup).c_str()<<" count "<<systmap.count(systematicsgroups.at(igroup))<<std::endl;
		     int i=systmap[systematicsgroups.at(igroup)];
		     SPXGraphUtilities::AddinQuadrature(vsystgroups.at(i),vsyst.at(isyst),steeringFile->GetTakeSignforTotalError());
		     //if (debug) std::cout<<cn<<mn<<"Added vector "<<vsyst.at(isyst)->GetName()<<" to vector "<<vsystgroups.at(i)->GetName()<<" index i= "<<i<<std::endl;
                     //if (debug) {    
		     // std::cout<<cn<<mn<<"vsyst["<<isyst<<"] "<<vsyst.at(isyst)->GetName()<<std::endl;
		     // vsyst.at(isyst)->Print();
		     // std::cout<<cn<<mn<<"vsystgroups["<<i<<"] "<<vsystgroups.at(i)->GetName()<<std::endl;
		     // vsystgroups.at(i)->Print();
		     //}
		    }
                   }
                  }

                  if (!ingroup) {
                   TString systname=vsyst.at(isyst)->GetName();
                   bool others=false;
                   for (int igroup=0; igroup<systematicsgroups.size(); igroup++) {
		    if (systematicsgroups.at(igroup)=="Others" || systematicsgroups.at(igroup)=="others" ) {
                     others=true;
                     //if (debug) std::cout<<cn<<mn<<"Systematic group others found"<<std::endl;                   
		     if (systmap.count(systematicsgroups.at(igroup))==0) { 
		      vsystgroups.push_back(vsyst.at(isyst));
                      TString sname="syst_";
                      sname+=systematicsgroups.at(igroup);
                      vsystgroups.back()->SetName(sname);
		      Color_t icol=systematicsgroupscolor.at(igroup);                    
                      SPXGraphUtilities::SetColors( vsystgroups.back(),icol);
		      //if (debug) std::cout<<cn<<mn<<"New group: "<<systematicsgroups.at(igroup).c_str()<< " with vector "<<vsyst.at(isyst)->GetName()
		      //			  << " vsystgroups["<<vsystgroups.size()-1<<"]="<< vsystgroups.back()->GetName() <<std::endl;
                      systmap[systematicsgroups.at(igroup)]=vsystgroups.size()-1;
                     } else {
		      int i=systmap[systematicsgroups.at(igroup)];
		      SPXGraphUtilities::AddinQuadrature(vsystgroups.at(i),vsyst.at(isyst));
		      //if (debug) std::cout<<cn<<mn<<"Added vector "<<vsyst.at(isyst)->GetName()<<" to vector "<<vsystgroups.at(i)->GetName()<<std::endl;
                     } 
                    }
                   }
                   if (!others) {
		     //if (debug) std::cout<<cn<<mn<<"No group found for "<<systname<<" isyst= "<<isyst<<" add to groups "<<std::endl;
		    vsystgroups.push_back(vsyst.at(isyst));
                    //sname.ReplaceAll("syst_","");
		    vsystgroups.back()->SetName(systname);
		    Color_t icol=SPXUtilities::IColBlue(++inotingroup);                    
                    SPXGraphUtilities::SetColors(vsystgroups.back(),icol);
                   }
                  }
                 }

                 if (debug) {
                  std::cout<<cn<<mn<<"  "<<std::endl;
		  std::cout<<cn<<mn<<"Number of systematic groups= " <<vsystgroups.size() <<" syst not in group "<<inotingroup<<std::endl;                  
                  for (int igroup=0; igroup<vsystgroups.size(); igroup++) {
		   std::cout<<" "<<std::endl;
		   std::cout<<cn<<mn<<" igroup= "<<igroup<<" systematic group= " <<vsystgroups.at(igroup)->GetName()
                                             <<" icol= "<<vsystgroups.at(igroup)->GetLineColor()<<std::endl;
		   vsystgroups.at(igroup)->Print();
                  }
	         }             

                 if (debug) std::cout<<cn<<mn<<"  "<<std::endl;
                 for (int igroup=0; igroup<vsystgroups.size(); igroup++) {
		  std::string systname=vsystgroups.at(igroup)->GetName();
                  if (debug) std::cout<<cn<<mn<<" Enter "<<systname.c_str()<<" to dataFileGraphMap "<<std::endl;
 		  dataFileGraphMap.insert(StringGraphPair_T(systname,vsystgroups.at(igroup) ));
                 }
                }

		if(dataFileGraphMap.count(pci.dataSteeringFile.GetFilename())>0) {
		 if(debug) {
		  std::cout << cn << mn << "Added data to map: [" << pci.dataSteeringFile.GetFilename() << "]" << std::endl;
		 }
		} else {
		 std::cerr << "---> Warning: Unable to add data to map: [" << pci.dataSteeringFile.GetFilename() << "]" << std::endl;
		}

                if (steeringFile->OrderColorSystematicNamesByAlphabeth()) {
 		 // order color surviving systematics by alphabeth
                 // above SetColor is overwritten as all systematics are contain in vsyst
		 if (debug) std::cout<<cn<<mn<<"Order systematics color by name alphabetical order"<<std::endl;
                 OrderSystVectorColorsByAlphabeth(vsyst);
                }

	}
}


void SPXPlot::DrawBand(SPXPDF *pdf, std::string option, SPXPlotConfigurationInstance pci) {
 std::string mn = "DrawBand: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (!pdf) {
  throw SPXGeneralException(cn+mn+"PDF object not found !");
 }

 int nbands=pdf->GetNBands();
 if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;

 bool detailedband=false;
 detailedband=pdf->HasDetailedBands();
 if (debug) if (detailedband) std::cout << cn << mn <<"detailedband TRUE" << std::endl;

 bool bandsdifferent=false;
 bandsdifferent=pdf->BandsHaveDifferentFillStyle();
 if (debug) if (bandsdifferent) std::cout << cn << mn <<" bandsdifferent TRUE" << std::endl;

 if (steeringFile->GetPlotMarker() || !bandsdifferent){
  for (int iband=0; iband<nbands; iband++) {
   TGraphAsymmErrors * gband   =pdf->GetBand(iband);
   TString gname=gband->GetName();
   std::cout << cn << mn <<iband<<" gname= "<<gname.Data() << std::endl;
   if (gname.Contains("_total_")) {
    std::cout << cn << mn <<"INFORMATION only plot total band " << std::endl;

    gband->Draw(option.c_str());

    if (debug) {
     std::cout<< cn << mn <<"\n Drew "<<gband->GetName()<< " with options "<<option.c_str()
                          << " fillcolor= " << gband->GetFillColor() 
                          << " fillstyle= " << gband->GetFillStyle() 
                          << " markerstyle= "<<gband->GetMarkerStyle() 
                          << std::endl;          
     gband->Print();
    }
    return;
   }
  }
 }

 double edgecolor  =pci.totalEdgeColor;
 double edgestyle  =pci.totalEdgeStyle;
 double fillcolor  =pci.totalFillColor;
 double fillstyle  =pci.totalFillStyle;
 double markerstyle=pci.totalMarkerStyle;

 std::vector < TGraphAsymmErrors *> graphs;

 for (int iband=0; iband<nbands; iband++) {
  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
  TString gname=gband->GetName();
  std::string  gtype=pdf->GetBandType(iband);

  if (debug) std::cout<<cn<<mn<<"gname= "<<gname.Data()<<" type= "<<gtype.c_str()<<std::endl;

  graphs.push_back(gband);
 }

 // order bands for better plotting
 std::map<int, TGraphAsymmErrors * > bands=  SPXUtilities::OrderBandMap(graphs);

 // now plot graph: largest first
 //std::cout<<cn<<mn<<" \n iterate over map " <<std::endl;
  for(std::map<int, TGraphAsymmErrors *>::reverse_iterator it=bands.rbegin(); it!=bands.rend(); ++it) {

  std::cout<<cn<<mn<<it->first<<" "<<it->second->GetName()<<std::endl;

  TGraphAsymmErrors *gband = it->second;
  TString gname=gband->GetName();

  if (gname.Contains("_total_")) { 
   fillcolor  =pci.totalFillColor;
   edgecolor  =pci.totalEdgeColor;
   edgestyle  =pci.totalEdgeStyle;
   fillstyle  =pci.totalFillStyle;
   markerstyle=pci.totalMarkerStyle;
  }

  if (gname.Contains("_scale_")) { 
   fillcolor  =pci.scaleFillColor;
   edgecolor  =pci.scaleEdgeColor;
   edgestyle  =pci.scaleEdgeStyle;
   fillstyle  =pci.scaleFillStyle;
   markerstyle=pci.scaleMarkerStyle;
  }

  if (gname.Contains("_alphas_")) { 
   fillcolor  =pci.alphasFillColor;
   edgecolor  =pci.alphasEdgeColor;
   edgestyle  =pci.alphasEdgeStyle;
   fillstyle  =pci.alphasFillStyle;
   markerstyle=pci.alphasMarkerStyle;
  }

  if (gname.Contains("_pdf_")) { 
   fillcolor  =pci.pdfFillColor;
   edgecolor  =pci.pdfEdgeColor;
   fillstyle  =pci.pdfFillStyle;
   edgestyle  =pci.pdfEdgeStyle;
   markerstyle=pci.pdfMarkerStyle;
  }

  if (gname.Contains("_corrections_")) { 
   fillcolor  =pci.correctionsFillColor;
   edgecolor  =pci.correctionsEdgeColor;
   fillstyle  =pci.correctionsFillStyle;
   edgestyle  =pci.correctionsEdgeStyle;
   markerstyle=pci.correctionsMarkerStyle;
  }

  if (debug) std::cout<<cn<<mn<<"edgecolor= "<<edgecolor<<" edgestyle= "<<edgestyle
                              <<" fillcolor= "<<fillcolor<<" fillstyle= "<<fillstyle<<std::endl;
 
  if (edgecolor!=0) { // 0 is default in SPXPlotConfiguration.h
   TH1D *hedgelow =SPXGraphUtilities::GetEdgeHistogram(gband,true);
   TH1D *hedgehigh=SPXGraphUtilities::GetEdgeHistogram(gband,false);
   hedgelow ->SetLineColor(abs(edgecolor));
   hedgehigh->SetLineColor(abs(edgecolor));
   if (edgestyle!=-1) { // -1 is default in SPXPlotConfiguration.h
    hedgelow ->SetLineStyle(edgestyle);
    hedgehigh->SetLineStyle(edgestyle);

    hedgelow ->SetLineWidth(4);
    hedgehigh->SetLineWidth(4);
   }
   hedgelow ->Draw("][,same");
   hedgehigh->Draw("][,same");
  }

  if (edgecolor>=0) {
   gband->Draw(option.c_str());
  }

  if (debug) {
   std::cout<< cn << mn <<"\n Drew "<<gband->GetName()<< " with options "<<option.c_str()
                        << " fillcolor= "   << gband->GetFillColor() 
                        << " fillstyle= "   << gband->GetFillStyle() 
                        << " markerstyle= " << gband->GetMarkerStyle() 
                        << std::endl;          
  }

 }
 return;
}

TString SPXPlot::FormatSqrtLabel(double sqrtsval){
 TString infolabel="";
 if (int(sqrtsval)%1000==0)
  infolabel.Form("#sqrt{s}= %.f %s",double(sqrtsval)/1000.,"TeV"); 
 else
  infolabel.Form("#sqrt{s}= %3.0f %s",double(sqrtsval),"GeV"); 

 return infolabel;
};

TString SPXPlot::FormatjetRLabel(double jetR){
 TString infolabel="";
 if (jetR<10)
  infolabel+=" R= 0.";
 else
  infolabel+=" R= ";

 infolabel+=jetR;

 return infolabel;
};

TString SPXPlot::FormatetabinLabel(TString varname, double binmin, double binmax){
 TString infolabel="";
 if (binmin!=0) {
  infolabel.Form(" %3.2f ",binmin); 
 } else {
  //std::cout<<" varname= "<<varname.Data()<<std::endl;
 varname.ReplaceAll("#leq","");
 varname.ReplaceAll("","");
 }
 infolabel+=varname;
 infolabel+=Form(" %3.2f ",binmax);
 return infolabel;
};


void SPXPlot::OrderSystVectorColorsByAlphabeth(std::vector<TGraphAsymmErrors *> vsyst){
 std::string mn = "OrderSystVectorColorsByAlphabeth: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (vsyst.size()==0) {
  std::cout<<cn<<mn<<"WARNING: no input graph found ! "<<std::endl;
  return;
 }

 std::vector<TGraphAsymmErrors *> vsysttmp;
 // vsysttmp.reserve(vsyst.size());
 copy(vsyst.begin(),vsyst.end(),back_inserter(vsysttmp));

 // std::cout<<" XXX graph copied ! "<<std::endl;
 // Now sort graph according to alphabethical order of graph names
 //
 sort(vsysttmp.begin(), vsysttmp.end(), SPXUtilities::SortbyAlphabeth);

 std::cout<<cn<<mn<<"Sorted graph members "<<vsysttmp.size()<<std::endl;

 for (int i=0; i<vsysttmp.size(); i++){
 
  TGraphAsymmErrors * graph=vsysttmp.at(i);
  if (!graph) {  
   throw SPXGraphException(cn+mn+"Graph not found");
  }

 
  Color_t icol=SPXUtilities::ICol(i);
 
  if (debug) {
   std::cout<<cn<<mn<<"graphname= "<<graph->GetName()<<" icol= "<<icol<<std::endl;
  }

  SPXGraphUtilities::SetColors(vsysttmp.at(i),icol);
 }

 vsysttmp.clear();

 return;

};

