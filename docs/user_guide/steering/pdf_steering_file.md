![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#PDF Steering File

###Description
The PDF steering file specifies settings specific to the PDFs.

###Options

**NOTE:** Where applicable, a **bold** value is the default.

##`[GEN]`
**Optional** `debug =` true or **false**: Turn on or off debug output

##`[DESC]`
**REQUIRED** `name =` PDF name: Used to determine PDFset

**REQUIRED** `type =` PDF type (ex: CT10, etc.)

**Optional** `author =` Grid author

**Optional** `order =` `LO`, `NLO`, `NNLO`: Order for convolution

**REQUIRED** `num_members =` Number of members of PDF (ex: 53, 41, 101, 33, etc.)

**Optional** `pdf_set_path =` Override the default PDFset path (`./PDFsets`)

##`[STYLE]`
**REQUIRED** `fill_style =` [PDF fill style](http://root.cern.ch/root/html/TAttFill.html)

**REQUIRED** `fill_color =` [PDF fill color](http://root.cern.ch/root/html/TAttFill.html)

**REQUIRED** `marker_style =` [PDF marker style](http://root.cern.ch/root/html/TAttMarker.html)


##`[PDF]`
**REQUIRED** `default_id =` Default PDF ID

**REQUIRED** `first_eig =` First eigenvector variation ID

**REQUIRED** `last_eig =` Last eigenvector variation ID

**REQUIRED** `first_quad =` First uncertainty to be added in quadrature

**REQUIRED** `last_quar =` Last uncertainty to be added in quadrature

**REQUIRED** `first_max =` First uncertainty in which to look for a maximum

**REQUIRED** `last_max =` Last uncertainty in which to look for a maximum

**Optional** `include_eig =` true or **false**: Include eigenvectors in band

**Optional** `include_quad =` true or **false**: Include paramaterization uncertainties added in quadrature

**Optional** `include_max =` true or **false**: Include model uncertainties among which one looks for a maximum

**REQUIRED** `band_type =` `user_alpha_s` **xor** `user_error_band`: Which band type to use

**REQUIRED** `error_type =` `pdf_band` **or** `alpha_s_band` **or** `scale_band`: Which error type to use

**REQUIRED** `error_size =` `one_sigma` **xor** `90_percent`: Size of errors

**REQUIRED** `alpha_s_error_number_up` Alpha S error number

**REQUIRED** `alpha_s_error_number_down` Alpha S error number

**REQUIRED** `alpha_s_pdf_name_up` Alpha S PDF name

**REQUIRED** `alpha_s_pdf_name_down` Alpha S PDF name

**REQUIRED** `alpha_s_pdf_histogram_name_up` Alpha S PDF histogram name

**REQUIRED** `alpha_s_pdf_histogram_name_down` Alpha S PDF histogram name


##Example
An example PDF steering file:

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
name = cteq61
type = CTEQ61
order = 1
num_members = 41

;==============================================================
; Style Options
;==============================================================
[STYLE]
fill_style = 3104
fill_color = 632
marker_style = 23

;==============================================================
; PDF Options
;==============================================================
[PDF]
default_id = 0
default_id_var = 0
band_type = use_error_band
error_type = pdf_band
error_size = 90_percent
alpha_s_error_number_up = 0
alpha_s_error_number_down = 0
alpha_s_pdf_name_up = none
alpha_s_pdf_name_down = none
alpha_s_pdf_histogram_name_up = none
alpha_s_pdf_histogram_name_down = none
```
