#!/usr/bin/env python
# -*- coding: utf-8 -*-

#import ROOT
#import re
import math

from optparse import OptionParser
    
if __name__ == "__main__":
   
   parser = OptionParser("usage: %prog [options] input")
   #parser.add_option(fname="data.txt" )
   opts, args = parser.parse_args()

   fname=str(args[0])

   fnameout=str(args[1])

   dirname=str(args[2])

   #dirname='Data/jet/atlas/incljets2011/'

   print " fname= ",fname," fnameout= ",fnameout," dirname= ",dirname

   # mystring = u"23443434";
   # print mystring.isnumeric();

   filein = open(fname, "r")

   namefiledatafull=fnameout+'_data.txt'
   namefilemetadata=fnameout
   namehadcorr=fnameout+'_hadcorr.txt'
   nameewcorr=fnameout+'_ewcorr.txt'
   #namesimple=fnameout+'.txt'

   filedatafull  =open(namefiledatafull, "w")
   filemetadata  =open(namefilemetadata, "w")
   #filedatasimple=open(namesimple, "w")
   fileewcorr    =open(nameewcorr, "w")
   filehadcorr   =open(namehadcorr, "w")

   filedatafull.write('; Data taken from arxiv1410.8857 \n')

# prepare meta data for steerings

   infoline='\n'+'[GEN]'
   infoline='\n'+'debug = false'
   infoline='\n'+'[DESC]'
   infoline='\n'+'name = ATLAS_2011_incljets'
   infoline+='\n'+'experiment = ATLAS'
   infoline='\n'+'reaction = p p --> jet jet'
   infoline+='\n'+'year = 2011'
   infoline+='\n'+'YRAP 0.0 0.5'
   infoline='\n'+'[GRAPH]'
   infoline+='\n'+'sqrt_s = 7000.0' 
   infoline+='\n'+'legend_label = ATLAS 2011'
   infoline+='\n'+'x_label = p_{T,jet}'
   infoline+='\n'+'y_label = d[s]/dp_{T,jet}'
   infoline+='\n'+'x_units = GeV'
   infoline+='\n'+'y_units = pb'
   infoline+='\n'+'y_bin_width_units= GeV'
   infoline+='\n'+'jet_algorithm_label = Anti-k_{t}' 
   infoline+='\n'+'jet_algorithm_radius = 4'

   infoline+='\n'+'doublediff_binname = #leq |y| < '
   infoline+='\n'+'doublediff_bin_value_min=  '
   infoline+='\n'+'doublediff_bin_value_max=  '
   infoline+='\n'+'doublediff_bin_value_width=  '

   infoline+='\n'+'[DATA]'
   infoline+='\n'+'data_format = spectrum'
   infoline+='\n'+'data_file ='+dirname+namefiledatafull
   #print 'normalizedToTotalSigma no'
   infoline+='\n'+'divided_by_bin_width = true' 
   infoline+='\n'+'divided_by_doublediff_bin_width = false' 
   infoline+='\n'+'error_in_percent = true'
   infoline+='\n'+' '

   #infolinesimple='\n'+'reaction  p p --> jet jet'
   #infolinesimple+='\n'+'YRAP 0.0 0.5'
   #infolinesimple+='\n'+'YEAR 2011'
   #infolinesimple+='\n'+'JETALGO ANTIKT'
   #infolinesimple+='\n'+'JETALGR 4'
   #infolinesimple+='\n'+'EXP ATLAS'
   #infolinesimple+='\n'

   filemetadata.write(infoline)

   #print 'arxiv'
   #print 'lumi'
   print infoline

   #filedatasimple.write(infolinesimple)

   # 2: xmin, xmax
   # 1: cross
   # 3: had+err
   # 3: ew corr
   # 2: data-stat, mc-stat
   # 69: syst +/-
   nsyst=2*69
   n=2+1+3+3+1+1+nsyst
#  print file.read(2)
#  filedatafull.write("mean  xmin xmax cross staterr mcstaterr syst[1-69] \n")
   #filedatasimple.write("  x  xlow xhigh   y     dy+  dy-  dy+  dy- \n")
   filehadcorr.write("hadronisation corrections \n  mean  xmin  xmax  npc  npc_up npc_down \n")
   fileewcorr.write(" electroweak corrections \n ewc  ewc_tree ewc_loop \n")

   datalinefull=''
   systlist=[[]]
   syststatmc=[]
   iline=0
   for line in filein:
     print line
     words = line.split()

     #print words
     mytest=u""+words[0]
     print "words[0] = ", words[0]," isnumeric " ,mytest.isnumeric()

     if ('xlow' in words[0]):
        names=words[11:n]
        print " names found ",names

     #print "after xlow Number of words in line= ",len(words)," n= ",n

     if (len(words)>n-1 and mytest.isnumeric()):
       #print words
       iline=iline+1
       xmin =float(words[0])
       xmax =float(words[1])
       cross=float(words[2])
       stat =float(words[9])
       statmc=float(words[10])
       syst =words[11:n]
       #print iline," xmin= ",xmin," xmax= ",xmax," cross= ",cross," stat= ",stat," statmc= ",statmc," syst= ",syst

       #systline.append(words[10])
       #for isyst in range(10,nsyst):
       #  systline.append(words[isyst])
       systlist.append(words[11:])
       syststatmc.append(words[10])

       #print iline," systlist= ", systlist[iline]

       #datalist = [xmin,xmax,cross,stat,syst[11:n]]
       #print newlist
       #print str(datalist)
       #outline=''.join(xmin)
       xmean=xmin+(xmax-xmin)/2.
       #dataline=' '+str(xmean)+' '+str(words[0])+' '+str(words[1])+' '+str(words[2])+' '+str(words[9])
       dataline=str(xmean)+' '+str(words[0])+' '+str(words[1])+' '+str(words[2])
