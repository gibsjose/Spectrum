# Spectrum v0.8.1
### 18 December, 2014

## Authors
@gibsjose @csembree Tancredi Carli

## Change Log

<<<<<<< HEAD
 // -SPXPDF: get and calculate methods for theory covariance matrix

  -added SPXGraphUtilities::SPXHistogramToVector
=======
  -SPXGraphUtilities: Introduce HistogramCutXAxis

  -SPXData: inititialize  DataCutXmin,  DataCutXmax to high values

  -SPXPlot: change to be able to run theory only
>>>>>>> release-0.98

  -SPXDATA: Removed bug in reading in covariance matrix starting with negative value
            Removed bug reading in last line of covariance matrix 
            corrected trasformation from covariance to correlation matrix

  -SPXRatio: include Convolue / Convolute

  -SPXData: fixed problem for data tables with line with one whitespace

  -SPXSteering: Add possibility to overload min/max overlay/ratio legend parameter from Plot field
 
  -Modification to include scale choice in uncertainties bands

  -SPXSteering: introduce flag if grid corrections should go to nominal

  -added python scripts to generate steering files

  -SPXSteering/SPXPlotConfiguration remove/contain_systematic_group are now in SPXPlotConfiguration
   before it did not work for many plot configurations Plot_0 Plot_1
  
  -SPXCrossSection::ApplyCorrections 
   correct now also nominal TGraphErrror for hadronisation and grid corrections

 -SPXPlot, SPXDataSteering, SPXGridSteering
  included flag to indicate if Data or Grids are divided by the
  double differential bin width and take this into account in 
  cross section normalisation.

 -Feature to only keep systematic uncertainty class of

 -Feature to remove class of systematics uncertainties 

 -SPXPlot/SPXRatio/SPXGraphUtilities
   added possibility to compare directly data to data 
   Labels are taken automatically from new dataFileLabelMap in SPXRatio
   filled via SPXPlot

 -SPXPlotType/SPXPlotConfiguration: introduced Type5 for data[] (pdf and grod does not matter) 

 -Included CT14, METAPDF

 -SPXPDF: can now run PDF without member to calculate alphas uncertainty

 -SPXSteering: Can now show systematics as line (linewidth via steering)
   show_systematics_as_lines 

 - SPXPDF:CalcSystPDF: correctly now updating all default 
          (set default to average for for NNPDF)
          was causing problems with Alphas uncertainty

 -SPXPDF: improved interface PHAPDF5/LHAPDF5 via SetLHAPDFPDFset
          
 - Added possibility to have functional form of scale in information legend

 - SPXPDF: after calculation of PDF error, LHAPDF reset to default PDF

 - steering to output rootfile and graphic format for figure
  
 - added beam uncertainty

 - SPXGRid and SPXDPF:
   added possibility to input list of grids (in grid steering) that are added together

 -added possibility to print total cross sections

 -SPXPlotConfiguration and  SPXSteering: added options: grid_marker_color, grid_marker_style, grid_fill_Style

 -SPXGraphUtilities::GetYUnitsScale: 
   corrected problem with order of pb and fb
   when transforming pb -> fb was applying 10^-3 instead of 10^3

 -SPXPlot and SPXRatio: Introduced logic such that the same grid file path name can
                        be used, if needed. Introduce in MAP a new key with 
                        newname=oldname_index (where index is the position in the grid name file                   

 -SPXGraphUtilities: Addition protection for last bins in MatchBinning

 -SPXPlot: steering to order systematics according to name of systematics

 -SPXUtilities: sort function to order graph according alphabethical order of names

 -SPXPlot::DrawLegend: special treatment for figure showing only hadronisation uncertainty 

 -SPXGridCorrection: simplied logic
                     can read in different format of corrections
                     like relative or absolute error (as given in correction file 
                     stored is always the absolute error in map and graph

 -SPXPDF: add HadBandOfType

 -SPXDATA: Remove bin also on correlation matrices

 -SPXPDF: Added function to set histograms containing the individual uncertainty histograms
          AddCorrection now also changes individual uncertainty histograms
          
 -SPXCrossSection: 
          change individual uncertainties in PDF class for corrections and re-binning

 -SPXData: add correlation type of systematics uncertainty
           true if correllated among bins, false if uncorrelated
 
 -SPXData: also remove bins from syst_p and syst_n of some data bins

 -SPXPDF: give access to individual PDF, alphas and scale components

 -SPXData: included possibility to cut on xmin and xmax
           Parse and CreateGraph need now be called from outside,
           since xmin and max cuts can be given by main steering

 -SPXPDF and SPXPlot: added Mark times if define TIMER on
  can be activated via makefile
 
 -SPXPlot: add systematics groups

 -SPXROOT: include TString
 
 -SPXGraphUtilities: AddinQuadrature, AddtoBand, SetColors

 -SPXUtilities: functions to choose color codes as function on index 1...n
                ICol IColGreen IColBlue IColRed

                added OrderLargestRelativeErrorGraphVector order graph according to largest relative error
                added SortLargestRelativeError corresponding sort function

                new functions:  AddtoBand,  AddinQuadrature

 -SPXData:: make public access to individual systematic uncertainty map
            Warning if systematics appears twice

  Added: lumi_value, lumi_error, add_lumi_uncertainty_to_systematics 
  All tables updated

- add possibility to plot individual systematic uncertainty using SPXRatio for ratiostyle DataOverData

- move helper function in SPXRatio.h to SPXRatio.cxx

- for DataOverConvolute also Draw statistical uncertainty in ratios 

- added in lumi_scale_factor in SPXData

- Improvement in Title offsets

- DrawLegend has now logic to put all different information to normal legend
  while all same information (jet R, eta bin etc) to information legend

- added 2010 inclusive jet data

- harmonized names of grid steering files (now called with same convention as data)

- SetAxes works now even if no DrawFrame

- changes in SPXPlotConfiguration::GetRatioStyle to avoid crash in case no ratio_Style is specified

- y_Scale (artificial scaling constant) now goes to both data and MC

- fixed problem that mean very different for PDF and scale, alphas in case of NNPDF error propagation
  where average of replica is used instead defulat PDF set
  alphas and scale uncertainty now also refer to the average

- protection to calculation lphas uncertainty if problems if PDF alphas variations samples

- Add addition label for information legend

- introduce steering fo x/y coordination of data uncertainty box

- correction bug with correctionsMarkerStyle in SPXPlotConfiguration.cxx

- for marker type and if not detailed band asked for only total band is plotted

- SPXPDF functions to test band properties 

- fix Staggering algorithm and introduce StaggerGraph in SPXGraphUtilities

- corrected bug adding errors in quadrature in Matchbinning

- Change code order, UpdateBand now does not re-calculate nominal from pdf->GetNominal()
  (was in conflict with Matchbinning)
  nominal graph now does not have the grid-corrections applied

- interface to LHAPDF6

- corrected staggered error bars 

- do not require presence of alphas variation PDF sample

- help in Spectrum

- first version of SPXDrawUtilities with BoxText
  Box for data statistical and total uncertainty in ratio
 

- order bands before plotting in SPXPlot:DrawBand
  new method OrderBandMap in SPXUtilities

- do not calculate correlation information if not needed, no debug ON per default (code and steering)
  introduce journal, data etc in information legend



### release-0.96

 -convoluteFileMap now contains PDF-object and not TGraph

 -new SPXPDF constructor
  can give name -> class open applgrid
  can give SPXGrid pointer -> applgrid is hold in SPXClass

 -introduced edge_style and edge_color 
  allows for Atlas style on theory uncertainties

 -Draw function in SPXRatio and SPXPlot

###v0.95

-changes in SPXPlotConfiguration to allow for data-only figures 

 -implemented detailed uncertainty bands (scale, pdf etc)

- SPXPlot:
  -moved MatchBinning to Cross section class
   data vector has now pointers, needed to call SPXData constructed
   to incorporate reading in correlations matrix
   (ensure that graphs are created at the beginning)
 -add reading legend


-SPXData 
  -reading correlation matrix
   statistical and total

 - calculation correllation matrix from systematic uncertainty components

  -introduced explicit constructor to make sure
   that graphs are created before reading in the correlation matrix
   (graph needed to transform covariance in correllation matrix)

- SPXCrossSection:

  -split CrossSectionInitialize into
   Initialize, ApplyCorrections, UpdateBandandHist
   (allow for total uncertainty to be calculated0

- introduced SPXChi2 for chi2 calculations
  (needs further testing)

### v0.8.1
* Fixed a bug with the INIH Library having too small of a buffer for plots with 5+ data/grid files
* The INIH Library is now compiled alongside Spectrum, so it is cross-platform and any changes made propagate throughout

### v0.8
* Added a flag '-m' to Spectrum to write the Data/Grid metadata to a file
* Fixed the bin matching to not match horizontally between data/convolutes and only matches convolutes with their corresponding data master
* Added mechanisms to include different bands in the convolute (PDF, Alpha S, Scale)

### v0.7.1
* Fixed ratio types: convolute / reference & convolute / nominal

### v0.7
* Added Grid Steering Files to Git
* Added Data Steering Files to Git
* Added 'test_all' script to Git
* Added Data Steering File Meta-Data
* Removed 'generatorID' and 'nTupleID' from Grid Meta-Data
* Added 'comments' field for Data and Grid Steering Files to enter in any comments
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

### v0.6
* Fixed bug with reading in +/- individual systematic errors (was checking for `_p` and `_n` in name in map, not `+` and `-`, as it should have been)
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

### v0.5
* Completely changed the example steering directory structure: Now all examples are in '/example'
* Implemented the SPXGraphUtilities::MatchBinning function to match the binning of a slave graph to the binning of a master
* Moved SPXGraphUtilities to it's own CXX file to improve compilation speed and modularity
* Improved the argument processing
* Added a '-t' argument and a 'Test' namespace: Using the '-t' flag tells Spectrum to enable test features that are currently under development
* Added pseudo methods for obtaining the AlphaSBandResults and ScaleBandResults from SPXCrossSection

### v0.4
* Fixed the makefile to not compile all sources each time
* Fixed an issue where DetermineOverlay/RatioFrameBounds would run even if overlay/ratio wasn't plotted
* Added data's total graph to the array to compute frame bounds (since the total graph is usually the largest)
* Allowed aliasing of data, grid, and pdf files in ratios instead of having to type the complete name each time
* Merged Spectrum T1/2/3 S/A all into one data type called 'Spectrum', and simplified the SPXData class
* Progress towards HERAFitter format, some bugs still exist, and some things need to be decided upon

### v0.2
* Implemented using the `!` flag in ratio_style_x to signal zeroing of numerator/denominator ratio graph Y errors:
* `ratio_style_0 = convolute / data` will add all the Y errors in quadrature (`DivideErrorType_t: AddErrors`)
* `ratio_style_0 = convolute / !data` will zero the denominator (data) Y errors when dividing (`DivideErrorType_t: ZeroNumGraphErrors`)
* `ratio_style_0 = !convolute / data` will zero the numerator (convolute) Y errors when dividing (`DivideErrorType_t: ZeroDenGraphErrors`)
* `ratio_style_0 = !convolute / !data` will zero BOTH the numerator and denominator Y errors when dividing (`DivideErrorType_t: ZeroAllErrors`)

### v0.1
* Allow plotting of data statistical/total error bands
* Fixed duplicate data/cross sections being plotted
* Implemented 'data_stat' and 'data_tot' ratio styles
* Fixed ratio Y axis title offset
