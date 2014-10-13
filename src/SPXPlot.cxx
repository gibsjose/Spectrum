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

const std::string cn = "SPXPlot::";

//Must define the static debug variable in the implementation
bool SPXPlot::debug;

//Initialize all plots
void Initialize(void) {

}

void Plot(void) {
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