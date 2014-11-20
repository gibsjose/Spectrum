# Spectrum v0.5
### 20 November, 2014

## Authors
@gibsjose @csembree Dr. Tancredi Carli

## Change Log
* Implemented the SPXGraphUtilities::MatchBinning function to match the binning of a slave graph to the binning of a master
* Moved SPXGraphUtilities to it's own CXX file to improve compilation speed and modularity
* Improved the argument processing
* Added a '-t' argument and a 'Test' namespace: Using the '-t' flag tells Spectrum to enable test features that are currently under development
* Added pseudo methods for obtaining the AlphaSBandResults and ScaleBandResults from SPXCrossSection

## In Progress (v1.0)
* Move ‘data/grid/pdf_directory' to the plot-level settings, and read as comma separated list
* Grid Hadronization and Electroweak corrections
* Actual Alpha S and Scale uncertainty bands from the SPXPDF/CrossSection class
* Legends
* HERAFitter data parsing in SPXData
* Enable/test data-only plot type in SPXPlotType
* Binning:
    * Create flag in Steering File: match_binning (default to ON) to specify whether the OVERLAY binning is matched
    * Create entry in [PLOT] section of SF to specify data_binning_master = data_file.txt, to choose the data that all other binnings will be matched to
        * If this is not specified AND a ratio is plotted in which the binning does not match, throw an exception
    * Turn on match_binning by default, but if it is turned off, don’t match binning for overlay plots
    * Even if match_binning is on, don’t match the binning if data is not plotted
* PNG Naming:
    * Allow for filename as argument after ‘-p’ to override default filename (plot name)
