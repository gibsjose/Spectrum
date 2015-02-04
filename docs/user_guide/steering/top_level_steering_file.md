![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#Top-Level Steering File

###Description
The top level steering file is used to control general plot settings, specify the specific data, grid, and pdf used, and to override some settings in the other steering files.

All steering file settings are separated by section (denoted in the `[SECTION]` heading), and within a given section each setting in the form `variable = value`

The character `;` denotes a comment. You may have comments after a variable is defined, but there are some bugs associated with this. For example, if you specify:

> `ratio_title =            ;This is where the ratio title goes...`

Then the variable `ratio_title` will actually be set to the comment, since it is the only thing after the `=`.

To avoid bugs associated with comments, it is good practice to never have a line such as:

> `y_overlay_min = ` <br>
> <br>

Which should be commented out with: <br>
> `; y_overlay_min =`

Finally, for boolean values, the possible values are:

> `true`, `yes`, `on`, or `1` <br>
> `false`, `no`, `off`, or `0`

###Options

**NOTE:** Where applicable, a **bold** value is the default.

##`[GEN]`
**Optional** `debug =` true or **false**: Turn on or off debug output

##`[GRAPH]`
**Optional** `plot_band =` true or **false**: Plot error bands instead of markers

**Optional** `plot_error_ticks =` true or **false**: Use small 'ticks' at the end of error bars

**Optional** `plot_marker = ` **true** or false: Plot markers on points

**Optional** `plot_staggered =` true or **false**: Stagger the plots horizontally for visibility

**Optional** `match_binning = ` **true** or false: Force convolutes plotted in the overlay section to match the data binning

**Optional** `grid_corr =` true or **false**: Apply grid corrections (electroweak, hadronization, etc.)

**Optional** `label_sqrt_s =` true or **false**: Show the square-root of S in the legend

**Optional** `x_legend =` X (right) position of the legend

**Optional** `y_legend =` Y (top) position of the legend

**Optional** `y_overlay_min =` Force Y minimum for Overlay

**Optional** `y_overlay_max =` Force Y maximum for Overlay

**Optional** `y_ratio_min =` Force Y minimum for Ratio

**Optional** `y_ratio_max =` Force Y maximum for Ratio

**Optional** 

**Optional**  `total_fill_style = ` band fill style for total uncertainty
**Optional**  `total_fill_color = ` band and marker fill color for total uncertainty
**Optional**  `total_marker_style = ` marker style for total uncertainty

**Optional**  `pdf_fill_style = ` band fill style for PDF uncertainty
**Optional**  `pdf_fill_color = ` band and marker fill color for pdf uncertainty
**Optional**  `pdf_marker_style = ` marker style for pdf uncertainty

**Optional**  `scale_fill_style = ` band fill style for scale uncertainty
**Optional**  `scale_fill_color = ` band and marker fill color for scale uncertainty
**Optional**  `scale_marker_style = ` marker style for scale uncertainty

**Optional**  `alphas_fill_style = ` fill style for alphas uncertainty
**Optional**  `alphas_fill_color = ` band and marker fill color for alphas uncertainty
**Optional**  `alphas_marker_style = ` marker style for alphas uncertainty


##`[PLOT_n]`
This is where you define the specific plot settings. You can create as many `[PLOT_n]` sections as you want, as long as `n` starts at `0` and increments each plot instance. Keep in mind that each plot definition under a new plot section will be plotted **on a separate ROOT canvas**.

**NOTE:** The use of **or** and **xor** refer to their logic operator meaning. Thus, `option_a` **xor** `option_b` can be read as 'Exactly one of either option_a or option_b' and `option_a` **or** `option_b` can be read as 'option_a and/or option_b'

**REQUIRED** `plot_type = `
> `data, grid, pdf` **xor** <br>
> `data[], grid[], pdf` **xor** <br>
> `data, grid[], pdf` **xor** <br>
> `data, grid, pdf[]`

This defines the actual type of plot you would like to make. For example, a plot where you would like to compare multiple PDFs with one dataset would be of type `data, grid, pdf[]`, where the `[]` denotes multiplicity

Must be in the order data, grid, pdf. Do not switch order.

Can be data **xor** <br>
or     grid, pdf **xor** <br>
or     data, grid, pdf

**REQUIRED** `desc =` The description of the plot (used as the title if non-empty)

**Optional** `data_directory =` A list of data directories to be prepended to the corresponding data files below, separated by commas. This can either be empty, in which case `./` is prepended, have **one** value, in which case it is prepended to **all** data files, or have the exact same number as the number of data files. This holds for all directory settings.

**Optional** `grid_directory =`

**Optional** `pdf_directory =`

**REQUIRED** `data_steering_files =` A list of all data steering files, separated by commas

**REQUIRED** `grid_steering_files =` A list of all grid steering files, separated by commas

**REQUIRED** `pdf_steering_files =` A list of all PDF steering files, separated by commas

**Optional** `data_marker_style =` A list of data marker styles to be applied. This follows the same convention as directories, where the list should either be empty, contain one item, or contain exactly the number of items as the number of data steerings. Documentation for the marker styles can be found [here](http://root.cern.ch/root/html/TAttMarker.html).

**Optional** `data_marker_color =` A list of data marker colors. Documentation for the marker colors can be found [here](http://root.cern.ch/root/html/TAttMarker.html).

**Optional** `pdf_fill_style =` A list of PDF fill styles. Also follows the same convention in terms of number of items, but with respect to the number of PDF steering files. Documentation for the PDF fill styles can be found [here](http://root.cern.ch/root/html/TAttFill.html).

**Optional** `pdf_fill_color =` A list of PDF fill colors. If markers are plotted instead of bands, this will also be the color of the marker. Documentation for the PDF fill colors can be found [here](http://root.cern.ch/root/html/TAttFill.html).

**Optional** `pdf_marker_style =` A list of PDF marker styles. These have the same value options as `data_marker_style`

**Optional** `x_scale =` A list of X scales with which to artificially scale the graph's X axis

**Optional** `y_scale =` A list of Y scales with which to artificially scale the graph's Y axis

**Optional** `x_log =` true or **false**: Logarithmic X axis

**Optional** `y_log =` true or **false**: Logarithmic Y axis

**REQUIRED** `display_style =` `overlay` **or** `ratio`: Which sections of the plot to display. You must specify at least `display` or `ratio`

**REQUIRED** `overlay_style =` `data` **or** `convolute` **or** `pdf`: What to display on the overlay section

**Optional** `ratio_title =` A custom title for the ratio section. If not specified, will default to 'Ratio'

To actually define ratios, you must give them both a style and a value. Thus, there are two settings that must be set for each ratio: `ratio_style_n` and `ratio_n`, where `n` starts at `0` and increments for each ratio.

**Optional** `ratio_style_n =`
> `data_tot` **xor** <br>
> `data_stat` **xor** <br>
> `convolute / data` **xor** <br>
> `convolute / reference` **xor** <br>
> `convolute / nominal` **xor** <br>
> `data / convolute` **xor** <br>
> `data / data`

`ratio_style_0 = data_stat` put data statistical uncertainty in ratio (and does not add it in ratio uncertainty)
`ratio_style_0 = data_tot` put data total uncertainty in ratio (and does not add it in ratio uncertainty)

** Example ** Plots first total data statistics, then total data statistic, then ratio (data uncertainties are not in ratio)
`ratio_style_0 = data_tot` 
`ratio_style_1 = data_stat` 
`ratio_style_2 = data/convolute` 

**Optional** `ratio_n =` You **must** follow the exact syntax for specifying the ratios, otherwise you will be issued errors:

> To specify a convolute: `(grid_file.txt, pdf_file.txt)` <br>
> To specify data: `(data_file.txt)` <br>
> Thus, to specify `ratio_style_n = convolute / data`, `ratio_n` would be: `(grid_file.txt, pdf_file.txt) / (data_file.txt)`

For the special cases of `data_tot` and `data_stat`, you just need to specify a data file, and you do not need the `()`, although you can use them if you want.

**NOTE:** You can also alias the data, grid, and PDF files by using `data_i`, `grid_i`, and `pdf_i` in place of the full name, where `i` refers to the index in the `..._steering_files = ` list. Also note that the corresponding directory, if specified, will also be prepended to the ratio files. This means you should either follow exactly the name given in the `..._steering_files =` list, or use aliases.

> `ratio_style_0 = data_tot` <br>
> `ratio_0 = data_0`

Is the same as:

> `ratio_style_0 = data_tot` <br>
> `ratio_0 = data_steering_file.txt`

##Example
An example steering file:

```
;==============================================================
; General Options
;==============================================================
[GEN]
debug = true

;==============================================================
; Graphing Options
;==============================================================
[GRAPH]
; plot_band =
; plot_error_ticks =
plot_marker = true
plot_staggered = true
match_binning = false
x_legend = 0.9
y_legend = 0.9

;==============================================================
; Plot 0 Options
;==============================================================
[PLOT_0]
plot_type = data[], grid[], pdf

data_directory = Data/Zincl/atlas/
grid_directory = Grids/Zincl/atlas/sherpa/
pdf_directory = PDF

data_steering_files = atlas_2010_inclZpt_ee_born.txt,atlas_2010_inclZpt_mm_born.txt
grid_steering_files = atlas_2010_inclZpt_ee.txt,atlas_2010_inclZpt_mm.txt
pdf_steering_files = CT10.txt

x_scale = 1.0, 1.0
y_scale = 1.0, 1.0

x_log = false
y_log = true

data_marker_style = 20, 21
data_marker_color = 1, 1

;pdf_fill_style = 3005
;pdf_fill_color = 600
;pdf_marker_style = 21

desc = atlas_inclusive_Zptmm

display_style = overlay,ratio
overlay_style = convolute,data

ratio_title = NLO QCD/Data

ratio_style_0 = data_tot
ratio_0 = data_0

ratio_style_1 = data_stat
ratio_1 = data_0

ratio_style_2 = convolute / data
ratio_2 = ([grid_0, pdf_0]) / (data_0)

ratio_style_3 = convolute / data
ratio_3 = ([grid_1, pdf_0]) / (data_1)
```
