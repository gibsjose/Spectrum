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

		if(options.count("data_marker_style") == 0) {
			throw SPXParseException("The options map MUST contain a vector for data_marker_style");
		}

		if(options.count("data_marker_color") == 0) {
			throw SPXParseException("The options map MUST contain a vector for data_marker_color");
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
	unsigned int dmsSize = options["data_marker_style"].size();
	unsigned int dmcSize = options["data_marker_color"].size();
	unsigned int pfsSize = 0;
	unsigned int pfcSize = 0;
	unsigned int pmsSize = 0;
	unsigned int xsSize = 0;
	unsigned int ysSize = 0;
	if(options.count("pdf_fill_style")) {
		pfsSize = options["pdf_fill_style"].size();
	}
	if(options.count("pdf_fill_color")) {
		pfcSize = options["pdf_fill_color"].size();
	}
	if(options.count("pdf_marker_style")) {
		pmsSize = options["pdf_marker_style"].size();
	}
	if(options.count("x_scale")) {
		xsSize = options["x_scale"].size();
	}
	if(options.count("y_scale")) {
		ysSize = options["y_scale"].size();
	}

	//Check vector sizes that do not depend on the plot type
	if(rSize != rsSize) {
		throw SPXParseException("Size of ratio vector DOES NOT match the size of the ratio_style vector");
	}
	if(dmsSize != dsfSize) {
		throw SPXParseException("Size of data_marker_style vector DOES NOT match the size of the data_steering_files vector");
	}
	if(dmcSize != dsfSize) {
		throw SPXParseException("Size of data_marker_color vector DOES NOT match the size of the data_steering_files vector");
	}
	if(xsSize != dsfSize) {
		throw SPXParseException("Size of x_scale vector DOES NOT match the size of the data_steering_files vector");
	}
	if(ysSize != dsfSize) {
		throw SPXParseException("Size of y_scale vector DOES NOT match the size of the data_steering_files vector");
	}
	if((pfsSize != 0) && (pfsSize != psfSize)) {
		throw SPXParseException("Size of pdf_fill_style vector DOES NOT match the size of the pdf_steering_files vector");
	}
	if((pfcSize != 0) && (pfcSize != psfSize)) {
		throw SPXParseException("Size of pdf_fill_color vector DOES NOT match the size of the pdf_steering_files vector");
	}
	if((pmsSize != 0) && (pmsSize != psfSize)) {
		throw SPXParseException("Size of pdf_marker_style vector DOES NOT match the size of the pdf_steering_files vector");
	}

	//Check directory size: Could either be of length 1, in which case the directory is prepended to each steering file, or
	//	the exact length of the steering file list
	if((ddrSize != 1) && (ddrSize != dsfSize)) {
		throw SPXParseException("Size of data_directory vector MUST be either '1' or the size of the data_steering_files vector");
	}
	if((gdrSize != 1) && (gdrSize != gsfSize)) {
		throw SPXParseException("Size of grid_directory vector MUST be either '1' or the size of the grid_steering_files vector");
	}
	if((pdrSize != 1) && (pdrSize != psfSize)) {
		throw SPXParseException("Size of pdf_directory vector MUST be either '1' or the size of the pdf_steering_files vector");
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
			dms.push_back(options["data_marker_style"][0]);		//Just one instance of data marker style
			dmc.push_back(options["data_marker_color"][0]);		//Just one instance of data marker color

			if(pfsSize) {
				pfs.push_back(options["pdf_fill_style"][0]);		//Just one instance of pdf fill style
				pfc.push_back(options["pdf_fill_color"][0]);		//Just one instance of pdf fill color
				pms.push_back(options["pdf_marker_style"][0]);		//Just one instance of pdf marker style
			}

			if(xsSize) {
				xsc.push_back(options["x_scale"][0]);			//Just one instance of x scale
				ysc.push_back(options["y_scale"][0]);			//Just one instance of y scale
			}
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
			dms.push_back(options["data_marker_style"][i]);
			dmc.push_back(options["data_marker_color"][i]);

			if(pfsSize) {
				pfs.push_back(options["pdf_fill_style"][0]);		//Just one instance of pdf fill style
				pfc.push_back(options["pdf_fill_color"][0]);		//Just one instance of pdf fill color
				pms.push_back(options["pdf_marker_style"][0]);		//Just one instance of pdf marker style
			}

			if(xsSize) {
				xsc.push_back(options["x_scale"][i]);
				ysc.push_back(options["y_scale"][i]);
			}
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
			dms.push_back(options["data_marker_style"][0]);		//Just one instance of data marker style
			dmc.push_back(options["data_marker_color"][0]);		//Just one instance of data marker color

			if(pfsSize) {
				pfs.push_back(options["pdf_fill_style"][i]);
				pfc.push_back(options["pdf_fill_color"][i]);
				pms.push_back(options["pdf_marker_style"][i]);
			}

			if(xsSize) {
				xsc.push_back(options["x_scale"][0]);			//Just one instance of x scale
				ysc.push_back(options["y_scale"][0]);			//Just one instance of y scale
			}
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
			dms.push_back(options["data_marker_style"][0]);		//Just one instance of data marker style
			dmc.push_back(options["data_marker_color"][0]);		//Just one instance of data marker color

			if(pfsSize) {
				pfs.push_back(options["pdf_fill_style"][0]);		//Just one instance of pdf fill style
				pfc.push_back(options["pdf_fill_color"][0]);		//Just one instance of pdf fill color
				pms.push_back(options["pdf_marker_style"][0]);		//Just one instance of pdf marker style
			}

			if(xsSize) {
				xsc.push_back(options["x_scale"][0]);			//Just one instance of x scale
				ysc.push_back(options["y_scale"][0]);			//Just one instance of y scale
			}
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

	//Parse the ratio_style vector
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
		pci.pdfDirectory = pdr[i];
		pci.dataSteeringFile = SPXDataSteeringFile(ddr[i] + "/" + dsf[i]);
		pci.gridSteeringFile = SPXGridSteeringFile(gdr[i] + "/" + gsf[i]);
		pci.pdfSteeringFile = SPXPDFSteeringFile(pdr[i] + "/" + psf[i]);
		pci.dataMarkerStyle = atoi(dms[i].c_str());
		pci.dataMarkerColor = atoi(dmc[i].c_str());

		if(pfsSize) {
			pci.pdfFillStyle = atoi(pfs[i].c_str());
			pci.pdfFillColor = atoi(pfc[i].c_str());
		} else {
			pci.pdfFillStyle = PC_EMPTY_STYLE;
			pci.pdfFillColor = PC_EMPTY_COLOR;
		}

		if(pmsSize) {
			pci.pdfMarkerStyle = atoi(pms[i].c_str());
		} else {
			pci.pdfMarkerStyle = PC_EMPTY_STYLE;
		}

		if(xsSize) {
			pci.xScale = atof(xsc[i].c_str());
			pci.yScale = atof(ysc[i].c_str());
		} else {
			pci.xScale = 1.0;
			pci.yScale = 1.0;
		}

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
