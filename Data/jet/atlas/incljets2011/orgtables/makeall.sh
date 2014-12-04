#!/bin/bash
#
echo start maketable
export dirnamedata=Data/jet/atlas/incljets2011/
export dirnamegrid=Grids/jet/nlojet/atlas/incljets2011/

python maketable.py sys_R0$2_Eta$1.txt atlas_2011_jet_antiktr0$2_incljetpt_eta$1 $dirnamedata
cp atlas_2011_jet_antiktr0$2_incljetpt_eta$1_data.txt /home/tcarli/Spectrum/$dirnamedata/.
cp atlas_2011_jet_antiktr0$2_incljetpt_eta$1_metadata.txt /home/tcarli/Spectrum/$dirnamedata/.
cp atlas_2011_jet_antiktr0$2_incljetpt_eta$1_hadcorr.txt /home/tcarli/Spectrum/$dirnamegrid/.
cp atlas_2011_jet_antiktr0$2_incljetpt_eta$1_ewcorr.txt /home/tcarli/Spectrum/$dirnamegrid/.
#python maketable.py sys_R04_Eta1.txt atlas_2011_jet_antiktr04_incljetpt_eta2