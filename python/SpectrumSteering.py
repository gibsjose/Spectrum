#!/usr/bin/env python
# -*- coding: utf-8 -*-
from subprocess import call
from collections import OrderedDict

def CreateStringfromList(mylist):

 mystring=''
 for i in mylist:
  #print 'i= ',i
  mystring=mystring+i+','

 mystring=mystring[:-1]
 return mystring

def RepeatString(myname,n):
 mystring=''
 for i in range(0,n):
  #print 'i= ',i
  mystring=mystring+myname+','

 mystring=mystring[:-1]
 return mystring


def counted(f):
    def wrapped(*args, **kwargs):
        wrapped.calls += 1
        return f(*args, **kwargs)
    wrapped.calls = 0
    return wrapped
class SpectrumSteering(object):
 
  name='SpectrumSteering'
  #fout=file('test.txt')
  fout=open('test.txt', "w")  

  def __init__(self):
     print ' SpectrumSteering initialized '

  def launch(self):
     #SpectrumSteering.fout.write('Hello')     
     print ' call Spectrum disabled'
     #call(["ls -l","test.txt"])
     #call(["./Spectrum", "test.txt"])

     call(["ls","-l", "test.txt"])
     #call(["./Spectrum", "test.txt"])

  @counted
  def SetRatio(self,mystring):
   name='SetRatio'
   print name,' mystring= ',mystring

#   print 'Number of calls ',self.SetRatio.calls
 
   # The below is a bit ugly. Is there a better way to program this ?
   for i in range(0,9):
    if not self.ratio_0:
     self.ratio_0=mystring
     break

    if not self.ratio_1:
     self.ratio_1=mystring
     break

    if not self.ratio_2:
     self.ratio_2=mystring
     break

    if not self.ratio_3:
     self.ratio_3=mystring
     break

    if not self.ratio_4:
     self.ratio_4=mystring
     break

    if not self.ratio_5:
     self.ratio_5=mystring
     break

    if not self.ratio_6:
     self.ratio_6=mystring
     break

    if not self.ratio_7:
     self.ratio_7=mystring
     break

    if not self.ratio_8:
     self.ratio_8=mystring
     break

    if not self.ratio_9:
     self.ratio_9=mystring
     break

  def SetRatioStyle(self,mystring):
   name='SetRatioStyle'
   print name,' mystring= ',mystring
 
   # The below is a bit ugly. Is there a better way to program this ?
   for i in range(0,9):
    if not self.ratio_0:
     self.ratio_style_0=mystring
     break

    if not self.ratio_1:
     self.ratio_style_1=mystring
     break

    if not self.ratio_2:
     self.ratio_style_2=mystring
     break

    if not self.ratio_3:
     self.ratio_style_3=mystring
     break

    if not self.ratio_4:
     self.ratio_style_4=mystring
     break

    if not self.ratio_5:
     self.ratio_style_5=mystring
     break

    if not self.ratio_6:
     self.ratio_style_6=mystring
     break

    if not self.ratio_7:
     self.ratio_style_7=mystring
     break

    if not self.ratio_8:
     self.ratio_style_8=mystring
     break

    if not self.ratio_9:
     self.ratio_style_9=mystring
     break


  def CreateSteering(self):
     name='CreateSteering:'

     print ' '
     SpectrumSteering.fout.write('\n'+self.name+'\n')

#     ordereddict= OrderedDict([('data_directory',''),('data_steering_files','')])
#     vardict=OrderedDict(ordereddict.items()+orgdict.items())

#     vardict= OrderedDict( self.__dict__)

     vardict=self.__dict__

     for key in vardict:
      #print self.name,name,'Set ', key, ' to', vardict[key]
      if key!='name':
       self.SetVar(key)

     #for key in self.__dict__:
     # print 'Set ', key, ' to', self.__dict__[key]
     # self.SetVar(key)

  def ShowAll(self):
     print ' '
     for key in self.__dict__:
      print self.name,key,' ',self.__dict__[key]

  def SetVar(self,variable):
#      if variable: 
#       print 'variable= ',variable
#      else:
#       print variable, 'default '

     val=self.__dict__[variable]

     txt=''
     if isinstance(val, (bool)):
      txt+=str(variable)+' = '
      if val:
       txt+='true'
      else:
       txt+='false'
     else:
      if isinstance(val, (float,int)):
       if val!=0:
        txt+=str(variable)+' = '+str(val)
      else:
       if val:
        txt+=str(variable)+' = '+val

     if txt:
      print self.name,' write to file: ',txt
      SpectrumSteering.fout.write(txt+'\n')
     return


class Gen(SpectrumSteering):
   name='[Gen]'

   def __init__(self):
    self.debug = True
    self.output_rootfile=''
    self.output_graphicformat=''

