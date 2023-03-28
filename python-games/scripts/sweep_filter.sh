#!/bin/bash

PERCENT_EVIL=0.5
PAYOFF=0.8
SEC_INVESTMENT=${1}
SEC_INVESTMENT_CONVERSION_RATE=0.5
WEALTH_GAP=0.5
ATTACK_COST_CONVERSION_RATE=0.3

cd ../data
echo 'Running filters to sweep PAYOFF...'
head -n1 df.csv > filtered_PAYOFF_$SEC_INVESTMENT.csv
cat df.csv | awk -v PERCENT_EVIL=$PERCENT_EVIL -F, '$1 == PERCENT_EVIL' | awk -v SEC_INVESTMENT=$SEC_INVESTMENT -F, '$7 == SEC_INVESTMENT' | awk -v SICR=$SEC_INVESTMENT_CONVERSION_RATE -F, '$4 == SICR' | awk -v ACCR=$ATTACK_COST_CONVERSION_RATE -F, '$5 == ACCR' | awk -v WEALTH_GAP=$WEALTH_GAP -F, '$3 == WEALTH_GAP' >> filtered_PAYOFF_$SEC_INVESTMENT.csv
echo 'Storing to CSV in data folder...'
echo 'Done with PAYOFF'
cd -

cd ../data
echo 'Running filters to sweep PERCENT_EVIL...'
head -n1 df.csv > filtered_PERCENT_EVIL_$SEC_INVESTMENT.csv
cat df.csv | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF' | awk -v SEC_INVESTMENT=$SEC_INVESTMENT -F, '$7 == SEC_INVESTMENT' | awk -v SICR=$SEC_INVESTMENT_CONVERSION_RATE -F, '$4 == SICR' | awk -v ACCR=$ATTACK_COST_CONVERSION_RATE -F, '$5 == ACCR' | awk -v WEALTH_GAP=$WEALTH_GAP -F, '$3 == WEALTH_GAP' >> filtered_PERCENT_EVIL_$SEC_INVESTMENT.csv
echo 'Storing to CSV in data folder...'
echo 'Done with PERCENT_EVIL'
cd -

cd ../data
echo 'Running filters to sweep SEC_INVESTMENT...'
head -n1 df.csv > filtered_SEC_INVESTMENT_$SEC_INVESTMENT.csv
cat df.csv | awk -v PERCENT_EVIL=$PERCENT_EVIL -F, '$1 == PERCENT_EVIL' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF' | awk -v SICR=$SEC_INVESTMENT_CONVERSION_RATE -F, '$4 == SICR' | awk -v ACCR=$ATTACK_COST_CONVERSION_RATE -F, '$5 == ACCR' | awk -v WEALTH_GAP=$WEALTH_GAP -F, '$3 == WEALTH_GAP' >> filtered_SEC_INVESTMENT_$SEC_INVESTMENT.csv
echo 'Storing to CSV in data folder...'
echo 'Done with SEC_INVESTMENT'
cd -

cd ../data
echo 'Running filters to sweep SICR...'
head -n1 df.csv > filtered_SICR_$SEC_INVESTMENT.csv
cat df.csv | awk -v PERCENT_EVIL=$PERCENT_EVIL -F, '$1 == PERCENT_EVIL' | awk -v SEC_INVESTMENT=$SEC_INVESTMENT -F, '$7 == SEC_INVESTMENT' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF'  | awk -v ACCR=$ATTACK_COST_CONVERSION_RATE -F, '$5 == ACCR' | awk -v WEALTH_GAP=$WEALTH_GAP -F, '$3 == WEALTH_GAP' >> filtered_SICR_$SEC_INVESTMENT.csv
echo 'Storing to CSV in data folder...'
echo 'Done with SICR'
cd -

cd ../data
echo 'Running filters to sweep WEALTH_GAP...'
head -n1 df.csv > filtered_WEALTH_GAP_$SEC_INVESTMENT.csv
cat df.csv | awk -v PERCENT_EVIL=$PERCENT_EVIL -F, '$1 == PERCENT_EVIL' | awk -v SEC_INVESTMENT=$SEC_INVESTMENT -F, '$7 == SEC_INVESTMENT' | awk -v SICR=$SEC_INVESTMENT_CONVERSION_RATE -F, '$4 == SICR' | awk -v ACCR=$ATTACK_COST_CONVERSION_RATE -F, '$5 == ACCR' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF'  >> filtered_WEALTH_GAP_$SEC_INVESTMENT.csv
echo 'Storing to CSV in data folder...'
echo 'Done with WEALTH_GAP'
cd -

cd ../data
echo 'Running filters to sweep ACCR...'
head -n1 df.csv > filtered_ACCR_$SEC_INVESTMENT.csv
cat df.csv | awk -v PERCENT_EVIL=$PERCENT_EVIL -F, '$1 == PERCENT_EVIL' | awk -v SEC_INVESTMENT=$SEC_INVESTMENT -F, '$7 == SEC_INVESTMENT' | awk -v SICR=$SEC_INVESTMENT_CONVERSION_RATE -F, '$4 == SICR' | awk -v PAYOFF=$PAYOFF -F, '$2 == PAYOFF'  | awk -v WEALTH_GAP=$WEALTH_GAP -F, '$3 == WEALTH_GAP' >> filtered_ACCR_$SEC_INVESTMENT.csv
echo 'Storing to CSV in data folder...'
echo 'Done with ACCR'
cd -
