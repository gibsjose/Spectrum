//************************************************************/
//
//	Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		19.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>

#include "SPXROOT.h"
#include "SPXAtlasStyle.h"
#include "SPXSteeringFile.h"
#include "SPXAnalysis.h"
#include "SPXException.h"

namespace Test {
	bool TestFeatures = false;
}

namespace Options {
	bool Metadata = false;
}

int main(int argc, char *argv[]) {

	if((argc - 1) < 1) {
		std::cout << "@usage: Spectrum [-p] <steering_file>" << std::endl;
		exit(0);
	}
 
        if (!std::string(argv[1]).compare("-h")){
	 std::cout << "Spectrum help " << std::endl;
	 std::cout << "Spectrum -p DrawApplication " << std::endl;
	 std::cout << "Spectrum -t Testfeatures " << std::endl;
	 std::cout << "Spectrum -m write metadata to text file " << std::endl;
	 std::cout << "Spectrum -latex_table not yet implemented " << std::endl;
	 exit(0);
	}

	std::string file;

	Test::TestFeatures = false;
	Options::Metadata = false;
	bool drawApplication = true;

	std::cout << "==================================" << std::endl;
	std::cout << "      	   Spectrum		        " << std::endl;
	std::cout << "==================================" << std::endl <<std::endl;

	//Process arguments
	for(int i = 1; i < argc; i++) {
		std::string arg = std::string(argv[i]);

		//PNG Only mode
		if(!arg.compare("-p")) {
			drawApplication = false;
		}

		//Test mode (implements test features)
		else if(!arg.compare("-t")) {
			Test::TestFeatures = true;
		}

		//Print Data/Grid Metadata to a file
		else if(!arg.compare("-m")) {
			Options::Metadata = true;
		}
                else if(!arg.compare("-latex_table")) {
		  std::cout<<" option latex_table not yet implemented "<<endl;
                  exit (0);
 		}


		//No known flag: Treat as file name
		else {
			file = arg;
		}
	}

	//Set Atlas Style (SPXAtlasStyle.h)
	SetAtlasStyle();

	TApplication *spectrum;

	if(drawApplication) {
		spectrum = new TApplication("Spectrum",0,0);
		spectrum->SetReturnFromRun(true);
	}

	SPXSteeringFile steeringFile = SPXSteeringFile(file);

	//=========================================================
	//   Configuration
	//=========================================================
    try {
    	steeringFile.ParseAll(false);
		steeringFile.PrintAll();
    } catch(const SPXException &e) {
    	std::cerr << e.what() << std::endl;
    	std::cerr << "FATAL: Could not parse the steering file: " << file << std::endl;
    	exit(-1);
    }

	//=========================================================
	//   MetaData
	//=========================================================
	if(Options::Metadata) {
		try {
			steeringFile.PrintDataMetadata();
			steeringFile.PrintGridMetadata();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			std::cerr << "WARNING: Could not write metadata" << std::endl;
		}
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

	if(drawApplication) {
		spectrum->Run(kTRUE);
	}

	return 0;
}
