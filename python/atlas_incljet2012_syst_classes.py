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

#from optparse import OptionParser
    
if __name__ == "__main__":
   
 #parser = OptionParser("usage: %prog [options] input")
 #parser.add_option(fname="data.txt" )
 #opts, args = parser.parse_args()

 #name=str(args[0])

 name='main'

 gen=Gen()
 graph=Graph()
 plot=Plot()

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
 graph.calculate_chi2 = 1

 graph.label_sqrt_s=True 

 graph.x_legend=0.45
 graph.y_legend=0.90
 graph.x_info_legend=0.25
 graph.y_info_legend=0.40

 #graph.x_ratio_min=0.5
 #graph.x_ratio_max=1.5

 graph.y_ratio_min=0.5
 graph.y_ratio_max=1.5


 #print graph.__class__.__name__,graph.__dict__

 plot.plot_type = 'data'
 plot.pdf_directory = 'PDF'
 plot.data_directory = 'Data/jet/atlas/incljets2012'
 plot.data_steering_files = 'atlas_2012_jet_antiktr04_incljetpt_eta1_comb.txt'
 plot.desc = 'data_syst_groups_comb_JES_R4_ETA1'
 #plot.remove_systematic_group = 'JES'
 plot.contain_systematic_group = 'JES_Zjet'

 #plot.display_systematic_group = 'JES,JER,Others'
 #plot.display_systematic_group_fill_color = 623
 #plot.display_systematic_group_edge_color = 629
 #plot.display_systematic_group_edge_style = 1
 #plot.display_systematic_group_edge_width = 4

 plot.data_cut_xmax = 400.
 plot.data_cut_xmin = 100.

 plot.x_log=True
 plot.y_log=True
 plot.display_style = 'ratio'
 plot.overlay_style = 'data'

 plot.ratio_title = 'Systematic uncertainties'
 plot.ratio_style_0 = 'data / !data'
 plot.ratio_0 = 'data_0 / data_0'

 graph.ShowAll()
 plot.ShowAll()

 gen.CreateSteering()
 graph.CreateSteering()
 plot.CreateSteering()

# graph.launch()





