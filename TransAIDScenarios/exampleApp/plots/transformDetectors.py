#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# Date: 2020/01/01
# Author: Dimitris Koutras
# Author: Vasilios Karagounis

import os, sys, errno

import xml.etree.ElementTree as ET
import numpy as np

import pickle
import pandas as pd
import time
import json

from sys import platform as _platform
import multiprocessing as mp
import threading as mt
import queue

seperator = "---------------------------------------------------------------------"

verbose = False
use_parallel = False
use_threads = False

enables = []
scenarios = []
scenariosFix = []
driverBehaviourList = [] 

trafficDemand = {}
seedEnables = {}

settings = {}
work_dir = {}
configDir = ""

#Detectors time interval (default for all UC is 300[sec])
time_step = 0
time_size = 0
pos_size = 0

seedSize = 0

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

input_data_file = "network"
output_data_file = "detectors_aggregated"

is_linux = 1
is_mac = 2
is_win = 3

OS_TYPE = 0
#---------------------------------------------------------------------------------------------------------------------------------------------
def find_platform():
    
    global OS_TYPE, use_parallel, use_threads

    if _platform == "linux" or _platform == "linux2":
        OS_TYPE = is_linux
    elif _platform == "darwin":
        OS_TYPE = is_mac
    elif _platform == "win32":
        OS_TYPE = is_win
    elif _platform == "win64":
        OS_TYPE = is_win
    else:
        print("Error : Unknown operating system")
        sys.exit(1)

    if use_parallel and OS_TYPE == is_win:
        use_parallel = False
        use_threads = True

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
#---------------------------------------------------------------------------------------------------------------------------------------------
def loadSettings():

    global workDirMain, configDir, enables, scenariosFix, work_dir, settings, scenarios, seedSize, startLOS
    global trafficDemand, driverBehaviourList, time_step, time_size

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    workDirMain =  os.path.join(parent, "results", "plots")
    configDir =  os.path.join(parent, "config")
    settingsDir = os.path.join(current, "settings", "plots.json")
    
    loadFileSettings(settingsDir)

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

    for s in scenarios:
        tmp = os.path.join(workDirMain, s)
        work_dir[s] = tmp
        mkdir_p(tmp)
        if not os.path.exists(tmp):
            print("Error :", tmp)
            sys.exit(1)

    trafficDemand = settings["level_Of_service"]
    driverBehaviourList = settings["schemes"]
    
    if settings["fast_debug"] == "True":
        seedSize = 1
    else: 
        seedSize = settings["seed_size"]

    startLOS = settings["transform_start_los"]

    time_step = settings["transform_detectors_time_step"]
    time_size = int(3600 / time_step)

#---------------------------------------------------------------------------------------------------------------------------------------------
def clearFiles():

    if settings["clean_old_data"] == "True":
        try:
            for s in scenarios:
                count = 0
                for file in os.listdir(work_dir[s]):
                    if file.startswith(output_data_file) and file.endswith(".pickle"):
                        path = os.path.join(work_dir[s], file)
                        os.remove(path)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s"%(count, s))
        except:
            pass

#---------------------------------------------------------------------------------------------------------------------------------------------
def printDisabledFunctions():

     for s in scenarios:
        func_enable = settings["plot_detectors_list_" +  str(scenariosFix.index(s))]
        for enStr in func_enable:
            if (func_enable[enStr] == 0):
                print("Warning :", enStr, "is disabled on", s)

#---------------------------------------------------------------------------------------------------------------------------------------------
def createFileNamePickle(m_path, m_name, m_demand, m_mix_id, m_behaviour):
    
    xml_name = [ m_name,
                traffic_Demand_Name, str(trafficDemand[m_demand]),
                trafic_Mix_Name, str(m_mix_id),
                driver_Behaviour_Name, m_behaviour]

    t_path = os.path.join(m_path, '_'.join(xml_name)) + '.pickle'

    return t_path

#---------------------------------------------------------------------------------------------------------------------------------------------
def clusteredDetectors():

    global pos_size

    cluster = []

    detectors_topology_ref = ""
    
    for root, subdirs, files in os.walk(configDir):
        for filename in files:
            if 'detectors' in filename and filename.endswith(".xml"):
                path = os.path.join(root, filename)
                detectors_topology_ref = path
                if verbose:
                    print(root, subdirs, filename, detectors_topology_ref)
                break
    
    if verbose:
        print("detectors_topology_ref:", detectors_topology_ref)

    if os.path.exists(detectors_topology_ref):
        try:
            tree = ET.parse(detectors_topology_ref)
            root = tree.getroot()
        except:
            print('Error: file : ' + detectors_topology_ref + ' is corrupted')
        finally:
            inductionLoop = root.findall('inductionLoop')

            for edge in settings["edges"]:          # Maximum length and step length for scanning edges, when generating detectors clusters
                for step in range( 0, settings["transform_detectors_max_length"], settings["transform_detectors_step_length"]):
                    side2side = []
                    for loop in inductionLoop:
                        if (loop.get('pos') == str(step)) and (edge in loop.get('lane')):
                            side2side.append(loop.get('id'))

                    if side2side != []:
                        cluster.append(side2side)
    else:
        print('Error: Could not find detectors topology reference file:', detectors_topology_ref)

    pos_size = len(cluster)

    if verbose:
        print("cluster data:", cluster)
        print('cluster pos size ', pos_size)

    return cluster

