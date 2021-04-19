#!/usr/bin/python
# Author: Leonhard Luecken
# Date: May, 24, 2019
#
# This script adds a line 
#   <param key="device.toc.file" value="{tocFile}"/>
# for each vType in the vType distributions in the xml 
# files of the containing directory and saves the resulting 
# templates into the directory

import os, subprocess
import xml.etree.ElementTree as ET                                                                                                                                                                   

#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("changeVehValue start")

    wdTemp = os.path.dirname(os.path.realpath(__file__))

    wd = wdTemp.replace("/tools", '/config/sumo/vTypes')

    #os.chdir(wd)

    xmlFiles = ["vTypesCVToC_MSE.add.tpl.xml", "vTypesCAVToC_MSE.add.tpl.xml"]

    print("\nFound files:%s"%str(xmlFiles))

    for fn in xmlFiles:
        fileChanged = False
        pathXml = os.path.join(wd, fn)

        tree = ET.parse(pathXml)
        root = tree.getroot()
        for vtd in root.findall("vTypeDistribution"):
            for vt in vtd.findall("vType"):
                if vt: #len(vt.getchildren()) > 0:

                    tocFileElem = ET.Element("param")
                    tocFileElem.attrib["key"] = "device.toc.dynamicToCThreshold"
                    tocFileElem.attrib["value"] = "0.000"
                    tocFileElem.set(tocFileElem.attrib["key"], tocFileElem.attrib["value"])

                    #tocFileElem.set('device.toc.dynamicToCThreshold', '0.000')
                    
                    
                    fileChanged = True
                else:
                    continue
        if fileChanged:
            # backup old file, write template
            #print ("File '%s': Creating template ..."%fn)
            #outfn = fn[:-8] + ".add.tpl.xml"
            #print(outfn)
            #with open(outfn, "w") as f:
            #    f.write(str(ET.tostring(root)))
            #subprocess.call(["mv", fn, archive_dir])
            
            #mydata = ET.tostring(root, encoding="unicode")
            #myfile = open(os.path.join(wd, "items.xml"), "w")
            #myfile.write(mydata)

            tree.write(pathXml, encoding = "unicode")
            print ("File :", fn, "changed")
        else:
            print ("File '%s': Skipping ... (no toc devices)"%fn)

    print("changeVehValue finished")