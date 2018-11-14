#!/bin/bash
mkdir results
for l in "los_A" "los_B" "los_C"
do
    echo "coping $l"
    for m in "mix_0" "mix_1" "mix_2"
    do
       echo "    coping $m"
       for d in "MSE" "PE" "PS" "OS" "OE"
       do 
           cp -a $l/$m/$d/.  results/ 
       done
    done
done
echo "Initiating unziping process ..."
cd results
gunzip *gz
echo "Results ready and unziped"

echo "Calling Python scritp for Graphs Generation"
python3 ../Runner_Analysis.py
echo "Finished Graphs" 

echo "Coping to Folder Analysis"
cp -r Analysis/ .. 

echo "Coping corruptedFiles.txt"
cp -r corruptedFiles.txt ..
