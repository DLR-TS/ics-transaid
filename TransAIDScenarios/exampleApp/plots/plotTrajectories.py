#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# Date: 2020/01/01
# Author: Vasilios Karagounis

import os, sys, errno
import matplotlib.pyplot as plt
import numpy as np
import xml.etree.ElementTree as ET
import pandas as pd
import seaborn as sns
import matplotlib.patches as mpatches

import pickle
import time
import json

from functools import reduce
from collections import defaultdict
from sys import platform as _platform

import multiprocessing as mp
import threading as mt
import queue

import traceback

seperator = "---------------------------------------------------------------------"

plt.rcParams.update({'figure.max_open_warning': 0}) #explicitly closed and may consume too much memory. (To control this warning, see the rcParam `figure.max_open_warning`)

verbose = False
use_parallel = False
use_threads = False

scenarios = []
scenariosFix = []

driverBehaviourList = []
trafficDemand = {}

enables = []
settings = {}
work_dir = {}

legends = []

workDirMain = ""
dir_figs = ""

seedSize = 0

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

LoS_Range_Str = ""
Mix_Range_Str = ""
losKeys = []

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

#--------------------------------------------------------------------------------------------------------------------------------------------
# load settings from json
#---------------------------------------------------------------------------------------------------------------------------------------------
def loadSettings():

    global workDirMain, enables, scenariosFix, settings, scenarios
    global trafficDemand, driverBehaviourList, seedSize, legends
    global LoS_Range_Str, Mix_Range_Str, losKeys, dir_figs, work_dir

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    workDirMain = os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings/plots.json")
    
    loadFileSettings(settingsDir)
    
    dir_figs = os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["trajectories"])

    if not os.path.exists(dir_figs):
        mkdir_p(dir_figs)

    if not enables:
        enables = list(settings["enables"])

    scenariosFix = list(settings["scenarios"])
    legendsFix = list(settings["plot_network_legends"])

    for id, enbl in enumerate(enables):
        if enbl == 1:
            scenarios.append(scenariosFix[id])
            legends.append(legendsFix[id])

    if not scenarios:
        print("Error: no scenarios found")
        sys.exit(1)

    print("Scenarios used:", scenarios)

    for s in scenarios:
        tmp = os.path.join(workDirMain, s)
        work_dir[s] = tmp

    if settings["fast_debug"] == "True":
        seedSize = 1
    else: 
        seedSize = settings["seed_size"]

    trafficDemand = settings["level_Of_service"]
    driverBehaviourList = settings["schemes"]

    for id, enbl in enumerate(enables):
        if enbl == 1:
            trafficMix = (settings["traffic_mix_" + str(id)])
            break

    trafficDemandSize = len(trafficDemand)
    trafficMixSize = len(trafficMix)

    losKeys = []
    for strLos in trafficDemand.keys():
        losKeys.append(strLos)

    LoS_Range_Str = "_LoS_" + losKeys[0][-1] + "-" + losKeys[trafficDemandSize - 1][-1] 

    mixKeys = []
    for numMix in trafficMix.values():
        mixKeys.append(str(numMix + int(settings["plot_network_png_mix_offset"])))

    Mix_Range_Str = "_Mix_" + mixKeys[0][-1] + "-" + mixKeys[trafficMixSize - 1][-1]     

#---------------------------------------------------------------------------------------------------------------------------------------------
def clearFiles():

    if settings["clean_old_data"] == "True":
        
        count = 0

        for s in scenarios:
            path = os.path.join(dir_figs, "Lanes", s)
            if os.path.exists(path):
                count = 0
                for file in os.listdir(path):
                    if file.endswith(".png") or file.endswith(".svg"):
                        temp = os.path.join(path, file)
                        os.remove(temp)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s, %s"%(count, "Lanes", s))
        
        count = 0
        path = os.path.join(dir_figs)
        if os.path.exists(path):
            for file in os.listdir(path):
                if file.endswith(".png") or file.endswith(".svg"):
                    temp = os.path.join(path, file)
                    os.remove(temp)
                    count+=1

            if count > 0: 
                print("Cleared %s files from %s"%(count, settings["folder_names"]["trajectories"]))

