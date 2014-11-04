# Spectrum v0.2
### 04 November, 2014

## Authors
@gibsjose @csembree Dr. Tancredi Carli

## Change Log
* Implemented using the `!` flag in ratio_style_x to signal zeroing of numerator/denominator ratio graph Y errors:
    * `ratio_style_0 = convolute / data` will add all the Y errors in quadrature (`DivideErrorType_t: AddErrors`)
    * `ratio_style_0 = convolute / !data` will zero the denominator (data) Y errors when dividing (`DivideErrorType_t: ZeroNumGraphErrors`)
    * `ratio_style_0 = !convolute / data` will zero the numerator (convolute) Y errors when dividing (`DivideErrorType_t: ZeroDenGraphErrors`)
    * `ratio_style_0 = !convolute / !data` will zero BOTH the numerator and denominator Y errors when dividing (`DivideErrorType_t: ZeroAllErrors`)

## In Progress (v1.1)
* Legends (overlay/ratio)
* Steering File Generator program
* Clarify/fix optional vs. required steering file options
* HERAPDF data format in SPXDataFormat
* Data only plot type in SPXPlotType
