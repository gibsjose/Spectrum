//************************************************************/
//
//	Frame Options Implementation
//
//	Implements the SPXPlotConfiguration class, which describes the options
//	for each frame to plot. The options are:
//
//		data_steering_files -- Comma separated list of all data steering files
//		grid_steering_files -- Comma separated list of all grid steering files
//		marker_style -- Comma separated list of marker style options
//		marker_color -- Comma separated list of marker color options
//
//		Optional options:
//
//			ref_line_style -- Reference line style list
//			ref_line_color -- Reference line color list
//			desc -- Description of the entire plot (not a list)
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		29.09.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/
#include <stdlib.h> //atoi

#include "SPXPlotConfiguration.h"
#include "SPXUtilities.h"

//Class names for debug statements
const std::string cn = "SPXPlotConfiguration::";

//Must define the static debug variable in the implementation
bool SPXPlotConfiguration::debug=false;
bool SPXPlotConfigurationInstance::debug=false;

//Constructs an SPXPlotConfiguration object with a map of string vectors, where keys are
//	options["data_directory"] --> Vector of data directories
//	options["grid_directory"] --> Vector of grid directories
//	options["pdf_directory"] --> Vector of PDF directories
//	options["data_steering_files"] --> Vector of data steering files
//	options["grid_steering_files"] --> Vector of grid steering files
//	options["pdf_steering_files"] --> Vector of PDF steering files
//	options["data_marker_style"] --> Vector of data marker styles
//	options["data_marker_color"] --> Vector of data marker colors
//
//	options["total_fill_style"] --> Vector of Total fill style (optional)
//	options["total_fill_color"] --> Vector of Total fill colors (optional)
//	options["total_edge_color"] --> Vector of Total edge colors (optional)
//	options["total_edge_style"] --> Vector of Total edge style (optional)
//	options["total_marker_style"] --> Vector of Total marker styles (optional)
//
//	options["pdf_fill_style"] --> Vector of PDF fill style (optional)
//	options["pdf_fill_color"] --> Vector of PDF fill colors (optional)
//	options["pdf_edge_color"] --> Vector of PDF edge colors (optional)
//	options["pdf_edge_style"] --> Vector of PDF edge style (optional)
//	options["pdf_marker_style"] --> Vector of PDF marker styles (optional)
//
//	options["scale_fill_style"] --> Vector of scale fill style (optional)
//	options["scale_fill_color"] --> Vector of scale fill colors (optional)
//	options["scale_edge_color"] --> Vector of scale edge colors (optional)
//	options["scale_edge_style"] --> Vector of scale edge style (optional)
//	options["scale_marker_style"] --> Vector of scale marker styles (optional)
//
//	options["alphas_fill_style"] --> Vector of alphas fill style (optional)
//	options["alphas_fill_color"] --> Vector of alphas fill colors (optional)
//	options["alphas_edge_style"] --> Vector of alphas edge style (optional)
//	options["alphas_edge_color"] --> Vector of alphas edge colors (optional)
//	options["alphas_marker_style"] --> Vector of alphas marker styles (optional)
//
//	options["x_scale"] --> Vector of X Scales (optional)
//	options["y_scale"] --> Vector of Y Scales (optional)
//
//	options["plot_type"] --> Plot type
//	options["desc"] --> Description
//	options["display_style"] --> Display style
//	options["overlay_style"] --> Overlay style
//	options["ratio_title"] --> Title for the ratio section
//	options["ratio_style"] --> Vector of ratio Styles
//	options["ratio"] --> Vector of ratio strings
//	options["x_log"] --> X Axis logarithmic
//	options["y_log"] --> Y Axis logarithmic

SPXPlotConfiguration::SPXPlotConfiguration(std::map<std::string, std::vector<std::string> > & options, unsigned int id) {
	this->SetDefaults();

	this->id = id;

	//Attempt to parse the options instances
	try {
		this->Parse(options);
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXParseException("Unable to parse SPXPlotConfiguration instances");
	}
}