#---------------------------------------------------------------------------------------------------------------------------------------------
def printDisabledFunctions():

    for s in scenarios:
        func_enable = settings["plot_trajectories_list_" +  str(scenariosFix.index(s))]
        for enStr in func_enable:
            if func_enable[enStr] == 0:
                print("Warning :", enStr, "is disabled on", s)

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
#
#---------------------------------------------------------------------------------------------------------------------------------------------
class DataVehList():
    def __init__(self):

        self.xPos = []
        self.yPos = []
        self.time = []
        self.laneId = []
        self.tocEvent = []

    @property
    def xPosList(self):
        return self.xPos

    @property
    def yPosList(self):
        return self.yPos

    @property
    def timeList(self):
        return self.time

    @property
    def laneList(self):
        return self.laneId

    @property
    def tocEventList(self):
        return self.tocEvent

    ###########################
    def addXPos(self, value):
        self.xPos.append(value)

    def addYPos(self, value):
        self.yPos.append(value)
   
    def addTime(self, value):
        self.time.append(value)
    
    def addLaneId(self, value):
        self.laneId.append(value)
    
    def addTocEvent(self, value):
        self.tocEvent.append(value)


#---------------------------------------------------------------------------------------------------------------------------------------------
def plotTocLanes(path, scenario, sid, use_parallel, q):
   
    global verbose, settings

    func_list = settings["plot_trajectories_list_" +  str(sid)]

    if func_list["TocLanes"] == 0:
        return

    pathSave = os.path.join(dir_figs, "Lanes", scenario)

    if not os.path.exists(pathSave):
        mkdir_p(pathSave)

    scenario_legend = settings["scenarios_legends_dict"][scenario]
    
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]

    driverBehaviourList = settings["schemes"]

    lanesCount = int(settings["plot_trajectories_toc_lanes_count"])

    for td_id, td_name in enumerate(trafficDemand):
        for tm_id, tm_name in enumerate(trafficMix):
            for db_name in driverBehaviourList:

                for seed in range(seedSize):

                    if seedEnables[td_name][tm_name][seed] == 0:
                        if (use_parallel):
                            q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                        else:
                            print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                        continue

                    if verbose:
                        print(scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)

                    ############################################################################################################
                    fnStr = createFileNameXml(path, 'output', td_name, tm_id, db_name, seed)

                    dataVehTocs = {}

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
                            
                            for cf in list(root):

                                vehId = cf.get("id")
                                
                                if vehId in dataVehTocs:
                                    dv = dataVehTocs[vehId]
                                else:    
                                    dv = dataVehTocs[vehId] = DataVehList()
                                
                                x = float(cf.get("x"))
                                y = float(cf.get("y"))
                                t = float(cf.get("t"))
                                
                                if t >= settings["plot_trajectories_time_limits"][0] and t <= settings["plot_trajectories_time_limits"][1] and x >= settings["plot_trajectories_distance_limits"][0] and x <= settings["plot_trajectories_distance_limits"][1]:
                                    try:
                                        dv.addXPos(x)
                                        dv.addYPos(y)
                                        dv.addTime(t)
                                        dv.addLaneId(cf.get("lane")[-1])
                                        dv.addTocEvent(cf.tag)
                                    except:
                                        print("Error:", cf)
                                        pass
                                    
                                    dataVehTocs[vehId] = dv

                    else:
                        print("File not found:", fnStr)
                        continue
                            
                    if not dataVehTocs:
                        print("Data array is empty:", fnStr)
                        continue
                    
                    ###############################################################################################
                    fnStrPickle = createFileNamePickle(work_dir[scenario], 'trajectories', td_name, tm_id, db_name, seed)

                    if not os.path.exists(fnStrPickle):
                        print('Error: Could not find demand file:', fnStrPickle)
                        continue

                    try:
                        pickle_in = open(fnStrPickle, 'rb')
                        df = pickle.load(pickle_in)
                        pickle_in.close()
                    except:
                        if use_parallel:
                            q.put('Error: file : ' + fnStrPickle + ' is corrupted')
                        else:
                            print('Error: file : ', fnStrPickle, ' is corrupted')
                        continue
                    finally:
                        
                        #predefine states, colors
                        vehState = {}
                        vehColor = {}
                        vehIsCav = {}

                        for veh in df:
                            if veh not in vehState:
                                vehState[veh] = False
                                vehColor[veh] = settings["plot_trajectories_toc_colors"]["Manual"]
                                if "LV" in veh:
                                    vehIsCav[veh] = 1
                                elif "CAV" or "CV" in veh:
                                    vehIsCav[veh] = 2
                                else:
                                    vehIsCav[veh] = 0
                        
                        for ln in range(0, lanesCount):
                            for veh in df:
                                if veh not in vehIsCav or vehIsCav[veh] == 0:
                                    continue
                            
                                x = np.array(df[veh]['xCor'])
                                y = np.array(df[veh]['yCor'])
                                mode = np.array(df[veh]['mode'])
                                
                                laneIDlist = []
                                lanesId = df[veh]['laneID']
                                for lnid, lnstr in enumerate(lanesId):
                                    laneIDlist.append(int(lnstr[-1:]))
                                
                                #################################################################################################3
                                if vehIsCav[veh] == 2: #CV or CAV
                                    
                                    if veh not in dataVehTocs:
                                        continue
                                    
                                    timeTocs = np.array(dataVehTocs[veh].timeList)
                                    tocs = np.array(dataVehTocs[veh].tocEventList)

                                    mrm_toc_down_on_same_sec = False
                                    veh_same_sec = ""

                                    for i in range(0, len(x) -1):

                                        if i >= len(laneIDlist) or laneIDlist[i] != ln:
                                            continue
                                        
                                        t = float(df[veh]['time'][i])

                                        if t < float(settings["plot_trajectories_time_limits"][0]) or t > float(settings["plot_trajectories_time_limits"][1]):
                                            continue

                                        if x[i] < float(settings["plot_trajectories_distance_limits"][0]) or x[i] > float(settings["plot_trajectories_distance_limits"][1]):
                                            continue
                                        
                                        try:
                                            distEv = [x[i]]
                                            timeEv = [t]
                                            
                                            distEv.append( np.sqrt( (x[i + 1] - x[i])**2 + (y[i + 1] - y[i])**2 ) + x[i])
                                            timeEv.append(float(df[veh]['time'][i + 1]))

                                            #if td_id == 1 and tm_id == 0:
                                            #    print(veh, timeEv, distEv)

                                            if vehState[veh] == False:
                                                vehState[veh] = True
                                                if mode[i] == "ACC":
                                                    vehColor[veh] = settings["plot_trajectories_toc_colors"]["ACC"]
                                                elif mode[i] == "CACC":
                                                    vehColor[veh] = settings["plot_trajectories_toc_colors"]["CACC"]
                                            elif vehState[veh] == True:
                                                if mrm_toc_down_on_same_sec and veh_same_sec == veh:
                                                    
                                                    if verbose:
                                                        print(veh_same_sec, "has performed MRM and ToCdown at the same time (second): ", timeEv)

                                                    vehColor[veh_same_sec] = settings["plot_trajectories_toc_colors"]["ToCdown"]
                                                    mrm_toc_down_on_same_sec = False
                                                    veh_same_sec = ""
                                                    if i > 0: i = i - 1 # go back one step
                                                else :
                                                    mrm = False
                                                    for t_id, t_toc in enumerate(timeTocs):
                                                        if t_toc >= timeEv[0] and t_toc <= timeEv[1]:
                                                            if tocs[t_id] == "TOR":
                                                                vehColor[veh] = settings["plot_trajectories_toc_colors"]["TOR"]
                                                                break
                                                            elif tocs[t_id] == "MRM":
                                                                vehColor[veh] = settings["plot_trajectories_toc_colors"]["MRM"]
                                                                mrm = True
                                                                veh_same_sec = veh
                                                                #break  mrm_toc_down_on_same_sec is not working with break enable
                                                            elif tocs[t_id] == "ToCdown":
                                                                if mrm:
                                                                   mrm_toc_down_on_same_sec = True
                                                                   veh_same_sec = veh
                                                                else:
                                                                    vehColor[veh] = settings["plot_trajectories_toc_colors"]["ToCdown"]
                                                                break
                                                            elif tocs[t_id] == "ToCup":
                                                                if mode[i] == "ACC":
                                                                    vehColor[veh] = settings["plot_trajectories_toc_colors"]["ACC"]
                                                                elif mode[i] == "CACC":
                                                                   vehColor[veh] = settings["plot_trajectories_toc_colors"]["CACC"]
                                                                break
                                           
                                            distEv = np.array(distEv)
                                            timeEv = np.array(timeEv)

                                            #plot every tiny, little, small, line
                                            plt.plot(timeEv, distEv, c = vehColor[veh], linewidth = settings["plot_trajectories_toc_line_width"])

                                        except Exception as e:
                                            print("error in vehicle 1:", veh)
                                            print(traceback.format_exc())
                                            continue

                                elif vehIsCav[veh] == 1: #just one color
                                   
                                    distEv = []
                                    timeEv = []
                                    for i in range(0, len(x) - 1):

                                        if i >= len(laneIDlist) or laneIDlist[i] != ln:
                                            
                                            if len(distEv):
                                                plt.plot(timeEv, distEv, c = vehColor[veh], linewidth = settings["plot_trajectories_toc_line_width"]) 
                                                distEv = []
                                                timeEv = []

                                            continue

                                        t = float(df[veh]['time'][i])

                                        if t < float(settings["plot_trajectories_time_limits"][0]) or t > float(settings["plot_trajectories_time_limits"][1]):
                                            if len(distEv):
                                                plt.plot(timeEv, distEv, c = vehColor[veh], linewidth = settings["plot_trajectories_toc_line_width"]) 
                                                distEv = []
                                                timeEv = []

                                            continue

                                        if x[i] < float(settings["plot_trajectories_distance_limits"][0]) or x[i] > float(settings["plot_trajectories_distance_limits"][1]):
                                            if len(distEv):
                                                plt.plot(timeEv, distEv, c = vehColor[veh], linewidth = settings["plot_trajectories_toc_line_width"]) 
                                                distEv = []
                                                timeEv = []
                                            continue
                                        
                                        try:
                                            distEv.append((x[i], np.sqrt((x[i + 1] - x[i])**2 + (y[i + 1] - y[i])**2) + x[i]))
                                            timeEv.append((t, (float(df[veh]['time'][i + 1]))))

                                        except:
                                            print("error in vehicle:", veh)
                                            print(traceback.format_exc())
                                            continue
                                    
                                    distEv = np.array(distEv)
                                    timeEv = np.array(timeEv)

                                    if len(distEv):
                                        plt.plot(timeEv, distEv, c = vehColor[veh], linewidth = settings["plot_trajectories_toc_line_width"])


                            plt.xlabel(settings["plot_trajectories_toc_labels_x"][0], fontsize = settings["plot_trajectories_fonts_size"][1])
                            plt.ylabel(settings["plot_trajectories_toc_labels_y"][0], fontsize = settings["plot_trajectories_fonts_size"][1])
                            plt.xticks(fontsize = settings["plot_trajectories_fonts_size"][0])
                            plt.yticks(fontsize = settings["plot_trajectories_fonts_size"][0])
                            
                            titleStr = "%s (%s/%s/Seed %d/Lane %d)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name], seed, ln)
                            plt.title(titleStr, fontsize = settings["plot_trajectories_fonts_size"][2], weight = 'bold')


                            labels = ["Manual","ACC","CACC","TOR","MRM"]
                            colors = ["yellow","blue","green","grey","red"]
                            
                            handles = []
                            for id in range(5):
                                handles.append(mpatches.Patch(color=colors[id], label=labels[id]))

                            plt.legend(handles=handles, borderaxespad = 0., handletextpad = 0.5, loc = 'center', bbox_to_anchor = tuple(settings["plot_trajectories_marker_anchor_lanes"]), ncol = 6, mode = "expand", frameon = False, prop = dict(size = settings["plot_trajectories_marker_font_size_lanes"]))

                            savePath = os.path.join(pathSave, 'Lanes_fcd_%s_%s_seed_%d_lane_%d' %(td_name, settings["traffic_mix_path"][tm_name], seed, ln))

                            if settings["plot_network_save_svg"] == "True":
                                plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight', facecolor = 'white')

                            plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight', facecolor = 'white')
                            plt.clf()
                            plt.close()
                            if verbose:
                                if use_parallel:
                                    q.put('Saving file:' + savePath)
                                else:
                                    print('Saving file:', savePath)
    
    #################################################################################################


