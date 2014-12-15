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
bool SPXPlotConfiguration::debug;
bool SPXPlotConfigurationInstance::debug;

//Constructs an SPXPlotConfiguration object with a map of string vectors, where keys are
//	options["data_directory"] --> Vector of data directories
//	options["grid_directory"] --> Vector of grid directories
//	options["pdf_directory"] --> Vector of PDF directories
//	options["data_steering_files"] --> Vector of data steering files
//	options["grid_steering_files"] --> Vector of grid steering files
//	options["pdf_steering_files"] --> Vector of PDF steering files
//	options["data_marker_style"] --> Vector of data marker styles
//	options["data_marker_color"] --> Vector of data marker colors
//	options["pdf_fill_style"] --> Vector of PDF fill style (optional)
//	options["pdf_fill_color"] --> Vector of PDF fill colors (optional)
//	options["pdf_marker_style"] --> Vector of PDF marker styles (optional)
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
	std::vector<std::string> pfs;	//PDF fill style
	std::vector<std::string> pfc;	//PDF fill color
	std::vector<std::string> pms;	//PDF marker style
	std::vector<std::string> sfs;	//Scale fill style
	std::vector<std::string> sfc;	//Scale fill color
	std::vector<std::string> sms;	//Scale marker style
	std::vector<std::string> afs;	//AlphaS fill style
	std::vector<std::string> afc;	//AlphaS fill color
	std::vector<std::string> ams;	//AlphaS marker style
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
			throw SPXParseException("The options map MUST contain a vector for grid_directory");
		}

		if(options.count("pdf_directory") == 0) {
			throw SPXParseException("The options map MUST contain a vector for pdf_directory");
		}

		if(options.count("data_steering_files") == 0) {
			throw SPXParseException("The options map MUST contain a vector for data_steering_files");
		}

		if(options.count("grid_steering_files") == 0) {
			throw SPXParseException("The options map MUST contain a vector for grid_steering_files");
		}

		if(options.count("pdf_steering_files") == 0) {
			throw SPXParseException("The options map MUST contain a vector for pdf_steering_files");
		}
	}

	if(debug) {
		std::cout << cn << mn << "Parsing frame options vector:" << std::endl;

		std::vector<std::string> tmpVector;

		tmpVector = options["plot_type"];
		std::cout << "\tplot_type = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["desc"];
		std::cout << "\tdesc = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["display_style"];
		std::cout << "\tdisplay_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["overlay_style"];
		std::cout << "\toverlay_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["ratio_title"];
		std::cout << "\tratio_title = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["ratio_style"];
		std::cout << "\tratio_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["ratio"];
		std::cout << "\tratio = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["x_log"];
		std::cout << "\tx_log = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["y_log"];
		std::cout << "\ty_log = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_directory"];
		std::cout << "\tdata_directory = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["grid_directory"];
		std::cout << "\tgrid_directory = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["pdf_directory"];
		std::cout << "\tpdf_directory = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_steering_files"];
		std::cout << "\tdata_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["grid_steering_files"];
		std::cout << "\tgrid_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["pdf_steering_files"];
		std::cout << "\tpdf_steering_files = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_marker_style"];
		std::cout << "\tdata_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		tmpVector = options["data_marker_color"];
		std::cout << "\tdata_marker_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;

		if(options.count("pdf_fill_style")) {
			tmpVector = options["pdf_fill_style"];
			std::cout << "\tpdf_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No PDF fill style option specified" << std::endl;
		}

		if(options.count("pdf_fill_color")) {
			tmpVector = options["pdf_fill_color"];
			std::cout << "\tpdf_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No PDF fill color option specified" << std::endl;
		}

		if(options.count("pdf_marker_style")) {
			tmpVector = options["pdf_marker_style"];
			std::cout << "\tpdf_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No PDF marker style option specified" << std::endl;
		}

		if(options.count("alphas_fill_style")) {
			tmpVector = options["alphas_fill_style"];
			std::cout << "\talphas_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Alphas fill style option specified" << std::endl;
		}

		if(options.count("alphas_fill_color")) {
			tmpVector = options["alphas_fill_color"];
			std::cout << "\talphas_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Alphas fill color option specified" << std::endl;
		}

		if(options.count("alphas_marker_style")) {
			tmpVector = options["alphas_marker_style"];
			std::cout << "\talphas_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Alphas marker style option specified" << std::endl;
		}

		if(options.count("scale_fill_style")) {
			tmpVector = options["scaöe_fill_style"];
			std::cout << "\tscale_fill_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Scale fill style option specified" << std::endl;
		}

		if(options.count("scale_fill_color")) {
			tmpVector = options["scale_fill_color"];
			std::cout << "\tscale_fill_color = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Scale fill color option specified" << std::endl;
		}

		if(options.count("scale_marker_style")) {
			tmpVector = options["scale_marker_style"];
			std::cout << "\tscale_marker_style = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Scale marker style option specified" << std::endl;
		}

		if(options.count("x_scale")) {
			tmpVector = options["x_scale"];
			std::cout << "\tx_scale = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No X Scale option specified" << std::endl;
		}

		if(options.count("y_scale")) {
			tmpVector = options["y_scale"];
			std::cout << "\ty_scale = " << SPXStringUtilities::VectorToCommaSeparatedList(tmpVector) << std::endl;
		} else {
			if(debug) std::cout << cn << mn << "No Y Scale option specified" << std::endl;
		}
	}

	//Parse the plot_type
	try {
		plotType = SPXPlotType(options["plot_type"].at(0));
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXParseException("Could not parse Plot Type");
	}

	//Get vector sizes
	unsigned int rsSize = options["ratio_style"].size();
	unsigned int rSize = options["ratio"].size();
	unsigned int ddrSize = options["data_directory"].size();
	unsigned int gdrSize = options["grid_directory"].size();
	unsigned int pdrSize = options["pdf_directory"].size();
	unsigned int dsfSize = options["data_steering_files"].size();
	unsigned int gsfSize = options["grid_steering_files"].size();
	unsigned int psfSize = options["pdf_steering_files"].size();
	unsigned int dmsSize = 0;
	unsigned int dmcSize = 0;
	unsigned int pfsSize = 0;
	unsigned int pfcSize = 0;
	unsigned int pmsSize = 0;
	unsigned int sfsSize = 0;   
	unsigned int sfcSize = 0;	
	unsigned int smsSize = 0;   
	unsigned int afsSize = 0;   
	unsigned int afcSize = 0;   
	unsigned int amsSize = 0;   
	unsigned int xsSize  = 0;
	unsigned int ysSize  = 0;
	if(options.count("data_marker_style")) {
		dmsSize = options["data_marker_style"].size();
	}
	if(options.count("data_marker_color")) {
		dmcSize = options["data_marker_color"].size();
	}
	if(options.count("pdf_fill_style")) {
		pfsSize = options["pdf_fill_style"].size();
	}
	if(options.count("pdf_fill_color")) {
		pfcSize = options["pdf_fill_color"].size();
	}	
	if(options.count("pdf_marker_style")) {
		pmsSize = options["pdf_marker_style"].size();
	}
	if(options.count("scale_fill_style")) {
		sfsSize = options["scale_marker_style"].size();
	}
	if(options.count("scale_fill_color")) {
		sfcSize = options["scale_fill_color"].size();
	}
	if(options.count("scale_marker_style")) {
		smsSize = options["scale_marker_style"].size();
	}
	if(options.count("alphas_fill_style")) {
		afsSize = options["alphas_marker_style"].size();
	}
	if(options.count("alphas_fill_color")) {
		afcSize = options["alphas_fill_color"].size();
	}
	if(options.count("alphas_marker_style")) {
		amsSize = options["alphas_marker_style"].size();
	}

	if(options.count("x_scale")) {
		xsSize = options["x_scale"].size();
	}
	if(options.count("y_scale")) {
		ysSize = options["y_scale"].size();
	}

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
	if((pmsSize != 0) && (pmsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of pdf_marker_style vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((sfsSize != 0) && (sfsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_fill_style vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}

	if((sfcSize != 0) && (sfcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_fill_color vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
	if((smsSize != 0) && (smsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of scale_marker_style vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
	if((afsSize != 0) && (afsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_fill_style vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
	if((afcSize != 0) && (afcSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_fill_color vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
		throw SPXParseException(oss.str());
	}
	if((amsSize != 0) && (amsSize != psfSize)) {
		std::ostringstream oss;
		oss << "Size of alphas_marker_style vector (" << pmsSize << ") DOES NOT match the size of the pdf_steering_files vector (" << psfSize << ")";
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
		if(dsfSize != 1) {
			throw SPXParseException("Size of data_steering_files vector MUST be 1 (ONE) for Plot Type 1 (data, grid, pdf)");
		}
		if(gsfSize != 1) {
			throw SPXParseException("Size of grid_steering_files vector MUST be 1 (ONE) for Plot Type 1 (data, grid, pdf)");
		}
		if(psfSize != 1) {
			throw SPXParseException("Size of pdf_steering_files vector MUST be 1 (ONE) for Plot Type 1 (data, grid, pdf)");
		}

		numberOfConfigurationInstances = 1;

		//Fill all other vectors as needed
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			dsf.push_back(options["data_steering_files"][0]);	//Just one instance of data steering file
			gsf.push_back(options["grid_steering_files"][0]);	//Just one instance of grid steering file
			psf.push_back(options["pdf_steering_files"][0]);	//Just one instance of pdf steering file

			if(dmsSize) dms.push_back(options["data_marker_style"][0]);		//Just one instance of data marker style
			if(dmcSize) dmc.push_back(options["data_marker_color"][0]);		//Just one instance of data marker color

			if(pfsSize) pfs.push_back(options["pdf_fill_style"][0]);		//Just one instance of pdf fill style
			if(pfcSize) pfc.push_back(options["pdf_fill_color"][0]);		//Just one instance of pdf fill color
			if(pmsSize) pms.push_back(options["pdf_marker_style"][0]);		//Just one instance of pdf marker style

			if(sfsSize) sfs.push_back(options["scale_fill_style"][0]);		
			if(sfcSize) sfc.push_back(options["scale_fill_color"][0]);		
			if(smsSize) sms.push_back(options["scale_marker_style"][0]);		
			if(afsSize) afs.push_back(options["alphas_fill_style"][0]);		
			if(afcSize) afc.push_back(options["alphas_fill_color"][0]);		
			if(amsSize) ams.push_back(options["alphas_marker_style"][0]);		

			if(xsSize) 	xsc.push_back(options["x_scale"][0]);				//Just one instance of x scale
			else		xsc.push_back("1.0");
			if(ysSize) 	ysc.push_back(options["y_scale"][0]);				//Just one instance of y scale
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
			dsf.push_back(options["data_steering_files"][i]);
			gsf.push_back(options["grid_steering_files"][i]);
			psf.push_back(options["pdf_steering_files"][0]);	//Just one instance of pdf steering file

			if(dmsSize) dms.push_back(options["data_marker_style"][i]);
			if(dmcSize) dmc.push_back(options["data_marker_color"][i]);

			if(pfsSize) pfs.push_back(options["pdf_fill_style"][0]);		//Just one instance of pdf fill style
			if(pfcSize) pfc.push_back(options["pdf_fill_color"][0]);		//Just one instance of pdf fill color
			if(pmsSize) pms.push_back(options["pdf_marker_style"][0]);		//Just one instance of pdf marker style

			if(sfsSize) sfs.push_back(options["scale_fill_style"][0]);	   
			if(sfcSize) sfc.push_back(options["scale_fill_color"][0]);	   
			if(smsSize) sms.push_back(options["scale_marker_style"][0]);	   
			if(afsSize) sfs.push_back(options["alphas_fill_style"][0]);	   
			if(afcSize) sfc.push_back(options["alphas_fill_color"][0]);	   
			if(amsSize) sms.push_back(options["alphas_marker_style"][0]);	   

			if(xsSize > 1) 			xsc.push_back(options["x_scale"][i]);
			else if(xsSize == 1)	xsc.push_back(options["x_scale"][0]);
			else					xsc.push_back("1.0");

			if(ysSize > 1) 			ysc.push_back(options["y_scale"][i]);
			else if(ysSize == 1)	ysc.push_back(options["y_scale"][0]);
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
			dsf.push_back(options["data_steering_files"][0]);	//Just one instance of data steering file
			gsf.push_back(options["grid_steering_files"][0]);	//Just one instance of grid steering file
			psf.push_back(options["pdf_steering_files"][i]);

			if(dmsSize) dms.push_back(options["data_marker_style"][0]);		//Just one instance of data marker style
			if(dmcSize) dmc.push_back(options["data_marker_color"][0]);		//Just one instance of data marker color

			if(pfsSize) pfs.push_back(options["pdf_fill_style"][i]);
			if(pfcSize) pfc.push_back(options["pdf_fill_color"][i]);
			if(pmsSize) pms.push_back(options["pdf_marker_style"][i]);

			if(sfsSize) sfs.push_back(options["scale_fill_style"][i]);
			if(sfcSize) sfc.push_back(options["scale_fill_color"][i]);
			if(smsSize) sms.push_back(options["scale_marker_style"][i]);


			if(afsSize) afs.push_back(options["alphas_fill_style"][i]);
			if(afcSize) afc.push_back(options["alphas_fill_color"][i]);
			if(amsSize) ams.push_back(options["alphas_marker_style"][i]);

			if(xsSize) 	xsc.push_back(options["x_scale"][0]);				//Just one instance of x scale
			else		xsc.push_back("1.0");

			if(ysSize) 	ysc.push_back(options["y_scale"][0]);				//Just one instance of y scale
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
			dsf.push_back(options["data_steering_files"][0]);	//Just one instance of data steering file
			gsf.push_back(options["grid_steering_files"][i]);
			psf.push_back(options["pdf_steering_files"][0]);	//Just one instance of pdf steering file

			if(dmsSize) dms.push_back(options["data_marker_style"][0]);		//Just one instance of data marker style
			if(dmcSize) dmc.push_back(options["data_marker_color"][0]);		//Just one instance of data marker color

			if(pfsSize) pfs.push_back(options["pdf_fill_style"][0]);		//Just one instance of pdf fill style
			if(pfcSize) pfc.push_back(options["pdf_fill_color"][0]);		//Just one instance of pdf fill color
			if(pmsSize) pms.push_back(options["pdf_marker_style"][0]);		//Just one instance of pdf marker style

			if(sfsSize) sfs.push_back(options["scale_fill_style"][0]);
			if(sfcSize) sfc.push_back(options["scale_fill_color"][0]);
			if(smsSize) sms.push_back(options["scale_marker_style"][0]);

			if(afsSize) afs.push_back(options["alphas_fill_style"][0]);
			if(afcSize) afc.push_back(options["alphas_fill_color"][0]);
			if(amsSize) ams.push_back(options["alphas_marker_style"][0]);


			if(xsSize) 	xsc.push_back(options["x_scale"][0]);				//Just one instance of x scale
			else		xsc.push_back("1.0");

			if(ysSize) 	ysc.push_back(options["y_scale"][0]);				//Just one instance of y scale
			else		ysc.push_back("1.0");
		}
	}

	//Process directories
	if(ddrSize == 1) {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			ddr.push_back(options["data_directory"][0]);
		}
	} else {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			ddr.push_back(options["data_directory"][i]);
		}
	}

	if(gdrSize == 1) {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			gdr.push_back(options["grid_directory"][0]);
		}
	} else {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			gdr.push_back(options["grid_directory"][i]);
		}
	}

	if(pdrSize == 1) {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			pdr.push_back(options["pdf_directory"][0]);
		}
	} else {
		for(int i = 0; i < numberOfConfigurationInstances; i++) {
			pdr.push_back(options["pdf_directory"][i]);
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

		//Copy the ratio string vector
		ratios = options["ratio"];
	} else {
		ratioStyles.clear();
		ratios.clear();
	}

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
	for(int i = 0; i < numberOfConfigurationInstances; i++) {
		SPXPlotConfigurationInstance pci;

		pci.SetDefaults();

		pci.id = i;

		//Copy from vectors to pci
		pci.dataDirectory = ddr[i];
		pci.gridDirectory = gdr[i];
		pci.pdfDirectory  = pdr[i];
		pci.dataSteeringFile = SPXDataSteeringFile(ddr[i] + "/" + dsf[i]);
		pci.gridSteeringFile = SPXGridSteeringFile(gdr[i] + "/" + gsf[i]);
		pci.pdfSteeringFile  = SPXPDFSteeringFile(pdr[i]  + "/" + psf[i]);

		if(dmsSize) pci.dataMarkerStyle = atoi(dms[i].c_str());
		else 		pci.dataMarkerStyle = PC_EMPTY_STYLE;

		if(dmcSize) pci.dataMarkerColor = atoi(dmc[i].c_str());
		else	 	pci.dataMarkerColor = PC_EMPTY_COLOR;

		if(pfsSize) pci.pdfFillStyle = atoi(pfs[i].c_str());
		else 		pci.pdfFillStyle = PC_EMPTY_STYLE;

		if(pfcSize) pci.pdfFillColor = atoi(pfc[i].c_str());
		else 		pci.pdfFillColor = PC_EMPTY_COLOR;

		if(pmsSize) pci.pdfMarkerStyle = atoi(pms[i].c_str());
		else		pci.pdfMarkerStyle = PC_EMPTY_STYLE;

		if(sfsSize) pci.scaleFillStyle = atoi(sfs[i].c_str());
		else 		pci.scaleFillStyle = PC_EMPTY_STYLE;

		if(sfcSize) pci.scaleFillColor = atoi(sfc[i].c_str());
		else 		pci.scaleFillColor = PC_EMPTY_COLOR;

		if(smsSize) pci.scaleMarkerStyle = atoi(sms[i].c_str());
		else		pci.scaleMarkerStyle = PC_EMPTY_STYLE;

		if(afsSize) pci.alphasFillStyle = atoi(afs[i].c_str());
		else 		pci.alphasFillStyle = PC_EMPTY_STYLE;

		if(afcSize) pci.alphasFillColor = atoi(afc[i].c_str());
		else 		pci.alphasFillColor = PC_EMPTY_COLOR;

		if(amsSize) pci.alphasMarkerStyle = atoi(ams[i].c_str());
		else		pci.alphasMarkerStyle = PC_EMPTY_STYLE;

		if(xsSize)	pci.xScale = atof(xsc[i].c_str());
		else		pci.xScale = 1.0;

		if(ysSize)	pci.yScale = atof(ysc[i].c_str());
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
