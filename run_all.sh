#zsh
# 
FILES=("atlas_incljet2012_syst_classes.py")

for ifile in ${FILES[@]}; do
 echo run $ifile
 source run.sh $ifile
done
#

