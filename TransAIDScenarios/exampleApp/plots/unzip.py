#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# Date: 2020/01/01
# Author: Unkown
# Author: Vasilios Karagounis

import os, sys, errno
import gzip
import shutil
import time
import json
import multiprocessing as mp

seperator = "---------------------------------------------------------------------"

verbose = False
use_parallel = False

settings = {}
work_dir = {}
copy_dir = ""

enables = []
scenarios = []

#---------------------------------------------------------------------------------------------------------------------------------------------
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

#---------------------------------------------------------------------------------------------------------------------------------------------
def unzip(fromPath, topath, counter, scenario):
    
    with gzip.open(fromPath, 'rb') as f_in:
        with open(topath, 'wb') as f_out:
            shutil.copyfileobj(f_in, f_out)
            if verbose:
                print("\runzipping file : ", fromPath, "                                                                               ",end = '', flush = True)
            else:
                print("\runzipping file : ", counter + 1, " ", scenario, "                      " , end = '', flush = True)

#---------------------------------------------------------------------------------------------------------------------------------------------
def unzipThread(pathList, q):

    counter = 0
    for idStr in pathList:
        with gzip.open(idStr[0], 'rb') as f_in:
            with open(idStr[1], 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
                counter +=1
    q.put(str(counter))

#---------------------------------------------------------------------------------------------------------------------------------------------
def loadFileSettings(path):
    
    global settings, verbose

    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                settings = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            if verbose:
                print("Json settings path :", path)        
    else:
         print("Json settings path not found :", path)   

#---------------------------------------------------------------------------------------------------------------------------------------------
# load settings from json
def loadSettings():
    #https://www.btelligent.com/en/blog/best-practice-working-with-paths-in-python-part-1/
    global enables, copy_dir, work_dir, settings, scenarios

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    copy_dir =  os.path.join(parent, "results")
    pasteDirTmp =  os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings", "plots.json")
  
    loadFileSettings(settingsDir)
    
    clearFiles(pasteDirTmp)

    time.sleep(1)
    
    mkdir_p(os.path.join(pasteDirTmp))

    if not enables:
        enables = list(settings["enables"])

    scenariosFix = list(settings["scenarios"])

    for id, enbl in enumerate(enables):
        if enbl == 1:
            scenarios.append(scenariosFix[id])

    if not scenarios:
        print("Error: no scenarios found")
        sys.exit(1)

    print("Scenarios used:", scenarios)

    work_dir = {}
    for s in scenarios:
        tmp = os.path.join(pasteDirTmp, s)
        work_dir[s] = tmp
        mkdir_p(tmp)
        if not os.path.exists(tmp):
            print("Error :", tmp)
            sys.exit(1)

#---------------------------------------------------------------------------------------------------------------------------------------------
def clearFiles(path):

    if settings["clean_old_data"] == "True":
        try:
            if os.path.exists(path):
                shutil.rmtree(os.path.join(path))
                print("Clearing folder: %s"%path)
        except:
            pass

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data unzipping proccess started")
    start_time = time.time()
    
    if "--verbose" in sys.argv:
        verbose = True
    
    for arg in sys.argv:
        if "--en=" in arg:
            temp = arg.replace("--en=", "")
            enables = [int(n) for n in temp.split(",")]

    loadSettings()

    use_parallel = settings["use_parallel"] == "True"    

    if "--parallel" in sys.argv:
        use_parallel = True
        
    if use_parallel:
        print("Using parallel processing. Number of processors:", mp.cpu_count())

    paths = {}
    countZip = 0

    pathsDict = []
    for s in scenarios:
        pathsDict.append([])

    for root, subdirs, files in os.walk(copy_dir):
        if "config" not in root:
            for filename in files:
                if ('.gz' in filename) and ('stdout' not in filename):
                    for sid, s in enumerate(scenarios):
                        if s in root:
                            temp_path = os.path.join(root, filename)
                            if not os.path.exists(temp_path):
                                print("Error : path does not exist :", temp_path)
                                continue
                            if use_parallel:
                                pathsDict[sid].append([ temp_path, os.path.join(work_dir[s], filename.replace('.gz', '')) ])
                            else:
                                unzip(temp_path, os.path.join(work_dir[s], filename.replace('.gz', '')), countZip, s)
                                countZip+=1

    if not use_parallel:
        print("\r                                                                                                                                                   ", end = '', flush = True)

    if use_parallel:

        print("Please wait...")

        jobs = []

        q = mp.Queue() 

        for sid, s in enumerate(scenarios):
            temp = mp.Process(target = unzipThread, args=(pathsDict[sid], q,  ))
            jobs.append(temp)
        
        for th in jobs:
            th.start()

        for th in jobs:
            th.join()

        while not q.empty():
            countZip += int(q.get())

        jobs.clear()

    end_time = time.time()
    elapsed = end_time - start_time

    print("\rUnzipped", countZip, "files.")

    print("Data unzipping proccess completed after", elapsed, "[sec]")
    print(seperator,"\n")
