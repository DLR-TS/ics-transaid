#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# Date    2020/01/01
# Author  Yun-Pang Floetteroed
# Author  Robert Alms
# Author  Vasilios Karagounis

import os, sys, errno
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
from pandas import DataFrame
import numpy as np
import pylab

import time
import pathlib
import json
import warnings

from sys import platform as _platform
import multiprocessing as mp
import threading as mt
import queue

seperator = "---------------------------------------------------------------------"

scenarios = []
scenariosFix = []

trafficDemand = {}
driverBehaviourList = []

enables = []
settings = {}
work_dir = {}

workDirMain = ""
work_dir_figs = {}
dir_figs = ""

seedSize = 0

traffic_Demand_Name  = "TD"
trafic_Mix_Name  = "TM"
driver_Behaviour_Name  = "DB"

verbose = False
use_parallel = False
use_threads = False

ucXLim = []  #Total length of network [0, 1100]
ucYLim = [] #Total height of network [54.7, 59.7]

ucTimeLim = []  #Total time of network [0, 3600]

pylab.subplots_adjust(hspace = 0.000)

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

    global workDirMain, work_dir_figs, work_dir, dir_figs, enables, scenariosFix, settings, scenarios
    global trafficDemand, driverBehaviourList, ucXLim, ucYLim, ucTimeLim, seedSize

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    workDirMain =  os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings", "plots.json")
    
    loadFileSettings(settingsDir)

    dir_figs =  os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["lc"])
    if not os.path.exists(dir_figs):
        mkdir_p(dir_figs)

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

    ucXLim = settings["plot_lc_locations_x_lim"]
    ucYLim = settings["plot_lc_locations_y_lim"]
    ucTimeLim = settings["plot_lc_locations_time_lim"]

#---------------------------------------------------------------------------------------------------------------------------------------------
def clearFiles():

    for s in scenarios:
        count = 0
        for file in os.listdir(work_dir_figs[s]):
            if file.endswith(".png") or file.endswith(".svg"):
                path = os.path.join(work_dir_figs[s], file)
                os.remove(path)
                count+=1
        if count > 0: 
            print("Cleared %s files from %s"%(count, s))

    path_csv = str("%s/lane_changes_statistics.csv"%dir_figs)
    if os.path.exists(path_csv):
        os.remove(path_csv)

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
    def __init__(self, correction = [], handleTime = False):

        self.x_left = []
        self.x_right = []
        self.y_left = []
        self.y_right = []

        self.x_List = []
        self.y_List = []

        self.y_left_correction = correction[0]
        self.y_right_correction = correction[1]

    def addLists(self, check_exclude, q):
        self.x_List = self.x_left + self.x_right
        self.y_List = self.y_left + self.y_right

        if check_exclude:
            xMin = xMax = 0.0
            if handleTime:
                xMin = float(ucTimeLim[0])
                xMax = float(ucTimeLim[1])
            else:
                xMin = float(ucXLim[0])
                xMax = float(ucXLim[1])
          
            if verbose and self.x_List and (max(self.x_List) > xMax or 
                          min(self.x_List) < xMin or 
                          max(self.y_List) > float(ucYLim[1]) or 
                          min(self.y_List) < float(ucYLim[0])):
                print("**** Abnormal values in the SSM-files when LOS")   
           
            for index, item in enumerate(self.y_List):
                if item < ucYLim[0] or item > ucYLim[1]:
                    self.x_List.pop(index)
                    self.y_List.pop(index)
                    #print("removing y item at index:", index)

    @property
    def xList(self):
        return self.x_List

    @property
    def yList(self):
        return self.y_List

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
        self.y_left.append(value + self.y_left_correction)
   
    def addXRight(self, value):
        self.x_right.append(value)
   
    def addYRight(self, value):
        self.y_right.append(value + self.y_right_correction)
    

