![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#Grid Steering File

###Description
The grid steering file specifies settings specific to the grids.
###Options

**NOTE:** Where applicable, a **bold** value is the default.

##`[GEN]`
**Optional** `debug =` true or **false**: Turn on or off debug output

##`[DESC]`
**Optional** `name =` Grid name

**Optional** `author =` Grid author

**Optional** `lumi_config_file=` Sub-process configuration file for grid program (i.e APPLgrid)

**Optional** `scale =` Choice of renormalization scale (Q^2)

**Optional** `reference_journal_name =` Reference journal name

**Optional** `reference_link_to_arxiv =` Reference link to arXiv

**Optional** `nlo_program_name =` NLO program used to generate grids (ex: Sherpa)

**Optional** `grid_program_name =` Program used as an interface between NLO program and APPLgrid (ex: mcgrid)

**Optional** `observable_definition_link_to_code =` Link to the piece of code/file which defines the observable and applies event selection cut, fills APPLgrid, etc.

##`[GRAPH]`
**REQUIRED** `x_units =` `MeV` **xor** `GeV` **xor** `TeV`

**REQUIRED** `y_units =` `nb` **xor** `pb` **xor** `fb`

**Optional** `y_scale =` Artificially scale the Y axis

**Optional** `grid_divided_by_bin_width =` true or **false**: Whether the grid has **already** been divided by the bin width

**Optional** `reference_divided_by_bin_width =` true or **false**: Whether the reference has **already** been divided by the bin width

##`[GRID]`
**REQUIRED** `grid_file =` Path to the grid's ROOT file

**Optional** `lowest_order =` An integer (0 to 2) representing:

> `0` --> `LO` **xor** <br>
> `1` --> `NLO` **xor** <br>
> `2` --> `NNLO`

Set the lowest order of what is in the grid

##Example
An example data steering file:

```
;==============================================================
; General Options
;==============================================================
[GEN]
debug = true

;==============================================================
; Description
;==============================================================
[DESC]
name = atlas_2011_top_partons_mtt
author = John Smith
lumi_config_file = top.config
scale = Mtop
reference_journal_name = Phys.Rev. D90 (2014) 072004
reference_link_to_arxiv = http://arxiv.org/abs/arXiv:1407.0371
nlo_program_name = mcfm
grid_program_name= mcfm-bridge
observable_definition_link_to_code = unknown

;==============================================================
; Graphing Options
;==============================================================
[GRAPH]
x_units = GeV
y_units = fb
grid_divided_by_bin_width = true
reference_divided_by_bin_width = true

;==============================================================
; Grid Options
;==============================================================
[GRID]
grid_file = grid--TTbar_mttatlas.root
lowest_order = 2
```
