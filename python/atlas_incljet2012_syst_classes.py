#!/usr/bin/env python
# -*- coding: utf-8 -*-
import math
from subprocess import call

import sys
sys.path.insert(0, 'python')

#
import SpectrumSteering
from SpectrumSteering import Gen
from SpectrumSteering import Plot
from SpectrumSteering import Graph
from SpectrumSteering import EtaLoop


#from optparse import OptionParser
    
if __name__ == "__main__":
   
 print 'Number of arguments:', len(sys.argv), 'arguments.'
 print 'Argument List:', str(sys.argv)

 if  len(sys.argv) < 3:
  print 'launch with atlas_incljet2012_syst_classes ieta ir '
  exit()

 eta=int(sys.argv[1])
 rad=int((sys.argv[2]))

 print 'eta= ',eta,' rad= ',rad

 name='main'

 gen=Gen()
 graph=Graph()
 
# print name,' \n Set attributes '

 gen.debug=False

 graph.plot_band=False
 graph.plot_marker=True
 graph.plot_staggered=True
 graph.match_binning=True
 graph.apply_grid_corr=True

# graph.show_systematics_as_lines=5.
 graph.show_individual_systematics=0
 graph.show_total_systematics=1
 graph.order_systematic_colorbyalphabeth=True
# graph.calculate_chi2 = 1
# graph.label_chi2 = True
# graph.calculate_chi2 = 0
 graph.label_chi2 = False

 graph.grid_parameter_scan=False
 graph.alternative_grid_divided_by_doublediff_bin_width=False

 graph.label_date=False 
 graph.label_sqrt_s=True 
 graph.label_scaleform=True 

 graph.x_legend=0.35
 graph.y_legend=0.92

 graph.x_info_legend=0.45
 graph.y_info_legend=0.27

 graph.band_with_pdf= False
 graph.band_with_alphas= False
 graph.band_with_scale = False
 graph.band_with_gridcorrection = False
 graph.band_total = False


 graph.label_informationlegend="ATLAS internal"
 graph.grid_parameter_scan = False
 graph.label_chi2 = False

 year=2012
 griddir=''
 dataset=''

 if year==2010:
  dataset=''

 if year==2011:
  dataset=''

 if year==2012:
  dataset='_highmu'

 gen.CreateSteering()
 graph.CreateSteering()

   
#loop over eta
# rlist=['4','6'] 
 rlist=['4'] 

 iplot=0

# Attention Spectrum software does not work in loop need to pot one by one
# for ieta in range(1,7):
 for ieta in range(eta,eta+1):
# for ieta in range(1,2): 
  for ir in range(rad,rad+1):
#  for ir in rlist: 

   print 'in plot ', ieta ,' r= ',ir,' iplot= ',iplot

   plot=Plot()
   plot.name='[Plot_'+str(iplot)+']'
   iplot=iplot+1

   #plot.data_cut_xmax=1500
   #plot.data_cut_xmin=100

   plot.remove_systematic_group = 'lumi'
   plot.display_systematic_group = 'JES,JER,other'
   plot.display_systematic_group_fill_color = '623,400,1'
   plot.display_systematic_group_edge_color = '629,419,-1'
   plot.display_systematic_group_edge_style = '1,1,2'
   plot.display_systematic_group_edge_width = '4,4,4'

#   if ieta==1:
#     plot.y_ratio_max=2.0
#     plot.y_ratio_min=0.5

#   plot.x_info_legend=0.8

#   if ieta==1:
#     plot.y_ratio_max=1.8
#     plot.y_ratio_min=0.4

#   if ieta==2:
#     plot.y_ratio_max=2.0
#     plot.y_ratio_min=0.3

#   if ieta==3:
#     plot.y_ratio_max=4.1
#     plot.y_ratio_min=-0.2

#   if ieta==4:
#     plot.y_ratio_max=2.0
#     plot.y_ratio_min=0.41

#   if ieta==5:
#     plot.y_ratio_max=3.0
#     plot.y_ratio_min=0.2

#   if ieta==6:
#     plot.y_ratio_max=3.0
#     plot.y_ratio_min=0.2

#   plot.x_legend = 0.3
#   plot.x_info_legend = 0.8

   plot.data_directory = 'Data/jet/atlas/incljets'+str(year)
   plot.data_steering_files = 'atlas_'+str(year)+'_jet_antiktr0'+str(ir)+'_incljetpt_eta'+str(ieta)+dataset+'.txt'

   plot.plot_type = 'data'  
   plot.desc = 'atlas_inclusive_jet'+str(year)+'_data_syst_groups'+dataset+'_r0'+str(ir)+'_eta'+str(ieta)

   plot.x_log=True
   plot.y_log=True

   plot.display_style = 'ratio'
   plot.overlay_style = 'data'

   plot.ratio_title = 'Systematic uncertainties'
   plot.ratio_style_0 = 'data/ !data'
   plot.ratio_0 = 'data_0 / data_0'

   plot.CreateSteering()

# graph.ShowAll()
# plot.ShowAll()



# graph.launch()





