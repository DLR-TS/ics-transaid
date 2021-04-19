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
import pandas as pd
import pickle
import time
import json

import multiprocessing as mp
import threading as mt
from sys import platform as _platform
import queue

seperator = "---------------------------------------------------------------------"

# TIP all function calulate mean(average) values
verbose = False
use_parallel = False
use_threads = False

enables = []
settings = {}
work_dir = {}

scenarios = []
scenariosFix = []
driverBehaviourList = []

trafficDemand = {}
seedEnables = {}

seedSize = 0

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

workDirMain = ""

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

#----------------------------------------------------------
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
def loadSettings():

    global workDirMain, work_dir, enables, settings, scenarios, scenariosFix
    global trafficDemand, driverBehaviourList, seedSize

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    workDirMain =  os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings", "plots.json")

    loadFileSettings(settingsDir)

    scenariosFix = list(settings["scenarios"])

    if not enables:
        enables = list(settings["enables"])

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

    if settings["fast_debug"] == "True":
        seedSize = 1
    else:
        seedSize = settings["seed_size"]

    trafficDemand = settings["level_Of_service"]
    driverBehaviourList = settings["schemes"]

#---------------------------------------------------------------------------------------------------------------------------------------------
def clearFiles():
    
    if settings["clean_old_data"] == "True":
        try:
            for s in scenarios:
                count = 0
                for file in os.listdir(work_dir[s]):
                    if file.startswith("trajectories") and file.endswith(".pickle"):
                        path = os.path.join(work_dir[s], file)
                        os.remove(path)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s"%(count, s))
        except:
            pass

#---------------------------------------------------------------------------------------------------------------------------------------------
def createFileNameXml(m_path, m_name, m_demand, m_mix_id, m_behaviour, m_seed):
    
    name = [ m_name,
                traffic_Demand_Name, str(trafficDemand[m_demand]),
                trafic_Mix_Name, str(m_mix_id),
                driver_Behaviour_Name, m_behaviour,
                'seed', str(m_seed) ]

    t_path = os.path.join(m_path, '_'.join(name)) + '.xml'

    return t_path

#---------------------------------------------------------------------------------------------------------------------------------------------
def createFileNamePickle(m_path, m_name, m_demand, m_mix_id, m_behaviour, seed):
    
    name = [ m_name,
                traffic_Demand_Name, str(trafficDemand[m_demand]),
                trafic_Mix_Name, str(m_mix_id),
                driver_Behaviour_Name, m_behaviour,
                'seed', str(seed)]

    t_path = os.path.join(m_path, '_'.join(name)) + '.pickle'

    return t_path  

#---------------------------------------------------------------------------------------------------------------------------------------------
def loadFile(pickleFileStr):

    if os.path.isfile(pickleFileStr):
        file_in = open(pickleFileStr, 'rb')
        rtnDict = pickle.load(file_in)
        file_in.close()
    else:
        a = {}
        rtnDict = pd.DataFrame(a)

    return rtnDict

