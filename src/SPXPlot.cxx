//************************************************************/
//
//	Plot Implementation
//
//	Implements the SPXPlot class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT objects
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		13.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/
//

#include <sstream>
#include <string.h> //memcpy

#include "SPXPlot.h"
#include "SPXSummaryFigures.h" 

//#ifdef DEVELOP
//#include "SPXSummaryFigures.h" 
//#include "SPXGlobalCorrelation.h" 
//#endif

const std::string cn = "SPXPlot::";

//Must define the static debug variable in the implementation
bool SPXPlot::debug;

#ifdef DEVELOP
SPXpValue* pvalue;
SPXGlobalCorrelation *gcm;
#endif
SPXSummaryFigures * summaryfigures;

//SPXSummaryFigures * summaryfigures;

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

	PreparationDrawRatio();
	DrawRatio();
        DrawBox(); 

	DrawLegend();

#ifdef DEVELOP
        if (steeringFile->GetCalculateChi2()>0){       
	 if (debug) {
 	  std::cout<<" "<<std::endl;
	  std::cout<<cn<<mn<<"Now call pValue calculation "<<std::endl;
	  std::cout<<cn<<mn<<"Data size "<<data.size()<<std::endl;
	  std::cout<<cn<<mn<<"CrossSection size "<<crossSections.size()<<std::endl;
	  std::cout<<cn<<mn<<"id= "<<id<<std::endl;
	 }

         if (id==0) {     
          gcm= new SPXGlobalCorrelation(steeringFile);
         }

	 gcm->SetData(data);
         if (id==steeringFile->GetNumberOfPlotConfigurations()-1) {
	  std::cout<<cn<<mn<<"Build the global correllation matrix: "<<std::endl;
	  gcm->BuildGlobalCovarianceMatrix();
          if (!gcm->CheckMatrices()) {
           std::cout<<cn<<mn<<"Problem in global covariance matrix "<<std::endl;
           std::cerr<<cn<<mn<<"Problem in global covariance matrix "<<std::endl;
          } else { 
           if (debug) {
 	    std::cout<<cn<<mn<<"Print the global correllation matrix: "<<std::endl;
            gcm->GetGlobalCovarianceMatrix()->Print();
           }
          }
         }

         if (id==0) {
          pvalue = new SPXpValue(); //class to analyze data and theory agreement
          pvalue->SetSteeringFile(steeringFile); 
         }

         pvalue->SetPlotNumber(id);         
         pvalue->SetDataandTheory(data,crossSections);
         //pvalue->GetDataTheoryAndUncertainties();         

         //std::cout<<cn<<mn<<"id= "<<id<<std::endl;
	 //std::cout<<cn<<mn<<"Number of plots= "<<steeringFile->GetNumberOfPlotConfigurations()<<std::endl;

         if (id==steeringFile->GetNumberOfPlotConfigurations()-1) {
	   //std::cout<<cn<<mn<<"calling SPXpValue finalize method "<<std::endl;
          pvalue->Finalize();         
         }
	 */
	} else {
	 std::cout<<cn<<mn<<"calculate_chi2 option not ON SPXpValue not called "<<id<<std::endl;
        } 


	SPXCImodel* pcimodel= new SPXCImodel(data,crossSections, steeringFile);
        pcimodel->SetPlotNumber(id); 
        pcimodel->SetRatioPad(ratioPad); 
        pcimodel->Analyze(); 

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

         if (id==steeringFile->GetNumberOfPlotConfigurations()-1) {
	  std::cout<<cn<<mn<<"Now Draw summary figure "<<std::endl;
          summaryfigures->Draw();         
         }
         if (id==steeringFile->GetNumberOfPlotConfigurations()-1) {
	  std::cout<<cn<<mn<<"Now Draw summary figure "<<std::endl;
          summaryfigures->Draw();         
         }