#       datalinefull+=dataline+' '+str(words[9])+' '+str(words[10])
       datalinefull+=dataline+' '+str(words[9])

       sigmatotplus=float(words[10])*float(words[10])
       sigmatotminus=float(words[10])*float(words[10])
       #print " words[10]= ",words[10]
       for i in range(11,n-1):
         if i%2 == 1:
           #print " i= ",i," words[",i,"]= ",words[i]," words[",i+1,"]= ",words[i+1]
           sigmatotplus+=float(words[i])*float(words[i])
           sigmatotminus+=float(words[i+1])*float(words[i+1])
           sigmatot=math.sqrt(sigmatotplus)
           #print i," test sigmatot= ",sigmatot

       sigmatotplus=math.sqrt(sigmatotplus)
       #print " sigmatotplus= ",sigmatotplus

       sigmatotminus=math.sqrt(sigmatotminus)
       #print " sigmatotminus= ",sigmatotminus

       sigmatot=(sigmatotplus+sigmatotminus)/2.
       #print " sigmatot= ",sigmatot

#       datalinefull=datalinefull
       #print "datalinefull= ",datalinefull

       #sigmastat =stat/100.*cross
       #sigmatotplus =sigmatotplus/100.*cross
       #sigmatotminus=sigmatotminus/100.*cross

       #strsigmatot=str(sigmatot)
       strsigmatot='{:3.2f}'.format(sigmatot)
       strsigmatotplus='{:3.2f}'.format(sigmatotplus)
       strsigmatotminus='{:3.2f}'.format(sigmatotminus)

       #strsigmastat='{:e}'.format(sigmastat)
       #strsigmatotplus='{:e}'.format(sigmatotplus)
       #strsigmatotminus='{:e}'.format(sigmatotminus)
       #print  ' sigma formated ',strsigmatot 
       datalinefull+=' '+strsigmatotplus+' '+strsigmatotminus+'\n'

       #datalinesimple=dataline+' '+strsigmastat+' '+strsigmastat+' '+strsigmatotplus+' '+strsigmatotminus
       #datalinesimple=datalinesimple+'\n'
       #print "datalinesimple= ",datalinesimple
       #filedatasimple.write(datalinesimple)

       hadcorrup=float(words[3])*(1.+float(words[4])/100.)
       hadcorrdn=float(words[3])*(1.+float(words[5])/100.)

       strhadcorrup='{:4.3f}'.format(hadcorrup)
       strhadcorrdn='{:4.3f}'.format(hadcorrdn)

       hadcorrline=' '+str(xmean)+' '+str(xmin)+' '+str(xmax)+' '+str(words[3])+' '+strhadcorrup+' '+strhadcorrdn
       #print " hadcorrline= ",hadcorrline
       hadcorrline=hadcorrline+'\n'
       filehadcorr.write(hadcorrline)

       ewcorrup=float(words[3])*(1.+float(words[4])/100.)
       ewcorrdn=float(words[3])*(1.+float(words[5])/100.)

       strewcorrup='{:4.3f}'.format(ewcorrup)
       strewcorrdn='{:4.3f}'.format(ewcorrdn)

       ewcorrline=' '+str(xmean)+' '+str(xmin)+' '+str(xmax)+' '+str(words[6])+' '+str(words[7])+' '+str(words[8])
       #print " ewcorrline= ",ewcorrline
       ewcorrline=ewcorrline+'\n'
       fileewcorr.write(ewcorrline)

   print " iline= ",iline
   for i in range(1,iline+1): 
      print "systlist[",i,"]=",systlist[i]

   print ' number of names= ', len(names), " len systlist= ",len(systlist)

   print ' syststatmc= ',syststatmc

   systline='\n'
   systline+='syst_statmc+ '
   for ibin in range(0,len(syststatmc)): 
      systline+=' '+syststatmc[ibin]
   systline+='\n'
   systline+='syst_statmc- '
   for ibin in range(0,len(syststatmc)): 
      systline+=' '+syststatmc[ibin]
   
   #print ' systline= ',systline

   for isyst in range(0,nsyst):
      mynamep=str(names[isyst]).replace("+sys","syst_sys")
      mynamem=str(names[isyst]).replace("-sys","syst_sys")

      if "+" in str(names[isyst]): 
         systline+="\n"+mynamep+'+'
      if "-" in str(names[isyst]): 
         systline+="\n"+mynamem+'-'

      for ibin in range(1,iline+1): 
         systline+=' '+systlist[ibin][isyst]
         #print "systline[",i,"]=",systline
         #print "systline[",isyst,"]= ",systline

   #print "all systline= ",systline

   filedatafull.write('; systematics')
   filedatafull.write(systline)
   #      for i in range(11,n):
   #         datalinefull=datalinefull+' '+str(words[i])

   filedatafull.write("\n;mean xmin xmax cross staterr   +syst    -syst \n")
   filedatafull.write(datalinefull)

print "end"
