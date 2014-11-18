# Spectrum v0.2
### 04 November, 2014

## Authors
@gibsjose @csembree Dr. Tancredi Carli

## Change Log
* Fixed the makefile to not compile all sources each time
* Fixed an issue where DetermineOverlay/RatioFrameBounds would run even if overlay/ratio wasn't plotted
* Added data's total graph to the array to compute frame bounds (since the total graph is usually the largest)
* Allowed aliasing of data, grid, and pdf files in ratios instead of having to type the complete name each time
* Merged Spectrum T1/2/3 S/A all into one data type called 'Spectrum', and simplified the SPXData class
* Progress towards HERAFitter format, some bugs still exist, and some things need to be decided upon

## In Progress (v1.0)
* Legends (overlay/ratio)
* Steering File Generator program
* HERAFitter data parsing in SPXData
* Data only plot type in SPXPlotType
* Print out convolute histograms upon reading them ->Print(“all”) or something like that
* Move ‘data/grid/pdf_directory' to the plot-level settings, and read as comma separated list
* Binning:
    * Create new function: SPXGraphUtilities::MatchBinning(g1, g2) : Matches g2 to the binning of g1
    * Create flag in Steering File: match_binning (default to ON) to specify whether the OVERLAY binning is matched
    * Ratio binning MUST be matched in order to be able to divide the TGraphs
    * Create entry in [PLOT] section of SF to specify data_binning_master = data_file.txt, to choose the data that all other binnings will be matched to
        * If this is not specified AND a ratio is plotted in which the binning does not match, throw an exception
    * Turn on match_binning by default, but if it is turned off, don’t match binning for overlay plots
    * Even if match_binning is on, don’t match the binning if data is not plotted
* PNG Naming:
    * Allow for filename as argument after ‘-p’ to override default filename (plot name)