#---------------------------------------------------------------------------------------------------------------------------------------------
def mixedWork():

    func_enables = {}

    #check if there is no reason to continue 

    tm_id_to_search = 0    
    for s in scenarios:
        func_temp = settings["plot_trajectories_list_" +  str(scenariosFix.index(s))]
        for enStr in func_temp:
            tm_id_to_search = scenariosFix.index(s)
            if func_temp[enStr] == 1 and enStr != "TocLanes" and enStr not in func_enables:
                func_enables[enStr] = "1"

    if not func_enables:
        return

    trafficMix = settings["traffic_mix_" + str(tm_id_to_search)]
    seedEnables = settings["seed_" + str(tm_id_to_search)]

    trafficMixSize = len(trafficMix)

    ParamCombiLabel = {}

    if trafficMixSize == 1:
        ParamCombiLabel = {0:"B/1", 1:"C/1", 2:"D/1"}
    elif trafficMixSize == 2:
        ParamCombiLabel = {0:"B/1", 1:"B/2", 2:"C/1", 3:"C/2", 4:"D/1", 5:"D/2"}
    elif trafficMixSize == 3:
        ParamCombiLabel = {0:"B/1", 1:"B/2", 2:"B/3", 3:"C/1", 4:"C/2", 5:"C/3", 6:"D/1", 7:"D/2", 8:"D/3"}

    maxAccel = 0.0
    maxDecel = 0.0

    for s in scenarios:
       
        dictOut = {}

        for KPI in func_enables:

            trafficMix = settings["traffic_mix_" + str(scenariosFix.index(s))]
            
            dim1 = len(trafficDemand)
            dim2 = len(trafficMix)
            dim3 = len(driverBehaviourList)
            matrix = [ [ [ 0 for i in range(dim3) ] for j in range(dim2) ] for k in range(dim1) ]
            
            for td_id, td_name in enumerate(trafficDemand):
                for tm_id, tm_name in enumerate(trafficMix):
                    for db_id, db_name in enumerate(driverBehaviourList):

                        res = defaultdict(list)
                        
                        for seed in range(seedSize):

                            if seedEnables[td_name][tm_name][seed] == 0:
                                if (use_parallel):
                                    q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                                else:
                                    print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                                continue

                            if verbose:
                                print(s, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)
                            
                            fnStrPickle = createFileNamePickle(work_dir[s], 'trajectories', td_name, tm_id, db_name, seed)

                            if not os.path.exists(fnStrPickle):
                                print('Error: Could not find demand file:', fnStrPickle)
                                continue
                            
                            try:
                                pickle_in = open(fnStrPickle, 'rb')
                                df = pickle.load(pickle_in)
                                pickle_in.close()
                            except:
                                if use_parallel:
                                    q.put('Error: file : ' + fnStrPickle + ' is corrupted')
                                else:
                                    print('Error: file : ', fnStrPickle, ' is corrupted')
                                continue
                            finally:
                                #################################################################################################
                                if KPI == "Speed" :
                                    res = defaultdict(list)
                                    for veh in df:
                                        for val in df[veh]['speed']:
                                            res["Speed"].append(val * settings["convert_m_s_to_km_h"])
                                        #for val in df[veh]['time']:
                                        #    res["time"].append(val)

                                elif KPI == "Acceleration" :
                                    res = defaultdict(list)
                                    for veh in df:
                                        for val in df[veh]['acceleration']:
                                            newval = float(val)
                                            if (type(newval) == float):
                                                res["Acceleration"].append(newval)
                                                if newval > maxAccel:
                                                    maxAccel = newval
                                                elif newval < maxDecel:
                                                    maxDecel = newval
                                        #for val in df[veh]['time']:
                                        #    res["time"].append(val)

                        matrix[td_id][tm_id][db_id] = res

            dictOut[KPI] = matrix

        dictOutPath = os.path.join(workDirMain, s, "trajectories_data_extra") + '.pickle'

        pickle_out = open(dictOutPath, 'wb')
        pickle.dump(dictOut, pickle_out)
        pickle_out.close()
        print(dictOutPath)

    #-----------------------------------------------------------------------------------------------------------------
    if "Acceleration" in func_enables:
        print("max acceleration:", maxAccel)
        print("max deceleration:", maxDecel)

    resultFiles = {}
    resultList = []
    resultDicts = {}

    for sId in range(len(scenarios)):
        path = os.path.join(workDirMain, scenarios[sId], "trajectories_data_extra") + ".pickle"
        resultList.append(path)

    if not resultList:
        print("resultList is empty")
        sys.exit(1)

    for lId in range(len(resultList)):
        resultFiles[scenarios[lId]] = resultList[lId]

    #----------------------------------------------------------
    # Load pickled contents from files
    #----------------------------------------------------------

    for batchID, fn in resultFiles.items():
        try:
            with open(fn, "rb") as f:
                resultDicts[batchID] = pickle.load(f)
        except Exception as e:
            print("Error when trying to open batch file '%s'"%fn)
            raise e

    df = defaultdict(list)

    for KPI in func_enables:
        for td_id, td_name in enumerate(trafficDemand):
            for tm_id, tm_name in enumerate(trafficMix):
                for db_id, db_name in enumerate(driverBehaviourList):
                    for batchID, resultDict in resultDicts.items():
                        
                        matrix = resultDict[KPI]
                        stats = matrix[td_id][tm_id][db_id]

                        if stats:
                            for k, v in stats.items():
                                lenStats = len(v)
                                if not k in df.keys():
                                    # Ensure equal length of all columns for the DataFrame
                                    df[k] = [None] * len(df["batchID"])
                                df[k].extend(v)
                            lenStats = len(v)

                            df["batchID"].extend(lenStats * [batchID])
                            df["Level of Service"].extend(lenStats * [td_id])
                            df["Traffic Mix"].extend(lenStats * [tm_id])
                            df["scheme"].extend(lenStats * [db_id])
                            df["KPI"].extend(lenStats * [KPI])

    # Ensure equal length of all columns for the DataFrame
    dfLength = len(df["batchID"])
    for k, v in df.items():
        v.extend([None]*(dfLength - len(v)))

    df = pd.DataFrame(df)

    # Set plot style
    sns.set(style="whitegrid")
    
    #################################################################################################
    if "Speed" in func_enables:
        plotSpeed(ParamCombiLabel, df, "box", True)
        plotSpeed(ParamCombiLabel, df, "box", False)
        plotSpeed(ParamCombiLabel, df, "violin", False)
        print("Speed plots created")
   
    #################################################################################################
    if "Acceleration" in func_enables:
        plotAcceleration(ParamCombiLabel, df, "box", True)
        plotAcceleration(ParamCombiLabel, df, "box", False)
        plotAcceleration(ParamCombiLabel, df, "violin", False)
        print("Acceleration plots created")

