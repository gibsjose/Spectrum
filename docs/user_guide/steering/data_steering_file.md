![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#Data Steering File

###Description
The data steering file specifies settings specific to the data set.

###Options

**NOTE:** Where applicable, a **bold** value is the default.

##`[GEN]`
**Optional** `debug =` true or **false**: Turn on or off debug output

##`[DESC]`
**Optional** `name =` Data name

**Optional** `experiment =` Experiment name (CMS, ATLAS, etc.)

**Optional** `reaction =` Reaction (ex: `P P --> top top`)

**Optional** `data_set_year =` Year of dataset

**Optional** `data_set_lumi =` Dataset luminosity

**Optional** `reference_journal_name =` Reference journal name

**Optional** `reference_journal_year =` Reference journal year

**Optional** `reference_arxiv_number =` Reference arXiv ID

**Optional** `reference_arxiv_year =` Reference arXiv year

**Optional** `publication_status =` Publication status

##`[GRAPH]`
**REQUIRED** `sqrt_s =` Value of square-root of S

**REQUIRED** `legend_label =` Label describing dataset to be displayed on legend

**REQUIRED** `x_label =` Label for X axis (supports LaTeX)

**REQUIRED** `y_label =` Label for Y axis (supports LaTeX)

**REQUIRED** `x_units =` `MeV` **xor** `GeV` **xor** `TeV`

**REQUIRED** `y_units =` `nb` **xor** `pb` **xor** `fb`

**REQUIRED** `y_bin_width_units = `MeV` **xor** `GeV` **xor** `TeV`: Whatever is in the Y-Axis label: ```[1 / ...]```

**Optional** `jet_algorithm_label =` Jet algorithm label

**Optional** `jet_algorithm_radius =` Jet algorithm radius

##`[DATA]`
**REQUIRED** `data_format =` `spectrum` **xor** `herafitter`

**REQUIRED** `data_file =` Path to data file

**Optional** `divided_by_bin_width =` true or **false**: Whether the data has **already** been divided by the bin width

**Optional** `normalized_to_total_sigma =` true or **false**: Whether the data has **already** been normalized to the total sigma

**Optional** `error_in_percent =` true or **false**: Whether the errors in the data file are reported in percents

**Optional** `lumi_scale_factor =` Luminosity scale factor

##Example
An example data steering file:

```
;==============================================================
; General Options
;==============================================================
[GEN]
debug = false

;==============================================================
; Description
;
; http://inspirehep.net/record/1304289
;==============================================================
[DESC]
name = ATLAS_2011_top_partons_mtt
experiment = ATLAS
data_set_year = 2011
data_set_lumi = 4.5 fb^{-1}
reaction = p p --> top top
reference_journal_name = Phys.Rev. D90 (2014) 072004
reference_journal_year = 2014
reference_arxiv_number = arXiv:1407.0371
reference_arxiv_year = 2014
publication_status = published

;==============================================================
; Graph Options
;==============================================================
[GRAPH]
sqrt_s = 7000.0
legend_label = ATLAS 2014
x_label = m_{t,#bar{t}} [GeV]
y_label = 1/#sigma d#sigma/dm_{t,#bar{t}} [1/TeV]
x_units = GeV
y_units = pb
y_bin_width_units = TeV
; jet_algorithm_label = Anti_k{t}
; jet_algorithm_number = 4

[DATA]
data_format = spectrum
data_file =atlas_2011_top_partons_mtt_data.txt
divided_by_bin_width = true
error_in_percent = true
normalized_to_total_sigma = true
```