class Graph(SpectrumSteering):

   name='[Graph]'

   debug=True

   def __init__(self):

    self.show_individual_systematics = 0
    self.plot_marker = True
    self.plot_error_ticks = True
    self.plot_staggered = True
    self.match_binning = True
    self.apply_grid_corr = True
    self.apply_nominal_corr = True
    self.contain_grid_corr = ''
    self.show_systematics_as_lines = 0.
    self.show_total_systematics = 0
    self.order_systematic_colorbyalphabeth = True
    self.label_sqrt_s = True
    self.label_scaleform = True
    self.label_date = True
    self.label_scaleform = True
    self.label_scaleform_alternative_scale_choice= True
    self.label_informationlegend =""

    self.take_sign_intoaccount_for_total_error = True

    self.alternative_grid_divided_by_bin_width=True
    self.alternative_grid_divided_by_doublediff_bin_width=True

    self.grid_parameter_scan = True

    self.band_with_pdf= True 
    self.band_with_alphas= True 
    self.band_with_scale= True 
    self.band_with_alternative_scalechoice= True 
    self.band_total= True 
    self.band_with_gridcorrection= True 
    self.band_with_beamuncertainty = 0

    # this can be an array -> need to do something
    self.ren_scales =0
    self.fac_scales =0

    self.y_overlay_min=0
    self.y_overlay_max=0
    self.x_overlay_min=0
    self.x_overlay_max=0
    self.y_ratio_min=0
    self.y_ratio_max=0

    self.x_legend = 0.
    self.y_legend = 0.
    self.x_info_legend = 0.
    self.y_info_legend = 0.

    self.calculate_chi2=0.
    self.label_chi2= True


class Plot(SpectrumSteering):

   name='[PLOT_XXX]'
    
   def __init__(self):
     self.plot_type = ''

     self.pdf_directory = ''
     self.grid_directory = ''
     self.data_directory = ''

     self.pdf_steering_files = ''
     self.grid_steering_files = ''
     self.data_steering_files = ''

     self.desc = ''
     self.remove_systematic_group = ''
     self.contain_systematic_group = ''

     self.y_overlay_min=0
     self.y_overlay_max=0
     self.x_overlay_min=0
     self.x_overlay_max=0
     self.y_ratio_min=0
     self.y_ratio_max=0

     self.x_legend = 0.
     self.y_legend = 0.
     self.x_info_legend = 0.
     self.y_info_legend = 0.

     self.data_cut_xmax = 0.
     self.data_cut_xmin = 0.
     self.contain_systematic_group = ''
     self.remove_systematic_group = ''
     self.display_systematic_group = ''
     self.display_systematic_group_fill_color = ''
     self.display_systematic_group_edge_color = ''
     self.display_systematic_group_edge_style = ''
     self.display_systematic_group_edge_width = ''
     self.x_log=True
     self.y_log=True

     self.data_marker_style=''
     self.data_marker_color=''

     self.total_fill_style=0
     self.total_fill_color=0
     self.total_edge_style=0
     self.total_edge_color=0
     self.total_marker_style=0
     self.total_marker_color=0

     self.pdf_fill_style=0
     self.pdf_fill_color=0
     self.pdf_edge_style=0
     self.pdf_edge_color=0
     self.pdf_marker_style=0
     self.pdf_marker_color=0

     self.scale_fill_style=0
     self.scale_fill_color=0
     self.scale_fill_style=0
     self.scale_edge_style=0
     self.scale_edge_color=0
     self.scale_marker_style=0
     self.scale_marker_color=0

     self.alternative_scale_choice_fill_style=0
     self.alternative_scale_choice_fill_color=0
     self.alternative_scale_choice_edge_style=0
     self.alternative_scale_choice_edge_color=0
     self.alternative_scale_choice_marker_style=0
     self.alternative_scale_choice_marker_color=0

     self.alphas_fill_style=0
     self.alphas_fill_color=0
     self.alphas_edge_style=0
     self.alphas_edge_color=0
     self.alphas_marker_style=0
     self.alphas_marker_color=0

     self.corrections_fill_style=0
     self.corrections_fill_color=0
     self.corrections_edge_color=0
     self.corrections_edge_style=0
     self.corrections_marker_style=0
     self.corrections_marker_color=0

     self.beamuncertainty_fill_style=0
     self.beamuncertainty_fill_color=0
     self.beamuncertainty_edge_style=0
     self.beamuncertainty_edge_color=0
     self.beamuncertainty_marker_style=0
     self.beamuncertainty_marker_color=0

     self.data_cut_xmax=0
     self.data_cut_xmin=0

     self.display_style = ''
     self.overlay_style = ''

     self.ratio_title = ''
     self.ratio_style_0 = ''
     self.ratio_style_1 = ''
     self.ratio_style_2 = ''
     self.ratio_style_3 = ''
     self.ratio_style_4 = ''
     self.ratio_style_5 = ''
     self.ratio_style_6 = ''
     self.ratio_style_7 = ''
     self.ratio_style_8 = ''
     self.ratio_style_9 = ''

     self.ratio_0 = ''
     self.ratio_1 = ''
     self.ratio_2 = ''
     self.ratio_3 = ''
     self.ratio_4 = ''
     self.ratio_5 = ''
     self.ratio_6 = ''
     self.ratio_7 = ''
     self.ratio_8 = ''
     self.ratio_9 = ''




class EtaLoop:
  etalist = []

  def __init__(self, ieta):
     self.ieta = ieta
     self.etalist.append(self)
