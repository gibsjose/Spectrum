
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
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
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

//	options["grid_marker_style"] --> Vector of grid marker styles
//	options["grid_marker_color"] --> Vector of grid marker colors
//	options["grid_fill_style"] --> Vector of grid marker styles
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

//	options["AlternativeScale_fill_style"] --> Vector of alternative scale choice fill style (optional)
//	options["AlternativeScale_fill_color"] --> Vector of alternative scale choice fill colors (optional)
//	options["AlternativeScale_edge_color"] --> Vector of alternative scale choice edge colors (optional)
//	options["AlternativeScale_edge_style"] --> Vector of alternative scale choice edge style (optional)
//	options["AlternativeScale_marker_style"] --> Vector of alternative scale choice marker styles (optional)
//
//	options["alphas_fill_style"] --> Vector of alphas fill style (optional)
//	options["alphas_fill_color"] --> Vector of alphas fill colors (optional)
//	options["alphas_edge_style"] --> Vector of alphas edge style (optional)
//	options["alphas_edge_color"] --> Vector of alphas edge colors (optional)
//	options["alphas_marker_style"] --> Vector of alphas marker styles (optional)

//	options["beamuncertainty_fill_style"] --> Vector of beamuncertainty fill style (optional)
//	options["beamuncertainty_fill_color"] --> Vector of beamuncertainty fill colors (optional)
//	options["beamuncertainty_edge_style"] --> Vector of beamuncertainty edge style (optional)
//	options["beamuncertainty_edge_color"] --> Vector of beamuncertainty edge colors (optional)
//	options["beamuncertainty_marker_style"] --> Vector of beamuncertainty marker styles (optional)
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

		throw SPXParseException(cn+"Unable to parse SPXPlotConfiguration instances");
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

	std::vector<std::string> gms;	//Grid marker styles
	std::vector<std::string> gmc;	//Grid marker colors
	std::vector<std::string> gfs;	//Grid fill styles

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

	std::vector<std::string> xfs;	//alternative Scale choice fill style
	std::vector<std::string> xfc;	//alternative Scale choice fill color
	std::vector<std::string> xes;	//alternative Scale choice edge style
	std::vector<std::string> xec;	//alternative Scale choice edge color
	std::vector<std::string> xms;	//alternative Scale choice marker style

	std::vector<std::string> afs;	//AlphaS fill style
	std::vector<std::string> afc;	//AlphaS fill color
	std::vector<std::string> aes;	//AlphaS edge style
	std::vector<std::string> aec;	//AlphaS edge color
	std::vector<std::string> ams;	//AlphaS marker style

	std::vector<std::string> bfs;	//beamuncertainty fill style
	std::vector<std::string> bfc;	//beamuncertainty fill color
	std::vector<std::string> bes;	//beamuncertainty edge style
	std::vector<std::string> bec;	//beamuncertainty edge color
	std::vector<std::string> bms;	//beamuncertainty marker style

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
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for plot_type");
		}

		if(options.count("desc") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for desc");
		}

		if(options.count("display_style") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for display_style");
		}

		//@TODO If display style doesn't contain overlay, this shouldn't be required
		if(options.count("overlay_style") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for overlay_style");
		}

		//@TODO If display style doesn't contain ratio, these shouldn't be required
		if(options.count("ratio_title") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for ratio_title");
		}

		if(options.count("ratio_style") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain a vector for ratio_style");
		}

		if(options.count("ratio") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain a vector for ratio");
		}

		if(options.count("x_log") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for x_log");
		}

		if(options.count("y_log") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain an entry for y_log");
		}

		if(options.count("data_directory") == 0) {
			throw SPXParseException(cn+mn+"The options map MUST contain a vector for data_directory");
		}

		if(options.count("grid_directory") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for grid_directory");
		  std::cout<<cn<<mn<<"INFO no grid_directory found !"<<std::endl;
		}

		if(options.count("pdf_directory") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for pdf_directory");
		  std::cout<<cn<<mn<<"INFO no pdf_directory found !"<<std::endl;
		}

		if(options.count("data_steering_files") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for data_steering_files");
		  std::cout<<cn<<mn<<"INFO no data_steering_files found !"<<std::endl;
		}

		if(options.count("grid_steering_files") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for grid_steering_files");
		  std::cout<<cn<<mn<<"INFO no grid_steering_files found !"<<std::endl;
		}

		if(options.count("pdf_steering_files") == 0) {
		  //throw SPXParseException("The options map MUST contain a vector for pdf_steering_files");
		  std::cout<<cn<<mn<<"INFO no pdf_steering_files found !"<<std::endl;
		}
	}

	if(debug) {
		std::cout << cn << mn << "Parsing frame options vector:" << std::endl;

		std::vector<std::string> tmpVector;

		tmpVector = options["plot_type"];
		std::cout << cn << mn <<"\tplot_type = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["desc"];
		std::cout << cn << mn <<"\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

                if (options.count("y_overlay_min")>0) {
		 tmpVector = options["y_overlay_min"];
		 std::cout << cn << mn <<"\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else
                 std::cout << cn << mn <<"\t x_overlay_min not found " << std::endl;

                if (options.count("y_overlay_max")>0) {
		 tmpVector = options["y_overlay_max"];
		 std::cout << cn << mn <<"\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else
                 std::cout << cn << mn <<"\t x_overlay_max not found " << std::endl;

                if (options.count("y_ratio_max")>0) {
		 tmpVector = options["y_ratio_max"];
		 std::cout << cn << mn <<"\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else
                 std::cout << cn << mn <<"\t y_ratio_max not found " << std::endl;

                if (options.count("y_ratio_min")>0) {
		 tmpVector = options["y_ratio_min"];
		 std::cout << cn << mn <<"\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else
                 std::cout << cn << mn <<"\t y_ratio_min not found " << std::endl;

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

		if(options.count("data_marker_style")) {
		 tmpVector = options["data_marker_style"];
		 std::cout << cn << mn << "\tdata_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
 		} else {
		 if(debug) std::cout << cn << mn << "No data_marker_style option specified" << std::endl;
		}

		if(options.count("data_marker_color")) {
		 tmpVector = options["data_marker_color"];
		 std::cout << cn << mn << "\tdata_marker_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
 		} else {
		 if(debug) std::cout << cn << mn << "No data_marker_color option specified" << std::endl;
		}

		if(options.count("grid_marker_style")) {
		 tmpVector = options["grid_marker_style"];
		 std::cout << cn << mn << "\tgrid_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
 		} else {
		 if(debug) std::cout << cn << mn << "No grid_marker_style option specified" << std::endl;
		}

		if(options.count("grid_marker_color")) {
		 tmpVector = options["grid_marker_color"];
		 std::cout << cn << mn << "\tgrid_marker_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
 		} else {
		 if(debug) std::cout << cn << mn << "No grid_marker_color option specified" << std::endl;
		}

		if(options.count("grid_fill_style")) {
		 tmpVector = options["grid_fill_style"];
		 std::cout << cn << mn << "\tgrid_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
 		} else {
		 if(debug) std::cout << cn << mn << "No grid_fill_style option specified" << std::endl;
		}

		if(options.count("total_fill_style")) {
			tmpVector = options["total_fill_style"];
			std::cout << cn << mn << "\ttotal_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_fill_style option specified" << std::endl;
		}

		if(options.count("total_fill_color")) {
			tmpVector = options["total_fill_color"];
			std::cout << cn << mn << "\ttotal_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_fill_color option specified" << std::endl;
		}

		if(options.count("total_edge_color")) {
			tmpVector = options["total_edge_color"];
			std::cout << cn << mn << "\ttotal_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_edge_color option specified" << std::endl;
		}

		if(options.count("total_edge_style")) {
			tmpVector = options["total_edge_style"];
			std::cout << cn << mn << "\ttotal_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No total_edge_style option specified" << std::endl;
		}

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

		if(options.count("beamuncertainty_fill_style")) {
			tmpVector = options["beamuncertainty_fill_style"];
			std::cout << cn << mn << "\tbeamuncertainty_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No beamuncertainty_fill_style option specified" << std::endl;
		}

		if(options.count("beamuncertainty_fill_color")) {
			tmpVector = options["beamuncertainty_fill_color"];
			std::cout << cn << mn << "\tbeamuncertainty_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No beamuncertainty_fill_color option specified" << std::endl;
		}

		if(options.count("beamuncertainty_edge_style")) {
			tmpVector = options["beamuncertainty_edge_style"];
			std::cout << cn << mn << "\tbeamuncertainty_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No beamuncertainty_edge_style option specified" << std::endl;
		}

		if(options.count("beamuncertainty_edge_color")) {
			tmpVector = options["beamuncertainty_edge_color"];
			std::cout << cn << mn << "\tbeamuncertainty_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No beamuncertainty_edge_color option specified" << std::endl;
		}

		if(options.count("beamuncertainty_marker_style")) {
			tmpVector = options["beamuncertainty_marker_style"];
			std::cout << cn << mn <<"\tbeamuncertainty_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No beamuncertainty_marker_style option specified" << std::endl;
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
		//
		if(options.count("alternative_scale_choice_fill_style")) {
			tmpVector = options["alternative_scale_choice_fill_style"];
			std::cout << cn << mn <<"\talternative_scale_choice_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alternative_scale_choice_fill_style option specified" << std::endl;
		}

		if(options.count("alternative_scale_choice_fill_color")) {
			tmpVector = options["alternative_scale_choice_fill_color"];
			std::cout << cn<<mn<<"\talternative_scale_choice_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alternative_scale_choice_fill_color option specified" << std::endl;
		}

		if(options.count("alternative_scale_choice_edge_style")) {
			tmpVector = options["alternative_scale_choice_edge_style"];
			std::cout << cn << mn <<"\talternative_scale_choice_edge_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alternative_scale_choice_edge_style option specified" << std::endl;
		}

		if(options.count("alternative_scale_choice_edge_color")) {
			tmpVector = options["alternative_scale_choice_edge_color"];
			std::cout << cn<<mn<<"\t alternative_scale_choice_edge_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alternative_scale_choice_edge_color option specified" << std::endl;
		}

		if(options.count("alternative_scale_choice_marker_style")) {
			tmpVector = options["alternative_scale_choice_marker_style"];
			std::cout << cn << mn <<"\talternative_scale_choice_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No alternative_scale_choice_marker_style option specified" << std::endl;
		}
		//

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

	unsigned int gmsSize = 0;
	unsigned int gmcSize = 0;
	unsigned int gfillsSize = 0;

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

	unsigned int xfsSize = 0;   
	unsigned int xfcSize = 0;	
	unsigned int xesSize = 0;   
	unsigned int xecSize = 0;	
	unsigned int xmsSize = 0;   

	unsigned int afsSize = 0;   
	unsigned int afcSize = 0;   
	unsigned int aesSize = 0;   
	unsigned int aecSize = 0;   
	unsigned int amsSize = 0;   


	unsigned int bfsSize = 0;   
	unsigned int bfcSize = 0;   
	unsigned int besSize = 0;   
	unsigned int becSize = 0;   
	unsigned int bmsSize = 0;   

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

	if(options.count("grid_marker_style")) {
		gmsSize = options["grid_marker_style"].size();
	}
	if(options.count("grid_marker_color")) {
		gmcSize = options["grid_marker_color"].size();
	}
	if(options.count("grid_fill_style")) {
		gfillsSize = options["grid_fill_style"].size();
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
      
	if(options.count("alternative_scale_choice_fill_style")) {
		xfsSize = options["alternative_scale_choice_fill_style"].size();
	}
	if(options.count("alternative_scale_choice_fill_color")) {
		xfcSize = options["alternative_scale_choice_fill_color"].size();
	}
	if(options.count("alternative_scale_choice_edge_style")) {
		xesSize = options["alternative_scale_choice_edge_style"].size();
	}
	if(options.count("alternative_scale_choice_edge_color")) {
		xecSize = options["alternative_scale_choice_edge_color"].size();
	}
	if(options.count("alternative_scale_choice_marker_style")) {
		xmsSize = options["alternative_scale_choice_marker_style"].size();
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

	//---
	if(options.count("beamuncertainty_fill_style")) {
		bfsSize = options["beamuncertainty_fill_style"].size();
	}
	if(options.count("beamuncertainty_fill_color")) {
		bfcSize = options["beamuncertainty_fill_color"].size();
	}
	if(options.count("beamuncertainty_edge_style")) {
		besSize = options["beamuncertainty_edge_style"].size();
	}
	if(options.count("beamuncertainty_edge_color")) {
		becSize = options["beamuncertainty_edge_color"].size();
	}
	if(options.count("beamuncertainty_marker_style")) {
		bmsSize = options["beamuncertainty_marker_style"].size();
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

	if((gmsSize != 0) && (gmsSize != gsfSize)) {
		std::ostringstream oss;
		oss << "Size of grid_marker_style vector (" << gmsSize << ") DOES NOT match the size of the grid_steering_files vector (" << gsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((gmcSize != 0) && (gmcSize != gsfSize)) {
		std::ostringstream oss;
		oss << "Size of grid_marker_color vector (" << gmcSize << ") DOES NOT match the size of the grid_steering_files vector (" << gsfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((gfillsSize != 0) && (gfillsSize != gsfSize)) {
		std::ostringstream oss;
		oss << "Size of grid_fill_style vector (" << gfillsSize << ") DOES NOT match the size of the grid_steering_files vector (" << gsfSize << ")";
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

	if((xfsSize != 0) && (xfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alternative_scale_choice_fill_style vector (" << xfsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((xfcSize != 0) && (xfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alternative_scale_choice_fill_color vector (" << xfcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((xesSize != 0) && (xesSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alternative_scale_choice_edge_style vector (" << xesSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((xecSize != 0) && (xecSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alternative_scale_choice_edge_color vector (" << xecSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((xmsSize != 0) && (xmsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alternative_scale_choice_marker_style vector (" << xmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
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

	if((bfsSize != 0) && (bfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of beamuncertainty_fill_style vector (" << bfsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((bfcSize != 0) && (bfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of beamuncertainty_fill_color vector (" << bfcSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((besSize != 0) && (besSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of beamuncertainty_edge_style vector (" << besSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((becSize != 0) && (becSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of beamuncertainty_edge_color vector (" << becSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((bmsSize != 0) && (bmsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of beamuncertainty_marker_style vector (" << bmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
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

		  //if(debug) std::cout << cn << mn << "data_marker_style " << std::endl;

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(0));		//Just one instance of data marker style
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(0));		//Just one instance of data marker color

		  if(gmsSize) gms.push_back(options["grid_marker_style"].at(0));		//Just one instance of grid marker style
		  if(gmcSize) gmc.push_back(options["grid_marker_color"].at(0));		//Just one instance of grid marker color
		  if(gfillsSize) gfs.push_back(options["grid_fill_style"].at(0));		//Just one instance of grid fill stlye


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

		  if(xfsSize) xfs.push_back(options["alternative_scale_choice_fill_style"].at(0));		
		  if(xfcSize) xfc.push_back(options["alternative_scale_choice_fill_color"].at(0));		
		  if(xesSize) xes.push_back(options["alternative_scale_choice_edge_style"].at(0));		
		  if(xecSize) xec.push_back(options["alternative_scale_choice_edge_color"].at(0));		
		  if(xmsSize) xms.push_back(options["alternative_scale_choice_marker_style"].at(0));		

		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(0));		
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(0));		
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(0));		
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(0));		
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(0));		

		  if(bfsSize) bfs.push_back(options["beamuncertainty_fill_style"].at(0));		
		  if(bfcSize) bfc.push_back(options["beamuncertainty_fill_color"].at(0));		
		  if(besSize) bes.push_back(options["beamuncertainty_edge_style"].at(0));		
		  if(becSize) bec.push_back(options["beamuncertainty_edge_color"].at(0));		
		  if(bmsSize) bms.push_back(options["beamuncertainty_marker_style"].at(0));		

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
		  std::ostringstream oss;
		  oss<<cn<<mn<<"Size of data_steering_files ("<< dsfSize <<") vector MUST be > 1 for Plot Type 2 (data[], grid[], pdf)";
		  throw SPXParseException(oss.str());
		}
		if(gsfSize <= 1) {
		  std::ostringstream oss;
		  oss<<cn<<mn<<"Size of grid_steering_files ("<<gsfSize<<") vector MUST be > 1 for Plot Type 2 (data[], grid[], pdf)";
		  throw SPXParseException(oss.str());
		}
		if(psfSize != 1) {
		  std::ostringstream oss;
		  oss<<cn<<mn<<"Size of pdf_steering_files vector ("<<psfSize<<") MUST be 1 (ONE) for Plot Type 2 (data[], grid[], pdf)";
		  throw SPXParseException(oss.str());
		}

		if(dsfSize != gsfSize) {
		  std::ostringstream oss;
		  oss<<cn<<mn<<"Size of data_steering_files vector ("<<dsfSize<<") MUST match the size of the grid_steering_files vector "<<gsfSize<<" for Plot Type 2";
		  throw SPXParseException(oss.str());
		}

		numberOfConfigurationInstances = dsfSize;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  dsf.push_back(options["data_steering_files"].at(i));
		  gsf.push_back(options["grid_steering_files"].at(i));
		  psf.push_back(options["pdf_steering_files"].at(0));	//Just one instance of pdf steering file

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(i));
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(i));

		  if(gmsSize) gms.push_back(options["grid_marker_style"].at(i));
		  if(gmcSize) gmc.push_back(options["grid_marker_color"].at(i));
		  if(gfillsSize) gfs.push_back(options["grid_fill_style"].at(i));

		  if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));		//Just one instance of pdf fill style
		  if(tfcSize) tfc.push_back(options["total_fill_color"].at(0));		//Just one instance of pdf fill color
		  if(tecSize) tec.push_back(options["total_edge_color"].at(0));		//Just one instance of pdf edge color
		  if(tesSize) tes.push_back(options["total_edge_style"].at(0));		//Just one instance of pdf edge style
		  if(tmsSize) tms.push_back(options["total_marker_style"].at(0));		//Just one instance of pdf marker style

		  if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(0));		//Just one instance of pdf fill style
		  if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(0));		//Just one instance of pdf fill color
		  if(pesSize) pes.push_back(options["pdf_edge_style"].at(0));		//Just one instance of pdf edge style
		  if(pecSize) pec.push_back(options["pdf_edge_color"].at(0));		//Just one instance of pdf edge color
		  if(pmsSize) pms.push_back(options["pdf_marker_style"].at(0));		//Just one instance of pdf marker style

		  if(sfsSize) sfs.push_back(options["scale_fill_style"].at(0));	   
		  if(sfcSize) sfc.push_back(options["scale_fill_color"].at(0));	        //Just one instance of scale fill style
		  if(sesSize) ses.push_back(options["scale_edge_style"].at(0));	   
		  if(secSize) sec.push_back(options["scale_edge_color"].at(0));	        //Just one instance of scale fill style
		  if(smsSize) sms.push_back(options["scale_marker_style"].at(0));	   

		  if(xfsSize) xfs.push_back(options["alternative_scale_choice_fill_style"].at(0));	   
		  if(xfcSize) xfc.push_back(options["alternative_scale_choice_fill_color"].at(0));	        //Just one instance of scale fill style
		  if(xesSize) xes.push_back(options["alternative_scale_choice_edge_style"].at(0));	   
		  if(xecSize) xec.push_back(options["alternative_scale_choice_edge_color"].at(0));	        //Just one instance of scale fill style
		  if(xmsSize) xms.push_back(options["alternative_scale_choice_marker_style"].at(0));	   

		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(0));	   
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(0));	        //Just one instance of alphas fill style
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(0));	   
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(0));	        //Just one instance of alphas edge style
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(0));	   

		  if(bfsSize) bfs.push_back(options["beamuncertainty_fill_style"].at(0));	   
		  if(bfcSize) bfc.push_back(options["beamuncertainty_fill_color"].at(0));//Just one instance of beamuncertainty fill style
		  if(besSize) bes.push_back(options["beamuncertainty_edge_style"].at(0));	   
		  if(becSize) bec.push_back(options["beamuncertainty_edge_color"].at(0)); //Just one instance of beamuncertainty edge style
		  if(bmsSize) bms.push_back(options["beamuncertainty_marker_style"].at(0));	   

		  //---
		  if(cfsSize) cfs.push_back(options["corrections_fill_style"].at(0));	   
		  if(cfcSize) cfc.push_back(options["corrections_fill_color"].at(0));        //Just one instance of correction fill style
		  if(cesSize) ces.push_back(options["corrections_edge_style"].at(0));	   
		  if(cecSize) cec.push_back(options["corrections_edge_color"].at(0));        //Just one instance of correction edge style
		  if(cmsSize) cms.push_back(options["corrections_marker_style"].at(0));	   

		  if(xsSize > 1) 			xsc.push_back(options["x_scale"].at(i));
		  else if(xsSize == 1)	xsc.push_back(options["x_scale"].at(0));
			else					xsc.push_back("1.0");

		  if(ysSize > 1) 			ysc.push_back(options["y_scale"].at(i));
			else if(ysSize == 1)	ysc.push_back(options["y_scale"].at(0));
			else					ysc.push_back("1.0");
		}
	}

	//data, grid, pdf[]
	else if(plotType.IsType3()) {
		if(dsfSize != 1) {
			throw SPXParseException(cn+mn+"Size of data_steering_files vector MUST be 1 (ONE) for Plot Type 3 (data, grid, pdf[])");
		}
		if(gsfSize != 1) {
			throw SPXParseException(cn+mn+"Size of grid_steering_files vector MUST be 1 (ONE) for Plot Type 3 (data, grid, pdf[])");
		}
		if(psfSize <= 1) {
			throw SPXParseException(cn+mn+"Size of pdf_steering_files vector MUST be > 1 for Plot Type 3 (data, grid, pdf[])");
		}

		numberOfConfigurationInstances = psfSize;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  dsf.push_back(options["data_steering_files"].at(0));	//Just one instance of data steering file
		  gsf.push_back(options["grid_steering_files"].at(0));	//Just one instance of grid steering file
		  psf.push_back(options["pdf_steering_files"].at(i));

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(0));		//Just one instance of data marker style
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(0));		//Just one instance of data marker color

		  if(gmsSize) gms.push_back(options["grid_marker_style"].at(0));		
		  if(gmcSize) gmc.push_back(options["grid_marker_color"].at(0));		
		  if(gfillsSize) gfs.push_back(options["grid_fill_style"].at(0));		

		  if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));
		  if(tfcSize) tfc.push_back(options["total_fill_color"].at(i));
		  if(tecSize) tec.push_back(options["total_edge_color"].at(i));
		  if(tesSize) tes.push_back(options["total_edge_style"].at(i));
		  if(tmsSize) tms.push_back(options["total_marker_style"].at(i));

		  if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(i));
		  if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(i));
		  if(pesSize) pes.push_back(options["pdf_edge_style"].at(i));
		  if(pecSize) pec.push_back(options["pdf_edge_color"].at(i));
		  if(pmsSize) pms.push_back(options["pdf_marker_style"].at(i));

		  if(sfsSize) sfs.push_back(options["scale_fill_style"].at(i));
		  if(sfcSize) sfc.push_back(options["scale_fill_color"].at(i));
		  if(sesSize) ses.push_back(options["scale_edge_style"].at(i));
		  if(secSize) sec.push_back(options["scale_edge_color"].at(i));
		  if(smsSize) sms.push_back(options["scale_marker_style"].at(i));

		  if(xfsSize) xfs.push_back(options["alternative_scale_choice_fill_style"].at(i));
		  if(xfcSize) xfc.push_back(options["alternative_scale_choice_fill_color"].at(i));
		  if(xesSize) xes.push_back(options["alternative_scale_choice_edge_style"].at(i));
		  if(xecSize) xec.push_back(options["alternative_scale_choice_edge_color"].at(i));
		  if(xmsSize) xms.push_back(options["alternative_scale_choice_marker_style"].at(i));

		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(i));
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(i));
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(i));
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(i));
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(i));

		  if(bfsSize) bfs.push_back(options["beamuncertainty_fill_style"].at(i));
		  if(bfcSize) bfc.push_back(options["beamuncertainty_fill_color"].at(i));
		  if(besSize) bes.push_back(options["beamuncertainty_edge_style"].at(i));
		  if(becSize) bec.push_back(options["beamuncertainty_edge_color"].at(i));
		  if(bmsSize) bms.push_back(options["beamuncertainty_marker_style"].at(i));

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
			throw SPXParseException(cn+mn+"Size of data_steering_files vector MUST be 1 (ONE) for Plot Type 4 (data, grid[], pdf)");
		}
		if(gsfSize <= 1) {
			throw SPXParseException(cn+mn+"Size of grid_steering_files vector MUST be > 1 for Plot Type 4 (data, grid[], pdf)");
		}
		if(psfSize != 1) {
			throw SPXParseException(cn+mn+"Size of pdf_steering_files vector MUST be 1 (ONE) for Plot Type 4 (data, grid[], pdf)");
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

		  if(gmsSize) gms.push_back(options["grid_marker_style"].at(i));
		  if(gmcSize) gmc.push_back(options["grid_marker_color"].at(i));
		  if(gfillsSize) gfs.push_back(options["grid_fill_style"].at(i));

		  if(tfsSize) tfs.push_back(options["total_fill_style"].at(0));
		  if(tfcSize) tfc.push_back(options["total_fill_color"].at(0));
		  if(tecSize) tec.push_back(options["total_edge_color"].at(0));
		  if(tesSize) tes.push_back(options["total_edge_style"].at(0));
		  if(tmsSize) tms.push_back(options["total_marker_style"].at(0));

		  if(pfsSize) pfs.push_back(options["pdf_fill_style"].at(0));
		  if(pfcSize) pfc.push_back(options["pdf_fill_color"].at(0));
		  if(pesSize) pes.push_back(options["pdf_edge_style"].at(0));
		  if(pecSize) pec.push_back(options["pdf_edge_color"].at(0));
		  if(pmsSize) pms.push_back(options["pdf_marker_style"].at(0));

		  if(sfsSize) sfs.push_back(options["scale_fill_style"].at(0));
		  if(sfcSize) sfc.push_back(options["scale_fill_color"].at(0));
		  if(sesSize) ses.push_back(options["scale_edge_style"].at(0));
		  if(secSize) sec.push_back(options["scale_edge_color"].at(0));
		  if(smsSize) sms.push_back(options["scale_marker_style"].at(0));

		  if(xfsSize) xfs.push_back(options["alternative_scale_choice_fill_style"].at(0));
		  if(xfcSize) xfc.push_back(options["alternative_scale_choice_fill_color"].at(0));
		  if(xesSize) xes.push_back(options["alternative_scale_choice_edge_style"].at(0));
		  if(xecSize) xec.push_back(options["alternative_scale_choice_edge_color"].at(0));
		  if(xmsSize) xms.push_back(options["alternative_scale_choice_marker_style"].at(0));

		  if(afsSize) afs.push_back(options["alphas_fill_style"].at(0));
		  if(afcSize) afc.push_back(options["alphas_fill_color"].at(0));
		  if(aesSize) aes.push_back(options["alphas_edge_style"].at(0));
		  if(aecSize) aec.push_back(options["alphas_edge_color"].at(0));
		  if(amsSize) ams.push_back(options["alphas_marker_style"].at(0));

		  if(bfsSize) bfs.push_back(options["beamuncertainty_fill_style"].at(0));
		  if(bfcSize) bfc.push_back(options["beamuncertainty_fill_color"].at(0));
		  if(besSize) bes.push_back(options["beamuncertainty_edge_style"].at(0));
		  if(becSize) bec.push_back(options["beamuncertainty_edge_color"].at(0));
		  if(bmsSize) bms.push_back(options["beamuncertainty_marker_style"].at(0));

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
	//data[] (grid and pdf does not matter is not read in) for data-only plots
	else if(plotType.IsType5()) {
		if(dsfSize <= 1) {
		  std::ostringstream oss;
		  oss<<cn<<mn<<"Size of data_steering_files ("<< dsfSize <<") vector MUST be > 1 for Plot Type 5 (data[])";
		  throw SPXParseException(oss.str());
		}

		numberOfConfigurationInstances = dsfSize;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
		  dsf.push_back(options["data_steering_files"].at(i));

		  if(dmsSize) dms.push_back(options["data_marker_style"].at(i));
		  if(dmcSize) dmc.push_back(options["data_marker_color"].at(i));

		  if(xsSize > 1) 			xsc.push_back(options["x_scale"].at(i));
		  else if(xsSize == 1)	xsc.push_back(options["x_scale"].at(0));
			else					xsc.push_back("1.0");

		  if(ysSize > 1) 			ysc.push_back(options["y_scale"].at(i));
			else if(ysSize == 1)	ysc.push_back(options["y_scale"].at(0));
			else					ysc.push_back("1.0");
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

        //
	std::vector<std::string> myopt;

        myopt.push_back("y_overlay_max");
        myopt.push_back("y_overlay_min");
        myopt.push_back("x_overlay_max");
        myopt.push_back("x_overlay_min");

        myopt.push_back("y_ratio_max");
        myopt.push_back("y_ratio_min");

        myopt.push_back("x_legend");
        myopt.push_back("y_legend");
        myopt.push_back("x_info_legend");
        myopt.push_back("y_info_legend");

        for (int i=0; i<myopt.size(); i++) {
	 std::string name=myopt.at(i);
         if (options.count(name)>0) {
	  std::string tmp=options[name].at(0);        
          double val = SPXStringUtilities::StringToNumber<double>(tmp);
          plotoptions[name]=val;
         } else if (debug) std::cout<<cn<<mn<<"option= "<<name<<" not found ! "<<std::endl;
	} 
        //
        if (debug) {
	 std::cout<<cn<<mn<<"Print plotoptions map "<<std::endl;
	 for(std::map<std::string, double>::reverse_iterator it=plotoptions.rbegin(); it!=plotoptions.rend(); ++it) {
	  std::cout<<" plotoptions["<<it->first<<"]= "<<it->second<<endl;
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
				oss << cn<<mn<<"Unable to parse ratio_style_" << i;
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

		if(dmsSize) {
		 if (dms.size()<i) throw SPXParseException(cn+mn+"data marker style vector too short !");
                 else pci.dataMarkerStyle = atoi(dms.at(i).c_str());
		} else 		pci.dataMarkerStyle = PC_EMPTY_STYLE;

                ///if (debug) std::cout << cn << mn << "dms done size "<<dms.size()<<" i= "<<i<< std::endl;

		if(dmcSize) pci.dataMarkerColor = atoi(dmc.at(i).c_str());
		else	 	pci.dataMarkerColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "dmc done size "<<dmc.size()<<" i= "<<i<< std::endl;

		if(gmsSize) pci.gridMarkerStyle = atoi(gms.at(i).c_str());
		else 		pci.gridMarkerStyle = PC_EMPTY_STYLE;
                
                //if (debug) std::cout << cn << mn << "gms done size "<<gms.size()<<" i= "<<i<< std::endl;

		if(gmcSize) pci.gridMarkerColor = atoi(gmc.at(i).c_str());
		else	 	pci.gridMarkerColor = PC_EMPTY_COLOR;
 
                //if (debug) std::cout << cn << mn << "gmc done size "<<gmc.size()<<" i= "<<i<< std::endl;

		if(gfillsSize) pci.gridFillStyle = atoi(gfs.at(i).c_str());
		else 		pci.gridFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "gfs done size "<<gfs.size()<<" i= "<<i<< std::endl;

		if(tfsSize) pci.totalFillStyle = atoi(tfs.at(i).c_str());
		else 		pci.totalFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "tfs done size "<<tfs.size()<<" i= "<<i<< std::endl;

		if(tfcSize) pci.totalFillColor = atoi(tfc.at(i).c_str());
		else 		pci.totalFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "tfc done size "<<tfc.size()<<" i= "<<i<< std::endl;

		if(tesSize) pci.totalEdgeStyle = atoi(tes.at(i).c_str());
		else 		pci.totalEdgeStyle = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "tes done size "<<tes.size()<<" i= "<<i<< std::endl;

		if(tecSize) pci.totalEdgeColor = atoi(tec.at(i).c_str());
		else 		pci.totalEdgeColor = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "tec done size "<<tec.size()<<" i= "<<i<< std::endl;

		if(tmsSize) pci.totalMarkerStyle = atoi(tms.at(i).c_str());
		else		pci.totalMarkerStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "tms done size "<<tms.size()<<" i= "<<i<< std::endl;

		if(pfsSize) pci.pdfFillStyle = atoi(pfs.at(i).c_str());
		else 		pci.pdfFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "pfs done size "<<pfs.size()<<" i= "<<i<< std::endl;

		if(pfcSize) pci.pdfFillColor = atoi(pfc.at(i).c_str());
		else 		pci.pdfFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "pfc done size "<<pfc.size()<<" i= "<<i<< std::endl;

		if(pesSize) pci.pdfEdgeStyle = atoi(pes.at(i).c_str());
		else 		pci.pdfEdgeStyle = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "pes done size "<<pes.size()<<" i= "<<i<< std::endl;

		if(pecSize) pci.pdfEdgeColor = atoi(pec.at(i).c_str());
		else 		pci.pdfEdgeColor = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "pec done size "<<pec.size()<<" i= "<<i<< std::endl;

		if(pmsSize) pci.pdfMarkerStyle = atoi(pms.at(i).c_str());
		else		pci.pdfMarkerStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "pms done size "<<pms.size()<<" i= "<<i<< std::endl;

		if(sfsSize) pci.scaleFillStyle = atoi(sfs.at(i).c_str());
		else 		pci.scaleFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "sfs done size "<<sfs.size()<<" i= "<<i<< std::endl;

		if(sfcSize) pci.scaleFillColor = atoi(sfc.at(i).c_str()); 
		else 		pci.scaleFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "sfc done size "<<sfc.size()<<" i= "<<i<< std::endl;

		if(sesSize) pci.scaleEdgeStyle = atoi(ses.at(i).c_str());
		else 		pci.scaleEdgeStyle = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "ses done size "<<ses.size()<<" i= "<<i<< std::endl;

		if(secSize) pci.scaleEdgeColor = atoi(sec.at(i).c_str());
		else 		pci.scaleEdgeColor = PC_EMPTY_EDGE;
	       
                //if (debug) std::cout << cn << mn << "sec done size "<<sec.size()<<" i= "<<i<< std::endl;

		if(xfsSize) pci.AlternativeScaleChoiceFillStyle = atoi(xfs.at(i).c_str());
		else 	    pci.AlternativeScaleChoiceFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "xfs done size "<<xfs.size()<<" i= "<<i<< std::endl;

		if(xfcSize) pci.AlternativeScaleChoiceFillColor = atoi(xfc.at(i).c_str()); 
		else 	    pci.AlternativeScaleChoiceFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "xfc done size "<<xfc.size()<<" i= "<<i<< std::endl;

		if(xesSize) pci.AlternativeScaleChoiceEdgeStyle = atoi(xes.at(i).c_str());
		else 	    pci.AlternativeScaleChoiceEdgeStyle = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "xes done size "<<xes.size()<<" i= "<<i<< std::endl;

		if(xecSize) pci.AlternativeScaleChoiceEdgeColor = atoi(xec.at(i).c_str());
		else 	    pci.AlternativeScaleChoiceEdgeColor = PC_EMPTY_EDGE;
	       
                //if (debug) std::cout << cn << mn << "xec done size "<<xec.size()<<" i= "<<i<< std::endl;

		if(smsSize) pci.scaleMarkerStyle = atoi(sms.at(i).c_str());
		else		pci.scaleMarkerStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "sms done size "<<sms.size()<<" i= "<<i<< std::endl;

		if(afsSize) pci.alphasFillStyle = atoi(afs.at(i).c_str());
		else 		pci.alphasFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "afs done size "<<afs.size()<<" i= "<<i<< std::endl;

		if(afcSize) pci.alphasFillColor = atoi(afc.at(i).c_str());
		else 		pci.alphasFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "afc done size "<<afc.size()<<" i= "<<i<< std::endl;

		if(aesSize) pci.alphasEdgeStyle = atoi(aes.at(i).c_str());
		else 		pci.alphasEdgeStyle = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "aes done size "<<aes.size()<<" i= "<<i<< std::endl;

		if(aecSize) pci.alphasEdgeColor = atoi(aec.at(i).c_str());
		else 		pci.alphasEdgeColor = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "aec done size "<<aec.size()<<" i= "<<i<< std::endl;

		if(amsSize) pci.alphasMarkerStyle = atoi(ams.at(i).c_str());
		else		pci.alphasMarkerStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "ams done size "<<ams.size()<<" i= "<<i<< std::endl;

		if(bfsSize) pci.beamuncertaintyFillStyle = atoi(bfs.at(i).c_str());
		else 		pci.beamuncertaintyFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "bfs done size "<<bfs.size()<<" i= "<<i<< std::endl;

		if(bfcSize) pci.beamuncertaintyFillColor = atoi(bfc.at(i).c_str());
		else 		pci.beamuncertaintyFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "bfc done size "<<bfc.size()<<" i= "<<i<< std::endl;

		if(besSize) pci.beamuncertaintyEdgeStyle = atoi(bes.at(i).c_str());
		else 		pci.beamuncertaintyEdgeStyle = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "bes done size "<<bes.size()<<" i= "<<i<< std::endl;

		if(becSize) pci.beamuncertaintyEdgeColor = atoi(bec.at(i).c_str());
		else 		pci.beamuncertaintyEdgeColor = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "bec done size "<<bec.size()<<" i= "<<i<< std::endl;

		if(bmsSize) pci.beamuncertaintyMarkerStyle = atoi(bms.at(i).c_str());
		else		pci.beamuncertaintyMarkerStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "bms done size "<<bms.size()<<" i= "<<i<< std::endl;

		if(cfsSize) pci.correctionsFillStyle = atoi(cfs.at(i).c_str());
		else 		pci.correctionsFillStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "cfs done size "<<cfs.size()<<" i= "<<i<< std::endl;  

		if(cfcSize) pci.correctionsFillColor = atoi(cfc.at(i).c_str());
		else 		pci.correctionsFillColor = PC_EMPTY_COLOR;

                //if (debug) std::cout << cn << mn << "cfc done size "<<cfc.size()<<" i= "<<i<< std::endl;  

		if(cesSize) pci.correctionsEdgeStyle = atoi(ces.at(i).c_str());
		else 		pci.correctionsEdgeStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "ces done size "<<ces.size()<<" i= "<<i<< std::endl;  

		if(cecSize) pci.correctionsEdgeColor = atoi(cec.at(i).c_str());
		else 		pci.correctionsEdgeColor = PC_EMPTY_EDGE;

                //if (debug) std::cout << cn << mn << "cec done size "<<cec.size()<<" i= "<<i<< std::endl;  

		if(cmsSize) pci.correctionsMarkerStyle = atoi(cms.at(i).c_str());
		else		pci.correctionsMarkerStyle = PC_EMPTY_STYLE;

                //if (debug) std::cout << cn << mn << "cms done size "<<cms.size()<<" i= "<<i<< std::endl;  

		if(xsSize)	pci.xScale = atof(xsc.at(i).c_str());
		else		pci.xScale = 1.0;

		if(ysSize)	pci.yScale = atof(ysc.at(i).c_str());
		else		pci.yScale = 1.0;

                if (debug) std::cout << cn << mn << "finished do plot options "<< std::endl;

                // contains_systematic_class is vector<string> but elements should be the same for each configuration instance
              
                if (options.count("contain_systematic_group")>0) {
		 std::cout<<cn<<mn<<"fill contain_systematic_group nnumber= "<<options["contain_systematic_group"].size()<<std::endl;
                 for (int j=0; j<options["contain_systematic_group"].size(); j++)
		  options["contain_systematic_group"].at(j);
                 pci.containsystematicsclasses=options["contain_systematic_group"];
                } else if (debug) std::cout<<cn<<mn<<"contain_systematic_group not found in options"<<std::endl;        

                if (options.count("remove_systematic_group")>0)
                 pci.removesystematicsclasses=options["remove_systematic_group"];
                else if (debug) std::cout<<cn<<mn<<"remove_systematic_group not found in options"<<std::endl;        

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
