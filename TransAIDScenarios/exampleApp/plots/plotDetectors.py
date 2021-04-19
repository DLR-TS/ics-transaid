#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# Date: 2020/01/01
# Author: Dimitrios Koutras
# Author: Vasilios Karagounis

import os, sys, errno
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
import pickle

import time
import json
import pathlib

from sys import platform as _platform
import multiprocessing as mp
import threading as mt
import queue

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

work_dir_figs = {}
work_dir_figs_lc = {}
work_dir_figs_toc = {}

seedSize = 0

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

input_data_file = "detectors_aggregated"

# TODO: use standard style and fix plotting method instead of hiding shortcomings with this style
plt.style.use('fivethirtyeight')
# get rid of ugly gray background color
plt.rc_context({'axes.facecolor':'white'})
plt.rc_context({'axes.edgecolor':'white'})
# print(plt.rcParams)
# plt.style.use('seaborn')

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
    
    global settings, verbose, OS_TYPE

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

    try:
        PLATFORM_HOME = os.environ['HOME'] #sometimes this is failing
    except:
        PLATFORM_HOME = pathlib.Path.home()

    if OS_TYPE == is_linux:
        for search in settings["linux_search_sumo"]:
            checkPath = os.path.join(PLATFORM_HOME, search)
            if os.path.exists(checkPath):
                if verbose:
                    print("SUMO_HOME", checkPath)
                sys.path.append(os.path.join(checkPath, "tools"))
                break
    elif OS_TYPE == is_win:
        for search in settings["win_search_sumo"]:
            checkPath = os.path.join(PLATFORM_HOME, search)
            if os.path.exists(checkPath):
                if verbose:
                    print("SUMO_HOME", checkPath)
                sys.path.append(os.path.join(checkPath, "tools"))
                break

#---------------------------------------------------------------------------------------------------------------------------------------------
# load settings from json
#---------------------------------------------------------------------------------------------------------------------------------------------
def loadSettings():

    global work_dir_figs, work_dir_figs_lc, work_dir_figs_toc, work_dir, enables, scenariosFix, settings, scenarios
    global trafficDemand, driverBehaviourList, seedSize

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    workDirMain =  os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings", "plots.json")
   
    loadFileSettings(settingsDir)

    dir_figs =  os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["detectors"])
    if not os.path.exists(dir_figs):
        mkdir_p(dir_figs)

    dir_figs_lc =  os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["detectorslc"])
    if not os.path.exists(dir_figs_lc):
        mkdir_p(dir_figs_lc)
    
    dir_figs_toc =  os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["detectorsToC"])
    if not os.path.exists(dir_figs_toc):
        mkdir_p(dir_figs_toc)

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
        tmp = os.path.join(dir_figs, s)
        work_dir_figs[s] = tmp
        if not os.path.exists(tmp):
            mkdir_p(tmp)

    for s in scenarios:
        tmp = os.path.join(dir_figs_lc, s)
        work_dir_figs_lc[s] = tmp
        if not os.path.exists(tmp):
            mkdir_p(tmp)

    for s in scenarios:
        tmp = os.path.join(dir_figs_toc, s)
        work_dir_figs_toc[s] = tmp
        if not os.path.exists(tmp):
            mkdir_p(tmp)

    for s in scenarios:
        tmp = os.path.join(workDirMain, s)
        work_dir[s] = tmp
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
                for file in os.listdir(work_dir_figs[s]):
                    if file.endswith(".png") or file.endswith(".svg"):
                        path = os.path.join(work_dir_figs[s], file)
                        os.remove(path)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s/%s"%(count, settings["folder_names"]["detectors"], s))
                
                count = 0
                for file in os.listdir(work_dir_figs_lc[s]):
                    if file.endswith(".png") or file.endswith(".svg"):
                        path = os.path.join(work_dir_figs_lc[s], file)
                        os.remove(path)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s/%s"%(count, settings["folder_names"]["detectorslc"], s))

                count = 0
                for file in os.listdir(work_dir_figs_toc[s]):
                    if file.endswith(".png") or file.endswith(".svg"):
                        path = os.path.join(work_dir_figs_toc[s], file)
                        os.remove(path)
                        count+=1
                if count > 0: 
                    print("Cleared %s files from %s/%s"%(count, settings["folder_names"]["detectorsToC"], s))
        except:
            pass

