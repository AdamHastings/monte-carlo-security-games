#!/bin/bash

ATTACKERS=0.5
PAYOFF=0.8
MANDATE=${1}
EFFICIENCY=0.5
INEQUALITY=0.5
EFFORT=0.3

cd ../logs
echo 'Running filters to sweep PAYOFF...'
head -n1 sanitycheck.csv > filtered_PAYOFF_$MANDATE.csv
cat sanitycheck.csv | awk -v ATTACKERS=$ATTACKERS -F, '$1 == ATTACKERS' | awk -v MANDATE=$MANDATE -F, '$7 == MANDATE' | awk -v EFFICIENCY=$EFFICIENCY -F, '$4 == EFFICIENCY' | awk -v EFFORT=$EFFORT -F, '$5 == EFFORT' | awk -v INEQUALITY=$INEQUALITY -F, '$3 == INEQUALITY' >> filtered_PAYOFF_$MANDATE.csv
echo 'Storing to CSV in data folder...'
echo 'Done with PAYOFF'
cd -

cd ../logs
echo 'Running filters to sweep ATTACKERS...'
head -n1 sanitycheck.csv > filtered_ATTACKERS_$MANDATE.csv
cat sanitycheck.csv | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF' | awk -v MANDATE=$MANDATE -F, '$7 == MANDATE' | awk -v EFFICIENCY=$EFFICIENCY -F, '$4 == EFFICIENCY' | awk -v EFFORT=$EFFORT -F, '$5 == EFFORT' | awk -v INEQUALITY=$INEQUALITY -F, '$3 == INEQUALITY' >> filtered_ATTACKERS_$MANDATE.csv
echo 'Storing to CSV in data folder...'
echo 'Done with ATTACKERS'
cd -

cd ../logs
echo 'Running filters to sweep MANDATE...'
head -n1 sanitycheck.csv > filtered_MANDATE_$MANDATE.csv
cat sanitycheck.csv | awk -v ATTACKERS=$ATTACKERS -F, '$1 == ATTACKERS' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF' | awk -v EFFICIENCY=$EFFICIENCY -F, '$4 == EFFICIENCY' | awk -v EFFORT=$EFFORT -F, '$5 == EFFORT' | awk -v INEQUALITY=$INEQUALITY -F, '$3 == INEQUALITY' >> filtered_MANDATE_$MANDATE.csv
echo 'Storing to CSV in data folder...'
echo 'Done with MANDATE'
cd -

cd ../logs
echo 'Running filters to sweep EFFICIENCY...'
head -n1 sanitycheck.csv > filtered_EFFICIENCY_$MANDATE.csv
cat sanitycheck.csv | awk -v ATTACKERS=$ATTACKERS -F, '$1 == ATTACKERS' | awk -v MANDATE=$MANDATE -F, '$7 == MANDATE' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF'  | awk -v EFFORT=$EFFORT -F, '$5 == EFFORT' | awk -v INEQUALITY=$INEQUALITY -F, '$3 == INEQUALITY' >> filtered_EFFICIENCY_$MANDATE.csv
echo 'Storing to CSV in data folder...'
echo 'Done with EFFICIENCY'
cd -

cd ../logs
echo 'Running filters to sweep INEQUALITY...'
head -n1 sanitycheck.csv > filtered_INEQUALITY_$MANDATE.csv
cat sanitycheck.csv | awk -v ATTACKERS=$ATTACKERS -F, '$1 == ATTACKERS' | awk -v MANDATE=$MANDATE -F, '$7 == MANDATE' | awk -v EFFICIENCY=$EFFICIENCY -F, '$4 == EFFICIENCY' | awk -v EFFORT=$EFFORT -F, '$5 == EFFORT' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF'  >> filtered_INEQUALITY_$MANDATE.csv
echo 'Storing to CSV in data folder...'
echo 'Done with INEQUALITY'
cd -

cd ../logs
echo 'Running filters to sweep EFFORT...'
head -n1 sanitycheck.csv > filtered_EFFORT_$MANDATE.csv
cat sanitycheck.csv | awk -v ATTACKERS=$ATTACKERS -F, '$1 == ATTACKERS' | awk -v MANDATE=$MANDATE -F, '$7 == MANDATE' | awk -v EFFICIENCY=$EFFICIENCY -F, '$4 == EFFICIENCY' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF'  | awk -v INEQUALITY=$INEQUALITY -F, '$3 == INEQUALITY' >> filtered_EFFORT_$MANDATE.csv
echo 'Storing to CSV in data folder...'
echo 'Done with EFFORT'
cd -
