#!/bin/bash
# FP7 COLOMBO traffic-monitor data processing script

echo "This is the COLOMBO traffic-monitor data post-processing script..."
echo "."
echo "."  
echo "."
python3 comm-protocol-parser.py app-data.txt
echo "."
echo "."
echo "."
echo "...done. Please open the provided .cvs file to get traffic monitoring data. Enjoy !"