#---------------------------------------------------------------------------------------------------------------------------------------------
def printDisabledFunctions():

    for s in scenarios:
        func_enable = settings["plot_detectors_list_" +  str(scenariosFix.index(s))]
        for enStr in func_enable:
            if func_enable[enStr] == 0:
                print("Warning :", enStr, "is disabled on", s)

#---------------------------------------------------------------------------------------------------------------------------------------------
def createFileNameXml(m_path, m_name, m_demand, m_mix_id, m_behaviour, m_seed):
    
    xml_name = [ m_name,
                traffic_Demand_Name, str(trafficDemand[m_demand]),
                trafic_Mix_Name, str(m_mix_id),
                driver_Behaviour_Name, m_behaviour,
                'seed', str(m_seed) ]

    t_path = os.path.join(m_path, '_'.join(xml_name)) + '.xml'

    return t_path

#---------------------------------------------------------------------------------------------------------------------------------------------
#
#---------------------------------------------------------------------------------------------------------------------------------------------
class DataList():
    def __init__(self):

        self.x_left = []
        self.x_right = []
        self.y_left = []
        self.y_right = []

    @property
    def xListLeft(self):
        return self.x_left

    @property
    def xListRight(self):
        return self.x_right

    @property
    def yListLeft(self):
        return self.y_left

    @property
    def yListRight(self):
        return self.y_right
    
    def addXLeft(self, value):
        self.x_left.append(value)

    def addYLeft(self, value):
        self.y_left.append(value)
   
    def addXRight(self, value):
        self.x_right.append(value)
   
    def addYRight(self, value):
        self.y_right.append(value)

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWork(path, scenario, sid, use_parallel, q):

    multiWorkSpeedFlow(path, scenario, sid, use_parallel, q)
    multiWork_LC(path, scenario, sid, use_parallel, q)
    multiWork_ToC(path, scenario, sid, use_parallel, q)

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWorkSpeedFlow(path, scenario, sid, use_parallel, q):
   
    global verbose, work_dir_figs, settings

    func_enable = settings["plot_detectors_list_" +  str(sid)]
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]
    scenario_legend = settings["scenarios_legends_dict"][scenario]

    if func_enable["Detectors_speed"] == 0 and func_enable["Detectors_flow"] == 0:
        return None

    picklePath = os.path.join(path, '%s_%s'%(input_data_file,scenario)) + '.pickle'

    if verbose:
        print(work_dir_figs[scenario])

    if os.path.exists(picklePath):
        try:
            pickle_in = open(picklePath,'rb')
            result = pickle.load(pickle_in)
            pickle_in.close()

        except:
            if use_parallel:
                q.put('Error: file : ' + picklePath + ' is corrupted')
            else:
                print('Error: file : ', picklePath, ' is corrupted')
        finally:
            for funcId, funcName in enumerate(func_enable):
                
                if (func_enable[funcName] and (funcName == "Detectors_speed" or funcName == "Detectors_flow")):

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

                                    colorMap = settings["plot_detectors_color_map"][funcName] #'jet_r'
                                    time_step_size, pos_step_size = result[0][td_id][tm_id][db_id][seed].shape  #funcId
                                    
                                    funckId = 0
                                    if funcName == "Detectors_flow":
                                        funckId = 1

                                    plt.imshow(result[funckId][td_id][tm_id][db_id][seed].T,
                                               origin = 'lower', 
                                               vmin = settings["plot_detectors_kpi_range"][funcName]['min'],
                                               vmax = settings["plot_detectors_kpi_range"][funcName]['max'],
                                               cmap = colorMap,
                                               aspect = time_step_size / float(pos_step_size),
                                               interpolation = 'bilinear') #bilinear gaussian


                                    # pdb.set_trace()
                                    titleStr = "%s (%s/%s/Seed %s)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name], str(seed))
                                    plt.title(titleStr, fontsize = settings["plot_detectors_fonts_size"][2], weight = 'bold')

                                    cbr = plt.colorbar(pad = settings["plot_detectors_right_bar_offset"])
                                    cbr.set_label(settings["plot_detectors_kpi_units"][funcName], fontsize = settings["plot_detectors_fonts_size"][1])
                                    cbr.ax.tick_params(labelsize = settings["plot_detectors_fonts_size"][0])

                                    plt.xlabel(settings["plot_detectors_labels"][0], fontsize = settings["plot_detectors_fonts_size"][1])
                                    plt.ylabel(settings["plot_detectors_labels"][1], fontsize = settings["plot_detectors_fonts_size"][1])

                                    # increase by one so we can have ticks at plot's edges as well
                                    time_step_size += 1
                                    pos_step_size += 1
                                    xticksPositions = [i for i in range(time_step_size)]
                                    xticksFullLabels = [int(i * settings["plot_detectors_time_step"] / 60) for i in range(time_step_size)]
                                    yticksPositions = [i for i in range(pos_step_size)]
                                    yticksFullLabels = [i * settings["plot_detectors_length_step"] / 1000 for i in range(pos_step_size)]
                                    
                                    plt.xticks(xticksPositions[0::2], xticksFullLabels[0::2], horizontalalignment = 'right', fontsize = settings["plot_detectors_fonts_size"][0])
                                    plt.yticks(yticksPositions[0::settings["plot_detectors_y_ticks_interval"]],
                                                                yticksFullLabels[0::settings["plot_detectors_y_ticks_interval"]], 
                                                                verticalalignment = 'top' , fontsize = settings["plot_detectors_fonts_size"][0])

                                    plt.grid(False)
                                    plt.tight_layout()

                                    fileName = funcName + "_" + td_name + '_' + settings["traffic_mix_path"][tm_name] + '_seed_' + str(seed) 
                                    savePath =  os.path.join(work_dir_figs[scenario], fileName) 

                                    if settings["plot_network_save_svg"] == "True":
                                        fig.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight', facecolor = 'white')

                                    plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight', facecolor = 'white')
                                    plt.close()

                                    if verbose:
                                        if use_parallel:
                                            q.put('Saving file:' + savePath)
                                        else:
                                            print('Saving file:', savePath)
    else:
        if use_parallel:
            q.put('Error: Could not find pickle file:' + picklePath)
        else:
            print('Error: Could not find pickle file:', picklePath)
    

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWork_LC(path, scenario, sid, use_parallel, q):
   
    global verbose, work_dir_figs_lc, settings

    func_enable = settings["plot_detectors_list_" +  str(sid)]

    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]
    scenario_legend = settings["scenarios_legends_dict"][scenario]

    if func_enable["Detectors_LC"] == 0:
        return None

    picklePath = os.path.join(path, '%s_%s'%(input_data_file,scenario)) + '.pickle'

    if verbose:
        print(work_dir_figs_lc[scenario])

    import sumolib

    if os.path.exists(picklePath):
        try:
            pickle_in = open(picklePath, 'rb')
            result = pickle.load(pickle_in)
            pickle_in.close()
        except:
            if use_parallel:
                q.put('Error: file : ' + picklePath + ' is corrupted')
            else:
                print('Error: file : ', picklePath, ' is corrupted')
        finally:
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
                            
                            fnStr = createFileNameXml(path, 'outputLaneChanges', td_name, tm_id, db_name, seed)

                            data_lc = DataList()

                            if os.path.exists(fnStr):
                                for cf in sumolib.output.parse(fnStr, 'change'):
                                            
                                    x = float(cf.x)
                                    t = float(cf.time)
                                    
                                    if t > 20.0 and t <= settings["plot_detectors_marker_limits"][0] and x <= settings["plot_detectors_marker_limits"][1]:
                                        try:
                                            if cf.dir == "-1":
                                                data_lc.addXRight(x / settings["plot_detectors_length_step"])
                                                data_lc.addYRight(t / settings["plot_detectors_time_step"])
                                            elif cf.dir == "1":
                                                data_lc.addXLeft(x / settings["plot_detectors_length_step"])
                                                data_lc.addYLeft(t / settings["plot_detectors_time_step"])
                                        except:
                                            print("Error:", cf)
                                            pass
                            else:
                                print("File not found:", fnStr)
                                continue

                            colorMap = colorMap = settings["plot_detectors_color_map"]["Detectors_LC"] #'jet_r'
                            time_step_size, pos_step_size = result[0][td_id][tm_id][db_id][seed].shape # 0 = funcId == "Detectors_speed"

                            # increase by one so we can have ticks at plot's edges as well
                            time_step_size += 1
                            pos_step_size += 1

                            extents = (0.0, time_step_size -1, 0.0, pos_step_size - 1)
                            plt.figure(num = scenario)
                            im = plt.imshow(result[0][td_id][tm_id][db_id][seed].T,
                                       origin = 'lower',
                                       vmin = settings["plot_detectors_kpi_range"]["Detectors_speed"]['min'],
                                       vmax = settings["plot_detectors_kpi_range"]["Detectors_speed"]['max'],
                                       cmap = colorMap,
                                       aspect = time_step_size / float(pos_step_size),
                                       extent = extents,
                                       interpolation = 'bilinear') #bilinear gaussian
                            
                            # pdb.set_trace()
                            titleStr = "%s (%s/%s/Seed %s)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name], str(seed))
                            plt.title(titleStr, fontsize = settings["plot_detectors_fonts_size"][2], weight = 'bold')

                            cbr = plt.colorbar(pad = settings["plot_detectors_right_bar_offset"])
                            cbr.set_label(settings["plot_detectors_kpi_units"]["Detectors_speed"], fontsize = settings["plot_detectors_fonts_size"][1])
                            cbr.ax.tick_params(labelsize = settings["plot_detectors_fonts_size"][0])

                            plt.xlabel(settings["plot_detectors_labels"][0], fontsize = settings["plot_detectors_fonts_size"][1])
                            plt.ylabel(settings["plot_detectors_labels"][1], fontsize = settings["plot_detectors_fonts_size"][1])

                            xticksPositions = [i for i in range(time_step_size)]
                            xticksFullLabels = [int(i * settings["plot_detectors_time_step"] / 60) for i in range(time_step_size)]
                            
                            yticksPositions = [i for i in range(pos_step_size)]
                            yticksFullLabels = [i * settings["plot_detectors_length_step"] / 1000 for i in range(pos_step_size)]
                            
                            plt.xticks(xticksPositions[0::2], xticksFullLabels[0::2], horizontalalignment = 'center', fontsize = settings["plot_detectors_fonts_size"][0])
                            plt.yticks(yticksPositions[0::settings["plot_detectors_y_ticks_interval"]],  yticksFullLabels[0::settings["plot_detectors_y_ticks_interval"]], 
                                                        verticalalignment = 'center' , fontsize = settings["plot_detectors_fonts_size"][0])

                            if data_lc:
                                plt.scatter(data_lc.yListRight, data_lc.xListRight, marker = settings["plot_detectors_marker_lc"][1], label = settings["plot_detectors_marker_labels_lc"][1], s = settings["plot_detectors_marker_size"], c = settings["plot_detectors_marker_color_lc"][1])
                                plt.scatter(data_lc.yListLeft,  data_lc.xListLeft,  marker = settings["plot_detectors_marker_lc"][0], label = settings["plot_detectors_marker_labels_lc"][0], s = settings["plot_detectors_marker_size"], c = settings["plot_detectors_marker_color_lc"][0])                                
                                plt.legend(borderaxespad = 0., loc = 'center', bbox_to_anchor = tuple(settings["plot_detectors_marker_anchor_lc"]),  ncol = 2, mode = "expand", frameon = False, prop = dict(size = settings["plot_detectors_marker_legend_font_size_toc"]))

                            plt.grid(False)
                            #plt.tight_layout()

                            fileName = "Detectors_speed_lc" + "_" + td_name + '_' + settings["traffic_mix_path"][tm_name] + '_seed_' + str(seed)
                            savePath =  os.path.join(work_dir_figs_lc[scenario], fileName)

                            if settings["plot_network_save_svg"] == "True":
                                plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                            plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                            plt.close()

                            ####################################################################################33

                            if verbose:
                                if use_parallel:
                                    q.put('Saving file:' + savePath)
                                else:
                                    print('Saving file:', savePath)
    else:
        if use_parallel:
            q.put('Error: Could not find pickle file:' + picklePath)
        else:
            print('Error: Could not find pickle file:', picklePath)

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWork_ToC(path, scenario, sid, use_parallel, q):
   
    global verbose, work_dir_figs_toc, settings

    func_enable = settings["plot_detectors_list_" +  str(sid)]
    trafficMix = settings["traffic_mix_" + str(sid)]
    seedEnables = settings["seed_" + str(sid)]
    
    scenario_legend = settings["scenarios_legends_dict"][scenario]

    if func_enable["Detectors_ToC"] == 0:
        return None

    picklePath = os.path.join(path, '%s_%s'%(input_data_file, scenario)) + '.pickle'

    if verbose:
        print(work_dir_figs_toc[scenario])

    event_Tocs = settings["plot_detectors_marker_labels_toc"]

    import sumolib

    plt.figure(num = scenario)

    if os.path.exists(picklePath):
        try:
            pickle_in = open(picklePath, 'rb')
            result = pickle.load(pickle_in)
            pickle_in.close()
        except:
            if use_parallel:
                q.put('Error: file : ' + picklePath + ' is corrupted')
            else:
                print('Error: file : ', picklePath, ' is corrupted')
        finally:

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
                            
                            fnStr = createFileNameXml(path, 'output', td_name, tm_id, db_name, seed)

                            data = {}

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
                                    for idEvent in event_Tocs:
                                      
                                        edge = root.findall(idEvent)
                                        
                                        if edge:
                                            data_temp = DataList()
                                            for cf in edge:
                                            
                                                x = float(cf.get("x"))
                                                t = float(cf.get("t"))
                                                
                                                #if idEvent == "TOR" and td_id == 1 and tm_id == 0 and cf.get("lane") == "approach2_0":
                                                #    print("TOR event: " , cf.get("id"), cf.get("lane"), " xpos =", x, " time =", t)
                                                #
                                                #if idEvent == "ToCup" and td_id == 1 and tm_id == 0 and cf.get("lane") == "approach2_0":
                                                #    print("ToCup event: " , cf.get("id"), cf.get("lane"), " xpos =", x, " time =", t)

                                                if t > 20.0 and t <= settings["plot_detectors_marker_limits"][0] and x <= settings["plot_detectors_marker_limits"][1]:
                                                    try:
                                                        data_temp.addXRight(x / settings["plot_detectors_length_step"])
                                                        data_temp.addYRight(t / settings["plot_detectors_time_step"])
                                                    except:
                                                        print("Error:", cf)
                                                        pass
                                                    
                                            data[idEvent] = data_temp
                            else:
                                print("File not found:", fnStr)
                                continue
                            
                            if not data:
                                print("Data array is empty:", fnStr)
                                continue

                            colorMap = colorMap = colorMap = settings["plot_detectors_color_map"]["Detectors_LC"] # 'jet_r'
                            time_step_size, pos_step_size = result[0][td_id][tm_id][db_id][seed].shape # 0 = funcId == "Detectors_speed"

                            # increase by one so we can have ticks at plot's edges as well
                            time_step_size += 1
                            pos_step_size += 1

                            extents = (0.0, time_step_size -1, 0.0, pos_step_size - 1)
                            
                            im = plt.imshow(result[0][td_id][tm_id][db_id][seed].T,
                                       origin = 'lower',
                                       vmin = settings["plot_detectors_kpi_range"]["Detectors_speed"]['min'],
                                       vmax = settings["plot_detectors_kpi_range"]["Detectors_speed"]['max'],
                                       cmap = colorMap,
                                       aspect = time_step_size / float(pos_step_size),
                                       extent = extents,
                                       interpolation = 'bilinear') #bilinear gaussian
                            
                            # pdb.set_trace()
                            titleStr = "%s (%s/%s/Seed %s)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name], str(seed))
                            plt.title(titleStr, fontsize = settings["plot_detectors_fonts_size"][2], weight = 'bold')

                            cbr = plt.colorbar(pad = settings["plot_detectors_right_bar_offset"])
                            cbr.set_label(settings["plot_detectors_kpi_units"]["Detectors_speed"], fontsize = settings["plot_detectors_fonts_size"][1])
                            cbr.ax.tick_params(labelsize = settings["plot_detectors_fonts_size"][0])

                            plt.xlabel(settings["plot_detectors_labels"][0], fontsize = settings["plot_detectors_fonts_size"][1])
                            plt.ylabel(settings["plot_detectors_labels"][1], fontsize = settings["plot_detectors_fonts_size"][1])

                            xticksPositions = [i for i in range(time_step_size)]
                            xticksFullLabels = [int(i * settings["plot_detectors_time_step"] / 60) for i in range(time_step_size)]
                            
                            yticksPositions = [i for i in range(pos_step_size)]
                            yticksFullLabels = [i * settings["plot_detectors_length_step"] / 1000 for i in range(pos_step_size)]
                            
                            plt.xticks(xticksPositions[0::2], xticksFullLabels[0::2], horizontalalignment = 'center', fontsize = settings["plot_detectors_fonts_size"][0])
                            plt.yticks(yticksPositions[0::settings["plot_detectors_y_ticks_interval"]],  yticksFullLabels[0::settings["plot_detectors_y_ticks_interval"]], 
                                                        verticalalignment = 'center' , fontsize = settings["plot_detectors_fonts_size"][0])
                            
                            #https://matplotlib.org/3.1.3/api/markers_api.html

                            count_tocs = 0
                            for id_n, idEvent in enumerate(data):
                                plt.scatter(data[idEvent].yListRight, data[idEvent].xListRight, marker = settings["plot_detectors_marker_toc"][id_n], label = idEvent, s = settings["plot_detectors_marker_size"], c = settings["plot_detectors_marker_color_toc"][id_n])
                                count_tocs+=1
                           
                            plt.legend(borderaxespad = 0., handletextpad = 0.1, loc = 'center', bbox_to_anchor = tuple(settings["plot_detectors_marker_anchor_toc"]), ncol = count_tocs, mode = "expand", frameon = False, prop = dict(size = settings["plot_detectors_marker_legend_font_size_toc"]))

                            plt.grid(False)
                            plt.tight_layout()

                            fileName = "Detectors_speed_toc" + "_" + td_name + '_' + settings["traffic_mix_path"][tm_name] + '_seed_' + str(seed)
                            savePath =  os.path.join(work_dir_figs_toc[scenario], fileName)

                            if settings["plot_network_save_svg"] == "True":
                                plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                            
                            plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                            plt.close()

                            ####################################################################################33

                            if verbose:
                                if use_parallel:
                                    q.put('Saving file:' + savePath)
                                else:
                                    print('Saving file:', savePath)
    else:
        if use_parallel:
            q.put('Error: Could not find pickle file:' + picklePath)
        else:
            print('Error: Could not find pickle file:', picklePath)
 
#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data detectors plotting proccess started")
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
                temp = mp.Process(target = multiWork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q, ))
            if use_threads:
                temp = mt.Thread(target = multiWork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, q, ))

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

    if use_parallel:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time()- startTime
    print("\nData detectors plotting process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
