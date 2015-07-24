//************************************************************/
//
//	Grid Steering File Implementation
//
//	Implements the SPXGridSteeringFile class, which standardizes the
//	format and I/O of the Steering File Format (SFF), a file
//	format that allows for interfacing between Grids, Data,
//	and Cross Sections with the MyAnalysis PDF Program
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		01.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXGridSteeringFile.h"
#include "SPXUtilities.h"
#include "SPXException.h"

//Class name for debug statements
const std::string cn = "SPXGridSteeringFile::";

void SPXGridSteeringFile::SetDefaults(void) {
	std::string mn = "SetDefaults: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Setting default Grid Steering File data" << std::endl;

	name.clear();
	if(debug) std::cout << cn << mn << "name set to default: \" \"" << std::endl;

	comments.clear();
	if(debug) std::cout << cn << mn << "comments set to default: \" \"" << std::endl;

	author.clear();
	if(debug) std::cout << cn << mn << "author set to default: \" \"" << std::endl;

	lumiConfigFile.clear();
	if(debug) std::cout << cn << mn << "lumiConfigFile set to default: \" \"" << std::endl;

	scale.clear();
	if(debug) std::cout << cn << mn << "scale set to default: \" \"" << std::endl;

	referenceJournalName.clear();
	if(debug) std::cout << cn << mn << "referenceJournalName set to default: \" \"" << std::endl;

	referenceLinkToArXiv.clear();
	if(debug) std::cout << cn << mn << "referenceLinkToArXiv set to default: \" \"" << std::endl;

	nloProgramName.clear();
	if(debug) std::cout << cn << mn << "nloProgramName set to default: \" \"" << std::endl;

	gridProgramName.clear();
	if(debug) std::cout << cn << mn << "gridProgramName set to default: \" \"" << std::endl;

	observableDefinitionLinkToCode.clear();
	if(debug) std::cout << cn << mn << "observableDefinitionLinkToCode set to default: \" \"" << std::endl;

	xUnits.clear();
	if(debug) std::cout << cn << mn << "xUnits set to default: \" \"" << std::endl;

	yUnits.clear();
	if(debug) std::cout << cn << mn << "yUnits set to default: \" \"" << std::endl;

	gridDividedByBinWidth = true;
	if(debug) std::cout << cn << mn << "gridDividedByBinWidth set to default: \"true\"" << std::endl;

	gridDividedByDoubleDiffBinWidth = false;
	if(debug) std::cout << cn << mn << "gridDividedByDoubleDiffBinWidth set to default: \"false\"" << std::endl;

	referenceDividedByBinWidth = false;
	if(debug) std::cout << cn << mn << "referenceDividedByBinWidth set to default: \"false\"" << std::endl;

	referenceDividedByDoubleDiffBinWidth = false;
	if(debug) std::cout << cn << mn << "referenceDividedByDoubleDiffBinWidth set to default: \"false\"" << std::endl;

	yScale = 1.0;
	if(debug) std::cout << cn << mn << "yScale set to default: \"1.0\"" << std::endl;

	gridFilepath.clear();
	if(debug) std::cout << cn << mn << "gridFilepath set to default: \" \"" << std::endl;

	vgridFilepath.clear();
	if(debug) std::cout << cn << mn << "vgridFilepath set to default: \" \"" << std::endl;

	lowestOrder = 1;
	if(debug) std::cout << cn << mn << "lowestOrder set to default: \"NLO\" (1)" << std::endl;

	changesqrts = 1.;
	if(debug) std::cout << cn << mn << "changesqrts set to default: 1. " << std::endl;
}