#---------------------------------------------------------------------------------------------------------------------------------------------
def plotSpeed(paramCombiLabel, dataKPI, plotType, showFliers):
    
    KPI = "Speed"

    try:
        dataKPI["Parameter Combination (LoS/Mix)"] = dataKPI["Level of Service"] * 3 + dataKPI["Traffic Mix"]
    except:
        print("Unble to read df data")
    finally:
        
        dfKPI = dataKPI[(dataKPI["KPI"] == KPI)]
        labelsY = list(settings["plot_trajectories_toc_labels_y"])

        #https://matplotlib.org/users/dflt_style_changes.html?highlight=flierprops#boxplot
        flierprops = dict(marker = 'X', markerfacecolor = 'r', markersize = 0.1, linestyle = 'none') #, markeredgecolor = 'b'
        
        if plotType == "box":
            if showFliers == True:
                bp = sns.boxplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = KPI, hue = "batchID", linewidth = 0.8, fliersize = 0, orient = 'v', flierprops = flierprops)
            else:
                bp = sns.boxplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = KPI, hue = "batchID", linewidth = 0.8, fliersize = 0, orient = 'v')    
        elif plotType == "violin":
            bp = sns.violinplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = KPI, hue = "batchID",  split = True, linewidth = 0.8, fliersize = 0, inner = "quartile")
        
        bp.set(ylabel = labelsY[1])
        
        handles, labels = bp.get_legend_handles_labels()
        labels = legends

        plt.legend(handles[0:2], labels[0:2], borderaxespad = 0., loc='center',  bbox_to_anchor = (0.25, 1.00, 0.45, 0.15),  ncol = 2, mode = "expand", frameon = False)
        
        plt.gca().set_xticks(list(paramCombiLabel.keys()))
        plt.gca().set_xticklabels(list(paramCombiLabel.values()))
        
        if plotType == "box":
            if showFliers == True:
                plt.ylim(settings["plot_trajectories_speed_avg"]["box"])
            else:
                plt.ylim(settings["plot_trajectories_speed_avg"]["boxflier"])
        elif plotType == "violin":
            plt.ylim(settings["plot_trajectories_speed_avg"]["violin"])

        plt.gcf().set_size_inches((settings["plot_trajectories_speed_fig_dimension"][0] * 2, settings["plot_trajectories_speed_fig_dimension"][1]))
        plt.tight_layout()

        savePath = os.path.join(dir_figs, "Average_Network_Speed_fcd")

        if plotType == "box":
            savePath += "_box"
        elif plotType == "violin":
            savePath += "_violin"

        if showFliers == True:
             savePath += "_fliers"

        savePath += LoS_Range_Str + Mix_Range_Str
        
        if verbose:
            print(savePath)
        
        if settings["plot_network_save_svg"] == "True":
            plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

        plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
        plt.clf()
        plt.close()   
        
        if verbose:
            if use_parallel:
                q.put('Saving file:' + savePath)
            else:
                print('Saving file:', savePath)

