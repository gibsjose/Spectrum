//************************************************************/
//
//	Steering File Reader Test Program
//
//	Tests the INIH Library and SteeringFile classes and
//	subclasses
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		19.09.2014
//
//************************************************************/

#include <iostream>
#include "SPXSteeringFile.h"
#include "SPXException.h"

int main(int argc, char *argv[]) {

	if((argc - 1) < 1) {
		std::cout << "@usage: SteeringFileReader <steering_file>" << std::endl;
		exit(0);
	}

	std::string file;
	
	std::cout << "==================================" << std::endl;
	std::cout << "      Steering File Reader        " << std::endl;
	std::cout << "==================================" << std::endl <<std::endl;

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
	
	return 0;
}



