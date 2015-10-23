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
   
 #parser = OptionParser("usage: %prog [options] input")
 #parser.add_option(fname="data.txt" )
 #opts, args = parser.parse_args()

 #name=str(args[0])

 name='main'

 gen=Gen()
 graph=Graph()
 
 print name,' \n Set attributes '

#gen.debug=False

 graph.plot_marker=True
 graph.plot_staggered=True
 graph.match_binning=False
 graph.apply_grid_corr=False

 graph.show_systematics_as_lines=5.
 graph.show_individual_systematics=0
 graph.show_total_systematics=1
 graph.order_systematic_colorbyalphabeth=True
 #graph.calculate_chi2 = 1

 graph.label_chi2=False
 graph.grid_parameter_scan=False
 graph.alternative_grid_divided_by_doublediff_bin_width=False

 graph.label_sqrt_s=True 

 graph.x_legend=0.45
 graph.y_legend=0.95
 graph.x_info_legend=0.83
 graph.y_info_legend=0.28

 #graph.x_ratio_min=0.5
 #graph.x_ratio_max=1.5

# graph.y_ratio_min=0.96
# graph.y_ratio_max=1.05

 gen.CreateSteering()
 graph.CreateSteering()

   
 #loop over eta
 rlist=['4','6'] 
# rlist=['4'] 
# systlist=['JES_Zjet','JES_Gjet','JES_LAr','JES_Flavour','JES_MJB','JES_EtaIntercalibration','Pileup','JER'] 
# systlist=['JES_Zjet','JES_Gjet'] 
 systlist=['Pileup'] 

 iplot=0
 for ieta in range(1,6): 
  for ir in rlist: 
   for isyst in systlist: 
 
    print 'in plot ', ieta ,' r= ',ir,' syst= ',isyst,' iplot= ',iplot

    plot=Plot()
    plot.name='[Plot_'+str(iplot)+']'
    iplot=iplot+1

    plot.plot_type = 'data'  

    plot.data_directory = 'Data/jet/atlas/incljets2012'
    plot.data_steering_files = 'atlas_2012_jet_antiktr0'+str(ir)+'_incljetpt_eta'+str(ieta)+'_comb.txt'
    plot.desc = 'data_syst_groups_comb_'+isyst+'_R'+str(ir)+'_ETA'+str(ieta)

    #plot.remove_systematic_group = 'JES'
    plot.contain_systematic_group = isyst

    #plot.display_systematic_group = 'JES,JER,Others'
    #plot.display_systematic_group_fill_color = 623
    #plot.display_systematic_group_edge_color = 629
    #plot.display_systematic_group_edge_style = 1
    #plot.display_systematic_group_edge_width = 4

    #plot.data_cut_xmax = 400.
    #plot.data_cut_xmin = 100.

    #plot.x_log=True
    #plot.y_log=True

    plot.display_style = 'ratio'
    plot.overlay_style = 'data'

    plot.ratio_title = 'Systematic uncertainties'
    plot.ratio_style_0 = 'data / !data'
    plot.ratio_0 = 'data_0 / data_0'

    #plot.ShowAll()

    plot.CreateSteering()
# graph.ShowAll()
# plot.ShowAll()



# graph.launch()