#---------------------------------------------------------------------------------------------------------------------------------------------
def meanTimeSpace(tempDF, funcName, seed, cluster, time_size, pos_size):

    rtn = np.zeros(shape = (time_size, pos_size))

    for t_step in range(time_size):

        for p_step, detectorGroup in enumerate(cluster):

            clusterSpeeds = []
            clusterNvehContrib = []

            for detector in detectorGroup:

                # pdb.set_trace()

                #timeBegin -> time begin
                helperDF = tempDF.loc[ tempDF['Detectors_begin'] == t_step * time_step ]
                #detector  -> id
                helperDF = helperDF.loc[ helperDF['Detectors_id'] == detector ]
                # specific seed
                helperDF = helperDF.loc[ helperDF['Detectors_seed'] == seed ]

                #print(helperDF["Detectors_speed"])
                # edge.append(helperDF.describe().at['mean', funcName])

                if not helperDF[funcName].empty:
                    try:
                        clusterSpeeds.append(float(helperDF[funcName]))
                        clusterNvehContrib.append( float(helperDF['Detectors_nVehContrib']) )
                    except:
                        print("problem:", funcName, seed, cluster)
                        print("data:", helperDF[funcName])
                        pass

            if sum(clusterNvehContrib) == 0:
                continue

            avg = np.average(clusterSpeeds, weights = clusterNvehContrib)
            # pdb.set_trace()

            rtn[t_step][p_step] = avg

    return rtn
#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWork(path, scenario, sid, use_parallel, q):

    #func_enable = settings["plot_detectors_list_" +  str(sid)]
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]

    funcs = ["Detectors_speed", "Detectors_flow"] # FIX do not touch

    # results will contain the mean detectors value for a specific time and position  todo shape from values
    result = np.zeros(shape = (len(funcs), 3, 3, 1, 10, time_size, pos_size))

    for funcId, funcName in enumerate(funcs):
        for td_id, td_name in enumerate(trafficDemand):
                for tm_id, tm_name in enumerate(trafficMix):
                    for db_id, db_name in enumerate(driverBehaviourList):

                        if verbose:
                            print("Scenario:", scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)

                        pickleFileStr = createFileNamePickle(path, input_data_file, td_name, tm_id, db_name)

                        if not os.path.exists(pickleFileStr):
                            print('Error: Could not find demand file:', pickleFileStr)
                            continue

                        pickle_in = open(pickleFileStr, 'rb')
                        tempDF = pickle.load(pickle_in)
                        pickle_in.close()

                        for seed in range(seedSize):
                            if seedEnables[td_name][tm_name][seed] == 0:
                                if (use_parallel):
                                    q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                                else:
                                    print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                                continue

                            result[funcId][td_id][tm_id][db_id][seed] = meanTimeSpace(tempDF, funcName,
                                                                  seed,
                                                                  cluster,
                                                                  time_size,
                                                                  pos_size)

    dictOutPath = os.path.join(path, '%s_%s'%(output_data_file,scenario)) + '.pickle'

    pickle_out = open(dictOutPath, 'wb')
    pickle.dump(result, pickle_out)
    pickle_out.close()

    if use_parallel:
        q.put('Saving file:' + dictOutPath)
    else:
        print('Saving file:', dictOutPath)

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data detectors trasformation proccess started")
    startTime = time.time()

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

    find_platform()

    q = []
    if use_parallel:
        print("Using parallel processing. Number of processors:", mp.cpu_count())
        q = mp.Queue()
    
    if use_threads:
        print("Using thread processing")
        q = queue.Queue()

    cluster = clusteredDetectors()
    
    if use_parallel or use_threads:
        jobs = []
        
        print("\n%s"%seperator)
        clearFiles()

        print(seperator)
        printDisabledFunctions()

        print(seperator)
        print("Please wait...")

        for s in scenarios:
            if use_parallel:
                temp = mp.Process(target = multiWork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q,))
            if use_threads:
                temp = mt.Thread(target = multiWork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q,))

            jobs.append(temp)

        for th in jobs:
            th.start()

        for th in jobs:
            th.join()

        jobs.clear()
    else:
        print("\n%s"%seperator)
        clearFiles()

        print(seperator)
        printDisabledFunctions()

        for s in scenarios:
            print("\n%s"%seperator)
            print("Current scenario:", s, "\n")

            multiWork(work_dir[s], s, scenariosFix.index(s), False, q)
            
            print("\nScenario", s, "finished")
            print(seperator)

    if use_parallel or use_threads:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time()- startTime
    print("\nData detectors transformation process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
