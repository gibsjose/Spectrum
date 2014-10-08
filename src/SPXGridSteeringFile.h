//************************************************************/
//
//	Grid Steering File Header
//
//	Outlines the SPXGridSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections with the MyAnalysis PDF Program
//
//	@Author: 	Joe Gibson - CERN ATLAS
//	@Date:		01.10.2014
//
//************************************************************/

#include <iostream>
#include <string>

#include "INIReader.h"

#ifndef SPXGRIDSTEERINGFILE_H
#define SPXGRIDSTEERINGFILE_H

class SPXGridSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//Metadata
	std::string filename;

	//[GEN]
	bool debug;
	
	//[DESC]
	std::string name;
	
	//[GRAPH]
	std::string xUnits;
	std::string yUnits;
	bool dividedByBinWidth;
	
	//[GRID]
	std::string gridFilepath;
	std::string generatorID;
	std::string nTupleID;
	std::string lowestOrder;
	
	void SetDefaults(void);
	
public:

	void Print(void);
	void Parse(void);
	
	SPXGridSteeringFile(void) : filename(""), debug(false) {}
	
	explicit SPXGridSteeringFile(const std::string &filename) : debug(false) {	
		this->filename = filename;		
	}
	
	std::string GetFilename(void) const {
		return this->filename;
	}
	
	void SetDebug(bool b) {
		debug = b;
	}
	
	bool GetDebug(void) const {
		return this->debug;
	}
	
	const std::string & GetName(void) const {
		return this->name;
	}
	
	const std::string & GetXUnits(void) const {
		return this->xUnits;
	}
	
	const std::string & GetYUnits(void) const {
		return this->yUnits;
	}
	
	bool IsDividedByBinWidth(void) const {
		return this->dividedByBinWidth;
	}
	
	const std::string & GetGridFilepath(void) const {
		return this->gridFilepath;
	}
	
	const std::string & GetGeneratorID(void) const {
		return this->generatorID;
	}
	
	const std::string & GetNTupleID(void) const {
		return this->nTupleID;
	}	
	
	const std::string & GetLowestOrder(void) const {
		return this->lowestOrder;
	}
};

#endif
