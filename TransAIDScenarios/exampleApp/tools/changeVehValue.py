#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    createVehTypeDistribution.py
# @author  Vasilios Karagounis
# @date    2016-06-09

import os
import sys
import csv
import re
import xml.dom.minidom
import random
import argparse

#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("changeVehValue start")

    wdTemp = os.path.dirname(os.path.realpath(__file__))

    wd = wdTemp.replace("tools" ,"config/sumo/vTypes")

    os.chdir(wd)

    xmlFiles = ["vTypesCVToC_MSE.add.tpl.xml", "vTypesCAVToC_MSE.add.tpl.xml"]

    print("Found files:%s"%str(xmlFiles))

    for fn in xmlFiles:
        fileChanged = False
        tree = ET.parse(fn)
        root = tree.getroot()
        for vtd in root.findall("vTypeDistribution"):
            for vt in vtd.findall("vType"):
                if len(vt.getchildren()) > 0:
                    # params present => make template
                    tocFileElem = ET.Element("param")
                    tocFileElem.attrib["key"] = "device.toc.dynamicToCThreshold"
                    tocFileElem.attrib["value"] = "0.000"
                    vt.set(tocFileElem[])
                    fileChanged = True
                else:
                    continue
        if fileChanged:
            # backup old file, write template
            #print ("File '%s': Creating template ..."%fn)
            #outfn = fn[:-8] + ".add.tpl.xml"
            #with open(outfn, "w") as f:
            #    f.write(ET.tostring(root))
            #subprocess.call(["mv", fn, archive_dir])
        else:
            print ("File '%s': Skipping ... (no toc devices)"%fn)

    print("changeVehValue finished")