#endif
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

        //if (debug) {
	// std::cout<<cn<<mn<<"xMinOverlay = "<<xMinOverlay<<std::endl;
	// std::cout<<cn<<mn<<"xMaxOverlay = "<<xMaxOverlay<<std::endl;
	// std::cout<<cn<<mn<<"yMaxOverlay = "<<yMaxOverlay<<std::endl;
	// std::cout<<cn<<mn<<"yMinOverlay = "<<yMinOverlay<<std::endl;
        //}

        double xmin=steeringFile->GetXOverlayMin();
        double xmax=steeringFile->GetXOverlayMax();
        double ymin=steeringFile->GetYOverlayMin();
        double ymax=steeringFile->GetYOverlayMax();

        if (debug) {
	 std::cout<<cn<<mn<<"x_overlay_max= "<<xmax<<std::endl;
	 std::cout<<cn<<mn<<"x_overlay_min= "<<xmin<<std::endl;
	 std::cout<<cn<<mn<<"y_overlay_max= "<<ymax<<std::endl;
	 std::cout<<cn<<mn<<"y_overlay_min= "<<ymin<<std::endl;
        }
        // values can be overwritten bby steering parameters given in Plot field
        if (pc.CheckPlotOptions("x_overlay_max")){
         xmax=pc.GetPlotOptions("x_overlay_max");
	 if (debug) std::cout<<cn<<mn<<"x_overlay_max= "<<xmax<<std::endl;
        } 

        if (pc.CheckPlotOptions("x_overlay_min")){
         xmin=pc.GetPlotOptions("x_overlay_min");
	 if (debug) std::cout<<cn<<mn<<"x_overlay_min= "<<xmin<<std::endl;
        } 

        if (pc.CheckPlotOptions("y_overlay_max")){
         xmax=pc.GetPlotOptions("y_overlay_max");
	 if (debug) std::cout<<cn<<mn<<"y_overlay_max= "<<ymax<<std::endl;
        } 

        if (pc.CheckPlotOptions("y_overlay_min")){
         xmin=pc.GetPlotOptions("y_overlay_min");
	 if (debug) std::cout<<cn<<mn<<"y_overlay_min= "<<ymin<<std::endl;
        } 

        if (debug) {
	 std::cout<<cn<<mn<<"x_overlay_max= "<<xmax<<std::endl;
	 std::cout<<cn<<mn<<"x_overlay_min= "<<xmin<<std::endl;
	 std::cout<<cn<<mn<<"y_overlay_max= "<<ymax<<std::endl;
	 std::cout<<cn<<mn<<"y_overlay_min= "<<ymin<<std::endl;
        }
	//Force to steering file Y bounds if set
	//if(steeringFile->GetYOverlayMin() != MIN_EMPTY) {
	//	yMinOverlay = steeringFile->GetYOverlayMin();
	if(ymin != MIN_EMPTY) {
		yMinOverlay = ymin;
		if(debug) std::cout << cn << mn << "Forcing Overlay Y Axis Minimum to " << yMinOverlay << std::endl;
	}
	//if(steeringFile->GetYOverlayMax() != MAX_EMPTY) {
	//	yMaxOverlay = steeringFile->GetYOverlayMax();
	if(ymax != MAX_EMPTY) {
		yMaxOverlay = ymax;
		if(debug) std::cout << cn << mn << "Forcing Overlay Y Axis Maxmimum to " << yMaxOverlay << std::endl;
	}

	//if(steeringFile->GetXOverlayMin() != MIN_EMPTY) {
	//	xMinOverlay = steeringFile->GetXOverlayMin();
	if(xmin != MIN_EMPTY) {
		xMinOverlay = xmin;
		if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Minimum to " << xMinOverlay << std::endl;
	}

	if(xmax != MAX_EMPTY) {
		xMaxOverlay = xmax;
		if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Minimum to " << xMaxOverlay << std::endl;
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

        if (debug) {
	 std::cout<<"After DetermineRatioFrameBounds:"<<std::endl;
	 std::cout<<cn<<mn<<"yMaxRatio= "<<yMaxRatio<<std::endl;
	 std::cout<<cn<<mn<<"yMinRatio= "<<yMinRatio<<std::endl;
	 std::cout<<cn<<mn<<"xMaxRatio= "<<xMaxRatio<<std::endl;
	 std::cout<<cn<<mn<<"xMinRatio= "<<xMinRatio<<std::endl;
        }

        double xmin=steeringFile->GetXOverlayMin();
        double xmax=steeringFile->GetXOverlayMax();
        double ymin=steeringFile->GetYRatioMin();
        double ymax=steeringFile->GetYRatioMax();

        if (debug) {
	 std::cout<<"After reading steeringFile [Graph]:"<<std::endl;
	 std::cout<<cn<<mn<<"y_ratio_max= "<<ymax<<std::endl;
	 std::cout<<cn<<mn<<"y_ratio_min= "<<ymin<<std::endl;
	 std::cout<<cn<<mn<<"x_ratio_max= "<<xmax<<std::endl;
	 std::cout<<cn<<mn<<"x_ratio_min= "<<xmin<<std::endl;
        }

        if (pc.CheckPlotOptions("y_ratio_max")){
         ymax=pc.GetPlotOptions("y_ratio_max");
	 if (debug) std::cout<<cn<<mn<<"y_ratio_max= "<<ymax<<std::endl;
        } 

        if (pc.CheckPlotOptions("y_ratio_min")){
         ymin=pc.GetPlotOptions("y_ratio_min");
	 if (debug) std::cout<<cn<<mn<<"y_ratio_min= "<<ymin<<std::endl;
        } 

        if (pc.CheckPlotOptions("x_overlay_max")){
         xmax=pc.GetPlotOptions("x_overlay_max");
	 if (debug) std::cout<<cn<<mn<<"x_overlay_max= "<<xmax<<std::endl;
        } 

        if (pc.CheckPlotOptions("x_overlay_min")){
         xmin=pc.GetPlotOptions("x_overlay_min");
	 if (debug) std::cout<<cn<<mn<<"x_overlay_min= "<<xmin<<std::endl;
        } 

        if (debug) {
	 std::cout<<"After reading steeringFile [Plot]:"<<std::endl;
	 std::cout<<cn<<mn<<"y_ratio_max= "<<ymax<<std::endl;
	 std::cout<<cn<<mn<<"y_ratio_min= "<<ymin<<std::endl;
	 std::cout<<cn<<mn<<"x_ratio_max= "<<xmax<<std::endl;
	 std::cout<<cn<<mn<<"x_ratio_min= "<<xmin<<std::endl;
        }
        //
	//Force to steering file Y bounds if set
	//if(steeringFile->GetYRatioMin() != MIN_EMPTY) {
        if( ymin != MIN_EMPTY) {
	  //yMinRatio = steeringFile->GetYRatioMin();
          yMinRatio = ymin;
	  if(debug) std::cout << cn << mn << "Forcing Ratio Y Axis Minimum to " << yMinRatio << std::endl;
	}
	//if(steeringFile->GetYRatioMax() != MAX_EMPTY) {
	if( ymax != MAX_EMPTY) {
	  //yMaxRatio = steeringFile->GetYRatioMax();
          yMaxRatio = ymax;
	  if(debug) std::cout << cn << mn << "Forcing Ratio Y Axis Maxmimum to " << yMaxRatio << std::endl;
	}

	//Force Ratio X Min/Max to match Overlay, if plotted (should already match anyway...)
	if(ds.ContainsOverlay()) {
	 xMinRatio = xMinOverlay;
	 xMaxRatio = xMaxOverlay;
	} else { // otherwise read in values from Steering
	
	 //if(steeringFile->GetXOverlayMin() != MIN_EMPTY) {
         if( xmin != MIN_EMPTY) {
	   //xMinRatio = steeringFile->GetXOverlayMin();
	  xMinRatio = xmin;
	  if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Minimum to " << xMinRatio << std::endl;
	 }

	 //if(steeringFile->GetXOverlayMax() != MAX_EMPTY) {
         if( xmax!= MAX_EMPTY) {
	   //xMaxRatio = steeringFile->GetXOverlayMax();
          xMaxRatio = xmax;
	  if(debug) std::cout << cn << mn << "Forcing Overlay X Axis Maxmimum to " << xMaxRatio << std::endl;
	 }      
        }

        if (debug) {
	 std::cout<<cn<<mn<<"Drawing ratio frame with:"<<std::endl;
	 std::cout<<cn<<mn<<"yMaxRatio= "<<yMaxRatio<<std::endl;
	 std::cout<<cn<<mn<<"yMinRatio= "<<yMinRatio<<std::endl;
	 std::cout<<cn<<mn<<"xMaxRatio= "<<xMaxRatio<<std::endl;
	 std::cout<<cn<<mn<<"xMinRatio= "<<xMinRatio<<std::endl;
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

  	 if(pc.IsXLog()) {     
	   //xAxisOverlay->SetNdivisions(5);
	  xAxisRatio->SetMoreLogLabels(true);
         } 

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
	       TGraphAsymmErrors * graph=pdf->GetBand(iband);
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

void SPXPlot::PreparationDrawRatio(void) {
 std::string mn = "PrepationDrawRatio: ";
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

 return; 
}

void SPXPlot::DrawRatio(void) {
 std::string mn = "DrawRatio: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 //Do nothing if not drawing ratio
 SPXPlotConfiguration &pc = steeringFile->GetPlotConfiguration(id);
 SPXDisplayStyle &ds = pc.GetDisplayStyle();

 if(!ds.ContainsRatio()) {
  return;
 }

 //Counters for number of stat/tot ratios: Used for darkening stat/tot ratios and also
 // for sanity checking (data/stat graphs covering up other ratios)
 unsigned int statRatios = 0;
 unsigned int totRatios = 0;

 if (debug) std::cout<<cn<<mn<<"Number of ratio= "<< pc.GetNumberOfRatios()<<std::endl;

 for(int iratio = 0; iratio < pc.GetNumberOfRatios(); iratio++) {
  std::string ratioOptions;
  if(steeringFile->GetPlotBand()) 
   ratioOptions = "E2";

  if(steeringFile->GetPlotMarker() && !steeringFile->GetPlotBand())
   ratioOptions = "P";

  if(!steeringFile->GetPlotErrorTicks() && !steeringFile->GetPlotBand()) 
   ratioOptions += "Z";


 //Draw a line at 1, where ratios are relative to
  TLine *referenceLine = new TLine(xMinRatio, 1.0, xMaxRatio, 1.0);
  referenceLine->Draw();

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
  if (debug) {
   if (plotmarker) std::cout<<cn<<mn<<"plotmarker TRUE "<<std::endl;
   std::cout<<cn<<mn<<"Draw ratio "<<iratio<<" with options= "<<ratioOptions.c_str()<<std::endl;
  }

  ratios.at(iratio).Draw(ratioOptions.c_str(),statRatios, totRatios ,plotmarker);

 }

 return; 
}

void SPXPlot::DrawBox() {
 std::string mn = "DrawBox: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 double xbox    =steeringFile->GetXDataBoxLabel();
 double ybox    =steeringFile->GetYDataBoxLabel();

 for (int iratio=0; iratio<ratios.size(); iratio++) {
  //if (!ratios.at(iratio)){
  //throw SPXGeneralException(cn + mn + "SPXRatio not found !");
  //}  
  bool isdatastat=ratios.at(iratio).IsDataStat();
  bool isdatatot =ratios.at(iratio).IsDataTot();
  if (isdatastat || isdatatot) {
   int mcolor=ratios.at(iratio).GetDataGraphFillColor();
   if (isdatatot) mcolor++;
   if (debug) {
    std::cout<<cn<<mn<<"Draw with xbox= "<<xbox<<" ybox= "<<ybox<<std::endl;
    std::cout<<cn<<mn<<"isdatastat= "    <<isdatastat<<" isdatatot= "<<isdatatot<<std::endl;
    std::cout<<cn<<mn<<"iratio= "<<iratio<<"mcolor= "<<mcolor<<std::endl;
   }
   SPXDrawUtilities::DrawBox(xbox,ybox, mcolor,isdatatot, isdatastat);
  }
 }
 return; 
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

 bool dataonly=os.ContainsData() && !os.ContainsConvolute();

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
 if (debug&&dataonly)  std::cout << cn << mn <<"Is data only " << std::endl;

 int icontainsdata=0;
 int icontainsconvolute=0;
 int icontainsnominal=0;
 
 bool nodatawithmarker=false;
 for (int i = 0; i < pc.GetNumberOfRatios(); i++) {
  //
  SPXRatioStyle ratioStyle = pc.GetRatioStyle(i);
  if (debug) std::cout<<cn<<mn<<"ratio style= "<<ratioStyle.ToString()<<std::endl;

  TString name=TString(ratioStyle.ToString());
  if (name.Contains("data") &&!(name.Contains("data_tot") || name.Contains("data_stat") ||  name.Contains("convolute / data")) ) {
   if (debug) std::cout<<cn<<mn<<"ratio style contains data"<<std::endl;
   icontainsdata++;
  }

  if (TString(ratioStyle.ToString()).Contains("convolute")) {
   if (debug) std::cout<<cn<<mn<<"ratio style contains convolute"<<std::endl;
   icontainsconvolute++;
  }
  if (TString(ratioStyle.ToString()).Contains("nominal")) {
   if (debug) std::cout<<cn<<mn<<"ratio style contains nominal"<<std::endl;
   icontainsnominal++;
  }
 }

 if (icontainsdata==0) nodatawithmarker=true;
 if (debug) {
  if (nodatawithmarker) std::cout<<cn<<mn<<"ratio style contains NO data"<<std::endl;
 }

 // Look first, if properties of bands are different
 int old_fill_style=-999, old_fill_color=-999;
 int old_marker_style=-999, old_marker_color=-999;
 bool bandsdifferent=false;
 bool hadcorronly=false;
 bool bandcontainscorr=false;
 
 bool scalechoicedifferent=false;
 bool pdfsdifferent=false;

 std::string pdfold="";
 std::string scaleold="";

 debug=true;

 if (steeringFile->GetParameterScan())
  std::cout<<cn<<mn<<"Grid contain parameter scan= "<< std::endl;
 else if (debug) 
  std::cout<<cn<<mn<<"Grid does NOT contain parameter scan= "<< std::endl;

 for(int icross = 0; icross < crossSections.size(); icross++) {

  if (debug) {
   std::cout<<cn<<mn<<" "<< std::endl;
   std::cout<<cn<<mn<<"icross= "<<icross<<" Test band properties "<< std::endl;
  }

  SPXPDF * pdf=crossSections[icross].GetPDF();
  if (!pdf) throw SPXGeneralException(cn+mn+"PDF object not found !");

  SPXGrid * grid=crossSections[icross].GetGrid();
  if (!grid) throw SPXGeneralException(cn+mn+"Grid object not found !");

  if (pdfold!=pdf->GetPDFName()) {
   if (icross>0) pdfsdifferent=true;
   pdfold=pdf->GetPDFName();
  }
  //std::cout<<cn<<mn<<"pdfold= "<<scaleold<<" from Grid "<< pdf->GetPDFName() << std::endl;

  if (scaleold!=grid->GetScaleFunctionalForm()) {
   if (icross>0) scalechoicedifferent=true;
   scaleold=grid->GetScaleFunctionalForm();
  }
  //std::cout<<cn<<mn<<"scaleold= "<< scaleold <<" from Grid "<< grid->GetScaleFunctionalForm() << std::endl;

  bool bscale  =pdf->HasBandofType("_scale_");
  bool bAlternativeScaleChoice  =pdf->HasBandofType("_AlternativeScaleChoice_");
  bool bpdf    =pdf->HasBandofType("_pdf_");
  bool balphas =pdf->HasBandofType("_alphas_");
  bool bbeam   =pdf->HasBandofType("_BeamUncertainty_");
  bool bcorr=pdf->HasBandofType("_corrections_");
  bandcontainscorr=bcorr;
  // if (bcorr && !(bscale&&bpdf&&balphas&&bAlternativeScaleChoice)) hadcorronly=true;
  if (bcorr && !(bscale||bpdf||balphas||bAlternativeScaleChoice)) hadcorronly=true;

  bandsdifferent=pdf->BandsHaveDifferentProperties();
  if (debug) {

   if (bandsdifferent) std::cout<<cn<<mn<<"icross= "<<icross<<" Bands have different properties !"<< std::endl;
   else                std::cout<<cn<<mn<<"icross= "<<icross<<" Bands have same properties ! "<< std::endl;

   if (bAlternativeScaleChoice) std::cout<<cn<<mn<<"Bands contains AlternativeScaleChoice uncertainty"<< std::endl;
   if (bscale)     std::cout<<cn<<mn<<"Bands contains scale uncertainty"<< std::endl;
   if (bpdf)       std::cout<<cn<<mn<<"Bands contains PDF uncertainty"<< std::endl;
   if (balphas)    std::cout<<cn<<mn<<"Bands contains AlphaS uncertainty"<< std::endl;
   if (bbeam)      std::cout<<cn<<mn<<"Bands contains Beam uncertainty"<< std::endl;
   if (bcorr)   std::cout<<cn<<mn<<"Bands contains corrections uncertainty"<< std::endl;
   if (hadcorronly)std::cout<<cn<<mn<<"Bands contains ONLY corrections uncertainty"<< std::endl;
  }

  if (bandsdifferent) std::cout<<cn<<mn<<"One Cross section with Bands with have different properties !"<< std::endl;
  else                std::cout<<cn<<mn<<"All cross section have bands with have same properties ! "<< std::endl;
 }

 if (debug) std::cout<<cn<<mn<<" "<< std::endl;

 if (debug) {

  if (scalechoicedifferent) {
   std::cout<<cn<<mn<<"Scale choice in cross sections is different !"<< std::endl;
   for(int icross = 0; icross < crossSections.size(); icross++) {
    SPXGrid * grid=crossSections[icross].GetGrid();
    std::cout<<" icross= "<<icross<<" "<<grid->GetGridName()<<" scaleform= "<<grid->GetScaleFunctionalForm()<<std::endl;
   }
  } else std::cout<<cn<<mn<<"Scale choice is the same in all cross sections !"<< std::endl;

  if (pdfsdifferent) std::cout<<cn<<mn<<"Different PDF in cross sections !"<< std::endl;
  else               std::cout<<cn<<mn<<"Same PDF in cross sections !"<< std::endl;

 }
 //
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
  if (debug) std::cout << cn << mn <<"Overlay style contains data "<< std::endl;

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
      if (debug) std::cout<<cn<<mn<<"idata= "<<idata<<" beam uncertainty sqrt changed: sqrtsval= "<<sqrtsval<<" sqrtsvalold= "<<sqrtsvalold<<std::endl;
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
 //if (steeringFile->GetScaleFunctionalFormLabel() )  linesize+=0.02;


 leg = new TLegend();
 leg->SetBorderSize(0);
 leg->SetFillColor(0);
 //leg->SetFillColorAlpha(kYellow,0.);
 leg->SetFillStyle(0);
 leg->SetMargin(0.2);

 // Now analyse ratio

 //if (ratioonly && os.ContainsData() && !os.ContainsConvolute()) { 
 if (ratioonly && dataonly) { 
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

     TString opt="F";
     if (ratiographsordered.at(igraph)->GetFillStyle()==0) {
      opt="L";
     } else if (ratiographsordered.at(igraph)->GetLineColor()>0)  {
      opt="F";      
     } 

     if (ratiographsordered.at(igraph)->GetLineColor()<0){
      SPXGraphUtilities::SetColors(ratiographsordered.at(igraph),-ratiographsordered.at(igraph)->GetLineColor());
     } 

     if (gname.Contains("total")) {
      std::cout<<"Total uncertainty"<<std::endl;
      gname.ReplaceAll("total","Total");
      //continue;
     };

     //if (debug) SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)ratiographsordered.at(igraph));      

     if (debug) std::cout<<cn<<mn<<"Add in legend gname= "<<gname.Data()
                         <<" linecolor= "<<ratiographsordered.at(igraph)->GetLineColor()
                         <<" fillcolor= "<<ratiographsordered.at(igraph)->GetFillColor()
                         <<" size= "<<TString(gname).Sizeof()<<" opt= "<<opt.Data()<<std::endl;
     
     leg->AddEntry(ratiographsordered.at(igraph), gname,opt);
    }
    if (debug) {
     std::cout<<cn<<mn<<"Final legend "<<std::endl;
     leg->Print(); 
    }
   }
  }
 }

 // if (debug) std::cout<<cn<<mn<<"A namesize= "<<namesize<<std::endl;

 if (debug&&onlysyst) {
  std::cout<<cn<<mn<<" "<< std::endl;
  std::cout<<cn<<mn<<"Only systematics to legend, data label in information legend "<< std::endl;
 }

 std::vector <TString> vdatalabel;

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

   if (steeringFile->GetLabelDate()) {
    datalabel+=" ";
    datalabel+=data.at(idata)->GetDate();
   }

   vdatalabel.push_back(datalabel);

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
     //datalabel+=" ";
     //double binmin = data.at(idata)->GetDoubleBinValueMin();
     //double binmax = data.at(idata)->GetDoubleBinValueMax();
     //TString varname=data.at(idata)->GetDoubleBinVariableName();
     //datalabel+=this->FormatetabinLabel(varname, binmin,binmax);
     datalabel+=GetEtaLabel(data.at(idata));
    }

    if (differentlumi){
     datalabel+=" L= ";
     datalabel+= data.at(idata)-> GetDatasetLumi();
    }

    if (steeringFile->GetAddJournalLabel()) {
     datalabel+=" "+data.at(idata)->GetJournalLegendLabel();
     if (debug) std::cout<<cn<<mn<<"Add journal label datalabel  "<<datalabel.Data() <<std::endl;
    }

    if (steeringFile->GetAddJournalYear()) {
     if (debug) std::cout<<cn<<mn<<"Add journal year= "<<data.at(idata)->GetJournalYear()<<std::endl;
     datalabel+=" "+data.at(idata)->GetJournalYear();
     //datalabel=" #splitline{"+datalabel+"}{";
     //datalabel+=data.at(idata)->GetJournalYear();
     //datalabel+="}";
    }

    //if (!ratioonly && data.size()>0) { // ratioonly figures have data in the ratio, no separate label
    // do not plot data label when there is one band with different properties
    if (data.size()>0&&!bandsdifferent && !(dataonly&&ratioonly) &&!nodatawithmarker) { 
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
    if (!nodatawithmarker) {
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
       //double binmin = data.at(idata)->GetDoubleBinValueMin();
       //double binmax = data.at(idata)->GetDoubleBinValueMax();
       //TString varname=data.at(idata)->GetDoubleBinVariableName();
       //TString datalabel;
       //if (binmin!=0) {
       //datalabel.Form(" %3.2f ",binmin); 
       //} else {
       //varname.ReplaceAll("#leq","");
       //varname.ReplaceAll("","");
       // }
       //datalabel+=varname;
       //datalabel+=Form(" %3.2f ",binmax);
	datalabel=GetEtaLabel(data.at(idata));

       SPXPlotConfigurationInstance mypci=pc.GetPlotConfigurationInstance(idata);
       double yScale = mypci.yScale;
       if (debug) std::cout<<cn<<mn<<" etascan= "<< (etascan ? "ON" : "OFF")<<" yScale= "<<yScale<<std::endl;
       if (etascan&&yScale!=1) {
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
    } else {
     if (debug) std::cout<<cn<<mn<<"INFO etascan= "<< (etascan ? "ON" : "OFF")<<", but no data-set "<<std::endl;
    }
   }
  }
 }

 //if (debug) leg->Print();

 //if (debug) std::cout<<cn<<mn<<"C namesize= "<<namesize<<std::endl;
 
 if (ratioonly&&dataonly&&!onlysyst) {

  //std::vector <TString> vratiolabel;
  //std::vector <TGraphAsymmErrors> vratiograph;
  for (int i = 0; i < pc.GetNumberOfRatios(); i++) {
   //
   SPXRatioStyle ratioStyle = pc.GetRatioStyle(i);
   if (debug) std::cout<<cn<<mn<<"ratio style= "<<ratioStyle.ToString()<<std::endl;
   //
   SPXRatio ratioInstance = ratios.at(i);
   TString datadir=TString(ratioInstance.GetDataDirectory());
   //
   
   std::string num=ratioInstance.GetNumeratorDataLabel();
   std::string den=ratioInstance.GetDenominatorDataLabel();

   if (debug) {
    std::cout<<cn<<mn<<i<<" num= "<<num<<std::endl;   
    std::cout<<cn<<mn<<i<<" den= "<<den<<std::endl;   
   }

   TString ratiolabel = Form("%s / %s ",num.c_str(),den.c_str());

   if (debug) {
    std::cout<<cn<<mn<<i<<" num.BeginsWith= "<<TString(num).BeginsWith(datadir)<<std::endl;   
    std::cout<<cn<<mn<<i<<" den.BeginsWith= "<<TString(den).BeginsWith(datadir)<<std::endl;   
   }

   if (TString(num).BeginsWith(datadir) && TString(den).BeginsWith(datadir)) 
    ratiolabel.ReplaceAll(datadir,"");
   //ratiolabel.ReplaceAll(".txt","");

   //vratiolabel.push_back(ratiolabel);
   //vratiograph.push_back(ratioInstance.GetNumeratorGraph());
   if (debug) {
    std::cout<<cn<<mn<<i<<" datadir= "<<datadir<<std::endl;
    std::cout<<cn<<mn<<i<<" ratio= "<<ratiolabel<<std::endl;
    std::cout<<cn<<mn<<"graphname= "<<ratioInstance.GetDenominatorGraph()->GetName()<<std::endl;
   }

   leg->AddEntry(ratioInstance.GetDenominatorGraph(),ratiolabel, "P");

  }
 }

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
   if (!pdf) {std::cout<<cn<<mn<<"PDF not found ! "<<std::endl; continue;}

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
    if (gtype.compare(std::string("AlternativeScaleChoice"))==0)  nlouncertainty=true;
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

   //TString labelnlo="NLO QCD Uncertainties";
   TString labelnlo="NLO QCD";
   if(steeringFile->ApplyGridCorr() && !(bandsdifferent&&bandcontainscorr)) {
    labelnlo+=" #otimes ";
    labelnlo+=GetCorrectionLabel(crossSections[icross]);
   }

   if (!bandsdifferent) {
    if (nlolabel) {
     //TString text="";
     if (icross==0&&pdfsdifferent) { //only Draw text for the first cross section
       //text+="NLO QCD";
       //if(steeringFile->ApplyGridCorr()) {
       // text+=" #otimes ";
       // text+=GetCorrectionLabel(crossSections[icross]);
       //}
      labelnlo+=" with:";
      if (gridcorrectionfound&&!nlouncertainty) labelnlo="";
      if (debug) std::cout << cn << mn <<"Add label to legend: "<<labelnlo.Data()<< " namesize= "<<namesize<<std::endl;
      if (nlouncertainty) leg->AddEntry((TObject*)0, labelnlo, "");
     }
    }
   } else {
    if (nlouncertainty) {
     TString label="NLO QCD Uncertainties";
     if (labelnlo.Sizeof()>namesize) namesize=labelnlo.Sizeof();
     if (debug) std::cout << cn << mn <<"Add label to legend: "<<labelnlo.Data()<< " namesize= "<<namesize<<std::endl;
 
     leg->AddEntry((TObject*)0,labelnlo, "");
    }
    if (gridcorrectionfound&&!nlouncertainty)
     leg->AddEntry((TObject*)0,"Uncertainties", "");
   }

   if (bandsdifferent) { // uncertainty bands have different properties
    for (int iband=0; iband<nbands; iband++) {
     TGraphAsymmErrors * gband   =pdf->GetBand(iband);
     std::string         gtype   =pdf->GetBandType(iband);

     if (debug) {
      std::cout << cn << mn <<" "<< std::endl;
      std::cout << cn << mn <<"Different properties iband= "<<iband<<" gtype= "<< gtype.c_str()<< std::endl;
     }

     TString label="";
     TString bandtype=TString(gtype);

     if (debug) std::cout<<cn<<mn<<"bandtype= "<<bandtype.Data()<<std::endl;

     bandtype.ToUpper();
     if (bandtype.Contains("ALPHAS"))          bandtype="#alpha_{s}";
     if (bandtype.Contains("BEAMUNCERTAINTY")) bandtype="E_{beam}";
     if (bandtype.Contains("ALTERNATIVESCALECHOICE")) bandtype="Scale choice";
     else if (bandtype.Contains("SCALE"))             bandtype="Scale variation";
     if (bandtype.Contains("TOTAL"))  bandtype="Total";
     if (gtype.compare(std::string("pdf"))==0)
      label=pdftype+" "+bandtype;
     else
      label=bandtype;

     if (TString(gband->GetName()).Contains("corrections")) {
      label="Corrections ";
      label+=GetCorrectionLabel(crossSections[icross]);
      std::cout<<cn<<mn<<"corrections found label= "<<label<<std::endl;
     } 

     if (debug) std::cout<<cn<<mn<<"label= "<<label.Data()<<std::endl;

     if (steeringFile->GetPlotMarker()) { 
      if (debug) std::cout<<cn<<mn<<"add in legend iband= "<<iband<<" gband= "<<gband->GetName()
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
      if(gname.Contains("_AlternativeScaleChoice_")) edgecolor=pci.AlternativeScaleChoiceEdgeColor;
      if(gname.Contains("_pdf_"))    edgecolor=pci.pdfEdgeColor;
      if(gname.Contains("BeamUncertainty")) edgecolor=pci.beamuncertaintyEdgeColor;
      if(gname.Contains("_total_"))  edgecolor=pci.totalEdgeColor;
      if(gname.Contains("_corrections_")) edgecolor=pci.correctionsEdgeColor;

      //std::cout<<cn<<mn<<gname.Data()<<" edgecolor= "<<edgecolor<<std::endl;      
      if (edgecolor<0) {
       opt="L";
       TString hname="";
       hname+=gband->GetName();
       //hname.ReplaceAll(".txt","");
       hname+="LowEdge";
       gDirectory->ls();
       TH1D *hedge=(TH1D*)gDirectory->Get(hname);
       // need to use key since name has /
       //TKey *key = gDirectory->GetKey(hname);
       //if (!key) {
       //std::cout<<cn<<mn<<"WARNING key not found "<<hname.Data()<<std::endl;
       // std::cerr<<cn<<mn<<"WARNING key not found "<<hname.Data()<<std::endl;
       //}
       //TH1D* hedge = (TH1D*)key->ReadObjectAny(TH1D::Class());
       if (hedge) {
	if (debug) std::cout<<cn<<mn<<"hedge line= "<<hedge->GetLineWidth()<<std::endl;      
        leg->SetLineWidth(hedge->GetLineWidth());
        if (label.Sizeof()>namesize) namesize=label.Sizeof();
        if (debug) std::cout<<cn<<mn<<"Add label to legend: "<<label.Data()<<" opt= "<<opt<<" namesize= "<<namesize<<std::endl;
        leg->AddEntry(hedge, label, TString(opt));
       } else {
        if (label.Sizeof()>namesize) namesize=label.Sizeof();
	if (debug) std::cout<<cn<<mn<<"hedge= "<<hname<<" not found. Fill graph instead "<<std::endl;      
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

    //if (!pdfsdifferent&&!ratioonly) {
    if (!pdfsdifferent) {
     if (debug) std::cout<<cn<<mn<<"PDFs are not different !"<< std::endl;
     TString opt="";
     if (steeringFile->GetPlotMarker()) opt="P";
     if (steeringFile->GetPlotBand()) opt="LF";
     TString label="NLO QCD";
     if(steeringFile->ApplyGridCorr()) {
      label+=" #otimes ";
      label+=GetCorrectionLabel(crossSections[icross]);
     }
     //if (debug) std::cout<<cn<<mn<<"label= "<<label<< std::endl;
     TGraphAsymmErrors * gband=pdf->GetTotalBand();
     if (!gband) {
      if (!pdf->GetDoTotal()) {
       std::cout<<cn<<mn<<"Can not find TotalBand, since do_total swithced off in SPXPDF ! "<<std::endl;
       throw SPXGeneralException(cn+mn+"Switch on band_total option in steering !"); 
      } else {
       std::cout<<cn<<mn<<"ERROR gband not found ! "<<std::endl;
       throw SPXGeneralException(cn+mn+"gband not found !"); 
      }
     }
     if (icross==0) {
      if (debug) std::cout<<cn<<mn<<"icross=0 "<< std::endl;
      label+=" with ";

      label+=pdf->GetPDFtype();
      if (debug) std::cout<<cn<<mn<<"label= "<< label << std::endl;
       if (steeringFile->GetLabelChi2() && !pdfsdifferent &&!steeringFile->GetParameterScan() && !scalechoicedifferent) {
        if (debug) std::cout<<cn<<mn<<"Add Chi2 to label icross= "<<icross<<std::endl;
        if (icross>=data.size()) {
         std::cout<<cn<<mn<<"WARNING: Something is wrong Number of cross-sections= "<<data.size()<<" but idata= "<<icross<<std::endl; 
         throw SPXGeneralException(cn+mn+"Numer of cross-sections and data do not match !"); 
        }
        double chi2= SPXChi2::CalculateSimpleChi2(pdf, data.at(icross));
        if (debug) std::cout<<cn<<mn<<"Chi2= "<<chi2<<" pdf= "<<pdf->GetPDFtype()<<" Data= "<< data.at(icross)->GetTotalErrorGraph()->GetName()<<std::endl;   
         int ndf=data.at(icross)->GetTotalErrorGraph()->GetN();
         //if (data.at(icross)->IsNormalized()) n-=1; //needs to be implemented in data
         label+=Form(" #chi^{2}=%3.1f/%d",chi2,ndf);  
       }

       if (nlouncertainty) {
        if (debug) std::cout<<cn<<mn<<"nlouncertainty " << std::endl;
        if (label.Sizeof()>namesize) namesize=label.Sizeof();
        if (debug) std::cout<<cn<<mn<<"Add in legend nlouncertainty gband= "<<gband->GetName()<<" namesize= "<<namesize<<std::endl;
        if (scalechoicedifferent || steeringFile->GetParameterScan()) {
	 if (debug) std::cout<<cn<<mn<<"empty option "<<""<<std::endl;
	 leg->AddEntry((TObject*)0, label, "");
        } else { 
	 if (ratioonly&&icross==0) opt.ReplaceAll("P","");
	 if (debug) std::cout<<cn<<mn<<"label= "<<label.Data()<<" opt= "<<opt.Data()<<std::endl;
         leg->AddEntry(gband, label, opt);
        }
       }
     } 
     if (scalechoicedifferent) {
      SPXGrid * grid=crossSections[icross].GetGrid();
      TString label="#mu_{R} = #mu_{F}=";
      label+=grid->GetScaleFunctionalForm();
      if (nlouncertainty) {
       if (label.Sizeof()>namesize) namesize=label.Sizeof();
       if (debug) std::cout<<cn<<mn<<"Add in legend scalechoicedifferent gband= "<<gband->GetName()<<" namesize= "<<namesize<<std::endl;
       leg->AddEntry(gband,label, opt);
      }
     }
    } else {

     for (int iband=0; iband<nbands; iband++) {
      TGraphAsymmErrors * gband   =pdf->GetBand(iband);
      std::string         gtype   =pdf->GetBandType(iband);

      if (debug) std::cout<<cn<<mn<<"iband= "<<iband<<" gtype= "<< gtype.c_str()<< std::endl;
     
      if (gtype.compare(std::string("pdf"))==0){
       if (debug) std::cout<<cn<<mn<<"Band is of type PDF"<< std::endl;
       pdffound=true; npdf++;
       if (nlolabel) {
   
        double Escale = pdf->GetChangeSqrtS();
        // check if CMS energy in grid was changed "<<endl;
        std::cout<<cn<<mn<<"Escale= "<<Escale<<std::endl;

        TString labelname=pdftype;
        //int pdfcount=std::count (vpdf.begin(), vpdf.end(), pdftype);

        if (Escale!=1) {
         //double sqrtsval = data.at(0)->GetSqrtS();
         double sqrtsval=1.0;
	 labelname+=Form(" f*#sqrt{s} with f= %3.2f",sqrtsval*Escale);
        }

        int labelcount=std::count (vlabel.begin(), vlabel.end(), labelname);
        if (debug) std::cout<<cn<<mn<<"icross= "<<icross<<" label= "<<labelname.Data()<<" labelcount= "<<labelcount<<std::endl;

        // only plot label once 
        if (labelcount<1) {
         TString opt="";
         if (steeringFile->GetPlotMarker()) {
          opt="PE";
          if (ratioStyle.IsDataOverConvolute()) opt=""; // for Data over convolute do not plot marker
         } else if (steeringFile->GetPlotBand()) {
          opt="LF";
         } else std::cout << cn << mn <<"WARNING: do not know what to do not plotMarker, not plotBand"<< std::endl;

         if (!(steeringFile->GetParameterScan() && ratioonly)) { 
	  if (debug) std::cout<<cn<<mn<<"Add in legend iband= "<<iband
	 		     <<" gband= "<<gband->GetName()<<" labelcount= "<<labelcount<<" opt= "<<opt.Data()<<std::endl;

          if (steeringFile->GetLabelChi2()) {
           if (debug) std::cout<<cn<<mn<<"Add Chi2 to label icross= "<<icross<<std::endl;
           if (icross>=data.size()) {
            std::cout<<cn<<mn<<"WARNING: Something is wrong NUmber of crosssection "<<data.size()<<" but idata= "<<icross<<std::endl; 
            throw SPXGeneralException(cn+mn+"CrossSection and data do not match !"); 
           }
           double chi2= SPXChi2::CalculateSimpleChi2(pdf, data.at(icross));
           if (debug) std::cout<<cn<<mn<<"Chi2= "<<chi2<<" pdf= "<<pdf->GetPDFName()<<" Data= "<< data.at(icross)->GetTotalErrorGraph()->GetName()<<std::endl;     
         
           int ndf=data.at(icross)->GetTotalErrorGraph()->GetN();
           //if (data.at(icross)->IsNormalized()) n-=1; //needs to be implemented in data
           labelname+=Form(" #chi^{2}=%3.1f/%d",chi2,ndf);
          }

   	  if (debug) std::cout<<cn<<mn<<"Add legend label= "<<labelname.Data()<<std::endl;      
          if (labelname.Sizeof()>namesize) namesize=labelname.Sizeof();
          leg->AddEntry(gband, labelname, opt);
          vlabel.push_back(labelname);
         }
        }
       }
      }

      if (nbands==1) { // band has same properties because there is only one uncertainty
       if (debug) std::cout<<cn<<mn<<"icross= "<<icross<<" nbands==1"<<std::endl;
       if (!etascan) {
        if (vlabel.size()!=1) { 
  	 if (debug) std::cout<<cn<<mn<<"Add legend gband= "<<gband->GetName()<<" gtype= "<<gtype.c_str()<<std::endl;
	 std::string text=GetCorrectionLabel(crossSections[icross]);
         leg->AddEntry(gband, TString(text), "LF");
        }
       } else {

	std::cout<<cn<<mn<<"etascan is ON "<<std::endl;
	TString label="";
	//if (TString(gtype).Contains("ew")) label="ew correction";
        //if (icross==0) leg->AddEntry((TObject*)0,label, "");        
	if (debug) std::cout<<cn<<mn<<"Setting line width to "<<gband->GetLineWidth()<<std::endl;
        leg->SetLineWidth(gband->GetLineWidth());
        label=GetEtaLabel(data.at(icross)); 
        TString opt="LF"; if (TString(gtype).Contains("ew")) opt="L";
        if (label.Sizeof()>namesize) namesize=label.Sizeof();
        leg->AddEntry(gband, label, opt);        
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

   if (steeringFile->GetParameterScan()) {
    if (debug) std::cout<<cn<<mn<<"Parameter scan "<<std::endl;
    SPXGrid * grid=crossSections[icross].GetGrid();
    if (!grid)  { std::cout<<cn<<mn<<"grid not found ! "<<std::endl; return;}

    // start with empty line such that NLO QCD with PDF label is one single line
    leg->AddEntry((TObject*)0, "", "");
    TGraphAsymmErrors * gband   =pdf->GetTotalBand();
    if (!gband) { std::cout<<cn<<mn<<"gband not found ! "<<std::endl; return;}

    TString opt="";
    if (steeringFile->GetPlotMarker()) opt="P";
    if (steeringFile->GetPlotBand()) opt="LF";

    double par=grid->GetParameterValue();
    if (debug) std::cout<<cn<<mn<<"icross= "<<icross<<" parameter found par= "<<par<<std::endl; 
    TString parname=TString(grid->GetParameterName());
    TString parunit=TString(grid->GetParameterUnit());
    TString label=parname;
    TString lval=Form("= %3.1f ",par);
    label+=lval;  
    label+=parunit;
    
    if (debug) std::cout<<cn<<mn<<"label= "<<label<<std::endl; 

    if (data.size()>1) std::cout<<cn<<mn<<"WARNING for parameter scan Number of data should be 1 ! data.size()= "<<data.size()<<std::endl;      
    double chi2= SPXChi2::CalculateSimpleChi2(pdf, data.at(0)); //parameter scan has only one data set
    if (debug) std::cout<<cn<<mn<<"Chi2= "<<chi2<<" pdf= "<<pdf->GetPDFtype()<<" Data= "<< data.at(0)->GetTotalErrorGraph()->GetName()<<std::endl;   
    int ndf=data.at(0)->GetTotalErrorGraph()->GetN();
    label+=Form(" #chi^{2}=%3.1f/%d",chi2,ndf);  

    if (label.Sizeof()>namesize) namesize=label.Sizeof();
    if (debug) std::cout<<cn<<mn<<"Add in legend gband= "<<gband->GetName()<<" label= "<<label.Data()<<" namesize= "<<namesize<<std::endl;
    leg->AddEntry(gband, TString(label), opt);
   } else if (debug) std::cout<<cn<<mn<<"Parameter scan is OFF "<<std::endl;

  }
  gPad->RedrawAxis();
 }


 double x1=0., y1=0., x2=0., y2=0.;
 double xlegend=steeringFile->GetXLegend();
 double ylegend=steeringFile->GetYLegend();

 if (pc.CheckPlotOptions("x_legend")){
  xlegend=pc.GetPlotOptions("x_legend");
  if (debug) std::cout<<cn<<mn<<"x_legend= "<<xlegend<<std::endl;
 } 
 if (pc.CheckPlotOptions("y_legend")){
  ylegend=pc.GetPlotOptions("y_legend");
  if (debug) std::cout<<cn<<mn<<"y_legend= "<<ylegend<<std::endl;
 } 

 //leg->Print();
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

 double fac=0.50, xfac=1.0;
 //if (nraw>7) {fac*=1.0; xfac*=3.0;}
 if (nraw>7&&!onlysyst) {fac*=1.0; xfac*=3.5;}
 if (debug) std::cout<<cn<<mn<<"fac= "<<fac<<" xfac= "<<xfac<<std::endl;

 /*
 if (nraw>5)  {
  if (debug) std::cout<<cn<<mn<<"set Legend to two columns "<<std::endl;
  leg->SetNColumns(2);
  leg->SetFillColor(5);
  fac*=1.0; 
  if (namesize>30) xfac*=1.5;
  //csize =charactersize*0.5; 
 }
 */
 //leg->SetEntrySeparation(0.1);
 leg->SetTextSize(csize); 

 if (onlysyst) {
  if (nraw>5)  leg->SetNColumns(2);
  if (nraw>8)  leg->SetNColumns(3);
  if (nraw>12) leg->SetNColumns(4);
  if (debug) std::cout<<cn<<mn<<"Legend now has "<<leg->GetNColumns()<<" columns "<<std::endl;
 }

 if (onlysyst) {
  if (leg->GetNColumns()<3)      {fac*=1.0; xfac*=1.5;}
  else if (leg->GetNColumns()>2) {fac*=0.9; xfac*=1.4;}
  else if (leg->GetNColumns()>3) {fac*=0.9; xfac*=1.4;}
 }

 double bwidth=fac*namesize*charactersize; 

 x1 = xlegend-bwidth; x2=xfac*xlegend;
 double xmin=0.18;

 if (debug) {
  std::cout<<cn<<mn<<"Number of Legend columns= "<<leg->GetNColumns()<<std::endl;
  std::cout<<cn<<mn<<"fac= "<<fac<<" xfac= "<<xfac<<std::endl;
  std::cout<<cn<<mn<<"x1= "<<x1<<" x2= "<<x2<<std::endl;
 }

 if (x1<xmin) {x1=xmin; x2+=xmin;}

 double lsize=nraw;

 //if (debug) std::cout<<cn<<mn<<"lsize= "<<lsize<<" nraw= "<<nraw<<std::endl;

 if (nraw>4)  lsize*=0.6;
 if (debug) std::cout<<cn<<mn<<"lsize= "<<lsize<<" nraw= "<<nraw<<std::endl;

 if (onlysyst) {
  if (leg->GetNColumns()==1)  {lsize*=1.5;}
  if (leg->GetNRows()>10)     {lsize*=0.15;}
  else if (leg->GetNRows()>6) {lsize*=0.2;}
  else if (leg->GetNRows()>4) {lsize*=0.3;}
  else if (leg->GetNRows()>3) {lsize*=0.4;}
 } 

 if (debug) std::cout<<cn<<mn<<"lsize= "<<lsize<<" linesize= "<<linesize<<std::endl;

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

 leg->SetFillColor(5);

 //if (debug) leg->Print();
 leg->Draw();

 // Now build second Legend with info

 leginfo = new TLegend();
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
  if (steeringFile->GetParameterScan() && idata>0) break;
  //TString infolabel = "#font[9]{";
  TString infolabel = "";

  if (onlysyst) { // for systematic only move data label to leginfo
   TString datalabel=data.at(idata)->GetLegendLabel();
   leginfo->AddEntry((TObject*)0, datalabel, "");
  }

  //if (!etascan&&steeringFile->GetInfoLegendLabel().size()>0) {
  if (steeringFile->GetInfoLegendLabel().size()>0) {
   if (idata==0) { // Draw info label only once
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
    if (!differentlumi) { // data-set with different lumi go the leg not leginfo
     if (lumi!=lumiold) {
      lumiold=lumi; 
      if (debug) std::cout<<cn<<mn<<"Add to info legend dataset lumi= "<<lumi.c_str()<<std::endl;
      if (TString(lumi).Sizeof()>leginfomax) leginfomax=TString(lumi).Sizeof();
      TString label="L= ";
      label+=TString(lumi);
      leginfo->AddEntry((TObject*)0, label,"");
     }
    }
   }
  }

  if (onlysyst) {
   if (steeringFile->GetSystematicClasses().size()==0) {
    double xcut=steeringFile->ShowIndividualSystematics();
    TString mypercent="%";
    TString label="";
    if (xcut>0) {
     label="All syst>";
     int exp=0; double x=0.;
     SPXMathUtilities::frexp10(xcut, exp, x);
     std::cout<<" xcut= "<<xcut<<" exp= "<<exp<<" x= "<<x<<std::endl;
     if (exp>2 || exp<-2) {
      label+=SPXDrawUtilities::FormatwithExp(xcut);
      std::cout<<" xcut= "<<xcut<<" label= "<<label<<std::endl;
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

 for (int icross = 0; icross < crossSections.size(); icross++) {
  if (icross>0) {
   std::cout<<cn<<mn<<"WARNING Labels only implemented for one cross section icross= "<<icross<<std::endl;
   continue;
  }
  SPXGrid *grid=crossSections[icross].GetGrid();
  if (!grid) {std::cout<<cn<<mn<<"grid not found ! "<<std::endl; continue;}

  if (steeringFile->GetAddonLegendNLOProgramName() ) {
   if (debug) std::cout<<cn<<mn<<"Add NLO Program name to Legend "<<std::endl;      
   TString label=grid->GetNLOProgramNameName();
   if (TString(label).Sizeof()>leginfomax) leginfomax=TString(label).Sizeof();
   leginfo->AddEntry((TObject*)0, label,"");
  }       

  //if (!pdfsdifferent&&ratioonly) {
  // TString label="NLO QCD ";
  // label+==grid->GetPDFName();
  // if (label.Sizeof()>leginfomax) leginfomax=label.Sizeof();
  // leginfo->AddEntry((TObject*)0, label,"");
  //}

  if ((steeringFile->GetParameterScan() && ratioonly)) { 
   TString label="NLO QCD with ";
   SPXPDF * pdf=crossSections[icross].GetPDF();
   if (!grid) {std::cout<<cn<<mn<<"pdf not found ! "<<std::endl; continue;}
   //label+=pdf->GetPDFName();
   label+=pdf->GetPDFtype();
   if (label.Sizeof()>leginfomax) leginfomax=label.Sizeof();
   leginfo->AddEntry((TObject*)0, label,"");
  }

  if (steeringFile->GetScaleFunctionalFormLabel() ) {
   if (!scalechoicedifferent) {
    TString label="#mu_{R} = #mu_{F}=";
    label+=grid->GetScaleFunctionalForm();
    if (TString(label).Sizeof()>leginfomax) leginfomax=TString(label).Sizeof();
    leginfo->AddEntry((TObject*)0, label,"");
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

 if (pc.CheckPlotOptions("x_info_legend")){
  x2info=pc.GetPlotOptions("x_info_legend");
  if (debug) std::cout<<cn<<mn<<"x2info= "<<x2info<<std::endl;
 } 

 if (pc.CheckPlotOptions("y_info_legend")){
  y2info=pc.GetPlotOptions("y_info_legend");
  if (debug) std::cout<<cn<<mn<<"y2info= "<<y2info<<std::endl;
 } 

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

	std::string filename = GetPNGFilename(pc.GetDescription());

	//Create/get the PNG filename
        if (!steeringFile->GetOutputGraphicFormat().empty()) {
	 TString epsfilename=filename;
         //if (debug) std::cout<<cn<<mn<<" output format= "<< steeringFile->GetOutputGraphicFormat()<<std::endl;
         
         epsfilename.ReplaceAll("png",TString(steeringFile->GetOutputGraphicFormat()));
	 if (debug) std::cout<<cn<<mn<<"id= "<<id<<std::endl;
         TString text="_plot_";
         text+=id;
         epsfilename.ReplaceAll(text,"");

         if (debug) {
          std::cout<<cn<<mn<<"Print epsfilename= "<< epsfilename.Data()<<std::endl;
         }
         canvas->Print(epsfilename.Data());
        }

        if (steeringFile->GetOutputRootfile()) {
	 TString rootfilename=filename;
         TString text="_plot_";
         text+=id;
         rootfilename.ReplaceAll(text,"");
         rootfilename.ReplaceAll("png","root");
	 this->WriteRootFile(rootfilename);
        }

	//Print PNG File

	canvas->Print(filename.c_str());

        return;
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

 if (debug) std::cout<<cn<<mn<<"Number of ratio= "<<pc.GetNumberOfRatios()<<std::endl;

 for(int i = 0; i < pc.GetNumberOfRatios(); i++) {

  SPXRatioStyle ratioStyle = pc.GetRatioStyle(i);
  std::string ratioString  = pc.GetRatio(i);

  try {
   SPXRatio ratioInstance = SPXRatio(pc, ratioStyle);
   
   ratioInstance.AddDataFileGraphMap(dataFileGraphMap);
   ratioInstance.AddDataFileLabelMap(dataFileLabelMap);

   ratioInstance.AddReferenceFileGraphMap(referenceFileGraphMap);
   ratioInstance.AddNominalFileGraphMap(nominalFileGraphMap);
   ratioInstance.AddConvoluteFileGraphMap(convoluteFileGraphMap);
   ratioInstance.AddConvoluteFilePDFMap(convoluteFilePDFMap);
   ratioInstance.Parse(ratioString);

   bool matchbin=steeringFile->GetMatchBinning();
   if (debug) {
    if (matchbin)
     std::cout<<cn<<mn<<"Set Matchbinning to ON "<<std::endl;
    else
     std::cout<<cn<<mn<<"Set Matchbinning to OFF "<<std::endl;
   }

   ratioInstance.SetMatchBinning(matchbin);

   //if (steeringFile->ShowTotalSystematics()!=0) {
   // int icol=steeringFile->ShowTotalSystematics();
   // std::cout<<cn<<mn<<"Show total systematic uncertainty in ratio color= "<<icol<<std::endl;
   // ratioInstance.SetAddTotalSystematics(icol);
   //}

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
                //
		StringPair_T key = StringPair_T(keyname, pci.pdfSteeringFile.GetFilename());

		if(debug) std::cout << cn << mn << "Checking for prior existence of convolute with key = [" << key.first << \
			", " << key.second << "]" << std::endl;

		if(crossSectionSet.count(key) != 0) {
		  //	if(debug) std::cout << cn << mn << "Convolute with grid filename \"" << key.first << "\" and pdf filename \"" \
		  //		<< key.second << "\" has already been processed: Will not process duplicate" << std::endl;

		  //				continue;
                  std::cout<<cn<<mn<<"is ok to process duplicates -> to be checked "<<std::endl;
 
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
	       std::cout<<cn<<mn<<"Enter this grid with name= "<<newname.str()<<std::endl;
	       convolutePair = StringPair_T(newname.str(), pci.pdfSteeringFile.GetFilename());
              } 

              convoluteFilePDFMap.insert(StringPairPDFPair_T(convolutePair, pdf));

              int markerstyle=-99, fillcolor=-99,fillstyle=-99, edgecolor=-99, edgestyle=-99;
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
		//
		std::string theoryname=gband->GetName();
       
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

                if (gtype.compare(std::string("AlternativeScaleChoice"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.AlternativeScaleChoiceMarkerStyle;
                 fillcolor  =pci.AlternativeScaleChoiceFillColor;
                 fillstyle  =pci.AlternativeScaleChoiceFillStyle;
                 edgecolor  =pci.AlternativeScaleChoiceEdgeColor;
                 edgestyle  =pci.AlternativeScaleChoiceEdgeStyle;
                } 

                if (gtype.compare(std::string("alphas"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.alphasMarkerStyle;
                 fillcolor  =pci.alphasFillColor;
                 fillstyle  =pci.alphasFillStyle;
                 edgecolor  =pci.alphasEdgeColor;
                 edgestyle  =pci.alphasEdgeStyle;
                }

                if (gtype.compare(std::string("BeamUncertainty"))==0){
		 //if (debug) std::cout << cn << mn <<" matched "<< gtype.c_str() <<std::endl;
                 markerstyle=pci.beamuncertaintyMarkerStyle;
                 fillcolor  =pci.beamuncertaintyFillColor;
                 fillstyle  =pci.beamuncertaintyFillStyle;
                 edgecolor  =pci.beamuncertaintyEdgeColor;
                 edgestyle  =pci.beamuncertaintyEdgeStyle;
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
		 std::cout << cn << mn <<i<<" gband: "<< gband->GetName()<<" Setting: \n"
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

                  if (debug) SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)gband);

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

                if (debug) SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)nomGraph);
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

                        if (masterXUnits.empty()) {
			 if(debug) std::cout << cn << mn << "No master Xunits found. No data steering ? "<< std::endl;
                         std::cout << cn << mn << "WARNING: no data object found --> will not normalise cross-sections"<< std::endl;
                         //masterXUnits=slaveXUnits;
                         return;
                        }                        

                        if (masterYUnits.empty()) {
			 if(debug) std::cout << cn << mn << "No master Yunits found. No data steering ? "<< std::endl;
                         //masterYUnits=slaveYUnits;
                         std::cout << cn << mn << "WARNING: no data object found --> will not normalise cross-sections"<< std::endl;
                         return;
                        }                        
                     
                       	TGraphAsymmErrors * gNom = crossSections[i].GetNominal();
                        if (!gNom) std::cout << cn << mn << "gNom graph not found at index " << i << std::endl;

			TGraphAsymmErrors * gRef = crossSections[i].GetGridReference();
                        if (!gRef) std::cout << cn << mn << "gRef graph not found at index " << i << std::endl;

			//Determine the scale from the unit difference between data and grid
			double xScale = SPXGraphUtilities::GetXUnitsScale(masterXUnits, slaveXUnits);
			double yScale = SPXGraphUtilities::GetYUnitsScale(masterYUnits, slaveYUnits);

			if(debug) {
			 std::cout << cn << mn << "Scales determined from data/grid unit differential: " << std::endl;
                         if (!masterXUnits.empty())
			  std::cout << "\t Data X Units: " << masterXUnits << std::endl;
			 std::cout << "\t Grid X Units: " << slaveXUnits << std::endl << std::endl;
                         if (!masterXUnits.empty())
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

                        bool dataDividedByDoubleDiffBinWidth =  pci->dataSteeringFile.IsDividedByDoubleDiffBinWidth();
                        bool gridDividedByDoubleDiffBinWidth =  pci->gridSteeringFile.IsGridDividedByDoubleDiffBinWidth();
                        bool referenceDividedByDoubleDiffBinWidth= pci->gridSteeringFile.IsReferenceDividedByDoubleDiffBinWidth();

			if(debug) {
			 std::cout << cn << mn << "normalizeToTotalSigma is " << (normalizeToTotalSigma ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "dataDividedByBinWidth is " << (dataDividedByBinWidth ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "gridDividedByBinWidth is " << (gridDividedByBinWidth ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "referenceDividedByBinWidth is " << (referenceDividedByBinWidth ? "ON" : "OFF") << std::endl;

			 std::cout << cn << mn << "dataDividedByDoubleDiffBinWidth is " << (dataDividedByDoubleDiffBinWidth ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "gridDividedByDoubleDiffBinWidth is " << ( gridDividedByDoubleDiffBinWidth ? "ON" : "OFF") << std::endl;
			 std::cout << cn << mn << "referenceDividedByDoubleDiffBinWidth is " << ( referenceDividedByDoubleDiffBinWidth ? "ON" : "OFF") << std::endl;
			}

                        bool AlternativeGridChoicegridDividedByDoubleDiffBinWidth =  pci->gridSteeringFile.IsAlternativeGridChoiceGridDividedByDoubleDiffBinWidth();
                        bool  isAlternativeGridDividedByBinWidth =  pci->gridSteeringFile.IsAlternativeGridChoiceGridDividedByBinWidth();
                        if (isAlternativeGridDividedByBinWidth) {
			 if (debug) std::cout<<cn<<mn<<"AlternativeGridChoiceGridDividedByBinWidth is ON "<<std::endl;
			 //std::cerr<<cn<<mn<<"AlternativeGridChoiceGridDividedByBinWidth not yet implemented "<<std::endl;
                        }

			if(!dataDividedByBinWidth && gridDividedByBinWidth) {
			 throw SPXGraphException(cn + mn + "Grid IS divided by the bin with but the data IS NOT: Not supported");
			}

			if(!dataDividedByDoubleDiffBinWidth && gridDividedByDoubleDiffBinWidth) {
			 throw SPXGraphException(cn + mn + "Grid IS divided by the double diff variable bin with but the data IS NOT: Not supported");
			}

			if(!dataDividedByDoubleDiffBinWidth && AlternativeGridChoicegridDividedByDoubleDiffBinWidth) {
			 throw SPXGraphException(cn + mn + "AlternativeGridChoiceGrid IS divided by the double diff variable bin with but the data IS NOT: Not supported");
			}

			double totalSigmaNom = SPXGraphUtilities::GetTotalSigma(gNom, gridDividedByBinWidth);
			double totalSigmaRef = SPXGraphUtilities::GetTotalSigma(gRef, referenceDividedByBinWidth);
			if(debug) {
			  std::cout<<cn<<mn<< "Nominal Total Sigma = " << totalSigmaNom   << std::endl;
			  std::cout<<cn<<mn<< "Reference Total Sigma = "<< totalSigmaRef  << std::endl;
			}
                        //
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

                        double AlternativeGridChoiceyBinWidthScale=yBinWidthScale;

			if(dataDividedByDoubleDiffBinWidth && !AlternativeGridChoicegridDividedByDoubleDiffBinWidth) {
			 double dbinwidth= pci->dataSteeringFile.GetDoubleBinValueWidth();
			 if(debug) std::cout << cn << mn << "AlternativeGridChoice Dividing Cross Section by the Double Diff Bin Width" << dbinwidth<< std::endl;
			 AlternativeGridChoiceyBinWidthScale*=dbinwidth;
                        }

			if(dataDividedByDoubleDiffBinWidth && !gridDividedByDoubleDiffBinWidth) {
			 double dbinwidth= pci->dataSteeringFile.GetDoubleBinValueWidth();
			 if(debug) std::cout << cn << mn << "Dividing Cross Section by the Double Diff Bin Width" << dbinwidth<< std::endl;
			 yBinWidthScale*=dbinwidth;
                        }

			if(referenceDividedByDoubleDiffBinWidth) {
			 if (gridDividedByDoubleDiffBinWidth) {
                           std::ostringstream oss;
                           oss << cn <<mn<<"reference divided by double bin width -> not supported ";
                           throw SPXParseException(oss.str());
                         }
			}

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

        		  if (debug) std::cout << cn << mn << "Calculate total sigma  gridDividedByBinWidth= " << (gridDividedByBinWidth ? "ON" : "OFF") << std::endl;  
                          double totalSigma=SPXGraphUtilities::GetTotalSigma(gband, gridDividedByBinWidth);
        		  if (debug) std::cout << cn << mn << "for gband totalSigma= " << totalSigma << std::endl;  

                          if (TString(gband->GetName()).Contains("AlternativeGridChoice")) {
        		   if (debug) std::cout << cn << mn << "Normalise band " << gband->GetName() << std::endl;  
    			   if(dataDividedByBinWidth && !isAlternativeGridDividedByBinWidth) {

			   if(debug) std::cout << cn << mn << "AlternativeGridChoice Divide by binwidth "<< std::endl;
                            SPXGraphUtilities::DivideByBinWidth(gband);
                           }

			   if (debug) std::cout << cn << mn << "Scaling by 1 / Y Bin Width Scale: " << (1.0 / yBinWidthScale) << std::endl;
                           SPXGraphUtilities::Scale(gband, 1.0, (1.0 / AlternativeGridChoiceyBinWidthScale));
   			   if(normalizeToTotalSigma) {
			    if(totalSigma== 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma is zero");
			    if(debug) std::cout << cn << mn << "Scaling by 1 / total sigma: " << std::scientific << (1.0 / totalSigma) << std::endl;
			    SPXGraphUtilities::Scale(gband, 1.0, (1.0 / totalSigma));
                           }
			  } else { 
    			   if(dataDividedByBinWidth && !gridDividedByBinWidth) {

			    if(debug) std::cout << cn << mn << "Divide by binwidth "<< std::endl;
                            SPXGraphUtilities::DivideByBinWidth(gband);
                           }

			   if (debug) std::cout << cn << mn << "Scaling by 1 / Y Bin Width Scale: " << (1.0 / yBinWidthScale) << std::endl;
                           SPXGraphUtilities::Scale(gband, 1.0, (1.0 / yBinWidthScale));
   			   if(normalizeToTotalSigma) {
			    if(totalSigma== 0) throw SPXGeneralException(cn + mn + "Divide by zero error: Total Sigma is zero");
			    if(debug) std::cout << cn << mn << "Scaling by 1 / total sigma: " << std::scientific << (1.0 / totalSigma) << std::endl;
			    SPXGraphUtilities::Scale(gband, 1.0, (1.0 / totalSigma));
                            if (debug) {
			     std::cout << cn << mn << "Scaled cross-section " << gband->GetName() << std::endl;
                             gband->Print();
                            }
                           }
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

                if (key.empty()) {
 		 std::cout << cn << mn << "Key is empty, can not create data object" << std::endl;
                 return;
                }

		//Add data steering file to data set
		dataSet.insert(key);

		if(debug) std::cout << cn << mn << "Added data with key = [" << key << "] to dataSet" << std::endl;

                SPXData *dataInstance = new SPXData(pci);
                if (!dataInstance) throw SPXGeneralException(cn+mn+"Problem to create dataInstance");

                if (steeringFile->GetTakeSignforTotalError()) {
                 dataInstance->SetTakeSignforTotalError(true);
		} 

                if (steeringFile->GetAddMCStattoTotalStatError()>0) {
		 dataInstance->SetAddMCStattoTotalStatError(steeringFile->GetMCstatNametoAddtoTotal());
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
                
                // >GetSystematicClassesTo* now included in PlotConfiguration since otherwise one can only do one plot
		//std::vector<std::string> removesyst=steeringFile ->GetSystematicClassesToRemove();               
		//std::vector<std::string> containsyst=steeringFile->GetSystematicClassesToKeep();
                std::vector<std::string> removesyst=pci.removesystematicsclasses;
                std::vector<std::string> containsyst=pci.containsystematicsclasses;

                if (removesyst.size()>0 && containsyst.size()>0) {
                 std::ostringstream oss;
                 oss <<cn<<mn<<"You can either use option remove_systematic_group or contain_systematic_group but not both";
                 throw SPXParseException(oss.str());
                }

                if (removesyst.size()>0) {
                 if (debug) std::cout<<cn<<mn<<"Number of systematic classes to be removed nsyst="<<removesyst.size()<<std::endl;

                 for (int i=0; i<removesyst.size(); i++) {
		  if (debug) std::cout<<cn<<mn<<"Remove systematics that contain name="<<removesyst.at(i)<<std::endl;
		  dataInstance->RemoveSystematicthatContains(removesyst.at(i));
                 }
                }

                if (containsyst.size()>0) {
                 if (debug) std::cout<<cn<<mn<<"Number of systematic classes to keep nsyst="<<containsyst.size()<<std::endl;

                 for (int i=0; i<containsyst.size(); i++) {
		  if (debug) std::cout<<cn<<mn<<"Keep systematics that contain name="<<containsyst.at(i)<<std::endl;
		  dataInstance->KeepSystematicthatContains(containsyst.at(i));
                 }
                }

                dataInstance->CreateGraphs();

                if (debug) {
		 dataInstance->PrintGraphs();
                }

   		if(debug) std::cout << cn << mn << "Chi2 calculation method= " << steeringFile->GetCalculateChi2() << std::endl;

                if (steeringFile->GetCalculateChi2()>0) {
   		 if(debug) std::cout << cn << mn << "Read-in information on correlations among bins " << std::endl;
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
		std::string label=data[i]->GetLegendLabel();
		dataFileLabelMap[pci.dataSteeringFile.GetFilename()]= label;
                //dataFileLabelMap.insert(pci.dataSteeringFile.GetFilename(),label);

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
                   if (debug) std::cout<<cn<<mn<<systname.c_str()<<" emax= "<<emax<<std::endl;
		   if (fabs(emax)>steeringFile->ShowIndividualSystematics()) {
		    if (debug) {
		     std::cout<<cn<<mn<<"Add systematic "<<systname.c_str()<<" to dataFileGraphMap"<< std::endl;
                     //vsyst.at(isyst)->Print(); 
                     std::cout<<cn<<mn<<systname.c_str()<<" Largest systematics "<<emax<<"% above "<<steeringFile->ShowIndividualSystematics()<<"%"<<std::endl;
                    }
		    icountsyst++;

                    Color_t icol=SPXUtilities::ICol(icountsyst);

                    SPXPlot::SetSystGraphProperties( vsyst.at(isyst),icol);

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
		 std::vector<int>          systematicsgroupsedgecolor = steeringFile->GetSystematicClassesEdgeColor();
		 std::vector<int>          systematicsgroupsedgestyle = steeringFile->GetSystematicClassesEdgeStyle();
		 std::vector<int>          systematicsgroupsedgewidth = steeringFile->GetSystematicClassesEdgeWidth();

                 if (systematicsgroups.size()!=systematicsgroupscolor.size()) {
                  std::ostringstream oss;
                  oss<<cn<<mn<<"Systematics group vectors have not the same size fill color vsystgroups= "
                             <<systematicsgroups.size()<<" systematicsgroupscolor= "<<systematicsgroupscolor.size();
		  throw SPXGraphException(oss.str());
                 }

                 if (systematicsgroups.size()!=systematicsgroupsedgecolor.size()) {
                  std::ostringstream oss;
                  oss<<cn<<mn<<"Systematics group vectors have not the same size edge color vsystgroups= "
                             <<systematicsgroups.size()<<" systematicsedgescolor= "<<systematicsgroupsedgecolor.size();
		  throw SPXGraphException(oss.str());
                 }

                 if (systematicsgroups.size()!=systematicsgroupsedgestyle.size()) {
                  std::ostringstream oss;
                  oss<<cn<<mn<<"Systematics group vectors have not the same size edge style vsystgroups= "
                             <<systematicsgroups.size()<<" systematicsedgestyle= "<<systematicsgroupsedgestyle.size();
		  throw SPXGraphException(oss.str());
                 }

                 if (systematicsgroups.size()!=systematicsgroupsedgewidth.size()) {
                  std::ostringstream oss;
                  oss<<cn<<mn<<"Systematics group vectors have not the same size edge width vsystgroups= "
                             <<systematicsgroups.size()<<" systematicsedgestyle= "<<systematicsgroupsedgewidth.size();
		  throw SPXGraphException(oss.str());
                 }

                 if (debug) {
                  std::cout<<cn<<mn<<"Number of systematic groups= " <<systematicsgroups.size() <<std::endl;                  
                  //std::cout<<cn<<mn<<" " <<std::endl;                  
                  for (int igroup=0; igroup<systematicsgroups.size(); igroup++) {
                   std::cout<<cn<<mn<<igroup<<" systematic group= " <<systematicsgroups.at(igroup)
                                            <<" fill_color= "<< systematicsgroupscolor.at(igroup) 
                                            <<" edge_color= "<< systematicsgroupsedgecolor.at(igroup) 
                                            <<" edge_style= "<< systematicsgroupsedgestyle.at(igroup) 
                                            <<" edge_width= "<< systematicsgroupsedgewidth.at(igroup) 
                                            <<std::endl;
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

		    //if (debug) std::cout<<cn<<mn<<"Test if isyst= "<<isyst<<" contains "<<systematicsgroups.at(igroup)<<" igroup= "<<igroup<<" systname= "<<systname.Data()<<std::endl;
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
                     //SPXGraphUtilities::SetColors( vsystgroups.back(),icol);
                     SPXPlot::SetSystGraphProperties( vsystgroups.back(),icol);

                     Color_t ilinecolor=systematicsgroupsedgecolor.at(igroup);    
		     vsystgroups.back()->SetLineColor(ilinecolor);

                     Style_t ilinestyle=systematicsgroupsedgestyle.at(igroup);    
		     vsystgroups.back()->SetLineStyle(ilinestyle);

                     int ilinewidth=systematicsgroupsedgewidth.at(igroup);    
		     vsystgroups.back()->SetLineWidth(ilinewidth);

                     if (debug) {
		       std::cout<<cn<<mn<<sname<<" vector set to ilinecolor= "
                                <<ilinecolor<<" ilinestyle= "<<ilinestyle
                                <<" linewidth= "<<ilinewidth<<" icol= "<<icol<<std::endl;
                      SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)vsystgroups.back());
                     }

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
                   if (debug) std::cout<<cn<<mn<<"Check if in others Number of groups="<<systematicsgroups.size()<<std::endl;                   
                   for (int igroup=0; igroup<systematicsgroups.size(); igroup++) {
		    if (debug) std::cout<<cn<<mn<<"Check systematicgroup["<<igroup<<"]="<<systematicsgroups.at(igroup)<<std::endl;                   
		    if (systematicsgroups.at(igroup)=="Others" || systematicsgroups.at(igroup)=="others" ||
                        systematicsgroups.at(igroup)=="Other"  || systematicsgroups.at(igroup)=="other" ) {
                     others=true;
                     if (debug) std::cout<<cn<<mn<<"Systematic group others found"<<std::endl;                   
		     if (systmap.count(systematicsgroups.at(igroup))==0) { 
		      vsystgroups.push_back(vsyst.at(isyst));
                      TString sname="syst_";
                      sname+=systematicsgroups.at(igroup);
                      vsystgroups.back()->SetName(sname);
		      Color_t icol=systematicsgroupscolor.at(igroup);                    
                      //SPXGraphUtilities::SetColors( vsystgroups.back(),icol);
                      SPXPlot::SetSystGraphProperties( vsystgroups.back(),icol);
                      Color_t ilinecolor=systematicsgroupsedgecolor.at(igroup);    
 		      vsystgroups.back()->SetLineColor(ilinecolor);

                      Style_t ilinestyle=systematicsgroupsedgestyle.at(igroup);    
		      vsystgroups.back()->SetLineStyle(ilinestyle);

                      int ilinewidth=systematicsgroupsedgewidth.at(igroup);    
  		      vsystgroups.back()->SetLineWidth(ilinewidth);
                      
                      if (debug) { 
		       std::cout<<cn<<mn<<"New group: "<<systematicsgroups.at(igroup).c_str()<< " with vector "<<vsyst.at(isyst)->GetName()
		        		<< " vsystgroups["<<vsystgroups.size()-1<<"]="<< vsystgroups.back()->GetName() <<std::endl;
		       SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)vsystgroups.back());
                      }
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
                    //SPXGraphUtilities::SetColors(vsystgroups.back(),icol);
                    SPXPlot::SetSystGraphProperties( vsystgroups.back(),icol);
                   }
                  }
                 }

                 if (debug) {
                  std::cout<<cn<<mn<<"  "<<std::endl;
		  std::cout<<cn<<mn<<"Number of systematic groups= " <<vsystgroups.size() <<" syst not in group "<<inotingroup<<std::endl;                  
                  for (int igroup=0; igroup<vsystgroups.size(); igroup++) {
		   std::cout<<" "<<std::endl;
		   std::cout<<cn<<mn<<"igroup= "<<igroup<<" systematic group= " <<vsystgroups.at(igroup)->GetName()<<std::endl;
                   //SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors *)vsystgroups.at(igroup));
		   //vsystgroups.at(igroup)->Print();
                  }
	         }             

                 if (debug) std::cout<<cn<<mn<<"Number is vsystgroup= "<<vsystgroups.size()<<std::endl;
                 for (int igroup=0; igroup<vsystgroups.size(); igroup++) {
		  std::string systname=vsystgroups.at(igroup)->GetName();
                  if (debug) {
                   std::cout<<cn<<mn<<"Enter "<<systname.c_str()<<" to dataFileGraphMap "<<std::endl;
                   //SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors*)vsystgroups.at(igroup));
                  } 
 		  dataFileGraphMap.insert(StringGraphPair_T(systname,vsystgroups.at(igroup) ));
                 }

                 if (debug) {
                   std::cout<<cn<<mn<<"Finished to put systematics to dataFileGraphMap "<<std::endl;
		 }

                 // at the end at total uncertainty
                 if (steeringFile->ShowTotalSystematics()!=0) {
                  if (debug) std::cout<<cn<<mn<<"ShowTotalSystematics option is ON "<<std::endl;

		  TGraphAsymmErrors *gsystot=data[i]->GetSystematicErrorGraph();
                  if (!gsystot) {
                   std::cout<<cn<<mn<<"WARNING total systematic uncertainty graph not found !"<<std::endl;
                  } else {
		   std::string systname="Total";
                   TString gname="syst_total";
                   //gname+=TString(gsystot->GetName()).ReplaceAll("_syst","");
                   if (debug) std::cout<<cn<<mn<<"gname= "<<gname.Data()<<std::endl;
                   gsystot->SetName(gname);
                   // negative sign to show total uncertainty as line see SPXRatio::Draw()
                   int icol=-1*steeringFile->ShowTotalSystematics();
                   if (debug) std::cout<<cn<<mn<<"icol= "<<icol<<std::endl;

                   gsystot->SetLineColor(icol);
                   gsystot->SetFillColor(icol);

                   if (systematicsgroupsedgewidth.size()>0)
                    gsystot->SetLineWidth(systematicsgroupsedgewidth.back());

                   if (debug) std::cout<<cn<<mn<<"Enter "<<systname.c_str()<<" to dataFileGraphMap "<<std::endl;
          	   dataFileGraphMap.insert(StringGraphPair_T(systname,gsystot));
                  }
                 }
                 if (debug) {
                  std::cout<<cn<<mn<<"Finished to put total uncertainty graph to  dataFileGraphMap. "<<std::endl;
		 }

                }

                if (debug) {
                 std::cout<<cn<<mn<<"Show dataFileGraphMap "<<std::endl;
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
                 // above SetColor is overwritten as all systematics are contained in vsyst
		 if (debug) std::cout<<cn<<mn<<"Order systematics color by name alphabetical order"<<std::endl;

                 if (steeringFile->GetSystematicClassesColor().size()>0) {
		  if (debug) std::cout<<cn<<mn<<"INFO no color reordering, since color defined by groups"<<std::endl;
                 } else {
                  OrderSystVectorColorsByAlphabeth(vsyst);
                 }
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

  if (gname.Contains("_AlternativeScaleChoice_")) { 
   fillcolor  =pci.AlternativeScaleChoiceFillColor;
   edgecolor  =pci.AlternativeScaleChoiceEdgeColor;
   edgestyle  =pci.AlternativeScaleChoiceEdgeStyle;
   fillstyle  =pci.AlternativeScaleChoiceFillStyle;
   markerstyle=pci.AlternativeScaleChoiceMarkerStyle;
  }

  if (gname.Contains("_alphas_")) { 
   fillcolor  =pci.alphasFillColor;
   edgecolor  =pci.alphasEdgeColor;
   edgestyle  =pci.alphasEdgeStyle;
   fillstyle  =pci.alphasFillStyle;
   markerstyle=pci.alphasMarkerStyle;
  }

  if (gname.Contains("_beamuncertainty_")) { 
   fillcolor  =pci.beamuncertaintyFillColor;
   edgecolor  =pci.beamuncertaintyEdgeColor;
   edgestyle  =pci.beamuncertaintyEdgeStyle;
   fillstyle  =pci.beamuncertaintyFillStyle;
   markerstyle=pci.beamuncertaintyMarkerStyle;
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
    gband->SetLineWidth(4);
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


void SPXPlot::WriteRootFile(TString rootfilename){
 std::string mn = "WriteRootFile: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 rootfile=0;
 
 if (debug) std::cout<<cn<<mn<<"Output rootfile "<<rootfilename<<std::endl;

 rootfile= new TFile(rootfilename,"recreate");
 if (!rootfile) {

  std::cout<<cn<<mn<<"Problem to open rootfile= "<<rootfilename.Data()<<std::endl;
  std::cerr<<cn<<mn<<"Problem to open rootfile= "<<rootfilename.Data()<<std::endl;
  throw SPXROOTException(cn+mn+"Pointer to rootfile not found ! Can not output root objects.");

 } else if (debug){
  std::cout<<cn<<mn<<"Opened rootfile= "<<rootfilename<<std::endl;
 }

 for(int icross = 0; icross < crossSections.size(); icross++) {

  SPXPDF * pdf=crossSections[icross].GetPDF();
  if (!pdf) {std::cout<<cn<<mn<<" PDF object not found "<<std::endl; return;}

  if (debug) {
   std::cout<<cn<<mn<<"Pdf "<< pdf->GetPDFtype() << std::endl;
  }

  int nbands=pdf->GetNBands();
  if (debug) {
   std::cout<<cn<<mn<<"Number of uncertainty bands: "<< nbands << std::endl;
  }

  bool gridcorrectionfound=false;
  for (int iband=0; iband<nbands; iband++) {
   TGraphAsymmErrors * gband   =pdf->GetBand(iband);
   if (!gband) {std::cout<<cn<<mn<<" gband not found ! iband= "<<iband<<std::endl; continue;}
   if (debug) std::cout<<cn<<mn<<"Write out "<<pdf->GetBandType(iband)<<std::endl;
   gband->Write();

   std::string gtype   =pdf->GetBandType(iband);
   //if (debug) {
   // std::cout<<cn<<mn<<"Type "<<gtype.c_str()<<" name= "<<gband->GetName() << std::endl;
   //gband->Print();
   //}

   //if(debug) std::cout<<cn<<mn<<"Grid correction are applied " << std::endl;
   //std::vector<std::string> corrlabel=crossSections[icross].GetCorrectionLabels();
   //for(int ic = 0; ic < corrlabel.size(); ic++) {
   // if (gtype.compare(corrlabel[ic])==0) gridcorrectionfound=true;	   
   // if (debug) std::cout << cn << mn <<"Grid correction: "<<corrlabel[ic].c_str() << std::endl;
   //}
  
   //if (debug) {
   // if (gridcorrectionfound) std::cout << cn << mn <<"Grid corrections found !"<< std::endl;
   // else                     std::cout << cn << mn <<"No Grid corrections found !"<< std::endl;
   //}
  }

  if(debug) {
   std::cout<<cn<<mn<< "  " << std::endl;
   std::cout<<cn<<mn<<"Access individual PDF components "<< std::endl;
  }

  int npdfcomponents=pdf->GetNumberOfIndividualPDFComponents();

  if(debug) {
    std::cout<<cn<<mn<<"Number of individual PDF "<<pdf->GetPDFName()<<" components =  "<< npdfcomponents << std::endl;
  }

  for (int ipdf=0; ipdf<npdfcomponents; ipdf++) {
   TH1D *hcomp= pdf->GetIndividualPDFComponent(ipdf);
   if (!hcomp) {std::cout<<cn<<mn<<"Histogram for component ipdf= "<<ipdf<<" not found "<<std::endl; continue;}
   std::cout<<cn<<mn<<"Write PDF component "<<hcomp->GetName()<<std::endl;
   hcomp->Write();
   //hcomp->Print("all");
  }

  if(debug) {
   std::cout<<cn<<mn<< "  " << std::endl;
   std::cout<<cn<<mn<<"Access individual scale components "<< std::endl;
  }

  int nscalecomponents=pdf->GetNumberOfIndividualScaleVariations();

  if(debug) {
   std::cout<<cn<<mn<<"Number of individual scale variations  " << std::endl;
  }

  for (int iscale=0; iscale<nscalecomponents; iscale++) {
   TH1D *hcomp= pdf->GetIndividualScaleVariation(iscale);
   if (!hcomp) {std::cout<<cn<<mn<<"Histogram for component iscale= "<<iscale<<" not found ! "<<std::endl; continue;}
   std::cout<<"Write scale component "<<hcomp->GetName()<<std::endl;
   hcomp->Write();
  }


  int nalphascomponents=pdf->GetNumberOfIndividualAlphaSVariations();

  if(debug) {
   std::cout<<cn<<mn<<"Number of individual alphas variations  " << std::endl;
  }

  for (int ialphas=0; ialphas<nalphascomponents; ialphas++) {
   TH1D *hcomp= pdf->GetIndividualAlphaSVariation(ialphas);
   if (!hcomp) {std::cout<<cn<<mn<<"Histogram for component ialphas= "<<ialphas<<" not found ! "<<std::endl; continue;}
   std::cout<<"Write AlphaS component "<<hcomp->GetName()<<std::endl;
   hcomp->Write();
  }
 }

 if(debug) {
  std::cout<<cn<<mn<<"Number Data sets  " << data.size()<< std::endl;
 }
 
 for(int idata = 0; idata < data.size(); idata++) {                   

  TString datalabel=data.at(idata)->GetLegendLabel();

  if (debug) {
   std::cout<<cn<<mn<< "  " << std::endl;
   std::cout<<cn<<mn<<"Access stat, total syst and total uncertainties Data name= "<< datalabel.Data() << std::endl;
  }

  TGraphAsymmErrors *statGraph = data[idata]->GetStatisticalErrorGraph();
  if (!statGraph) throw SPXGeneralException(cn+mn+"Can not open statistical uncertainty for"+datalabel.Data());
  else {
   TString filename=statGraph->GetName();
   //filename+=datalabel.Data();
   if (debug) std::cout<<cn<<mn<<"Write " << filename.Data() <<std::endl;
   statGraph->Write(filename);
  }

  TGraphAsymmErrors *systGraph = data[idata]->GetSystematicErrorGraph();
  if (!systGraph) throw SPXGeneralException(cn+mn+"Can not open systematic uncertainty for"+datalabel.Data());
  else {
   TString filename=systGraph->GetName();
   //filename+=datalabel.Data();
   if (debug) std::cout<<cn<<mn<<"Write " << filename.Data() <<std::endl;
   systGraph->Write(filename);  
  }

  TGraphAsymmErrors *totGraph  = data[idata]->GetTotalErrorGraph();
  if (!totGraph) throw SPXGeneralException(cn+mn+"Can not open statistical total for"+datalabel.Data());
  else {
   TString filename=totGraph->GetName();
   //filename+=datalabel.Data();
   if (debug) std::cout<<cn<<mn<<"Write " << filename.Data() <<std::endl;
   totGraph->Write(filename);  
  }

  if (debug) {
   std::cout<<cn<<mn<<"Print "<<systGraph->GetName() << std::endl;
   //systGraph->Print();
  }

  if (steeringFile->ShowIndividualSystematics()!=0) {

   std::vector <TGraphAsymmErrors *> vsyst;
   vsyst=data[idata]->GetSystematicsErrorGraphs();

   if(debug) {
    std::cout<<cn<<mn<<"Total number of systematics uncertainty components found= " << vsyst.size() <<std::endl;
    std::cout<<cn<<mn<<"Only show systematics above: "<<steeringFile->ShowIndividualSystematics()<<"%"<<std::endl;
   }

   std::vector<TGraphAsymmErrors * > vsystord=SPXUtilities::OrderLargestRelativeErrorGraphVector(vsyst);

   for (int isyst=0; isyst<vsystord.size(); isyst++) {
    std::string systname=vsystord.at(isyst)->GetName();

    if (!vsyst.at(isyst)) {
     std::cout<<cn<<mn<<"WARNING: systematic component not found isyst= "<<isyst<<std::endl;
     continue;
    } 

    if (debug) std::cout<<cn<<mn<<"Write systmatic components " << isyst <<std::endl;
    vsyst.at(isyst)->Write();
   }
  } else if (debug) std::cout<<cn<<mn<<"No systematic components asked !"  <<std::endl;
 }

 //for(int idata = 0; idata < data.size(); idata++) {                    
 // TString datalabel=data.at(idata)->GetLegendLabel();
 // if(debug) std::cout<<cn<<mn<<"Data name= "<<datalabel.Data() << std::endl;
 
 // if(debug) std::cout<<cn<<mn<<"Access total covariance matrix: "<< std::endl;
 // TMatrixT<double> *totcovmatrix=data.at(idata)->GetDataTotalCovarianceMatrix();
 // if (!totcovmatrix) {
 //  std::cout<<cn<<mn<<"Total covariance matrix not found ! for data "<<datalabel.Data() << std::endl; 
 // return;
 // } else {
 //  if (debug) totcovmatrix->Print();
 // }

 //if(debug) std::cout<<cn<<mn<<"Access statistical covariance matrix: "<< std::endl;
 // TMatrixT<double> *statcovmatrix=data.at(idata)->GetDataStatCovarianceMatrix();
 // if (!statcovmatrix) {
 //  std::cout<<cn<<mn<<"Statistical covariance matrix not found ! for data "<<datalabel.Data() << std::endl; 
   // return;
 // } else {
 //  if (debug) statcovmatrix->Print();
 // }
 //}

 if (debug) {
  std::cout<<cn<<mn<<"Writing out routfile: "<<std::endl;
  rootfile->Print();
 }

 std::cout<<cn<<mn<<"Write file "<<rootfilename.Data()<<std::endl;
 rootfile->Write();

 return;
}

void SPXPlot::SetSystGraphProperties(TGraphAsymmErrors * gsyst, Color_t icol){
 std::string mn = "SetSystGraphProperties: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 double linewidth=steeringFile->ShowIndividualSystematicsAsLine();
 if ( linewidth>0) {   
  gsyst->SetFillStyle(0);
  gsyst->SetLineWidth(int(linewidth));

  if (debug) std::cout<<cn<<mn<<"Set linewidth= "<<gsyst->GetLineWidth()<<std::endl;
 
 } else {
  gsyst->SetFillStyle(1001);
 }

 SPXGraphUtilities::SetColors(gsyst,icol);

 //if (debug)
 // std::cout<<cn<<mn<<"Calling SPXPrintGraphProperties "<<std::endl;  
 // SPXGraphUtilities::SPXPrintGraphProperties((TGraphErrors *)gsyst);
 //}
 return; 
}

std::string SPXPlot::GetCorrectionLabel(SPXCrossSection cross){
 std::string mn = ":GetCorrectionLabel: ";

 std::string text="";
 std::vector<std::string> corrlabel=cross.GetCorrectionLabels();
 //if (debug) std::cout<<cn<<mn<<"Number of corrections= "<<corrlabel.size()<<std::endl;           

 int ncorr=cross.GetNumberofCorrections();

 for(int ic = 0; ic < ncorr; ic++) {

  if (ic>corrlabel.size()){
   std::ostringstream oss;
   oss<<cn<<mn<<"Number of correction to be applied is "<<ncorr<<" but corrlabel vector size "<<corrlabel.size()<<std::endl;
   std::cout<<oss<<std::endl;
   throw SPXGraphException(oss.str());
  
  }

  if (ic>0) text+=" #otimes ";
  text+=corrlabel.at(ic);

  //if (debug) std::cout<<cn<<mn<<"grid correction label= "<<label.c_str()<<std::endl;
  //text+=TString(label); // do this better
 }
 
 return text;
}

TString SPXPlot::GetEtaLabel(SPXData *data){
 std::string mn = ":GetEtaLabel: ";

 if (!data) {
  std::ostringstream oss;
  oss<<cn<<mn<<"Data object not found !"<<std::endl;
  std::cout<<oss<<std::endl;
  throw SPXGraphException(oss.str());
 }

 double binmin = data->GetDoubleBinValueMin();
 double binmax = data->GetDoubleBinValueMax();
 TString varname=data->GetDoubleBinVariableName();
 TString datalabel="";

 //if (binmin!=0) {
 // datalabel.Form(" %3.2f ",binmin); 
 //} else {
 //varname.ReplaceAll("#leq","");
 // varname.ReplaceAll("","");
 //}
 //datalabel+=varname;
 //datalabel+=Form(" %3.2f ",binmax);

 datalabel=this->FormatetabinLabel(varname, binmin,binmax);

 return datalabel;
}