#---------------------------------------------------------------------------------------------------------------------------------------------
def plotAcceleration(paramCombiLabel, dataKPI, plotType, showFliers):

    KPI = "Acceleration"

    try:
        dataKPI["Parameter Combination (LoS/Mix)"] = dataKPI["Level of Service"] * 3 + dataKPI["Traffic Mix"]
    except:
        print("Unble to read df data")
    finally:
        
        dfKPI = dataKPI[(dataKPI["KPI"] == KPI)]
        labelsY = list(settings["plot_trajectories_toc_labels_y"])

        #https://matplotlib.org/users/dflt_style_changes.html?highlight=flierprops#boxplot
        flierprops = dict(marker = 'X', markerfacecolor = 'r', markersize = 0.1, linestyle = 'none') #, markeredgecolor = 'b'

        if plotType == "box":
            if showFliers == True:
                bp = sns.boxplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = KPI, hue = "batchID", linewidth = 0.8, fliersize = 0, orient = 'v', flierprops = flierprops)
            else:
                bp = sns.boxplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = KPI, hue = "batchID", linewidth = 0.8, fliersize = 0, orient = 'v')
        elif plotType == "violin":
                bp = sns.violinplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = KPI, hue = "batchID",  split = True, linewidth = 0.8, fliersize = 0, inner = "quartile")
        
        bp.set(ylabel = labelsY[2])
        handles, labels = bp.get_legend_handles_labels()
        labels = legends
        plt.legend(handles[0:2], labels[0:2], borderaxespad = 0., loc='center', bbox_to_anchor = (0.25, 1.00, 0.45, 0.15),  ncol = 2, mode = "expand", frameon = False)
        plt.gca().set_xticks(list(paramCombiLabel.keys()))
        plt.gca().set_xticklabels(list(paramCombiLabel.values()))
        
        if plotType == "box":
            if showFliers == True:
                plt.ylim(settings["plot_trajectories_acceleration_avg"]["boxflier"])
            else:
                plt.ylim(settings["plot_trajectories_acceleration_avg"]["box"])
        elif plotType == "violin":
            plt.ylim(settings["plot_trajectories_acceleration_avg"]["violin"])
        
        plt.gcf().set_size_inches((settings["plot_trajectories_acceleration_fig_dimension"][0] * 2, settings["plot_trajectories_acceleration_fig_dimension"][1]))
        plt.tight_layout()
        
        savePath = os.path.join(dir_figs, "Average_Network_Acceleration_fcd")
        
        if plotType == "box":
            savePath += "_box"
        elif plotType == "violin":
            savePath += "_violin"
        if showFliers == True:
             savePath += "_fliers"

        savePath += LoS_Range_Str + Mix_Range_Str + ".png"    

        if verbose:
            print(savePath)
        
        if settings["plot_network_save_svg"] == "True":
            plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"])

        plt.savefig(savePath + ".png", dpi = settings["plots_dpi"])
        plt.clf()
        plt.close()   
        
        if verbose:
            if use_parallel:
                q.put('Saving file:' + savePath)
            else:
                print('Saving file:', savePath)

