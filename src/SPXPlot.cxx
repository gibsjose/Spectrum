//************************************************************/
//
//	Plot Implementation
//
//	Implements the SPXPlot class, which connects all of the
//  other non-configuration classes (non-steering/options) and
//	manages the plotting, drawing, and ROOT objects
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		13.10.2014
//
//************************************************************/

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
	//TEMPORARY!!!
	SPXData::SetDebug(true);
	SPXPlotConfigurationInstance &pci = steeringFile->GetPlotConfigurationInstance(0, 0);
	SPXData data = SPXData(pci);

	try {
		data.Parse();	//Parse the data
		data.Print();	//Print the data to the console
		data.Draw();	//Draw the data in a frame
	} catch(const SPXException &e) {
		throw;
	}
}

void SPXPlot::InitializeData(void) {

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

		//Temporary pointers to settings
		//SPXPlotConfigurationInstance = steeringFile

		//Normalize the graphs based on the settings
	}
}