#---------------------------------------------------------------------------------------------------------------------------------------------
def dumpDictToFile(pickleFileStr, dict):
    
    pickle_out = open(pickleFileStr, "wb")
    pickle.dump(dict, pickle_out)
    pickle_out.close()

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiwork(path, scenario, sid, use_parallel, q):
  
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]

    for td_id, td_name in enumerate(trafficDemand):
        for tm_id, tm_name in enumerate(trafficMix):
            for db_id, db_name in enumerate(driverBehaviourList):

                if verbose:
                    print("Scenario:", scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)

                for seed in range(seedSize):
                    
                    if seedEnables[td_name][tm_name][seed] == 0:
                        if (use_parallel):
                            q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                        else:
                            print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                        continue
                
                    fnStr = createFileNameXml(path, 'trajectories', td_name, tm_id, db_name, seed)
                    
                    fnStrPickle = createFileNamePickle(path, 'trajectories', td_name, tm_id, db_name, seed)

                    tmpDict = {}

                    if os.path.exists(fnStr):
                        try:
                            tree = ET.parse(fnStr)
                            root = tree.getroot()
                        except:
                            if (use_parallel):
                                q.put('File : ' + fnStr + ' is corrupted\n')
                            else:
                                print('Error: file : ' + fnStr + ' is corrupted')
                        finally:
                            timeStep = root.findall('timestep')
                            # for every vehicle in the specific time step
                            if timeStep:
                                for step in timeStep:
                                    #if float(step.get('time')) > 3600.0: 
                                    #    break
                                    for vehicle in step:

                                        if vehicle.get('id') in tmpDict:
                                            tmpDict[vehicle.get('id')]['time'].append(step.get('time'))
                                            tmpDict[vehicle.get('id')]['posistion'].append( float(vehicle.get('pos')) )
                                            tmpDict[vehicle.get('id')]['laneID'].append(vehicle.get('lane'))
                                            tmpDict[vehicle.get('id')]['xCor'].append( float( vehicle.get('x')) )
                                            tmpDict[vehicle.get('id')]['yCor'].append( float( vehicle.get('y')) )
                                            tmpDict[vehicle.get('id')]['speed'].append( float(vehicle.get('speed')) )
                                            tmpDict[vehicle.get('id')]['acceleration'].append(vehicle.get('acceleration'))
                                            tmpDict[vehicle.get('id')]['type'].append(vehicle.get('type'))

                                            if  vehicle.get('caccControlMode'):
                                                tmpDict[vehicle.get('id')]['mode'].append( vehicle.get('caccControlMode') )
                                            else:
                                                tmpDict[vehicle.get('id')]['mode'].append( "" )

                                        else:
                                            if  vehicle.get('caccControlMode'):
                                                tmpDict[vehicle.get('id')] = {  
                                                                            'time':[step.get('time')],
                                                                            'posistion':[float(vehicle.get('pos'))],
                                                                            'laneID':[vehicle.get('lane')],
                                                                            'xCor':[float( vehicle.get('x'))],
                                                                            'yCor':[float( vehicle.get('y'))], 
                                                                            'speed':[float(vehicle.get('speed'))],
                                                                            'acceleration':[vehicle.get('acceleration')],
                                                                            'type':[vehicle.get('type')],
                                                                            'mode':[vehicle.get('caccControlMode')]}
                                            else:
                                                tmpDict[vehicle.get('id')] = {  
                                                                            'time':[step.get('time')],
                                                                            'posistion':[float(vehicle.get('pos'))],
                                                                            'laneID':[vehicle.get('lane')],
                                                                            'xCor':[float( vehicle.get('x'))],
                                                                            'yCor':[float( vehicle.get('y'))], 
                                                                            'speed':[float(vehicle.get('speed'))],
                                                                            'acceleration':[ vehicle.get('acceleration')],
                                                                            'type':[vehicle.get('type')],
                                                                            'mode':[ ""]}

                            try:
                                pickle_out = open(fnStrPickle, 'wb')
                                pickle.dump(tmpDict, pickle_out)
                                pickle_out.close
                            except:
                                pass

                            if verbose:        
                                if use_parallel:
                                    q.put('Saving file:' + fnStrPickle)
                                else:
                                    print('Saving file:', fnStrPickle)                                
                    else:
                        print("File not found:", fnStr)
                

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    
    print("\n%s"%seperator)
    print("Data transform fcd trajectories proccess started")
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

    if use_parallel:
        jobs = []

        print("\n%s"%seperator)
        clearFiles()

        print(seperator)
        print("Please wait...")

        for s in scenarios:
            if use_parallel:
                temp = mp.Process(target = multiwork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q,))
            if use_threads:
                temp = mt.Thread(target = multiwork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q,))

            jobs.append(temp)
        
        for th in jobs:
            th.start()

        for th in jobs:
            th.join()

        jobs.clear()

    else:    
        print("\n%s"%seperator)
        clearFiles()

        for s in scenarios:
            print("\n%s"%seperator)
            print("Current scenario:", s, "\n")

            multiwork(work_dir[s], s, scenariosFix.index(s), False, q)
            
            print("\nScenario", s, "finished")
            print(seperator)

    if use_parallel or use_threads:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time() - startTime
    
    print("\nData transform fcd trajectories proccess completed after ", completedTime, " [sec]")
    print(seperator,"\n")
