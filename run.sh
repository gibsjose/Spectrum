#zsh
# 
file=$1

rm test.txt
echo run python/$file
python python/$file
#
./Spectrum test.txt