#---------------------------------------------------------------------------------------------------------------------------------------------
def multiWork(path, scenario, sid, use_parallel, qArray, q):
   
    global verbose, work_dir_figs, settings

    func_enable = settings["plot_lc_locations_kpi_list"]

    trafficMix = settings["traffic_mix_" + str(sid)]
    trafficDemand = settings["level_Of_service"]
    driverBehaviourList = settings["schemes"]

    seedEnables = settings["seed_" + str(sid)]

    trafficMixTitles = settings["traffic_mix_titles"]
    dictLOSTitles = settings["level_Of_service_titles"]
    scenario_legend = settings["scenarios_legends_dict"][scenario]

    import sumolib

    df = DataFrame()
    dfSeries = DataFrame()

    for td_name in trafficDemand:
        for tm_id, tm_name in enumerate(trafficMix):
            for db_name in driverBehaviourList:

                if verbose:
                    print("Scenario:", scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)

                for seed in range(seedSize):

                    if seedEnables[td_name][tm_name][seed] == 0:
                        if (use_parallel):
                            q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                        else:
                            print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                        continue

                    fnStr = createFileNameXml(work_dir[scenario], 'outputLaneChanges', td_name, tm_id, db_name, seed)

                    if os.path.exists(fnStr):
                        
                        with warnings.catch_warnings():
                            warnings.simplefilter("ignore", category = RuntimeWarning)

                            if func_enable["LaneChanges"] == 1:
                                plt.figure(num = "LaneChanges", figsize = tuple(settings["plot_lc_locations_figs_size_lanechanges"]))

                            if func_enable["Frequency"] == 1:
                                plt.figure(num = "Frequency", figsize = tuple(settings["plot_lc_locations_figs_size_frequency"]))

                            if func_enable["Spatio"] == 1:
                                plt.figure(num = "Spatio", figsize = tuple(settings["plot_lc_locations_figs_size_spatio"]))

                            if func_enable["TimeSeries"] == 1:
                                plt.figure(num = "TimeSeries", figsize = tuple(settings["plot_lc_locations_figs_size_timeseries"]))

                            if func_enable["TimeSeriesFrequency"] == 1:
                                plt.figure(num = "TimeSeriesFrequency", figsize = tuple(settings["plot_lc_locations_figs_size_timeseriesfrequency"]))

                        data_lc = DataList(settings["plot_lc_locations_y_correction_lc"])
                        data_spatio = DataList([0,0])
                        data_series = DataList(settings["plot_lc_locations_y_correction_lc"], True)

                        numP = seed + 1
                        
                        #Detectors_speed_lc_los_B_mix_1_seed_0

                        labeltext = "%s_%s_%s_seed_%s" %(scenario, dictLOSTitles[td_name], trafficMixTitles[tm_name], seed)
                        labeltextNoSuffix = "%s (%s / %s)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name])
                        labeltextNoSuffix2 = "%s (%s / %s / Seed %s)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name], seed)

                        for cf in sumolib.output.parse(fnStr, 'change'):
                            
                            if func_enable["LaneChanges"] == 1 or func_enable["Frequency"] == 1 or func_enable["Spatio"] == 1 or func_enable["TimeSeries"] == 1 or func_enable["TimeSeriesFrequency"] == 1:
                                
                                if verbose: 
                                    print(cf)
                                
                                x = float(cf.x)
                                y = float(cf.y)
                                t = float(cf.time)

                                try:
                                    if func_enable["LaneChanges"] == 1 or func_enable["Frequency"] == 1:
                                        if cf.dir == "-1":
                                            data_lc.addXRight(x)
                                            data_lc.addYRight(y)
                                        elif cf.dir == "1":
                                            data_lc.addXLeft(x)
                                            data_lc.addYLeft(y)

                                    if func_enable["Spatio"] == 1:
                                        if (x >= settings["plot_lc_locations_x_lim"][0] and x <= settings["plot_lc_locations_x_lim"][1] and
                                            t >= settings["plot_lc_locations_time_lim"][0] and t <= settings["plot_lc_locations_time_lim"][1]):
                                            if cf.dir == "-1":
                                                data_spatio.addXRight(x)
                                                data_spatio.addYRight(t)
                                            elif cf.dir == "1":
                                                data_spatio.addXLeft(x)
                                                data_spatio.addYLeft(t)

                                    if func_enable["TimeSeries"] == 1 or func_enable["TimeSeriesFrequency"] == 1:
                                        if t <= settings["plot_lc_locations_time_lim"][1]: #3600.0:
                                            if cf.dir == "-1":
                                                data_series.addXRight(t)
                                                data_series.addYRight(y)
                                            elif cf.dir == "1":
                                                data_series.addXLeft(t)
                                                data_series.addYLeft(y)
                                except:
                                    print("Error:", cf)
                                    pass

                        if func_enable["LaneChanges"] == 1 or func_enable["Frequency"] == 1:
                            data_lc.addLists(bool(settings["plot_lc_locations_exclude_abnormal_values"] == "True"), q)
                            qArray.put("    seed:%s    number of lane changes events: %s\n" %(seed, len(data_lc.xList)))

                        if func_enable["TimeSeries"] == 1 or func_enable["TimeSeriesFrequency"] == 1:
                            data_series.addLists(bool(settings["plot_lc_locations_exclude_abnormal_values"] == "True"), q)

                        if func_enable["LaneChanges"] == 1 or func_enable["Frequency"] == 1:

                            if func_enable["LaneChanges"] == 1:
                                plt.figure("LaneChanges")

                                ax1 = pylab.subplot(seedSize, 1, numP)

                                ax1.scatter([], [], marker = " ", label = labeltext, s = settings["plot_lc_locations_font_size"][0])
                                ax1.scatter(data_lc.xListLeft,  data_lc.yListLeft,  marker = settings["plot_lc_locations_marker"][0], label = settings["plot_lc_locations_text"][0], s = settings["plot_lc_locations_font_size"][0], c = "red")
                                ax1.scatter(data_lc.xListRight, data_lc.yListRight, marker = settings["plot_lc_locations_marker"][1], label = settings["plot_lc_locations_text"][1], s = settings["plot_lc_locations_font_size"][0], c = "blue")

                                if settings["plot_lc_locations_lim_method"] == "Normal":
                                    plt.xticks(np.arange(ucXLim[0], ucXLim[1], step = settings["plot_lc_locations_step_length"]), fontsize = settings["plot_lc_locations_font_size"][0])
                                    plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_lc_locations_step_height"]), fontsize = settings["plot_lc_locations_font_size"][0])
                                elif settings["plot_lc_locations_lim_method"] == "UseTicks":
                                    ax1.set_xlim(ucXLim)
                                    ax1.set_ylim(ucYLim)
                                    plt.xticks(np.arange(ucXLim[0], ucXLim[1], step = settings["plot_lc_locations_step_length"]), fontsize = settings["plot_lc_locations_font_size"][0])
                                    plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_lc_locations_step_height"]), fontsize = settings["plot_lc_locations_font_size"][0])
                                    ax1.vlines(x = settings["plot_lc_locations_edge_ticks"], ymin = ucYLim[0], ymax = ucYLim[1], lw = 2, color = 'r')
                                elif settings["plot_lc_locations_lim_method"] == "LimitsOnly":
                                    ax1.set_xlim(ucXLim)
                                    ax1.set_ylim(ucYLim)
                                else:
                                    print("error: please adapt x- and y-limits to your use case!")

                                ax1.legend(loc = 'lower left', prop = {'size':settings["plot_lc_locations_font_size"][0]})
                                ax1.grid(True)

                                if numP == seedSize:
                                    ax1.set_xlabel(settings["plot_lc_locations_time_frequency_labels"][0], fontsize = settings["plot_lc_locations_font_size"][0])
                                    plt.setp(ax1.get_xticklabels(), visible = True, fontsize = settings["plot_lc_locations_font_size"][0])
                                else:
                                    ax1.set_xticklabels([])
                                    plt.setp(ax1.get_xticklabels(), visible = False, fontsize = settings["plot_lc_locations_font_size"][0])

                            labels = ['seed_%s'%(seed)] * len(data_lc.xList)

                            data = DataFrame(dict(x = data_lc.xList, y = data_lc.yList, label = labels))
                            df = df.append(data, ignore_index = True)

                        if func_enable["TimeSeries"] == 1 or func_enable["TimeSeriesFrequency"] == 1:

                            if func_enable["TimeSeries"] == 1:
                                plt.figure("TimeSeries")

                                ax2 = pylab.subplot(seedSize, 1, numP)

                                #ax2.scatter([], [], marker = " ", label = labeltext, s = settings["plot_lc_locations_font_size"][0])
                                #ax2.scatter(data_series.xListLeft,  data_series.yListLeft,  marker = "x", label = "Left Lane Change", s = settings["plot_lc_locations_font_size"][0], c = "red")
                                #ax2.scatter(data_series.xListRight, data_series.yListRight, marker = ".", label = "Right Lane Change", s = settings["plot_lc_locations_font_size"][0], c = "blue")
                                #plt.legend(handles = [line, sine, arcsine], labels  = ['Line', 'Sine', 'Arcsine'])
                                #plt.legend(labels = ["Left Lane Change", 'Right Lane Change'])

                                nbins = [settings["plot_lc_locations_timeseries_values"][0], settings["plot_lc_locations_timeseries_values"][1]] # set number of bin to your specific uc
                                # 2D Histogram
                                my_cmap = plt.cm.jet
                                my_cmap.set_under('w', 1)

                                ax2.hist2d(data_series.xList, data_series.yList, bins = nbins, cmap = my_cmap, vmin = settings["plot_lc_locations_timeseries_values"][2], vmax = settings["plot_lc_locations_timeseries_values"][3])

                                if settings["plot_lc_locations_lim_method"] == "Normal":
                                    plt.xticks(np.arange(ucTimeLim[0], ucTimeLim[1], step = settings["plot_lc_locations_timeseries_values"][4]), fontsize = settings["plot_lc_locations_font_size"][0])
                                    plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_lc_locations_timeseries_values"][5]), fontsize = settings["plot_lc_locations_font_size"][0])
                                elif settings["plot_lc_locations_lim_method"] == "UseTicks":
                                    ax2.set_xlim(ucTimeLim)
                                    ax2.set_ylim(ucYLim)
                                    plt.xticks(np.arange(ucTimeLim[0], ucTimeLim[1], step = settings["plot_lc_locations_timeseries_values"][4]), fontsize = settings["plot_lc_locations_font_size"][0])
                                    plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_lc_locations_timeseries_values"][5]), fontsize = settings["plot_lc_locations_font_size"][0])
                                    ax2.vlines(x = settings["plot_lc_locations_edge_ticks"], ymin = ucYLim[0], ymax = ucYLim[1], lw = 2, color = 'r')
                                elif settings["plot_lc_locations_lim_method"] == "LimitsOnly":
                                    ax2.set_xlim(ucTimeLim)
                                    ax2.set_ylim(ucYLim)
                                else:
                                    print("error: please adapt x- and y-limits to your use case!")

                                ax2.grid(True)

                                if numP == seedSize:
                                    ax2.set_xlabel(settings["plot_lc_locations_time_frequency_labels"][0], fontsize = settings["plot_lc_locations_font_size"][0])
                                    plt.setp(ax2.get_xticklabels(), visible = True, fontsize = settings["plot_lc_locations_font_size"][0])
                                else:
                                    ax2.set_xticklabels([])
                                    plt.setp(ax2.get_xticklabels(), visible = False, fontsize = settings["plot_lc_locations_font_size"][0])

                            labels = ['seed_%s'%(seed)] * len(data_series.xList)

                            data = DataFrame(dict(x = data_series.xList, y = data_series.yList, label = labels))
                            dfSeries = df.append(data, ignore_index = True)

                        if func_enable["Spatio"] == 1:
                            plt.figure("Spatio")

                            plt.xticks(fontsize = settings["plot_lc_locations_font_size"][2])
                            plt.yticks(fontsize = settings["plot_lc_locations_font_size"][2])

                            plt.scatter(data_spatio.yListLeft,  data_spatio.xListLeft,  marker = settings["plot_lc_locations_marker"][0], label = settings["plot_lc_locations_text"][0], s = settings["plot_lc_locations_font_size"][2], c = "red")
                            plt.scatter(data_spatio.yListRight, data_spatio.xListRight, marker = settings["plot_lc_locations_marker"][1], label = settings["plot_lc_locations_text"][1], s = settings["plot_lc_locations_font_size"][2], c = "blue")

                            plt.xlabel(settings["plot_lc_locations_spatio_labels"][0], fontsize = settings["plot_lc_locations_font_size"][3])
                            plt.ylabel(settings["plot_lc_locations_spatio_labels"][1], fontsize = settings["plot_lc_locations_font_size"][3])

                            plt.legend(loc = settings["plot_lc_spatio_legend_loc"], markerscale = settings["plot_lc_spatio_legend_scale_prop"][0], prop = {'size': settings["plot_lc_spatio_legend_scale_prop"][1]})
                            plt.title(labeltextNoSuffix2, fontsize = settings["plot_lc_locations_font_size"][4], weight = 'bold')

                            savePath = os.path.join(work_dir_figs[scenario], 'Lane_changes_spatio_%s_%s_seed_%s' %(td_name, settings["traffic_mix_path"][tm_name], seed))
                            
                            if settings["plot_network_save_svg"] == "True":
                                plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                            plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                            plt.close()
                            
                            if verbose:
                                if use_parallel:
                                    q.put('Saving file:' + savePath)
                                else:
                                    print('Saving file:', savePath)

            ###############################################################################3        
            if func_enable["LaneChanges"] == 1:
                
                plt.figure("LaneChanges")
                
                plt.title(labeltextNoSuffix, fontsize = settings["plot_lc_locations_font_size"][4], weight = 'bold')
                
                savePath = os.path.join(work_dir_figs[scenario], 'Lane_changes_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))

                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                plt.close()

                if verbose:
                    if use_parallel:
                        q.put('Saving file:' + savePath)
                    else:
                        print('Saving file:', savePath)
            ###############################################################################3        
            if func_enable["Frequency"] == 1:
                
                plt.figure("Frequency")

                x = df['x']
                y = df['y']
                nbins = [settings["plot_lc_locations_frequency_values"][0], settings["plot_lc_locations_frequency_values"][1]] # set number of bin to your specific uc
                # 2D Histogram
                my_cmap = plt.cm.jet
                my_cmap.set_under('w', 1)
                
                 #set vmax to your specific UC for the maximum value of the colorbar !!!
                h = plt.hist2d(x, y, bins = nbins, cmap = my_cmap, vmin = settings["plot_lc_locations_frequency_values"][2], vmax = settings["plot_lc_locations_frequency_values"][3])

                if verbose:
                    print('local maximum in hist: ', max(h[3]._A))
                
                max_norm = []
                max_norm.append(max(h[3]._A))

                cb = plt.colorbar(h[3], pad = settings["plot_lc_locations_right_bar_offset"])
                cb.set_label(label = settings["plot_lc_locations_frequency_labels"][2], size = settings["plot_lc_locations_font_size"][2])
                cb.ax.tick_params(labelsize = settings["plot_lc_locations_font_size"][2])

                plt.xticks(np.arange(ucXLim[0], ucXLim[1], step = settings["plot_lc_locations_frequency_values"][4]), fontsize = settings["plot_lc_locations_font_size"][2])
                plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_lc_locations_frequency_values"][5]), fontsize = settings["plot_lc_locations_font_size"][2])

                plt.xlabel(settings["plot_lc_locations_frequency_labels"][0], fontsize = settings["plot_lc_locations_font_size"][3])
                plt.ylabel(settings["plot_lc_locations_frequency_labels"][1], fontsize = settings["plot_lc_locations_font_size"][3])

                plt.title(labeltextNoSuffix, fontsize = settings["plot_lc_locations_font_size"][4], weight = 'bold')
                plt.grid(True)

                savePath = os.path.join(work_dir_figs[scenario], 'Lane_changes_frequency_histogram_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))

                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                plt.close()

                if verbose:
                    if use_parallel:
                        q.put('Saving file:' + savePath)
                    else:
                        print('Saving file:', savePath)
            
            ###############################################################################3        
            if func_enable["TimeSeries"] == 1:
                
                plt.figure("TimeSeries")
                plt.suptitle(labeltextNoSuffix, fontsize = settings["plot_lc_locations_font_size"][4], weight = 'bold')
                
                savePath = os.path.join(work_dir_figs[scenario], 'Lane_changes_time_series_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))
                
                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                plt.close()

                if verbose:
                    if use_parallel:
                        q.put('Saving file:' + savePath)
                    else:
                        print('Saving file:', savePath)
            
            ###############################################################################3        
            if func_enable["TimeSeriesFrequency"] == 1:
                
                plt.figure("TimeSeriesFrequency")

                x = dfSeries['x']
                y = dfSeries['y']
                nbins = [settings["plot_lc_locations_timeseriesfrequency_values"][0], settings["plot_lc_locations_timeseriesfrequency_values"][1]] # set number of bin to your specific uc
                # 2D Histogram
                my_cmap = plt.cm.jet
                my_cmap.set_under('w',1)
                
                 #set vmax to your specific UC for the maximum value of the colorbar !!!
                h = plt.hist2d(x, y, bins = nbins, cmap = my_cmap, vmin = settings["plot_lc_locations_timeseriesfrequency_values"][2], vmax = settings["plot_lc_locations_timeseriesfrequency_values"][3])

                if verbose:
                    print('local maximum in hist: ', max(h[3]._A))
                
                max_norm = []
                max_norm.append(max(h[3]._A))

                cb = plt.colorbar(h[3], pad = settings["plot_lc_locations_right_bar_offset"])
                cb.set_label(label = settings["plot_lc_locations_cb_label"], size = settings["plot_lc_locations_font_size"][2])
                cb.ax.tick_params(labelsize = settings["plot_lc_locations_font_size"][2])

                plt.xticks(np.arange(ucTimeLim[0], ucTimeLim[1], step = settings["plot_lc_locations_timeseriesfrequency_values"][4]), fontsize = settings["plot_lc_locations_font_size"][2])
                plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_lc_locations_timeseriesfrequency_values"][5]), fontsize = settings["plot_lc_locations_font_size"][2])

                plt.xlabel(settings["plot_lc_locations_time_frequency_labels"][0], fontsize = settings["plot_lc_locations_font_size"][3])
                plt.ylabel(settings["plot_lc_locations_time_frequency_labels"][1], fontsize = settings["plot_lc_locations_font_size"][3])

                plt.title(labeltextNoSuffix, fontsize = settings["plot_lc_locations_font_size"][4], weight = 'bold')
                plt.grid(True)

                savePath = os.path.join(work_dir_figs[scenario], 'Lane_changes_time_series_frequency_histogram_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))

                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                plt.close()

                if verbose:
                    if use_parallel:
                        q.put('Saving file:' + savePath)
                    else:
                        print('Saving file:', savePath)

            df.drop(df.index, inplace = True)
            dfSeries.drop(dfSeries.index, inplace = True)

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data ploting lane change locations plotting proccess started")
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

    qArray = {}

    for s in scenarios:
       if use_parallel:
           qArray[s] = mp.Queue()
       elif use_threads:
           qArray[s] = queue.Queue()
       else:
           qArray[s] = queue.Queue()

    if use_parallel:
        jobs = []
        
        print("\n%s"%seperator)
        clearFiles()

        print(seperator)
        print("Please wait...")

        for s in scenarios:
            if use_parallel:
                temp = mp.Process(target = multiWork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, qArray[s], q, ))
            if use_threads:
                temp = mt.Thread(target = multiWork, args=(work_dir[s], s, scenariosFix.index(s), use_parallel or use_threads, qArray[s], q, ))

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
            
            multiWork(work_dir[s], s, scenariosFix.index(s), False, qArray[s], q)
            
            print("\nScenario", s, "finished")
            print(seperator)

    outf = open("%s/lane_changes_statistics.csv"%dir_figs, 'w')
    for s in scenarios:
        while not qArray[s].empty():
            outf.write(qArray[s].get())
    
    outf.close()

    if use_parallel or use_threads:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time() - startTime
    print("Data ploting lane change locations process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
