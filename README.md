![Spectrum](artwork/SpectrumBanner.png)

## Authors
@gibsjose @csembree Dr. Tancredi Carli

## Description
Spectrum plots cross-sections, theory results, and experimental data for proton-proton collisions. It is a collaboration between Dr. Tancredi Carli (CERN) and a few research assistants. Spectrum works with multiple grids, as well as data from multiple experiments, including CMS, ATLAS, and HERA.

### NOTE
Spectrum is a re-design of the previous [MyAnalysis](http://www.github.com/gibsjose/MyAnalysis) software. Upgrades have been made on the front-end of the software, as well as a re-design of the program structure. While most of Spectrum has been completely re-written, some of the most complex classes (like [SPXPDF](src/SPXPDF.cxx)) and some graphing/math utilities were taken directly from the previous software, which was written entirely by Cameron Embree and Dr. Carli.

## UML
![Top Level UML](uml/top_level_uml.png)

## Supported Data Formats
Spectrum currently supports the following data formats:
* Spectrum T1S (Total systematic errors, symmetric)
* Spectrum T1A (Total systematic errors, asymmetric)
* Spectrum T2S (Total + individual systematic errors, symmetric)
* Spectrum T2A (Total + individual systematic errors, asymmetric)
* Spectrum T3S (Individual systematic errors, symmetric)
* Spectrum T3A (Individual systematic errors, asymmetric)
* HERAFitter

More information on the supported data formats can be found in [data_formats.txt](Data/data_formats.txt)

## Installation
Installation instructions can be found [here](INSTALL.md)

## How to Run a Spectrum Plot
Simply edit your Steering file according to your plot specifications, and run Spectrum on it:

```bash
./Spectrum steering_file.txt
```

This will produce both a TCanvas with the interactive plot, as well as a PNG image saved to the `plots` directory.

If you would like to suppress the ROOT output and instead only output the PNG, use the `-p` flag:

```bash
./Spectrum -p steering_file.txt
```

## Steering Files
The term 'Steering File' refers to a (set of) configuration file(s) read by Spectrum to change the plot parameters. There is one top-level steering file, and any number of Data, Grid, and PDF steering files, depending on the plot type.

For example, say we wanted to plot ATLAS and CMS data, each with the same PDF (say, CT10).

The Steering File layout would look something like this:

* 1 Top-Level Steering File
* 2 Data Steering Files (ATLAS, CMS)
* 2 Grid Steering Files (ATLAS, CMS)
* 1 PDF Steering File (CT10)

In the top-level steering file, under plot_type, you would set it to `plot_type = data[], grid[], pdf`, and then specify the actual steering files in the `data_steering_files`, `grid_steering_files`, and `pdf_steering_files` variables.

See the examples below for more detailed look at what each file contains, which settings are required/optional, and the overall layout of the steering files.

### Top-Level Steering File
The top level steering file contains the basic information required for configuring plots. You can set overall settings like the base Data, Grid, and PDF directory, graphing options like whether to plot bands or markers, and also individual plot settings (denoted by the `[PLOT_x]` section heading) such as the actual data, grids, pdfs to be plotted, marker styles, fill colors, and a number of other settings.

[Example](Steering/example.txt) Steering File

### Data Steering File
The data steering file contains information specific to each dataset, such as the data name, experiment and reaction, as well as graphing information such as the value of sqrt(S), legend labels, axis labels, units, and so on. The data steering file is also where you give the filepath of the actual data file, as well as it's type (see **_Supported Data Formats_** above).

[Example](Data/example.txt) Data Steering File

### Grid Steering File
The grid steering file contains information specific to each grid, such as the grid name and units, as well as the filepath of the actual (.root) grid file.

[Example](Grids/example.txt) Grid Steering File

### PDF Steering File
The PDF steering file contains information specific to each PDF, such as the name, type, PDFset path, graphing options like fill style and color, as well as a number of other PDF-specific options.

[Example](PDF/example.txt) PDF Steering File

## Credits
Credits to all contributors and any external code I have used, whether modified or not can be found [here](CREDITS.md)