//Print the Grid Steering File Data in a nice format
void SPXGridSteeringFile::Print(void) {
	std::cout << "\n Grid Steering File: " << filename << std::endl;
	std::cout << "\t General Options [GEN]" << std::endl;
	std::cout << "\t\t Debug is " << (debug ? "ON" : "OFF") << std::endl << std::endl;
	std::cout << "\t Description [DESC]" << std::endl;
	std::cout << "\t\t Name: " << name << std::endl;
	std::cout << "\t\t Comments: " << comments << std::endl;
	std::cout << "\t\t Author: " << author << std::endl;
	std::cout << "\t\t Lumi Config File: " << lumiConfigFile << std::endl;
	std::cout << "\t\t Scale: " << scale << std::endl;
	std::cout << "\t\t Reference Journal Name: " << referenceJournalName << std::endl;
	std::cout << "\t\t Reference Link to arXiv: " << referenceLinkToArXiv << std::endl;
	std::cout << "\t\t NLO Program Name: " << nloProgramName << std::endl;
	std::cout << "\t\t Grid Program Name: " << gridProgramName << std::endl;
	std::cout << "\t\t Observable Definition Link To Code: " << observableDefinitionLinkToCode << std::endl << std::endl;
	std::cout << "\t Graphing Options [GRAPH]" << std::endl;
	std::cout << "\t\t X Units: " << xUnits << std::endl;
	std::cout << "\t\t Y Units: " << yUnits << std::endl;
	std::cout << "\t\t Y Scale: " << yScale << std::endl;
	std::cout << "\t\t Grid Divided by Bin Width? " << (gridDividedByBinWidth ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Grid Divided by double differential Bin Width variable? " << (gridDividedByDoubleDiffBinWidth ? "YES" : "NO") << std::endl;
	std::cout << "\t\t Reference Divided by Bin Width? " << (referenceDividedByBinWidth ? "YES" : "NO") << std::endl << std::endl;
	std::cout << "\t\t Reference Divided by  double differential  Bin Width variable? " << (referenceDividedByDoubleDiffBinWidth? "YES" : "NO") << std::endl << std::endl;
	std::cout << "\t Grid Options [GRID]" << std::endl;
	std::cout << "\t\t Grid File: " << gridFilepath << std::endl;

	std::cout << "\t\t Number of Grid Files: " << vgridFilepath.size() << std::endl;
        for (int i=0; i< vgridFilepath.size(); i++) {
	std::cout << "\t\t Name: " << vgridFilepath.at(i) << std::endl;
        }
	std::cout << "\t\t Correction Files: " << std::endl;
	for(int i = 0; i < correctionFiles.size(); i++) {
		std::cout << "\t\t\t " << correctionFiles.at(i) << std::endl;
	}

	std::cout << "\t\t Correction Labels: " << std::endl;
	for(int i = 0; i < correctionFilesLabel.size(); i++) {
		std::cout << "\t\t\t " << correctionFilesLabel.at(i) << std::endl;
	}

	std::cout << "\t\t Correction Authors: " << std::endl;
	for(int i = 0; i < correctionFilesAuthor.size(); i++) {
		std::cout << "\t\t\t " << correctionFilesAuthor.at(i) << std::endl;
	}

	std::cout << "\t\t Correction Programs: " << std::endl;
	for(int i = 0; i < correctionFilesProgram.size(); i++) {
		std::cout << "\t\t\t " << correctionFilesProgram.at(i) << std::endl;
	}

	std::cout << "\t\t Correction Refereces: " << std::endl;
	for(int i = 0; i < correctionFilesReference.size(); i++) {
		std::cout << "\t\t\t " << correctionFilesReference.at(i) << std::endl;
	}


	std::cout << "\t\t Lowest Order: " << lowestOrder << std::endl;
	std::cout << "\t\t Change cross section by: " << changesqrts<<"%" << std::endl;
}

void SPXGridSteeringFile::Parse(void) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
        this->SetDefaults();
        
        if (debug) std::cout<<cn<<mn<<"Checking filename=  "<<filename.c_str()<<std::endl;

	if(filename.empty()) {
	 //throw SPXFileIOException(filename, "Empty file string \"\" was given");
	 std::cout<<cn<<mn<<"WARNING no steering file given, do not know what to do, return "<<std::endl;
         return;
	}

        if (debug) std::cout<<cn<<mn<<"Start reader "<<std::endl;

	//Initialize reader
	reader = new INIReader(filename);

	if(reader->ParseError() < 0) {
		delete reader;

    	throw SPXFileIOException(filename, "INIReader::INIReader(): ParseError generated when parsing file");
	}

	std::string tmp;

	//General Options [GEN]
	debug = reader->GetBoolean("GEN", "debug", debug);
	if(debug) std::cout << cn << mn << "Debug is ON" << std::endl;

	//Set Defaults
	//this->SetDefaults();

	//Description [DESC]
	name = reader->Get("DESC", "name", "EMPTY");
	if(!name.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Grid name was not specified" << std::endl;
		name.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Grid Name: " << name << std::endl;
	}

	tmp = reader->Get("DESC", "comments", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Comments were not specified" << std::endl;
	} else {
		comments = tmp;
		if(debug) std::cout << cn << mn << "Successfully read Comments: " << comments << std::endl;
	}

	author = reader->Get("DESC", "author", "EMPTY");
	if(!author.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Author was not specified" << std::endl;
		author.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Author: " << author << std::endl;
	}

	lumiConfigFile = reader->Get("DESC", "lumi_config_file", "EMPTY");
	if(!lumiConfigFile.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Lumi Config File was not specified" << std::endl;
		lumiConfigFile.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Lumi Config File: " << lumiConfigFile << std::endl;
	}

	scale = reader->Get("DESC", "scale", "EMPTY");
	if(!scale.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Scale was not specified" << std::endl;
		scale.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Scale: " << scale << std::endl;
	}

	referenceJournalName = reader->Get("DESC", "reference_journal_name", "EMPTY");
	if(!referenceJournalName.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reference Journal Name was not specified" << std::endl;
		referenceJournalName.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Reference Journal Name: " << referenceJournalName << std::endl;
	}

	referenceLinkToArXiv = reader->Get("DESC", "reference_link_to_arxiv", "EMPTY");
	if(!referenceLinkToArXiv.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Reference Link to arXiv was not specified" << std::endl;
		referenceLinkToArXiv.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Reference Link to arXiv: " << referenceLinkToArXiv << std::endl;
	}

	nloProgramName = reader->Get("DESC", "nlo_program_name", "EMPTY");
	if(!nloProgramName.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "NLO Program Name was not specified" << std::endl;
		nloProgramName.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read NLO Program Name: " << nloProgramName << std::endl;
	}

	gridProgramName = reader->Get("DESC", "grid_program_name", "EMPTY");
	if(!gridProgramName.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Grid Program Name was not specified" << std::endl;
		gridProgramName.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Grid Program Name: " << gridProgramName << std::endl;
	}

	observableDefinitionLinkToCode = reader->Get("DESC", "observable_definition_link_to_code", "EMPTY");
	if(!observableDefinitionLinkToCode.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "Observable Definition Link to Code was not specified" << std::endl;
		observableDefinitionLinkToCode.clear();
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Observable Definition Link to Code: " << observableDefinitionLinkToCode << std::endl;
	}

	//Graping Options [GRAPH]
	xUnits = reader->Get("GRAPH", "x_units", "EMPTY");
	if(!xUnits.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "x_units", "You MUST specify the x_units");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read X Units: " << xUnits << std::endl;
	}

	yUnits = reader->Get("GRAPH", "y_units", "EMPTY");
	if(!yUnits.compare("EMPTY")) {
		throw SPXINIParseException("GRAPH", "y_units", "You MUST specify the y_units");
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Y Units: " << yUnits << std::endl;
	}

	yScale = reader->GetReal("GRAPH", "y_scale", yScale);
	if(debug) 
         std::cout << cn << mn << "yscale= " << yScale << std::endl;        

	gridDividedByBinWidth = reader->GetBoolean("GRAPH", "grid_divided_by_bin_width", gridDividedByBinWidth);
	if(debug) std::cout << cn << mn << "Grid Divided By Bin Width set to: " << (gridDividedByBinWidth ? "ON" : "OFF") << std::endl;

	gridDividedByDoubleDiffBinWidth = reader->GetBoolean("GRAPH", "grid_divided_by_doublediff_bin_width", gridDividedByDoubleDiffBinWidth);
	if(debug) std::cout << cn << mn << "Grid Divided By double differential varibale Bin Width set to: " << (gridDividedByDoubleDiffBinWidth ? "ON" : "OFF") << std::endl;

	referenceDividedByBinWidth = reader->GetBoolean("GRAPH", "reference_divided_by_doublediff_bin_width", referenceDividedByBinWidth);
	if(debug) std::cout << cn << mn << "Reference Divided By Bin Width set to: " << (referenceDividedByBinWidth ? "ON" : "OFF") << std::endl;

	referenceDividedByDoubleDiffBinWidth = reader->GetBoolean("GRAPH", "reference_divided_by_bin_width", referenceDividedByDoubleDiffBinWidth);
	if(debug) std::cout << cn << mn << "Reference Divided By Bin Width set to: " << (referenceDividedByDoubleDiffBinWidth ? "ON" : "OFF") << std::endl;

	//Grid Options [GRID]
	//gridFilepath = reader->Get("GRID", "grid_file", "EMPTY");
	//if(!gridFilepath.compare("EMPTY")) {
	//	throw SPXINIParseException("GRAPH", "grid_file", "You MUST specify the grid_file");
	//} else {
	//	if(debug) std::cout << cn << mn << "Successfully read Grid Filepath: " << gridFilepath << std::endl;
	//}
	//>>>>
	tmp = reader->Get("GRID", "grid_file", "EMPTY");
	if (!tmp.compare("EMPTY")) {
	 if(debug) std::cout << cn << mn << "No correction files were specified" << std::endl;
	 vgridFilepath.clear();
	} else {
	 //Parse into vector
	 vgridFilepath = SPXStringUtilities::CommaSeparatedListToVector(tmp);

	 if (debug) {
	  std::cout << cn << mn << "grid_files string \"" << tmp << "\" parsed into:" << std::endl;
	  for (int i = 0; i < vgridFilepath.size(); i++) {
	   std::cout << "\t " << vgridFilepath.at(i)<<std::endl;
	  }
	  std::cout << " " <<std::endl;
	 }
	}
        //
	//
	tmp = reader->Get("GRID", "correction_files", "EMPTY");
	if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No correction files were specified" << std::endl;
		correctionFiles.clear();
	} else {
		//Parse into vector
		correctionFiles = SPXStringUtilities::CommaSeparatedListToVector(tmp);

		if(debug) {
			std::cout << cn << mn << "correction_files string \"" << tmp << "\" parsed into:" << std::endl;
			for(int i = 0; i < correctionFiles.size(); i++) {
			 std::cout << "\t " << correctionFiles.at(i)<<std::endl;
			}
			std::cout << " " <<std::endl;
		}
	}

        if (correctionFiles.size()>0) {
  	 tmp = reader->Get("GRID", "correction_files_label", "EMPTY");
	 if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No correction file label were specified" << std::endl;
		correctionFilesLabel.clear();
	 } else {
		//Parse into vector
		correctionFilesLabel = SPXStringUtilities::CommaSeparatedListToVector(tmp);

		if(debug) {
			std::cout << cn << mn << "\n correction_files_label string \"" << tmp << "\" parsed into:" << std::endl;
			for(int i = 0; i < correctionFilesLabel.size(); i++) {
				std::cout << "\t " << correctionFilesLabel.at(i);
			}
			std::cout << " " << std::endl;
		}
	 }
         if (correctionFilesLabel.size()!=correctionFiles.size()) {
          std::ostringstream oss;
          oss <<cn<<mn<<"Correction file size "<<correctionFiles.size()
                      <<" is different from correction Label size= "<<correctionFilesLabel.size();
          throw SPXParseException(oss.str());
         }
	}

        if (correctionFiles.size()>0) {
  	 tmp = reader->Get("GRID", "correction_files_program", "EMPTY");
	 if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No correction file program name were specified" << std::endl;
		correctionFilesProgram.clear();
	 } else {
		//Parse into vector
		correctionFilesProgram = SPXStringUtilities::CommaSeparatedListToVector(tmp);

		if(debug) {
			std::cout << cn << mn << "\n correction_files_program string \"" << tmp << "\" parsed into:" << std::endl;
			for(int i = 0; i < correctionFilesProgram.size(); i++) {
				std::cout << "\t " << correctionFilesProgram.at(i);
			}
			std::cout << " " << std::endl;
		}
	 }
         if (correctionFilesProgram.size()!=correctionFiles.size() && correctionFilesProgram.size()!=0 ) {
          std::ostringstream oss;
          oss <<cn<<mn<<"Correction file size "<<correctionFiles.size()
                      <<" is different from correction Program size= "<<correctionFilesProgram.size();
          throw SPXParseException(oss.str());
         }
	}

        if (correctionFiles.size()>0) {
  	 tmp = reader->Get("GRID", "correction_files_author", "EMPTY");
	 if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No correction file author were specified" << std::endl;
		correctionFilesAuthor.clear();
	 } else {
		//Parse into vector
		correctionFilesAuthor = SPXStringUtilities::CommaSeparatedListToVector(tmp);

		if(debug) {
			std::cout << cn << mn << "\n correction_files_author string \"" << tmp << "\" parsed into:" << std::endl;
			for(int i = 0; i < correctionFilesAuthor.size(); i++) {
				std::cout << "\t " << correctionFilesAuthor.at(i);
			}
			std::cout << " " << std::endl;
		}
	 }
         if (correctionFiles.size()!=correctionFilesAuthor.size() && correctionFilesAuthor.size()!=0 ) {
          std::ostringstream oss;
          oss <<cn<<mn<<"Correction file size "<<correctionFiles.size()
                      <<" is different from correction Label size= "<<correctionFilesAuthor.size();
          throw SPXParseException(oss.str());
         }
	}

        if (correctionFiles.size()>0) {
  	 tmp = reader->Get("GRID", "correction_files_reference", "EMPTY");
	 if(!tmp.compare("EMPTY")) {
		if(debug) std::cout << cn << mn << "No correction file reference were specified" << std::endl;
		correctionFilesReference.clear();
	 } else {
		//Parse into vector
		correctionFilesReference = SPXStringUtilities::CommaSeparatedListToVector(tmp);

		if(debug) {
			std::cout << cn << mn << "\n correction_files_reference string \"" << tmp << "\" parsed into:" << std::endl;
			for(int i = 0; i < correctionFilesReference.size(); i++) {
				std::cout << "\t " << correctionFilesReference.at(i);
			}
			std::cout << " " << std::endl;
		}
	 }
         if (correctionFilesReference.size()!=correctionFiles.size() && correctionFilesReference.size()!=0 ) {
          std::ostringstream oss;
          oss <<cn<<mn<<"Correction file size "<<correctionFiles.size()
                      <<" is different from correction Reference size= "<<correctionFilesReference.size();
          throw SPXParseException(oss.str());
         }
	}


	lowestOrder = (unsigned int)reader->GetInteger("GRID", "lowest_order", lowestOrder);
	if(lowestOrder > 2) {
		std::cerr << "WARNING: Lowest Order number unrecognized: " << lowestOrder << " defaulting to NLO (1)" << std::endl;
		lowestOrder = 1;
	} else {
		if(debug) std::cout << cn << mn << "Successfully read Lowest Order: " << lowestOrder << std::endl;
	}


	changesqrts = reader->GetReal("GRID", "change_sqrts", changesqrts);
	if(changesqrts!=0.) {
	 std::cout << "INFO: Calculate cross section for cms energy changed by  " << changesqrts  << std::endl;
	}

	if(changesqrts<0.) {
	 std::cerr << "Changed cms energy needs to be positive Escale is relative energy scale (old/new), i.e. one corresponds to the nominal CMS energy " << std::endl;
	}

	delete reader;
}
