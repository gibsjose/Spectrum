# Spectrum v0.6.1
### 01 December, 2014

## Authors
@gibsjose @csembree Dr. Tancredi Carli

## Change Log
* Fixed bug with reading in +/- individual systematic errors (was checking for '_p' and '_n' in name in map, not '+' and '-', as it should have been)
* Update all example steering files to reflect new changes
* Move ‘data/grid/pdf_directory' to the plot-level settings, and read as comma separated list
* Grid Hadronization and Electroweak corrections
* Binning:
    * Create flag in Steering File: match_binning (default to ON) to specify whether the OVERLAY binning is matched
    * Turn on match_binning by default, but if it is turned off, don’t match binning for overlay plots
    * Even if match_binning is on, don’t match the binning if data is not plotted
* Fixed issue where not setting x_scale/y_scale resulted in errors
* Implemented more Grid MetaData
* Implemented y_scale in Grid to artificially scale sigma/delta sigmas
* Fixed makefile

## In Progress (v1.0)

* Actual Alpha S and Scale uncertainty bands from the SPXPDF/CrossSection class
* Legends
* HERAFitter data parsing in SPXData
* Enable/test data-only plot type in SPXPlotType
* PNG Naming:
    * Allow for filename as argument after ‘-p’ to override default filename (plot name)
