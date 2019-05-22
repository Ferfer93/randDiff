#!/bin/bash

NJ=100000000    #number of jumps
NW=10000       #how often to write
DIV=1000     #lattice divider
T=0.6         #particle temp
Tg=1.0      #temp trap distribution
G0=1.0      #escape rate prefactor
S=0.001       #spin
PERS=1      #persistence
GL=1        #pregen lattice
CUT=0

MAXCORR=100000000
BINCORR=100000

SIMNUM=200

dirname=test

mkdir -p runs
mkdir -p runs/$dirname

rm runs/$dirname/*

cp bin/fast_diff runs/$dirname/
cp bin/uni_corr runs/$dirname/
cp create_avgs.sh runs/$dirname/

if [ $PERS -eq 0 ]
then
    echo -e "T = $T, Tg = $Tg, G0 = $G0, no persistence\nNumber of jumps = $NJ\nWriting every $NW jumps\nLattice generated has 2*$NJ/$DIV positions" >> runs/$dirname/parameters.log
else
    echo -e "T = $T, Tg = $Tg, G0 = $G0, S = $S\nNumber of jumps = $NJ\nWriting every $NW jumps\nLattice generated has 2*$NJ/$DIV positions" >> runs/$dirname/parameters.log
fi

cd runs/$dirname/

for ((i=1;i<=SIMNUM;i++)); do
    echo "Running simulation $i out of $SIMNUM"

    ./fast_diff $NJ $NW $S $T $Tg $G0 $PERS $GL $DIV $CUT

    ex_c=$?
    if [ $ex_c -eq 0 ]
    then
        ./uni_corr times.out $MAXCORR $BINCORR &&

        mv uni_corr.out corr_m$i.out
    else
        echo "SMALL LATTICE WARNING" >> SLWAR.out
    fi
done

./create_avgs.sh

gnuplot crt_plt.plt
