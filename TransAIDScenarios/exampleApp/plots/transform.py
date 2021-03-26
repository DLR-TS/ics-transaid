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
import numpy as np
from collections import defaultdict
import pickle

import time
import json
import warnings
from sys import platform as _platform
import multiprocessing as mp
import threading as mt
import queue

warnings.simplefilter(action='ignore', category = FutureWarning)

seperator = "---------------------------------------------------------------------"
    
verbose = False
use_parallel = False
use_threads = False

scenarios = []
scenariosFix = []
driverBehaviourList = [] 

trafficDemand = {}
seedEnables = {}

enables = []
settings = {}
work_dir = {}

# these edges will not be considered in total CO2 Emissions
edgesExcludedCO2 = []

# Determine the quantiles to be computed for each KPI
percentiles = ()    #(.0, .05, .25, .50, .75, .95, 1.0)
percentileStr = ()  #("0%", "5%", "25%", "50%", "75%", "95%", "100%")

# Thresholds in secs for which to report TTCs less than
TTC_THRESHOLDS = np.linspace(0, 5, 11)

# TTC below this value will be ignored
TTC_POSITION = 0.0

seedSize = 0
startLOS = 0

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

input_data_file = "network"
output_data_file = "network_aggregated"

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

    global workDirMain, enables, scenariosFix, work_dir, settings, scenarios, seedSize, startLOS, TTC_POSITION
    global trafficDemand, driverBehaviourList, edgesExcludedCO2
    global percentiles, percentileStr, TTC_THRESHOLDS

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
    
    startLOS = settings["transform_start_los"]
    edgesExcludedCO2 = settings["transform_edges_excluded_CO2"]
    TTC_POSITION = settings["transform_ttc_pos"]

    percentiles = settings["transform_percentiles"]
    percentileStr = settings["transform_percentileStr"]
    
    a = int(settings["transform_ttc_threshold"]["0"])
    b = int(settings["transform_ttc_threshold"]["1"])
    c = int(settings["transform_ttc_threshold"]["2"])

    TTC_THRESHOLDS = np.linspace(a,b,c)

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
        func_enable = settings["plot_network_list_" +  str(scenariosFix.index(s))]
        for enStr in func_enable:
            if func_enable[enStr] == 0:
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
def multiWork(path, scenario, sid, use_parallel, q):

    func_enable = settings["plot_network_list_" +  str(sid)]
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]

    dictOut = {}

    dim1 = len(trafficDemand)
    dim2 = len(trafficMix)
    dim3 = len(driverBehaviourList)

    if verbose:
        print("Data dimensions:", dim1, dim2, dim3)

    for funcName in func_enable:
        
        if func_enable[funcName]:
            matrix = [ [ [ 0 for i in range(dim3) ] for j in range(dim2) ] for k in range(dim1) ]
            matrixHasData = False
            
            for td_id, td_name in enumerate(trafficDemand):
                for tm_id, tm_name in enumerate(trafficMix):
                    for db_id, db_name in enumerate(driverBehaviourList):

                        if verbose:
                            print("Scenario:", scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)

                        pickleFileStr = createFileNamePickle(path, input_data_file, td_name, tm_id, db_name)

                        if not os.path.exists(pickleFileStr):
                            print('Error: Could not find demand file:', pickleFileStr)
                            continue

                        #Read data from pickle
                        pickle_in = open(pickleFileStr, 'rb')
                        tempDF = pickle.load(pickle_in)
                        pickle_in.close()

                        res = defaultdict(list)

                        for seed in range(seedSize):

                            if seedEnables[td_name][tm_name][seed] == 0:
                                if (use_parallel):
                                    q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                                else:
                                    print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                                continue

                            #### AverageNetworkSpeed, Speed #########################################################################################
                            if funcName == "Speed":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Ntwk_seed == seed]
                                D = df_seed.describe(percentiles = percentiles)
                                res["seed"].append(seed)
                                res["mean"].append(D.at['mean', 'Ntwk_speed'])
                                res["std"].append(D.at['std', 'Ntwk_speed'])
                                for ps in percentileStr:
                                    res[ps].append(D.at[ps, "Ntwk_speed"])
                            
                            #new method for average network speed. Net Speed = sum(departDelay + arrival - depart ) / sum(routeLength) or Travel distance / Travel time
                            if funcName == "SpeedAvg":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Tripinfos_seed == seed]

                                df_routeLength = df_seed.Tripinfos_routeLength
                                df_travelTime = df_seed.Tripinfos_traveltime

                                res["seed"].append(seed)
                                for id, v in df_routeLength.items():
                                    res["speedAvg"].append(df_routeLength[id] * settings["convert_m_s_to_km_h"] / df_travelTime[id])
                                
                                #old data
                                #res["seed"].append(seed)
                                #res["speedAvg"].append(sum(df_seed.Tripinfos_routeLength) * settings["convert_m_s_to_km_h"] / sum(df_seed.Tripinfos_traveltime))


                            ### Throughput ###########################################################################################################
                            if funcName == "Throughput":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Throughput_seed == seed]
                                res["seed"].append(seed)
                                df_helper = df_seed[df_seed.Throughput_time == 3600.00]
                                res["throughput"].append(sum(df_helper.Throughput_ended))

                            ### SSM TTC ##############################################################################################################
                            if funcName == "SSM":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.SSM_seed == seed]
                                df_seed = df_seed[df_seed.SSM_Xposition > TTC_POSITION]

                                D = df_seed.describe(percentiles = percentiles)
                                res["seed"].append(seed)
                                for th in TTC_THRESHOLDS:
                                    nEvents = sum(df_seed.SSM_TTC <= th)/2. # divide by two since ego and foe will report the event
                                    res["ttc_le_" + str(th) + "_sec"].append(nEvents)

                            ### CO2 Emissions ########################################################################################################
                            if funcName == "Emissions":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Emission_seed == seed]
                                res["seed"].append(seed)

                                totalCO2 = sum(df_seed.Emission_CO2)
                                totalDist = sum(df_seed.Travel_dist)

                                excludedCO2 = 0.
                                excludedDist = 0.
                                for edgeID in edgesExcludedCO2:
                                    df_edgeCO2 = df_seed[df_seed.Emission_edge == edgeID]
                                    df_edgeDist = df_seed[df_seed.Travel_edge == edgeID]
                                    excludedCO2 += sum(df_edgeCO2.Emission_CO2)
                                    excludedDist += sum(df_edgeDist.Travel_dist)
                                    if verbose:
                                        print('Excluding {} [CO2] {} [m]'.format(str(excludedCO2),str(excludedDist)))
                                try:
                                    res["sum"].append( (abs(totalCO2-excludedCO2))/abs(totalDist-excludedDist) )
                                except:
                                    print(totalDist, excludedDist)

                                # res["sum"].append(sum(df_seed.Emission_CO2)/sum(df_seed.Travel_dist))
                                # res["sumCO2"].append(sum(df_seed.Emission_CO2))
                                # res["sumDist"].append(sum(df_seed.Travel_dist))

                            #Tocs = ToR + MRM
                            ### ToR ###############################################################################################################
                            if funcName == "ToR":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Tocs_seed == seed]
                                res["seed"].append(seed)
                                res["ToR"].append(len(df_seed[df_seed.Tocs_mode == 'ToCdown']))

                            ### MRM ################################################################################################################
                            if funcName == "MRM":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Tocs_seed == seed]
                                res["seed"].append(seed)
                                res["MRM"].append(len(df_seed[df_seed.Tocs_mode == 'MRM']))

                            ### Traveltime Tripinfos ################################################################################################
                            if funcName == "Traveltime":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.Tripinfos_seed == seed]
                                #df_seed_DistTrav = tempDF[tempDF.Emission_seed == seed] old code

                                res["seed"].append(seed)
                                #res["traveltime"].append(sum(df_seed.tripinfos_traveltime)/sum(df_seed_DistTrav.Travel_dist)) old code

                                res["traveltime"].append( sum(df_seed.Tripinfos_traveltime) * settings["convert_s_m_to_min_km"] / sum(df_seed.Tripinfos_routeLength) )
                                
                            ### LaneChanges #########################################################################################################
                            if funcName == "LaneChanges":
                                matrixHasData = True
                                df_seed = tempDF[tempDF.LaneChanges_seed == seed]
                                df_seed_DistTrav = tempDF[tempDF.Emission_seed == seed]

                                res["seed"].append(seed)
                                # df_helper = df_seed[df_seed.Throughput_time == 3600.00]
                                try:
                                    res["laneChanges"].append(sum(df_seed.NumberLaneChanges)/sum(df_seed_DistTrav.Travel_dist))
                                except:
                                    print(df_seed_DistTrav.Travel_dist)
                        
                        matrix[td_id][tm_id][db_id] = res

            if verbose:
                if matrixHasData:
                    if use_parallel:
                        q.put("\nData for %s finished."%funcName)
                        q.put("\nRaw data: %s\n"%matrix)
                    else:
                        print("\nData for %s finished."%funcName)
                        print("\nRaw data: %s\n"%matrix)
                else:
                    if use_parallel:
                        q.put("\nData for %s skipped."%funcName)
                    else:
                        print("\nData for %s skipped."%funcName)
            if matrixHasData:
                dictOut[funcName] = matrix

    dictOutPath = os.path.join(path, '%s_%s'%(output_data_file,scenario)) + '.pickle'

    pickle_out = open(dictOutPath, 'wb')
    pickle.dump(dictOut, pickle_out)
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
    print("Data trasformation proccess started")
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

            multiWork(work_dir[s], s, scenariosFix.index(s), False, q)
            
            print("\nScenario", s, "finished")
            print(seperator)

    if use_parallel or use_threads:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time()- startTime
    print("\nData transformation process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
