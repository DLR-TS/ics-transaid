#!/bin/bash
cd results

echo "Calling Python scritp for Graphs Generation"
python3 ../Runner_Analysis.py
echo "Finished Graphs" 

echo "Coping to Folder Analysis"
cp -r Analysis/ .. 

echo "Coping corruptedFiles.txt"
cp -r corruptedFiles.txt ..

