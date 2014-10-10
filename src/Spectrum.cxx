//************************************************************/
//
//	Spectrum
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		19.09.2014
//
//************************************************************/

#include <iostream>

#include "SPXROOT.h"
#include "SPXSteeringFile.h"
#include "SPXData.h"
#include "SPXException.h"

int main(int argc, char *argv[]) {

	if((argc - 1) < 1) {
		std::cout << "@usage: Spectrum <steering_file>" << std::endl;
		exit(0);
	}

	std::string file;
	
	std::cout << "==================================" << std::endl;
	std::cout << "      	   Spectrum		        " << std::endl;
	std::cout << "==================================" << std::endl <<std::endl;

	TApplication *spectrum = new TApplication("Spectrum",0,0);
  	spectrum->SetReturnFromRun(true);

	file = std::string(argv[1]);

	//Create Steering File Object and initialize with filename
	SPXSteeringFile steeringFile = SPXSteeringFile(file);
    
    //Parse the Steering File
    try {
    	steeringFile.Parse();
    	steeringFile.Print(); 
    } catch(const SPXException &e) {
    	std::cerr << e.what() << std:: endl;
    	std::cerr << "FATAL: Could not parse the steering file: " << file << std::endl;
    	exit(-1);
    }
    
    //Parse the PDF Steering Files
    try {
    	steeringFile.ParsePDFSteeringFiles();
    	steeringFile.PrintPDFSteeringFiles();
    } catch(const SPXException &e) {
    	std::cerr << e.what() << std::endl;
    	std::cerr << "FATAL: Could not parse the PDF Steering files" << std::endl;
    	exit(-1);
    }
    
    //Parse the Data Steering Files
	try {
		steeringFile.ParseDataSteeringFiles();
		steeringFile.PrintDataSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "FATAL: Could not parse the Data Steering files" << std::endl;
		exit(-1);
	}
	
	//Parse the Grid Steering Files
	try {
		steeringFile.ParseGridSteeringFiles();
		steeringFile.PrintGridSteeringFiles();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "FATAL: Could not parse the Grid Steering files" << std::endl;
		exit(-1);
	}

	//Create a data object and parse it
	SPXData::SetDebug(true);
	SPXFrameOptionsInstance &foi = steeringFile.GetFrameOptionsInstance(0, 0);
	SPXData data = SPXData(foi);

	try {
		data.Parse();	//Parse the data
		data.Print();	//Print the data to the console
		data.Draw();	//Draw the data in a frame
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "FATAL: Could not parse the data file" << std::endl;
		exit(-1);
	}

	spectrum->Run(kTRUE);

	return 0;
}