#-----------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data fcd trajectories plotting proccess started")
    startTime = time.time()

    if "--verbose" in sys.argv:
        verbose = True

    for arg in sys.argv:
        if "--en=" in arg:
            temp = arg.replace("--en=", "")
            enables = [int(n) for n in temp.split(",")]

    find_platform()

    loadSettings()

    use_parallel = settings["use_parallel"] == "True"

    if "--parallel" in sys.argv:
        use_parallel = True

    if use_parallel and OS_TYPE == is_win:
        use_parallel = False
        use_threads = False  #Struggled to succeed but with no luck TODO
    
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
        printDisabledFunctions()

        print(seperator)
        print("Please wait...")

        for s in scenarios:
            if use_parallel:
                temp = mp.Process(target = plotTocLanes, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q, ))
            if use_threads:
                temp = mt.Thread(target = plotTocLanes, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q, ))

            jobs.append(temp)

        for th in jobs:
            th.start()

        for th in jobs:
            th.join()

        jobs.clear()

        mixedWork()

    else:
        print("\n%s"%seperator)
        clearFiles()

        print(seperator)
        printDisabledFunctions()

        for s in scenarios:
            print("\n%s"%seperator)
            print("Current scenario:", s, "\n")

            plotTocLanes(work_dir[s], s, scenariosFix.index(s), False, q)
            
            print("\nScenario", s, "finished")
            print(seperator)

        mixedWork()

    if use_parallel:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time()- startTime
    print("\nData fcd trajectories plotting process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
