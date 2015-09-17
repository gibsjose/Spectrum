//************************************************************/
//
//	Grid Steering File Header
//
//	Outlines the SPXGridSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections with the MyAnalysis PDF Program
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		01.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include "INIReader.h"

#include "SPXException.h"

#ifndef SPXGRIDSTEERINGFILE_H
#define SPXGRIDSTEERINGFILE_H

class SPXGridSteeringFile {

private:

	//INI Reader
	INIReader *reader;

	//std::string grid_dir;
	//Metadata
	std::string filename;

	//[GEN]
	bool debug;

	//[DESC]
	std::string name;						       //Grid name
	std::string comments;						       //Grid comments
	std::string author;						       //Grid author
	std::string lumiConfigFile;					       //Sub-process configuration file for the grid program (APPLgrid)
	std::string scaleform;						       //Choice of renormalization scale (Q^2)
	std::string referenceJournalName;				       //Name of reference journal for the corresponding dataset
	std::string referenceLinkToArXiv;				       //Link to reference on arXiv for the corresponding dataset
	std::string nloProgramName;					       //NLO Program used to generate the grids
	std::string gridProgramName;					       //Program used as an interface between NLO program and APPLgrid
	std::string observableDefinitionLinkToCode;			//Link to the piece of code/file which defines the observable and applies \
															event selection cut, fills APPLgrid, etc.

	//[GRAPH]
	std::string xUnits;
	std::string yUnits;
	bool gridDividedByBinWidth;
	bool referenceDividedByBinWidth;

	bool gridDividedByDoubleDiffBinWidth;
	bool referenceDividedByDoubleDiffBinWidth;
	double yScale;

	//[GRID]
	std::vector <std::string> vgridFilepath;
	std::string gridFilepath;

	std::vector<std::string> correctionFiles;
	std::vector<std::string> correctionFilesLabel;
	std::vector<std::string> correctionFilesAuthor;
	std::vector<std::string> correctionFilesProgram;
	std::vector<std::string> correctionFilesReference;
	unsigned int lowestOrder;

	double changesqrts;

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

	//std::string GetGridDir(void) const {
	//	return this->grid_dir;
	//}

	//void SetGridDir(std::string mydir) {
	//  grid_dir=mydir;
        //  return;
	//}

	void SetDebug(bool b) {
		debug = b;
	}

	bool GetDebug(void) const {
		return this->debug;
	}

	const std::string & GetName(void) const {
		return this->name;
	}

	const std::string & GetComments(void) const {
		return this->comments;
	}

	const std::string & GetAuthor(void) const {
		return this->author;
	}

	const std::string & GetLumiConfigFile(void) const {
		return this->lumiConfigFile;
	}

	const std::string & GetScaleFunctionalForm(void) const {
		return this->scaleform;
	}

	const std::string & GetReferenceJournalName(void) const {
		return this->referenceJournalName;
	}

	const std::string & GetReferenceLinkToArXiv(void) const {
		return this->referenceLinkToArXiv;
	}

	const std::string & GetNLOProgramName(void) const {
		return this->nloProgramName;
	}

	const std::string & GetGridProgramName(void) const {
		return this->gridProgramName;
	}

	const std::string & GetObservableDefinitionLinkToCode(void) const {
		return this->observableDefinitionLinkToCode;
	}

	const std::string & GetXUnits(void) const {
		return this->xUnits;
	}

	const std::string & GetYUnits(void) const {
		return this->yUnits;
	}

	bool IsGridDividedByBinWidth(void) const {
		return this->gridDividedByBinWidth;
	}

	bool IsReferenceDividedByBinWidth(void) const {
		return this->referenceDividedByBinWidth;
	}

	bool IsGridDividedByDoubleDiffBinWidth(void) const {
		return this->gridDividedByDoubleDiffBinWidth;
	}

	bool IsReferenceDividedByDoubleDiffBinWidth(void) const {
		return this->referenceDividedByDoubleDiffBinWidth;
	}

	double GetYScale(void) const {
		return this->yScale;
	}


	const std::string & GetGridFilepath(void) const {
		return this->gridFilepath;
	}

	std::vector <std::string > GetGridFilepathVector(void) const {
		return this->vgridFilepath;
	}
 
        int GetNumberofGrids() {
	 return vgridFilepath.size();
        }

	//void SetGridFilepath(std::string &filepath) {
	//	this->gridFilepath = filepath;
	//}

	void PrependGridFilepath(std::string &path) {
		this->gridFilepath = path + "/" + this->gridFilepath;
	}

	void PrependGridFilepathVector(std::string &path) {
	  //std::cout<<" number of grids= "<<vgridFilepath.size()<<std::endl;
	  std::vector <std::string> tmp;
          tmp=vgridFilepath;
          vgridFilepath.clear();
	  for (int igrid=0; igrid<tmp.size(); igrid++) { 
	   std::string str=tmp.at(igrid);
	   std::string newname = path + "/" + str;

 	   //std::cout<<" oldname= "<<str.c_str()<<" newname= "<<newname.c_str()<<std::endl;

           vgridFilepath.push_back(newname);
 	   //std::cout<<" replaced name= "<<vgridFilepath.at(igrid).c_str()<<std::endl;
          }
	}

	unsigned int GetNumberOfCorrectionFiles(void) const {
		return this->correctionFiles.size();
	}

	const std::string & GetCorrectionFile(unsigned int index) const {

		if((index + 1) > correctionFiles.size()) {
			int top = correctionFiles.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXGridSteeringFile::GetCorrectionFile: Index out of range");
		}

		return correctionFiles.at(index);
	}


	const std::string & GetCorrectionFileLabel(unsigned int index) const {

		if((index + 1) > correctionFilesLabel.size()) {
			int top = correctionFilesLabel.size() - 1;
			throw SPXOutOfRangeException(top, index, "SPXGridSteeringFile::GetCorrectionFileLabel: Index out of range");
		}

		return correctionFilesLabel.at(index);
	}


	unsigned int GetLowestOrder(void) const {
		return this->lowestOrder;
	}

	double GetChangeSqrtS(void) const {
		return changesqrts;
	}
};

#endif
