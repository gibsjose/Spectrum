//************************************************************/
//
//	Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		19.09.2014
//
//************************************************************/

#include <iostream>

#include "SPXROOT.h"
#include "SPXSteeringFile.h"
#include "SPXAnalysis.h"
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

	SPXSteeringFile steeringFile = SPXSteeringFile(file);

	//=========================================================
	//   Configuration
	//=========================================================
    try {
    	steeringFile.ParseAll(true);
    } catch(const SPXException &e) {
    	std::cerr << e.what() << std::endl;
    	std::cerr << "FATAL: Could not parse the steering file: " << file << std::endl;
    	exit(-1);
    }

    //=========================================================
	//     Analysis
	//=========================================================
    try {
    	SPXAnalysis analysis = SPXAnalysis(&steeringFile);
    	analysis.Run();
    } catch(const SPXException &e) {
    	std::cerr << e.what() << std::endl;
    	std::cerr << "FATAL: Unable to perform successful analysis" << std::endl;
    	exit(-1);
    }

	spectrum->Run(kTRUE);

	return 0;
}