//Parse the map of vectors of strings into a vector of SPXPlotConfigurationInstances, convert
//	strings to integers where needed, and check each instance for errors (and IsEmpty) before
//	adding it to the vector (call AddOptionsInstance(), which checks IsEmpty and IsValid)
void SPXPlotConfiguration::Parse(std::map<std::string, std::vector<std::string> > &options) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Vectors for creating the actual instances
	std::vector<std::string> ddr;	//Data directories
	std::vector<std::string> gdr;	//Grid directories
	std::vector<std::string> pdr;	//PDF directories
	std::vector<std::string> dsf;	//Data steering files
	std::vector<std::string> gsf;	//Grid steering files
	std::vector<std::string> psf;	//PDF steering files

	std::vector<std::string> dms;	//Data marker styles
	std::vector<std::string> dmc;	//Data marker colors

	std::vector<std::string> tfs;	//Total fill style
	std::vector<std::string> tfc;	//Total fill color
	std::vector<std::string> tes;	//Total edge style
	std::vector<std::string> tec;	//Total edge color
	std::vector<std::string> tms;	//Total marker style

	std::vector<std::string> pfs;	//PDF fill style
	std::vector<std::string> pfc;	//PDF fill color
	std::vector<std::string> pes;	//PDF edge style
	std::vector<std::string> pec;	//PDF edge color
	std::vector<std::string> pms;	//PDF marker style

	std::vector<std::string> sfs;	//Scale fill style
	std::vector<std::string> sfc;	//Scale fill color
	std::vector<std::string> ses;	//Scale edge style
	std::vector<std::string> sec;	//Scale edge color
	std::vector<std::string> sms;	//Scale marker style

	std::vector<std::string> afs;	//AlphaS fill style
	std::vector<std::string> afc;	//AlphaS fill color
	std::vector<std::string> aes;	//AlphaS edge style
	std::vector<std::string> aec;	//AlphaS edge color
	std::vector<std::string> ams;	//AlphaS marker style

	std::vector<std::string> cfs;	//corrections fill style
	std::vector<std::string> cfc;	//corrections fill color
	std::vector<std::string> ces;	//corrections edge style
	std::vector<std::string> cec;	//corrections edge color
	std::vector<std::string> cms;	//corrections marker style

	std::vector<std::string> xsc;	//X-Scale
	std::vector<std::string> ysc;	//Y-Scale

	//Make sure all required vectors exist
	{
		if(options.count("plot_type") == 0) {
			throw SPXParseException("The options map MUST contain an entry for plot_type");
		}

		if(options.count("desc") == 0) {
			throw SPXParseException("The options map MUST contain an entry for desc");
		}

		if(options.count("display_style") == 0) {
			throw SPXParseException("The options map MUST contain an entry for display_style");
		}

		//@TODO If display style doesn't contain overlay, this shouldn't be required
		if(options.count("overlay_style") == 0) {
			throw SPXParseException("The options map MUST contain an entry for overlay_style");
		}

		//@TODO If display style doesn't contain ratio, these shouldn't be required
		if(options.count("ratio_title") == 0) {
			throw SPXParseException("The options map MUST contain an entry for ratio_title");
		}

		if(options.count("ratio_style") == 0) {
			throw SPXParseException("The options map MUST contain a vector for ratio_style");
		}

		if(options.count("ratio") == 0) {
			throw SPXParseException("The options map MUST contain a vector for ratio");
		}

		if(options.count("x_log") == 0) {
			throw SPXParseException("The options map MUST contain an entry for x_log");
		}

		if(options.count("y_log") == 0) {
			throw SPXParseException("The options map MUST contain an entry for y_log");
		}

		if(options.count("data_directory") == 0) {
			throw SPXParseException("The options map MUST contain a vector for data_directory");
		}

		if(options.count("grid_directory") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for grid_directory");
		  std::cout<<"INFO no grid_directory found !"<<std::endl;
		}

		if(options.count("pdf_directory") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for pdf_directory");
		  std::cout<<"INFO no pdf_directory found !"<<std::endl;
		}

		if(options.count("data_steering_files") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for data_steering_files");
		  std::cout<<"INFO no data_steering_files found !"<<std::endl;
		}

		if(options.count("grid_steering_files") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for grid_steering_files");
		  std::cout<<"INFO no grid_steering_files found !"<<std::endl;
		}

		if(options.count("pdf_steering_files") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for pdf_steering_files");
		  std::cout<<"INFO no pdf_steering_files found !"<<std::endl;
		}
	}

	if(debug) {
		std::cout << cn << mn << "Parsing frame options vector:" << std::endl;

		std::vector<std::string> tmpVector;

		tmpVector = options["plot_type"];
		std::cout << cn << mn <<"\tplot_type = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["desc"];
		std::cout << cn << mn <<"\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["display_style"];
		std::cout <<cn << mn << "\tdisplay_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["overlay_style"];
		std::cout << cn << mn <<"\toverlay_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["ratio_title"];
		std::cout << cn << mn << "\tratio_title = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["ratio_style"];
		std::cout << cn << mn << "\tratio_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["ratio"];
		std::cout << cn << mn << "\tratio = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["x_log"];
		std::cout << cn << mn << "\tx_log = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["y_log"];
		std::cout << cn << mn << "\ty_log = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_directory"];
		std::cout << cn << mn << "\tdata_directory = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["grid_directory"];
		std::cout << cn << mn << "\tgrid_directory = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["pdf_directory"];
		std::cout << cn << mn << "\tpdf_directory = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_steering_files"];
		std::cout << cn << mn << "\tdata_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["grid_steering_files"];
		std::cout << cn << mn << "\tgrid_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["pdf_steering_files"];
		std::cout << cn << mn << "\tpdf_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_marker_style"];
		std::cout << cn << mn << "\tdata_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_marker_color"];
		std::cout << cn << mn << "\tdata_marker_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		//if(debug) std::cout << cn << mn << "HUHU total_fill_style" << std::endl;               

		if(options.count("total_fill_style")) {
			tmpVector = options["total_fill_style"];
			std::cout << cn << mn << "\ttotal_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_fill_style  option specified" << std::endl;
		}


		if(options.count("total_fill_color")) {
			tmpVector = options["total_fill_color"];
			std::cout << cn << mn << "\ttotal_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_fill_color option specified" << std::endl;
		}

		if(options.count("total_edge_color")) {
			tmpVector = options["total_edge_color"];
			std::cout << cn << mn << "\t total_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_edge_color option specified" << std::endl;
		}

		if(options.count("total_edge_style")) {
			tmpVector = options["total_edge_style"];
			std::cout << cn << mn << "\t total_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_edge_style option specified" << std::endl;
		}

		//if(debug) std::cout << cn << mn << "HUHU total_marker_style" << std::endl;               


		if(options.count("total_marker_style")) {
			tmpVector = options["total_marker_style"];
			std::cout <<cn << mn << "\ttotal_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_marker_style option specified" << std::endl;
		}
		
		if(options.count("pdf_fill_style")) {
			tmpVector = options["pdf_fill_style"];
			std::cout << cn << mn << "\tpdf_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No pdf_fill_style option specified" << std::endl;
		}

		if(options.count("pdf_fill_color")) {
			tmpVector = options["pdf_fill_color"];
			std::cout << cn << mn << "\tpdf_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No pdf_fill_color option specified" << std::endl;
		}

		if(options.count("pdf_edge_color")) {
			tmpVector = options["pdf_edge_color"];
			std::cout << cn << mn << "\tpdf_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No pdf_edge_color option specified" << std::endl;
		}

		if(options.count("pdf_edge_style")) {
			tmpVector = options["pdf_edge_style"];
			std::cout << cn << mn << "\tpdf_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No pdf_edge_style option specified" << std::endl;
		}

		if(options.count("pdf_marker_style")) {
			tmpVector = options["pdf_marker_style"];
			std::cout << cn << mn << "\tpdf_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No pdf_marker_style option specified" << std::endl;
		}

		if(options.count("alphas_fill_style")) {
			tmpVector = options["alphas_fill_style"];
			std::cout << cn << mn << "\talphas_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alphas_fill_style option specified" << std::endl;
		}

		if(options.count("alphas_fill_color")) {
			tmpVector = options["alphas_fill_color"];
			std::cout << cn << mn << "\talphas_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alphas_fill_color option specified" << std::endl;
		}

		if(options.count("alphas_edge_style")) {
			tmpVector = options["alphas_edge_style"];
			std::cout << cn << mn << "\talphas_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alphas_edge_style option specified" << std::endl;
		}

		if(options.count("alphas_edge_color")) {
			tmpVector = options["alphas_edge_color"];
			std::cout << cn << mn << "\talphas_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alphas_edge_color option specified" << std::endl;
		}

		if(options.count("alphas_marker_style")) {
			tmpVector = options["alphas_marker_style"];
			std::cout << cn << mn <<"\talphas_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alphas_marker_style option specified" << std::endl;
		}

		if(options.count("scale_fill_style")) {
			tmpVector = options["scale_fill_style"];
			std::cout << cn << mn <<"\tscale_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No scale_fill_style option specified" << std::endl;
		}

		if(options.count("scale_fill_color")) {
			tmpVector = options["scale_fill_color"];
			std::cout << cn<<mn<<"\tscale_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No scale_fill_color option specified" << std::endl;
		}

		if(options.count("scale_edge_style")) {
			tmpVector = options["scale_edge_style"];
			std::cout << cn << mn <<"\tscale_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No scale_edge_style option specified" << std::endl;
		}

		if(options.count("scale_edge_color")) {
			tmpVector = options["scale_edge_color"];
			std::cout << cn<<mn<<"\tscale_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No scale_edge_color option specified" << std::endl;
		}

		if(options.count("scale_marker_style")) {
			tmpVector = options["scale_marker_style"];
			std::cout << cn << mn <<"\tscale_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No scale_marker_style option specified" << std::endl;
		}

		if(options.count("x_scale")) {
			tmpVector = options["x_scale"];
			std::cout << cn<<mn<<"\tx_scale = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No x_scale option specified" << std::endl;
		}

		if(options.count("y_scale")) {
			tmpVector = options["y_scale"];
			std::cout << cn<<mn<<"\ty_scale = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No y_scale option specified" << std::endl;
		}
	}

	//Parse the plot_type
	try {
		plotType = SPXPlotType(options["plot_type"].at(0));
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXParseException(cn+mn+"Could not parse Plot Type");
	}

	//Get vector sizes
	unsigned int rsSize  = options["ratio_style"].size();
	unsigned int rSize   = options["ratio"].size();
	unsigned int ddrSize = options["data_directory"].size();
	unsigned int gdrSize = options["grid_directory"].size();
	unsigned int pdrSize = options["pdf_directory"].size();
	unsigned int dsfSize = options["data_steering_files"].size();
	unsigned int gsfSize = options["grid_steering_files"].size();
	unsigned int psfSize = options["pdf_steering_files"].size();
	unsigned int dmsSize = 0;
	unsigned int dmcSize = 0;

	unsigned int tfsSize = 0;
	unsigned int tfcSize = 0;
	unsigned int tesSize = 0;
	unsigned int tecSize = 0;
	unsigned int tmsSize = 0;

	unsigned int pfsSize = 0;
	unsigned int pfcSize = 0;
	unsigned int pesSize = 0;
	unsigned int pecSize = 0;
	unsigned int pmsSize = 0;

	unsigned int sfsSize = 0;   
	unsigned int sfcSize = 0;	
	unsigned int sesSize = 0;   
	unsigned int secSize = 0;	
	unsigned int smsSize = 0;   

	unsigned int afsSize = 0;   
	unsigned int afcSize = 0;   
	unsigned int aesSize = 0;   
	unsigned int aecSize = 0;   
	unsigned int amsSize = 0;   

	unsigned int cfsSize = 0;   
	unsigned int cfcSize = 0;   
	unsigned int cesSize = 0;   
	unsigned int cecSize = 0;   
	unsigned int cmsSize = 0;   

	unsigned int xsSize  = 0;
	unsigned int ysSize  = 0;

	if(options.count("data_marker_style")) {
		dmsSize = options["data_marker_style"].size();
	}
	if(options.count("data_marker_color")) {
		dmcSize = options["data_marker_color"].size();
	}

	if(options.count("total_fill_style")) {
		tfsSize = options["total_fill_style"].size();
	}
	if(options.count("total_fill_color")) {
		tfcSize = options["total_fill_color"].size();
	}	
	if(options.count("total_edge_color")) {
		tecSize = options["total_edge_color"].size();
	}	
	if(options.count("total_edge_style")) {
		tesSize = options["total_edge_style"].size();
	}	

	if(options.count("total_marker_style")) {
		tmsSize = options["total_marker_style"].size();
	}

	if(options.count("pdf_fill_style")) {
		pfsSize = options["pdf_fill_style"].size();
	}
	if(options.count("pdf_fill_color")) {
		pfcSize = options["pdf_fill_color"].size();
	}	
	if(options.count("pdf_edge_color")) {
		pecSize = options["pdf_edge_color"].size();
	}	
	if(options.count("pdf_edge_style")) {
		pesSize = options["pdf_edge_style"].size();
	}	
	if(options.count("pdf_marker_style")) {
		pmsSize = options["pdf_marker_style"].size();
	}

	if(options.count("scale_fill_style")) {
		sfsSize = options["scale_fill_style"].size();
	}
	if(options.count("scale_fill_color")) {
		sfcSize = options["scale_fill_color"].size();
	}
	if(options.count("scale_edge_style")) {
		sesSize = options["scale_edge_style"].size();
	}
	if(options.count("scale_edge_color")) {
		secSize = options["scale_edge_color"].size();
	}
	if(options.count("scale_marker_style")) {
		smsSize = options["scale_marker_style"].size();
	}

	if(options.count("alphas_fill_style")) {
		afsSize = options["alphas_fill_style"].size();
	}
	if(options.count("alphas_fill_color")) {
		afcSize = options["alphas_fill_color"].size();
	}
	if(options.count("alphas_edge_style")) {
		aesSize = options["alphas_edge_style"].size();
	}
	if(options.count("alphas_edge_color")) {
		aecSize = options["alphas_edge_color"].size();
	}
	if(options.count("alphas_marker_style")) {
		amsSize = options["alphas_marker_style"].size();
	}

	if(options.count("corrections_fill_style")) {
		cfsSize = options["corrections_fill_style"].size();
	}
	if(options.count("corrections_fill_color")) {
		cfcSize = options["corrections_fill_color"].size();
	}
	if(options.count("corrections_edge_style")) {
		cesSize = options["corrections_edge_style"].size();
	}
	if(options.count("corrections_edge_color")) {
		cecSize = options["corrections_edge_color"].size();
	}
	if(options.count("corrections_marker_style")) {
		cmsSize = options["corrections_marker_style"].size();
	}

	if(options.count("x_scale")) {
		xsSize = options["x_scale"].size();
	}
	if(options.count("y_scale")) {
		ysSize = options["y_scale"].size();
	}

	if(debug) std::cout << cn << mn << "finished options " << std::endl;

	//Check vector sizes that do not depend on the plot type
	if(rSize != rsSize) {
		std::ostringstream oss;
		oss << "Size of ratio vector (" << rSize << ") DOES NOT match the size of the ratio_style vector (" << rsSize << ")";
		throw SPXParseException(oss.str());
	}

	if((dmsSize != 0) && (dmsSize != dsfSize)) {
		std::ostringstream oss;
		oss << "Size of data_marker_style vector (" << dmsSize << ") DOES NOT match the size of the data_steering_files vector (" << dsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((dmcSize != 0) && (dmcSize != dsfSize)) {
		std::ostringstream oss;
		oss << "Size of data_marker_color vector (" << dmcSize << ") DOES NOT match the size of the data_steering_files vector (" << dsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((xsSize > 1) && (xsSize != dsfSize)) {
		std::ostringstream oss;
		oss << "Size of x_scale vector (" << xsSize << ") DOES NOT match the size of the data_steering_files vector (" << dsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((ysSize > 1) && (ysSize != dsfSize)) {
		std::ostringstream oss;
		oss << "Size of y_scale vector (" << ysSize << ") DOES NOT match the size of the data_steering_files vector (" << dsfSize << ")";
		throw SPXParseException(oss.str());
	}


	if((tfsSize != 0) && (tfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of total_fill_style vector (" << tfsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((tfcSize != 0) && (tfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of total_fill_color vector (" << tfcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((tecSize != 0) && (tecSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of total_edge_color vector (" << tecSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((tesSize != 0) && (tesSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of total_edge_style vector (" << tesSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}


	if((tmsSize != 0) && (tmsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of total_marker_style vector (" << tmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((pfsSize != 0) && (pfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_fill_style vector (" << pfsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
	if((pfcSize != 0) && (pfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_fill_color vector (" << pfcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((pesSize != 0) && (pesSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_edge_style vector (" << pesSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
	if((pecSize != 0) && (pecSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_edge_color vector (" << pecSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((pmsSize != 0) && (pmsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_marker_style vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((sfsSize != 0) && (sfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_fill_style vector (" << sfsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((sfcSize != 0) && (sfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_fill_color vector (" << sfcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((sesSize != 0) && (sesSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_edge_style vector (" << sesSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((secSize != 0) && (secSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_edge_color vector (" << secSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}



	if((smsSize != 0) && (smsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_marker_style vector (" << smsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((afsSize != 0) && (afsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_fill_style vector (" << afsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((afcSize != 0) && (afcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_fill_color vector (" << afcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((aesSize != 0) && (aesSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_edge_style vector (" << aesSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((aecSize != 0) && (aecSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_edge_color vector (" << aecSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((amsSize != 0) && (amsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_marker_style vector (" << amsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((cfsSize != 0) && (cfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of corrections_fill_style vector (" << cfsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((cfcSize != 0) && (cfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of corrections_fill_color vector (" << cfcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((cesSize != 0) && (cesSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of corrections_edge_style vector (" << cesSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((cecSize != 0) && (cecSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of corrections_edge_color vector (" << cecSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((cmsSize != 0) && (cmsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of corrections_marker_style vector (" << cmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
       

	//Check directory size: Could either be of length 1, in which case the directory is prepended to each steering file, or
	//	the exact length of the steering file list
	if((ddrSize != 1) && (ddrSize != dsfSize)) {
		std::ostringstream oss;
		oss << "Size of data_directory vector (" << ddrSize << ") MUST be either '1' or the size of the data_steering_files vector (" << dsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((gdrSize != 1) && (gdrSize != gsfSize)) {
		std::ostringstream oss;
		oss << "Size of grid_directory vector (" << gdrSize << ") MUST be either '1' or the size of the grid_steering_files vector (" << gsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((pdrSize != 1) && (pdrSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_directory vector (" << pdrSize << ") MUST be either '1' or the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	//Check the rest of the vector sizes based on the plot type
	// AND determine the number of configuration instances
	unsigned int numberOfConfigurationInstances;

	//data, grid, pdf
	if(plotType.IsType1()) {
	  //if(dsfSize != 1) {
	  //		throw SPXParseException("Size of data_steering_files vector MUST be 1 (ONE) for Plot Type 1 (data, grid, pdf)");
	  //	}
		//if(gsfSize != 1) {
		//	throw SPXParseException("Size of grid_steering_files vector MUST be 1 (ONE) for Plot Type 1 (data, grid, pdf)");
		//}
		//if(psfSize != 1) {
		//	throw SPXParseException("Size of pdf_steering_files vector MUST be 1 (ONE) for Plot Type 1 (data, grid, pdf)");
		//}

  	        if(debug) std::cout << cn << mn << "plotType is 1 ! " << std::endl;

		numberOfConfigurationInstances = 1;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {



  	          //if(debug) std::cout << cn << mn << "data_steering_files count= " <<
		  //	      options.count("data_steering_files")<< std::endl;

  	          //if(debug) std::cout << cn << mn << "data_steering_files size= " <<
		  //options["data_steering_files"].size()<< std::endl;


                  if (options["data_steering_files"].size()>0)
		   dsf.push_back(options["data_steering_files"].at(0));	//Just one instance of data steering file

  	          //if(debug) std::cout << cn << mn << "grid_steering_files " << std::endl;

                  if (options["grid_steering_files"].size()>0)
		   gsf.push_back(options["grid_steering_files"].at(0));	//Just one instance of grid steering file

  	          //if(debug) std::cout << cn << mn << "pdf_steering_files " << std::endl;

                  if (options["pdf_steering_files"].size()>0)
		   psf.push_back(options["pdf_steering_files"].at(0));	//Just one instance of pdf steering file

  	          if(debug) std::cout << cn << mn << "data_marker_style " << std::endl;

		   if(dmsSize) dms.push_back(options["data_marker_style"].at(0));		//Just one instance of data marker style

		   if(dmcSize) dmc.push_back(options["data_marker_color"].at(0));		//Just one instance of data marker color
		   if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));		//Just one instance of total fill style
		   if(tfcSize) tfc.push_back(options["total_fill_color"].at(0));		//Just one instance of total fill color
		   if(tecSize) tec.push_back(options["total_edge_color"].at(0));		//Just one instance of total edge  color
		   if(tesSize) tes.push_back(options["total_edge_style"].at(0));		//Just one instance of total edge style
		   if(tmsSize) tms.push_back(options["total_marker_style"].at(0));		//Just one instance of total marker style

		   if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(0));		//Just one instance of pdf fill style
		   if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(0));		//Just one instance of pdf fill color
		   if(pesSize) pes.push_back(options["pdf_edge_style"].at(0));		//Just one instance of pdf edge style
		   if(pecSize) pec.push_back(options["pdf_edge_color"].at(0));		//Just one instance of pdf edge color
		   if(pmsSize) pms.push_back(options["pdf_marker_style"].at(0));		//Just one instance of pdf marker style

		   if(sfsSize) sfs.push_back(options["scale_fill_style"].at(0));		
		   if(sfcSize) sfc.push_back(options["scale_fill_color"].at(0));		
		   if(sesSize) ses.push_back(options["scale_edge_style"].at(0));		
		   if(secSize) sec.push_back(options["scale_edge_color"].at(0));		
		   if(smsSize) sms.push_back(options["scale_marker_style"].at(0));		

		   if(afsSize) afs.push_back(options["alphas_fill_style"].at(0));		
		   if(afcSize) afc.push_back(options["alphas_fill_color"].at(0));		
		   if(aesSize) aes.push_back(options["alphas_edge_style"].at(0));		
		   if(aecSize) aec.push_back(options["alphas_edge_color"].at(0));		
		   if(amsSize) ams.push_back(options["alphas_marker_style"].at(0));		

		   if(cfsSize) cfs.push_back(options["corrections_fill_style"].at(0));		
		   if(cfcSize) cfc.push_back(options["corrections_fill_color"].at(0));		
		   if(cesSize) ces.push_back(options["corrections_edge_style"].at(0));		
		   if(cecSize) cec.push_back(options["corrections_edge_color"].at(0));		
		   if(cmsSize) cms.push_back(options["corrections_marker_style"].at(0));		

		   if(xsSize) 	xsc.push_back(options["x_scale"].at(0));				//Just one instance of x scale
		   else	        xsc.push_back("1.0");

		   if(ysSize) 	ysc.push_back(options["y_scale"].at(0));				//Just one instance of y scale
                  else		ysc.push_back("1.0");
		}
	}

	//data[], grid[], pdf
	else if(plotType.IsType2()) {
		if(dsfSize <= 1) {
			throw SPXParseException("Size of data_steering_files vector MUST be > 1 for Plot Type 2 (data[], grid[], pdf)");
		}
		if(gsfSize <= 1) {
			throw SPXParseException("Size of grid_steering_files vector MUST be > 1 for Plot Type 2 (data[], grid[], pdf)");
		}
		if(psfSize != 1) {
			throw SPXParseException("Size of pdf_steering_files vector MUST be 1 (ONE) for Plot Type 2 (data[], grid[], pdf)");
		}

		if(dsfSize != gsfSize) {
			throw SPXParseException("Size of data_steering_files vector MUST match the size of the grid_steering_files vector for Plot Type 2");
		}

		numberOfConfigurationInstances = dsfSize;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  dsf.push_back(options["data_steering_files"].at(i));
		  gsf.push_back(options["grid_steering_files"].at(i));
		  psf.push_back(options["pdf_steering_files"].at(0));	//Just one instance of pdf steering file

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(i));
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(i));

		  if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));		//Just one instance of pdf fill style
		  if(tfcSize) tfc.push_back(options["total_fill_color"].at(0));		//Just one instance of pdf fill color
		  if(tecSize) tfc.push_back(options["total_edge_color"].at(0));		//Just one instance of pdf edge color
		  if(tecSize) tfc.push_back(options["total_edge_style"].at(0));		//Just one instance of pdf edge style
		  if(tmsSize) tms.push_back(options["total_marker_style"].at(0));		//Just one instance of pdf marker style

		  if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(0));		//Just one instance of pdf fill style
		  if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(0));		//Just one instance of pdf fill color
		  if(pesSize) pfs.push_back(options["pdf_edge_style"].at(0));		//Just one instance of pdf edge style
		  if(pecSize) pfc.push_back(options["pdf_edge_color"].at(0));		//Just one instance of pdf edge color
		  if(pmsSize) pms.push_back(options["pdf_marker_style"].at(0));		//Just one instance of pdf marker style

		  if(sfsSize) sfs.push_back(options["scale_fill_style"].at(0));	   
		  if(sfcSize) sfc.push_back(options["scale_fill_color"].at(0));	        //Just one instance of scale fill style
		  if(sesSize) sfs.push_back(options["scale_edge_style"].at(0));	   
		  if(secSize) sfc.push_back(options["scale_edge_color"].at(0));	        //Just one instance of scale fill style
		  if(smsSize) sms.push_back(options["scale_marker_style"].at(0));	   

		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(0));	   
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(0));	        //Just one instance of alphas fill style
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(0));	   
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(0));	        //Just one instance of alphas edge style
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(0));	   

		  if(cfsSize) cfs.push_back(options["corrections_fill_style"].at(0));	   
		  if(cfcSize) cfc.push_back(options["corrections_fill_color"].at(0));        //Just one instance of correction fill style
		  if(cesSize) ces.push_back(options["corrections_edge_style"].at(0));	   
		  if(cecSize) cec.push_back(options["corrections_edge_color"].at(0));        //Just one instance of correction edge style
		  if(cmsSize) cms.push_back(options["corrections_marker_style"].at(0));	   

		  if(xsSize > 1) 			xsc.push_back(options["x_scale"].at(i));
		  else if(xsSize == 1)	xsc.push_back(options["x_scale"].at(0));
			else					xsc.push_back("1.0");

			if(ysSize > 1) 			ysc.push_back(options["y_scale"][i]);
			else if(ysSize == 1)	ysc.push_back(options["y_scale"].at(0));
			else					ysc.push_back("1.0");
		}
	}

	//data, grid, pdf[]
	else if(plotType.IsType3()) {
		if(dsfSize != 1) {
			throw SPXParseException("Size of data_steering_files vector MUST be 1 (ONE) for Plot Type 3 (data, grid, pdf[])");
		}
		if(gsfSize != 1) {
			throw SPXParseException("Size of grid_steering_files vector MUST be 1 (ONE) for Plot Type 3 (data, grid, pdf[])");
		}
		if(psfSize <= 1) {
			throw SPXParseException("Size of pdf_steering_files vector MUST be > 1 for Plot Type 3 (data, grid, pdf[])");
		}

		numberOfConfigurationInstances = psfSize;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  dsf.push_back(options["data_steering_files"].at(0));	//Just one instance of data steering file
		  gsf.push_back(options["grid_steering_files"].at(0));	//Just one instance of grid steering file
		  psf.push_back(options["pdf_steering_files"].at(i));

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(0));		//Just one instance of data marker style
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(0));		//Just one instance of data marker color

		  if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));
		  if(tfcSize) tfc.push_back(options["total_fill_color"].at(i));
		  if(tecSize) tfc.push_back(options["total_edge_color"].at(i));
		  if(tesSize) tfc.push_back(options["total_edge_style"].at(i));
		  if(tmsSize) tms.push_back(options["total_marker_style"].at(i));

		  if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(i));
		  if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(i));
		  if(pesSize) pfs.push_back(options["pdf_edge_style"].at(i));
		  if(pecSize) pfc.push_back(options["pdf_edge_color"].at(i));
		  if(pmsSize) pms.push_back(options["pdf_marker_style"].at(i));

		  if(sfsSize) sfs.push_back(options["scale_fill_style"].at(i));
		  if(sfcSize) sfc.push_back(options["scale_fill_color"].at(i));
		  if(sesSize) ses.push_back(options["scale_edge_style"].at(i));
		  if(secSize) sec.push_back(options["scale_edge_color"].at(i));
		  if(smsSize) sms.push_back(options["scale_marker_style"].at(i));


		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(i));
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(i));
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(i));
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(i));
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(i));

		  if(cfsSize) cfs.push_back(options["corrections_fill_style"].at(i));
		  if(cfcSize) cfc.push_back(options["corrections_fill_color"].at(i));
		  if(cesSize) ces.push_back(options["corrections_edge_style"].at(i));
		  if(cecSize) cec.push_back(options["corrections_edge_color"].at(i));
		  if(cmsSize) cms.push_back(options["corrections_marker_style"].at(i));

		  if(xsSize) 	xsc.push_back(options["x_scale"].at(0));				//Just one instance of x scale
			else		xsc.push_back("1.0");

		  if(ysSize) 	ysc.push_back(options["y_scale"].at(0));				//Just one instance of y scale
			else		ysc.push_back("1.0");
		}
	}

	//data, grid[], pdf
	else if(plotType.IsType4()) {
		if(dsfSize != 1) {
			throw SPXParseException("Size of data_steering_files vector MUST be 1 (ONE) for Plot Type 4 (data, grid[], pdf)");
		}
		if(gsfSize <= 1) {
			throw SPXParseException("Size of grid_steering_files vector MUST be > 1 for Plot Type 4 (data, grid[], pdf)");
		}
		if(psfSize != 1) {
			throw SPXParseException("Size of pdf_steering_files vector MUST be 1 (ONE) for Plot Type 4 (data, grid[], pdf)");
		}

		numberOfConfigurationInstances = gsfSize;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
                        //Just one instance of data steering file
		  dsf.push_back(options["data_steering_files"].at(0));	
		  gsf.push_back(options["grid_steering_files"].at(i));
		  psf.push_back(options["pdf_steering_files"].at(0));	

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(0));
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(0));

		  if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));
		  if(tfcSize) tfc.push_back(options["total_fill_color"].at(0));
		  if(tecSize) tfc.push_back(options["total_edge_color"].at(0));
		  if(tesSize) tfc.push_back(options["total_edge_style"].at(0));
		  if(tmsSize) tms.push_back(options["total_marker_style"].at(0));

		  if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(0));
		  if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(0));
		  if(pesSize) pfs.push_back(options["pdf_edge_style"].at(0));
		  if(pecSize) pfc.push_back(options["pdf_edge_color"].at(0));
		  if(pmsSize) pms.push_back(options["pdf_marker_style"].at(0));

		  if(sfsSize) sfs.push_back(options["scale_fill_style"].at(0));
		  if(sfcSize) sfc.push_back(options["scale_fill_color"].at(0));
		  if(sesSize) ses.push_back(options["scale_edge_style"].at(0));
		  if(secSize) sec.push_back(options["scale_edge_color"].at(0));
		  if(smsSize) sms.push_back(options["scale_marker_style"].at(0));

		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(0));
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(0));
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(0));
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(0));
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(0));

		  if(cfsSize) cfs.push_back(options["corrections_fill_style"].at(0));
		  if(cfcSize) cfc.push_back(options["corrections_fill_color"].at(0));
		  if(cesSize) ces.push_back(options["corrections_edge_style"].at(0));
		  if(cecSize) cec.push_back(options["corrections_edge_color"].at(0));
		  if(cmsSize) cms.push_back(options["corrections_marker_style"].at(0));


		  if(xsSize) 	xsc.push_back(options["x_scale"].at(0));				
			else		xsc.push_back("1.0");

		  if(ysSize) 	ysc.push_back(options["y_scale"].at(0));				
			else		ysc.push_back("1.0");
		}
	}

	//Process directories
	if(ddrSize == 1) {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  ddr.push_back(options["data_directory"].at(0));
		}
	} else {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  ddr.push_back(options["data_directory"].at(i));
		}
	}

	if(gdrSize == 1) {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  gdr.push_back(options["grid_directory"].at(0));
		}
	} else {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  gdr.push_back(options["grid_directory"].at(i));
		}
	}

	if(pdrSize == 1) {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  pdr.push_back(options["pdf_directory"].at(0));
		}
	} else {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  pdr.push_back(options["pdf_directory"].at(i));
		}
	}

	//DEBUG: Print directories:
	if(debug) {
		for(int i = 0; i < ddr.size(); i++) {
			std::cout << cn << mn << "Data Directory [" << i << "]: " << ddr.at(i) << std::endl;
		}
		for(int i = 0; i < gdr.size(); i++) {
			std::cout << cn << mn << "Grid Directory [" << i << "]: " << gdr.at(i) << std::endl;
		}
		for(int i = 0; i < pdr.size(); i++) {
			std::cout << cn << mn << "PDF Directory [" << i << "]: " << pdr.at(i) << std::endl;
		}
	}

	//Set the general plot configuration data
	description = options["desc"].at(0);
	displayStyle = SPXDisplayStyle(options["display_style"].at(0));
	overlayStyle = SPXOverlayStyle(options["overlay_style"].at(0));
	ratioTitle = options["ratio_title"].at(0);

	//Parse the ratio_style vector if the display style contains 'ratio'
	if (debug) std::cout << cn << mn << "Loop over ratio_style" << std::endl;
	if(displayStyle.ContainsRatio()) {
		for(int i = 0; i < options["ratio_style"].size(); i++) {
			SPXRatioStyle rs;

			try {
				rs = SPXRatioStyle(options["ratio_style"].at(i), id, i);
				ratioStyles.push_back(rs);
			} catch(const SPXException &e) {
				std::cerr << e.what() << std::endl;

				std::ostringstream oss;
				oss << "Unable to parse ratio_style_" << i;
				throw SPXParseException(oss.str());
			}
		}
      	        if (debug) std::cout << cn << mn << "SPXRatioStyle finished" << std::endl;
		//Copy the ratio string vector
		ratios = options["ratio"];
	} else {
		ratioStyles.clear();
		ratios.clear();
	}

        if (debug) std::cout << cn << mn << "xLog and yLog" << std::endl;
	//Parse the xLog and yLog
	std::string xls = options["x_log"].at(0);
	std::string yls = options["y_log"].at(0);

	if(!xls.compare("true")) {
		xLog = true;
	} else {
		xLog = false;
	}

	if(!yls.compare("true")) {
		yLog = true;
	} else {
		yLog = false;
	}

	//For each options instance, create an SPXPlotConfigurationInstance object and add it to the vector
        if (debug) std::cout << cn << mn << "Loop over numberOfConfigurationInstances= "<<numberOfConfigurationInstances << std::endl;
	for(int i = 0; i < numberOfConfigurationInstances; i++) {
		SPXPlotConfigurationInstance pci;

		pci.SetDefaults();

		pci.id = i;

                //if (debug) std::cout << cn << mn << " ddr.size()= "<<ddr.size()<< std::endl;
		//Copy from vectors to pci
                if (ddr.size()>i) {
		 pci.dataDirectory = ddr.at(i);
                if (dsf.size()>i) 
		 pci.dataSteeringFile = SPXDataSteeringFile(ddr[i] + "/" + dsf.at(i));
                }

                //if (debug) std::cout << cn << mn << " gdr.size()= "<<gdr.size()<< std::endl;
                if (gdr.size()>i) {
 		 pci.gridDirectory = gdr.at(i);
                 if (gsf.size()>i) 
		  pci.gridSteeringFile = SPXGridSteeringFile(gdr[i] + "/" + gsf.at(i));
                }

                //if (debug) std::cout << cn << mn << " pdr.size()= "<<pdr.size()<< std::endl;
                if (pdr.size()>i) {
		 pci.pdfDirectory  = pdr.at(i);
                 if (psf.size()>i) 
		  pci.pdfSteeringFile  = SPXPDFSteeringFile(pdr[i]  + "/" + psf.at(i));
                }

                if (debug) std::cout << cn << mn << "Now do plot options "<< std::endl;

		if(dmsSize) pci.dataMarkerStyle = atoi(dms.at(i).c_str());
		else 		pci.dataMarkerStyle = PC_EMPTY_STYLE;

		if(dmcSize) pci.dataMarkerColor = atoi(dmc.at(i).c_str());
		else	 	pci.dataMarkerColor = PC_EMPTY_COLOR;

		if(tfsSize) pci.totalFillStyle = atoi(tfs.at(i).c_str());
		else 		pci.totalFillStyle = PC_EMPTY_STYLE;

		if(tfcSize) pci.totalFillColor = atoi(tfc.at(i).c_str());
		else 		pci.totalFillColor = PC_EMPTY_COLOR;

		if(tesSize) pci.totalEdgeStyle = atoi(tes.at(i).c_str());
		else 		pci.totalEdgeStyle = PC_EMPTY_EDGE;

		if(tecSize) pci.totalEdgeColor = atoi(tec.at(i).c_str());
		else 		pci.totalEdgeColor = PC_EMPTY_EDGE;

		if(tmsSize) pci.totalMarkerStyle = atoi(tms.at(i).c_str());
		else		pci.totalMarkerStyle = PC_EMPTY_STYLE;

		if(pfsSize) pci.pdfFillStyle = atoi(pfs.at(i).c_str());
		else 		pci.pdfFillStyle = PC_EMPTY_STYLE;

		if(pfcSize) pci.pdfFillColor = atoi(pfc.at(i).c_str());
		else 		pci.pdfFillColor = PC_EMPTY_COLOR;

		if(pesSize) pci.pdfEdgeStyle = atoi(pes.at(i).c_str());
		else 		pci.pdfEdgeStyle = PC_EMPTY_EDGE;

		if(pecSize) pci.pdfEdgeColor = atoi(pec.at(i).c_str());
		else 		pci.pdfEdgeColor = PC_EMPTY_EDGE;

		if(pmsSize) pci.pdfMarkerStyle = atoi(pms.at(i).c_str());
		else		pci.pdfMarkerStyle = PC_EMPTY_STYLE;

		if(sfsSize) pci.scaleFillStyle = atoi(sfs.at(i).c_str());
		else 		pci.scaleFillStyle = PC_EMPTY_STYLE;

		if(sfcSize) pci.scaleFillColor = atoi(sfc.at(i).c_str()); 
		else 		pci.scaleFillColor = PC_EMPTY_COLOR;

		if(sesSize) pci.scaleEdgeStyle = atoi(ses.at(i).c_str());
		else 		pci.scaleEdgeStyle = PC_EMPTY_EDGE;

		if(secSize) pci.scaleEdgeColor = atoi(sec.at(i).c_str());
		else 		pci.scaleEdgeColor = PC_EMPTY_EDGE;

		if(smsSize) pci.scaleMarkerStyle = atoi(sms.at(i).c_str());
		else		pci.scaleMarkerStyle = PC_EMPTY_STYLE;

		if(afsSize) pci.alphasFillStyle = atoi(afs.at(i).c_str());
		else 		pci.alphasFillStyle = PC_EMPTY_STYLE;

		if(afcSize) pci.alphasFillColor = atoi(afc.at(i).c_str());
		else 		pci.alphasFillColor = PC_EMPTY_COLOR;

		if(aesSize) pci.alphasEdgeStyle = atoi(aes.at(i).c_str());
		else 		pci.alphasEdgeStyle = PC_EMPTY_EDGE;

		if(aecSize) pci.alphasEdgeColor = atoi(aec.at(i).c_str());
		else 		pci.alphasEdgeColor = PC_EMPTY_EDGE;

		if(amsSize) pci.alphasMarkerStyle = atoi(ams.at(i).c_str());
		else		pci.alphasMarkerStyle = PC_EMPTY_STYLE;

		if(cfsSize) pci.correctionsFillStyle = atoi(cfs.at(i).c_str());
		else 		pci.correctionsFillStyle = PC_EMPTY_STYLE;
  
		if(cfcSize) pci.correctionsFillColor = atoi(cfc.at(i).c_str());
		else 		pci.correctionsFillColor = PC_EMPTY_COLOR;

		if(cesSize) pci.correctionsEdgeStyle = atoi(ces.at(i).c_str());
		else 		pci.correctionsEdgeStyle = PC_EMPTY_STYLE;

		if(cecSize) pci.correctionsEdgeColor = atoi(cec.at(i).c_str());
		else 		pci.correctionsEdgeColor = PC_EMPTY_EDGE;

		if(cmsSize) pci.correctionsMarkerStyle = atoi(cms.at(i).c_str());
		else		pci.correctionsMarkerStyle = PC_EMPTY_STYLE;

		if(xsSize)	pci.xScale = atof(xsc.at(i).c_str());
		else		pci.xScale = 1.0;

		if(ysSize)	pci.yScale = atof(ysc.at(i).c_str());
		else		pci.yScale = 1.0;

		//Attempt to add the configuration instance
		try {
			AddConfigurationInstance(pci);
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;

			throw SPXParseException("ERROR: Could not add options configuration to configuration instance vector: Instance was empty or invalid");
		}
	}
}

//Determines whether the Frame Options object is empty or not
bool SPXPlotConfiguration::IsEmpty(void) {
	std::string mn = "IsEmpty: ";

	//Return true if configurations instance vector is empty
	if(configurationInstances.size() == 0) {
		return true;
	}

	return false;
}

//Determines the validity of the frame options
bool SPXPlotConfiguration::IsValid(void) {
	std::string mn = "IsValid: ";

	//Valid, but empty
	if(IsEmpty()) {
		return true;
	}

	//Return false if ANY of the frame options are invalid
	for(int i = 0; i < configurationInstances.size(); i++) {
		if(!configurationInstances[i].IsValid()) {
			if(debug) std::cout << cn << mn << "An invalid options instance was found at index " << i << std::endl;
			return false;
		}
	}

	return true;
}
