#!/bin/bash
#
# test all macros
#
#./Spectrum Steering/good_test_v2.txt
#
# ATLAS inclusive jet cross section
#
# ATLAS inclusive jets 2011 7 TeV
echo "Run: "
./Spectrum Steering/atlas_incljet2011.txt
read -p " finished atlas_incljet2011 hit return to continue "
#
./Spectrum Steering/atlas_incljet2011_all.txt
read -p " finished atlas_incljet2011_all hit return to continue "
#
#
# ATLAS inclusive jets 2011 2.76 TeV
./Spectrum Steering/atlas_incljet2011_2p76TeV.txt
read -p " finished atlas_incljet2011_ 2.76 TeV hit return to continue "
#
# ATLAS inclusive jets 2010 7 TeV
./Spectrum Steering/atlas_incljet2010.txt
read -p " finished atlas_incljet2010 hit return to continue "
#
# ATLAS inclusive jets 2010 7 TeV
./Spectrum Steering/atlas_incljet2010_all.txt
read -p " finished atlas_incljet2010_all hit return to continue "
#
# ATLAS inclusive Z boson 2010 7 TeV
./Spectrum Steering/atlas_inclZ2010.txt
read -p " finished atlas_inclZ2010 hit return to continue "
#
#./Spectrum Steering/atlas_topnjets.txt
#
# Top parton
#
./Spectrum Steering/atlas_topparton_mtt.txt
read -p " finished atlas_topparton_mtt hit return to continue "
#
./Spectrum Steering/atlas_topparton_pttop.txt
read -p " finished atlas_topparton_pttop hit return to continue "
#
./Spectrum Steering/atlas_topparton_ttbarpt.txt
read -p " finished atlas_topparton_ttbarpt hit return to continue "
#
./Spectrum Steering/atlas_topparton_ttbary.txt
read -p " finished atlas_topparton_ttbary hit return to continue "
#
#
# band test
#
#./Spectrum Steering/test_band.txt
#
./Spectrum Steering/detailedtheoryband_atlas_incljet2011_2p76TeV.txt
read -p " finished detailedtheoryband_atlas_incljet2011_2p76TeV  hit return to continue "
#
#
./Spectrum Steering/detailedtheoryband_atlas_incljet2011.txt
read -p " finished detailedtheoryband_atlas_incljet2011  hit return to continue "
#
./Spectrum Steering/hadronisation_uncertainty_detailedtheoryband_atlas_incljet2011_2p76TeV.txt
read -p " finished hadronisation_uncertainty_detailedtheoryband_atlas_incljet2011_2p76TeV  hit return to continue "
#
./Spectrum Steering/atlas_top_dilepton_comparisonmcfmsherpa.txt
read -p " finished atlas_top_dilepton_comparisonmcfmsherpa.txt  hit return to continue "
#
