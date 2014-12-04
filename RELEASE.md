# Spectrum v0.7
### 04 December, 2014

## Authors
@gibsjose @csembree Dr. Tancredi Carli

## Change Log
* Added Grid Steering Files to Git
* Added Data Steering Files to Git
* Added 'test_all' script to Git
* Added Data Steering File Meta-Data
* Removed 'generatorID' and 'nTupleID' from Grid Meta-Data
* Added 'comments' field for Data and Grid Steering Files to enter in any comments
* Created a new ratio type: convolute / nominal
* Still testing convolute / reference... there are bugs still
* Re-designed the normalization of the cross sections in SPXPlot::NormalizeCrossSections:
    * Now dividing by the total sigma separately from dividing by the bin width, and only if specified
    * Always scaling by the y bin units, regardless of the bin width status
    * Dividing by the bin width based on the GRID's divide by bin width flag, not the reference
* Changed grid file's 'divided_by_bin_width' to two flags:
    * 'grid_divided_by_bin_width'
    * 'reference_divided_by_bin_width'
* Updated how the data TGraph is named
* Implemented naming the cross section TGraph properly (still a bug when trying to name AlphaS and Scale bands though...)
* Added 'nb' as a units option for y units
* Allowed the x/y_scale for the [PLOT] settings to be optional and have the same functionality as directories (either 0, 1, or N)
* Made 'data_marker_style' and 'data_marker_color' optional: defaulting to black (1) circle (20)
    * The mechanism is still in place to read defaults from the data steering file, if one wishes to put those fields in the DSF...
* Added more debug info throughout and cleaned up debug output
* Fixed some bugs where (not) specifying certain combinations of pdf_fill_style/color/marker_style would produce a crash
* Ignoring ratio styles/ratios if the display style is only 'overlay'
* Fixed a bug where specifying 'display_style = ..., ratio' and not giving any ratios crashed the program
* Implemented overriding the overlay/ratio y_min/max from the steering file
* Changed the FileIOException to produce a more descriptive output for the error

## In Progress (v1.0)

* Adding 'pdf' to the 'overlay_style', in lieu of 'reference' to plot only the PDF
* Actual Alpha S and Scale uncertainty bands from the SPXPDF/CrossSection class
* Legends
* HERAFitter data parsing in SPXData
* Enable/test data-only plot type in SPXPlotType
* PNG Naming:
    * Allow for filename as argument after ‘-p’ to override default filename (plot name)
