#zsh
# 
export dir=/afs/cern.ch/user/a/applgrid/work/public

export file=Grids/jet/atlas/incljets2010/nlojet/atlas-incljets-arxiv-1112.6297/r04
echo $dir/$file/ $file/.
mkdir -p $file
cp $dir/$file/*.root $file/.

echo done
echo

export file=Grids/jet/atlas/incljets2010/nlojet/atlas-incljets-arxiv-1112.6297/r06
echo $dir/$file/ $file/.
mkdir -p $file
cp -r $dir/$file/*.root $file/.

echo done
echo

export file=Grids/jet/atlas/incljets2010/nlojet/pereventscale
echo $dir/$file/ $file/.
mkdir -p $file
cp $dir/$file/*.root $file/.

echo done
echo

export file=Grids/jet/atlas/incljets2011/nlojet/
echo $dir/$file/*.root $file/.
cp $dir/$file/*.root $file/.

echo done
echo

export file=Grids/jet/atlas/incljets2011_2p76TeV/nlojet
echo $dir/$file/*.root $file/
cp $dir/$file/*.root $file/.

echo done
echo


export file=Grids/top/atlas/mcfm/ttbarresolvedpartons
echo $dir/$file/*.root $file/
cp $dir/$file/*.root $file/.

echo done
echo

export file=Grids/Zincl/atlas/sherpa/
echo $dir/$file/*.root $file/
cp $dir/$file/*.root $file/.

echo done
echo

export file=Grids/jet/atlas/dijets2011
echo $dir/$file/*.root $file/
cp $dir/$file/*.root $file/.

echo done
echo