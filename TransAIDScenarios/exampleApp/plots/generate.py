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

from sys import platform as _platform
import multiprocessing as mp
import threading as mt
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

trafficDemand = {}
driverBehaviourList = []

seedEnables = {}
seedSize = 0

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

output_data_file = "network"

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
                    if file.startswith(output_data_file) and file.endswith(".pickle"):
                        path = os.path.join(work_dir[s], file)
                        os.remove(path)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s"%(count, s))
        except:
            pass

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
def createFileNameXml(m_path, m_name, m_demand, m_mix_id, m_behaviour, m_seed):
    
    name = [ m_name,
                traffic_Demand_Name, str(trafficDemand[m_demand]),
                trafic_Mix_Name, str(m_mix_id),
                driver_Behaviour_Name, m_behaviour,
                'seed', str(m_seed) ]

    t_path = os.path.join(m_path, '_'.join(name)) + '.xml'

    return t_path

#---------------------------------------------------------------------------------------------------------------------------------------------
def createFileNamePickle(m_path, m_name, m_demand, m_mix_id, m_behaviour):
    
    name = [ m_name,
                traffic_Demand_Name, str(trafficDemand[m_demand]),
                trafic_Mix_Name, str(m_mix_id),
                driver_Behaviour_Name, m_behaviour]

    t_path = os.path.join(m_path, '_'.join(name)) + '.pickle'

    return t_path    

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWork(path, scenario, sid, use_parallel, q):
  
    func_enable = settings["plot_network_list_" +  str(sid)]
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]
    
    if (not func_enable["SpeedAvg"] and
        not func_enable["Throughput"] and
        not func_enable["SSM"] and
        not func_enable["Emissions"] and
        not func_enable["ToR"] and
        not func_enable["MRM"] and
        not func_enable["Traveltime"] and
        not func_enable["LaneChanges"] and 
        not func_enable["Detectors"]):
        
        if use_parallel:
            q.put('multiWork abandon (no enables)')
        else:
            print('multiWork abandon (no enables)')
        return

    for td_name in trafficDemand:
        for tm_id, tm_name in enumerate(trafficMix):
            for db_name in driverBehaviourList:

                if verbose:
                    print("Scenario:", scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)

                pickleFileStr = createFileNamePickle(path, output_data_file, td_name, tm_id, db_name)

                saveFrame = []
                
                countDataSaved = 0
                for seed in range(seedSize):
                    
                    if seedEnables[td_name][tm_name][seed] == 0:
                        if (use_parallel):
                            q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                        else:
                            print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                        continue

                    # Throughput #################################################################################################################
                    if func_enable["Throughput"]:
                        
                        fnStr = createFileNameXml(path, 'outputSummary', td_name, tm_id, db_name, seed)

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
                                
                                edge = root.findall('step')

                                if func_enable["Throughput"]:
                                    tmpDict = pd.DataFrame()
                                    tmpDict['Throughput_running'] = [float(i.get('running')) for i in edge]
                                    tmpDict['Throughput_ended'] = [float(i.get('ended'))   for i in edge]
                                    tmpDict['Throughput_seed'] = [ seed for i in edge ]
                                    tmpDict['Throughput_time'] = [float(i.get('time')) for i in edge]

                                    saveFrame.append(tmpDict)
                                    countDataSaved+=1
                                    if verbose: 
                                        print('Throughput')

                        else:
                            print('File does not exist :', fnStr)
                    # SSM #################################################################################################################
                    if func_enable["SSM"]: #Sarogate safety measures

                        fnStr = createFileNameXml(path, 'outputSSM', td_name, tm_id, db_name, seed)

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
                                element = root.findall('*/minTTC')
                                tmpDict = pd.DataFrame()
                                tmpDict['SSM_time'] = [float(i.get('time')) for i in element if i.get('value')!='NA' and i.get('time')!='NA']
                                tmpDict['SSM_TTC'] = [float(i.get('value')) for i in element if i.get('value')!='NA' and i.get('time')!='NA']
                                tmpDict['SSM_Xposition'] = [float( i.get('position').split(',')[0] ) for i in element if i.get('value')!='NA' and i.get('time')!='NA']
                                tmpDict['SSM_seed'] = [ seed for i in element if i.get('value')!='NA' and i.get('time')!='NA']

                                saveFrame.append(tmpDict)
                                countDataSaved+=1
                                if verbose: 
                                    print('SSM')

                    # Emissions #################################################################################################################
                    if func_enable["Emissions"]:
                        emissionList = [ 'outputEmission',
                                         traffic_Demand_Name , str( trafficDemand[td_name]),
                                         trafic_Mix_Name , str( tm_id  ),
                                         driver_Behaviour_Name , db_name,
                                         'seed' , str(seed)]

                        emissionStr = os.path.join(path, '_'.join(emissionList)) + '.xml'

                        meanDataList = [ 'outputMeandata',
                                         traffic_Demand_Name , str( trafficDemand[td_name]),
                                         trafic_Mix_Name , str( tm_id  ),
                                         driver_Behaviour_Name , db_name,
                                         'seed' , str(seed)]

                        meanDataListStr = os.path.join(path, '_'.join(meanDataList)) + '.xml'

                        if os.path.exists(emissionStr) and os.path.exists(meanDataListStr):
                            emissionRoot = None
                            meanDataRoot = None
                            try:
                                emissionTree = ET.parse(emissionStr)
                                emissionRoot = emissionTree.getroot()

                                meanDataTree = ET.parse(meanDataListStr)
                                meanDataRoot = meanDataTree.getroot()
                            except ET.ParseError:    
                                if (use_parallel):
                                    q.put('File : ' + emissionStr + " or " + meanDataListStr + ' is corrupted\n')
                                else:
                                    print('File : ' + emissionStr + " or " + meanDataListStr + ' is corrupted')
                            except:
                                if (use_parallel):
                                    q.put('File : ' + emissionStr + " or " + meanDataListStr + ' is corrupted\n')
                                else:
                                    print('File : ' + emissionStr + " or " + meanDataListStr + ' is corrupted')
                            finally:
                                if emissionRoot == None:
                                    if (use_parallel):
                                        q.put('File : ' + emissionStr + ' is corrupted\n')
                                    else:
                                        print('File : ' + emissionStr + ' is corrupted')
                                elif meanDataRoot == None:
                                    if (use_parallel):
                                        q.put('File : ' + meanDataListStr + ' is corrupted\n')
                                    else:
                                        print('File : ' + meanDataListStr + ' is corrupted')
                                else:    
                                    tmpDict = {}
                                    tmpDict['Emission_interval']=[]
                                    tmpDict['Emission_CO2']=[]
                                    tmpDict['Emission_edge']=[]
                                    tmpDict['Emission_seed']=[]

                                    tmpDict['Travel_interval']=[]
                                    tmpDict['Travel_dist']=[]
                                    tmpDict['Travel_edge']=[]
                                    tmpDict['Travel_seed']=[]

                                    for interval in emissionRoot.findall('interval'):
                                        #devided by 10^3 [mg]->[g]
                                        if interval:
                                            for edge in interval:
                                                tmpDict['Emission_interval'].append( [ interval.get('begin'), interval.get('end') ] )
                                                tmpDict['Emission_CO2'].append( float(edge.get('CO2_abs'))/1000 )
                                                tmpDict['Emission_edge'].append( edge.get('id') )
                                                tmpDict['Emission_seed'].append( seed )

                                    for interval in meanDataRoot.findall('interval'):
                                        #devided by 10^3 [mg]->[g]
                                        if interval:
                                            for edge in interval:
                                                tmpDict['Travel_interval'].append( [ interval.get('begin'), interval.get('end') ] )
                                                tmpDict['Travel_dist'].append( float( edge.get('sampledSeconds') ) * float( edge.get('speed') ) / 1000.0  )
                                                tmpDict['Travel_edge'].append( edge.get('id') )
                                                tmpDict['Travel_seed'].append( seed )

                                    tmpFrame = pd.DataFrame(tmpDict)
                                    saveFrame.append(tmpFrame)
                                    countDataSaved+=1
                                    if verbose: 
                                        print('Emissions')
                    # Tocs #################################################################################################################
                    if func_enable["ToR"] or func_enable["MRM"]:

                        fnStr = createFileNameXml(path, 'output', td_name, tm_id, db_name, seed)

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
                                tmpDict = pd.DataFrame()
                                tocStateChanges = list(root)
                                tmpDict['Tocs_seed'] = [seed for e in tocStateChanges]
                                tmpDict['Tocs_mode'] = [e.tag for e in tocStateChanges]
                                tmpDict['Tocs_id'] = [e.get("id") for e in tocStateChanges]
                                tmpDict['Tocs_time'] = [e.get("t") for e in tocStateChanges]
                                
                                saveFrame.append(tmpDict)
                                countDataSaved+=1
                                if verbose: 
                                    print('Tocs (ToR and MRM)')
                    # Tripinfos #################################################################################################################
                    if func_enable["Traveltime"] or func_enable["SpeedAvg"]: #Tripinfos
                        
                        fnStr = createFileNameXml(path, 'outputTripinfos', td_name, tm_id, db_name, seed)

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
                                
                                #https://sumo.dlr.de/docs/Simulation/Output/TripInfo.html

                                tmpDict = pd.DataFrame()
                                tripinfos = list(root)
                                tmpDict['Tripinfos_seed'] = [seed for e in tripinfos]
                                tmpDict['Tripinfos_traveltime'] = [float(e.get("departDelay")) + float(e.get("arrival")) - float(e.get("depart")) for e in tripinfos]
                                tmpDict['Tripinfos_vCat'] = [e.get("id").split(".")[0] for e in tripinfos]
                                tmpDict['Tripinfos_routeLength'] = [float(e.get("routeLength")) for e in tripinfos]

                                saveFrame.append(tmpDict)
                                countDataSaved+=1
                                if verbose: 
                                    print('Tripinfos')
                    # LaneChanges #################################################################################################################
                    if func_enable["LaneChanges"]:

                        fnStr = createFileNameXml(path, 'outputLaneChanges', td_name, tm_id, db_name, seed)

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
                                tmpDict = pd.DataFrame()

                                changesList = root.findall('change')

                                tmpDict['NumberLaneChanges'] = [len(changesList)]
                                tmpDict['LaneChanges_seed'] = [seed]

                                saveFrame.append(tmpDict)
                                countDataSaved+=1
                                if verbose: 
                                    print('LaneChanges')

                    # Detectors #################################################################################################################
                    if func_enable["Detectors"]:

                        fnStr = createFileNameXml(path, 'detectors', td_name, tm_id, db_name, seed)

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
                                tmpDict = pd.DataFrame()

                                edge = root.findall('interval')

                                tmpDict['Detectors_seed'] = [seed for i in edge]
                                tmpDict['Detectors_id'] = [(i.get('id')) for i in edge]
                                tmpDict['Detectors_begin'] = [float(i.get('begin'))   for i in edge]
                                tmpDict['Detectors_end'] = [ float(i.get('end'))   for i in edge ]
                                tmpDict['Detectors_nVehContrib'] = [float(i.get('nVehContrib')) for i in edge]
                                tmpDict['Detectors_flow'] = [float(i.get('flow')) for i in edge]
                                tmpDict['Detectors_occupancy'] = [float(i.get('occupancy')) for i in edge]
                                tmpDict['Detectors_speed'] = [float(i.get('speed')) for i in edge] 
                                tmpDict['Detectors_nVehEntered'] = [float(i.get('nVehEntered')) for i in edge]

                                #Fix
                                tmpDict['Detectors_speed'] = tmpDict['Detectors_speed'] * settings["convert_m_s_to_km_h"]

                                saveFrame.append(tmpDict)
                                countDataSaved+=1
                                if verbose: 
                                    print('Detectors')
                    
 
                    ###############################################################################################################################
                if countDataSaved:
                    dataDict = loadFile(pickleFileStr)
                    dataDict = dataDict.append(saveFrame, ignore_index = False, sort = False)
                    dumpDictToFile(pickleFileStr, dataDict)

                    if use_parallel:
                        q.put('Saving file:' + pickleFileStr)
                    else:
                        print('Saving file:', pickleFileStr)
                else:
                    if os.path.exists(pickleFileStr):
                        os.remove(pickleFileStr)

#---------------------------------------------------------------------------------------------------------------------------------------------
def printDisabledFunctions():

    for s in scenarios:
        func_enable = settings["plot_network_list_" +  str(scenariosFix.index(s))]
        for enStr in func_enable:
            if func_enable[enStr] == 0:
                print("Warning :", enStr, "is disabled on", s)

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data generation proccess started")
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

            multiWork(work_dir[s], s, scenariosFix.index(s),  False, q)

            print("\nScenario", s, "finished")
            print(seperator)

    if use_parallel or use_threads:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time() - startTime
    
    print("\nData generation proccess completed after ", completedTime, " [sec]")
    print(seperator,"\n")
