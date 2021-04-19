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

"""
- plot the location of the event location with TTC less than 3 sec
- calculate the average of critical events with TTC less than 3 sec for each mix.
"""

import os, sys, errno
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import pandas as pd
from pandas import DataFrame

import numpy as np

from scipy.stats import gaussian_kde
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

traffic_Demand_Name = "TD"
trafic_Mix_Name = "TM"
driver_Behaviour_Name = "DB"

verbose = False
use_parallel = False
use_threads = False

ucXLim = []  #Total length of network [0, 1100]
ucYLim = [] #Total height of network [54.7, 59.7]

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
    global ucXLim, ucYLim, seedSize, trafficDemand, driverBehaviourList

    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))

    workDirMain = os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings", "plots.json")

    loadFileSettings(settingsDir)

    dir_figs = os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["ttc"])
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
        customFolder = "less_than_" + str(settings["plot_ttc_locations_max_seconds"]) + "_sec"
        tmp = os.path.join(dir_figs, s, customFolder)
        work_dir_figs[s] = tmp
        if not os.path.exists(tmp):
            mkdir_p(tmp)

    for s in scenarios:
        tmp = os.path.join(workDirMain, s)
        work_dir[s] = tmp

    if settings["fast_debug"] == "True":
        seedSize = 1
    else: 
        seedSize = settings["seed_size"]

    trafficDemand = settings["level_Of_service"]
    driverBehaviourList = settings["schemes"]

    ucXLim = settings["plot_ttc_locations_x_lim"]
    ucYLim = settings["plot_ttc_locations_y_lim"]

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
                    print("Cleared %s files from %s"%(count, s))

            path_csv = str("%s/critical_plot_ttc_statistics.csv"%dir_figs)

            if os.path.exists(path_csv):
                os.remove(path_csv)
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
def multiWork(path, scenario, sid, use_parallel, qArray, q):
   
    global verbose, work_dir_figs, settings

    func_enable = settings["plot_ttc_locations_kpi_list"]

    trafficMix = settings["traffic_mix_" + str(sid)]
    trafficMixTitles = settings["traffic_mix_titles"]
    seedEnables = settings["seed_" + str(sid)]
    
    dictLOSTitles = settings["level_Of_service_titles"]   
    scenario_legend = settings["scenarios_legends_dict"][scenario]

    import sumolib

    ttc = {"los_B":{"mix_0":[], "mix_1":[], "mix_2":[]},
           "los_C":{"mix_0":[], "mix_1":[], "mix_2":[]},
           "los_D":{"mix_0":[], "mix_1":[], "mix_2":[]}}

    #ttcVariance = {"los_A":{"mix_0":[], "mix_1":[], "mix_2":[]},
    #           "los_B":{"mix_0":[], "mix_1":[], "mix_2":[]},
    #           "los_C":{"mix_0":[], "mix_1":[], "mix_2":[]}}

    df = DataFrame()
   
    for td_name in trafficDemand:
        for tm_id, tm_name in enumerate(trafficMix):
            for db_name in driverBehaviourList:

                if verbose:
                    print("Scenario:", scenario, ': Traffic demand:', td_name, 'Traffic mix:', tm_name, 'Behaviour:', db_name)
                
                countList = []
                conflictsSum = {'LV-LV':[], 'LV-CV':[], 'LV-CAV':[], 'CV-CAV':[], 'CV-CV':[], 'CAV-CAV':[] }
                conflicts = {'LV-LV':0, 'LV-CV':0, 'LV-CAV':0, 'CV-CAV':0, 'CV-CV':0, 'CAV-CAV':0 }

                for seed in range(seedSize):
                    if seedEnables[td_name][tm_name][seed] == 0:
                        if (use_parallel):
                            q.put('Disabled seed' + str(seed) + 'at:' + tm_name + "," + td_name + '\n')
                        else:
                            print('Disabled seed', seed,  'at:', tm_name, ",", td_name)
                        continue
                    
                    fnStr = createFileNameXml(path, 'outputSSM', td_name, tm_id, db_name, seed)

                    with warnings.catch_warnings():
                        warnings.simplefilter("ignore", category = RuntimeWarning)

                        if func_enable["Critical"] == 1:
                            plt.figure(num = "Critical", figsize = tuple(settings["plot_ttc_locations_figs_size_critical"]))

                        if func_enable["Density"] == 1:
                            plt.figure(num = "Density", figsize = tuple(settings["plot_ttc_locations_figs_size_density"]))

                        if func_enable["Frequency"] == 1:
                            plt.figure(num = "Frequency", figsize = tuple(settings["plot_ttc_locations_figs_size_frequency"]))

                        if func_enable["Intervention"] == 1:
                            plt.figure(num = "Intervention", figsize = tuple(settings["plot_ttc_locations_figs_size_intervention"]))

                        if func_enable["AverageIntervention"] == 1:
                            plt.figure(num = "AverageIntervention", figsize = tuple(settings["plot_ttc_locations_figs_size_average_intervention"]))

                        if func_enable["AverageTTC"] == 1:
                            plt.figure(num = "AverageTTC", figsize = tuple(settings["plot_ttc_locations_figs_size_average_TTC"]))
                    
                    numP = seed + 1
                    
                    labeltext = "%s_%s_%s_seed_%s" %(scenario, dictLOSTitles[td_name], trafficMixTitles[tm_name], seed)
                    labeltextNoSuffix = "%s (%s / %s)" %(scenario_legend, settings["level_Of_service_titles_plots"][td_name], settings["traffic_mix_titles_plots"][tm_name])

                    #######################################################################
                    # Critical
                    xListCritical = []
                    yListCritical = []
                    
                    #######################################################################
                    # AverageTTC
                    seedVal = []
                    dealedVehs = []

                    if os.path.exists(fnStr):

                        for cf in sumolib.output.parse(fnStr, 'conflict'):
                            if cf.minTTC[0].value != 'NA' and float(cf.minTTC[0].value) < settings["plot_ttc_locations_max_seconds"]:
                                #######################################################################
                                if func_enable["Critical"] == 1 or func_enable["Density"] == 1 or func_enable["Frequency"] == 1:
                                    x = float(cf.minTTC[0].position.split(',')[0])
                                    y = float(cf.minTTC[0].position.split(',')[1])

                                    if settings["plot_ttc_locations_obey_limits"] == "True":
                                        if (x >= float(ucXLim[0]) and x <= float(ucXLim[1])):
                                            xListCritical.append(x)
                                            yListCritical.append(y)
                                        else:
                                            if verbose:
                                                print("<<<< TTC outside of x,y-limits: x:%s; td_name:%s, tm_name:%s, seed:%s" %(x, td_name, tm_name, seed))
                                    else:
                                        xListCritical.append(x)
                                        yListCritical.append(y)
                                #######################################################################
                                if func_enable["Intervention"] == 1 or func_enable["AverageIntervention"] == 1 or func_enable["AverageTTC"] == 1:
                                    
                                    if [cf.begin, cf.ego, cf.foe] not in dealedVehs and [cf.begin, cf.foe, cf.ego] not in dealedVehs:
                                        dealedVehs.append([cf.begin, cf.ego, cf.foe])

                                        if func_enable["AverageTTC"] == 1:
                                            seedVal.append(float(cf.minTTC[0].value))

                                        if func_enable["Intervention"] == 1 or func_enable["AverageIntervention"] == 1:
                                            
                                            presentLV = ("LV" in cf.foe) or ("LV" in cf.ego)
                                            presentCAV = ("CAV" in cf.foe) or ("CAV" in cf.ego)
                                            presentCV = ("CV" in cf.foe) or ("CV" in cf.ego)

                                            if presentLV:
                                                if presentCV:
                                                    conflicts['LV-CV'] += 1
                                                elif presentCAV:
                                                    conflicts['LV-CAV'] += 1
                                                else:
                                                    conflicts['LV-LV'] += 1
                                            elif presentCV:
                                                if presentCAV:
                                                    conflicts['CV-CAV'] += 1
                                                else:
                                                    conflicts['CV-CV'] += 1
                                            elif presentCAV:
                                                conflicts['CAV-CAV'] += 1

                                        if verbose:
                                            print([cf.begin, cf.ego, cf.foe])
                    else:
                        print("File not found:", fnStr)
                        continue
                
                    #######################################################################
                    if func_enable["Critical"] == 1 or func_enable["Density"] == 1 or func_enable["Frequency"] == 1:

                        if settings["plot_ttc_locations_exclude_abnormal_values"] == "True":
                            if xListCritical and (max(xListCritical) > float(ucXLim[1]) or min(xListCritical) < float(ucXLim[0]) or 
                                 max(yListCritical) > float(ucYLim[1]) or min(yListCritical) < float(ucYLim[0])):
                                print('**** Abnormal values in the SSM-files when LOS:%s, tm_name:%s, seed:%s' %(td_name, tm_name, seed))

                            for index, item in enumerate(yListCritical):
                                if item < ucYLim[0] or item > ucYLim[1]:
                                    xListCritical.pop(index)
                                    yListCritical.pop(index)

                        qArray.put("    seed:%s    number of critical TTC events: %s\n" %(seed, len(xListCritical)))
                        countList.append(len(xListCritical))

                        if func_enable["Critical"] == 1:
                            plt.figure("Critical")

                            ax1 = pylab.subplot(seedSize, 1, numP)
                            ax1.scatter(xListCritical, yListCritical, label = labeltext, s = settings["plot_ttc_locations_font_size"][0])

                            if settings["plot_ttc_locations_lim_method"] == "Normal":
                                plt.xticks(np.arange(ucXLim[0], ucXLim[1], step = settings["plot_ttc_locations_step_length"]), fontsize = settings["plot_ttc_locations_font_size"][0])
                                plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_ttc_locations_step_height"]), fontsize = settings["plot_ttc_locations_font_size"][0])
                            elif settings["plot_ttc_locations_lim_method"] == "UseTicks":
                                ax1.set_xlim(ucXLim)
                                ax1.set_ylim(ucYLim)
                                plt.xticks(np.arange(ucXLim[0], ucXLim[1], step = settings["plot_ttc_locations_step_length"]), fontsize = settings["plot_ttc_locations_font_size"][0])
                                plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_ttc_locations_step_height"]), fontsize = settings["plot_ttc_locations_font_size"][0])
                                ax1.vlines(x = settings["plot_ttc_locations_edge_ticks"], ymin = ucYLim[0], ymax = ucYLim[1], lw = 2, color = 'r')
                            elif settings["plot_ttc_locations_lim_method"] == "LimitsOnly":
                                ax1.set_xlim(ucXLim)
                                ax1.set_ylim(ucYLim)
                            else:
                                print("error: please adapt x- and y-limits to your use case!")

                            ax1.legend(loc = 'upper left', prop = {'size':settings["plot_ttc_locations_font_size"][0]})
                            ax1.grid(True)

                            if numP == seedSize:
                                ax1.set_xlabel('meter', fontsize = settings["plot_ttc_locations_font_size"][0])
                                plt.setp(ax1.get_xticklabels(), visible = True, fontsize = settings["plot_ttc_locations_font_size"][0])
                            else:
                                ax1.set_xticklabels([])
                                plt.setp(ax1.get_xticklabels(), visible = False, fontsize = settings["plot_ttc_locations_font_size"][0])

                        labels = ['seed_%s'%(seed)] * len(xListCritical)
                        data = DataFrame(dict(x = xListCritical, y = yListCritical, label = labels))
                        
                        #result_df = data.drop_duplicates(keep='first')
                        
                        df = df.append(data, ignore_index = True)

                    #######################################################################
                    if func_enable["Intervention"] == 1 or func_enable["AverageIntervention"] == 1:
                      
                        for key, _ in conflictsSum.items():
                            conflictsSum[key].append(conflicts[key])

                        if func_enable["Intervention"] == 1:
                            plt.figure("Intervention")
                        
                            ax3 = pylab.subplot(5, 2, numP)

                            x_pos = [i for i, _ in enumerate(conflicts)]
                            x_label = [key for key, _ in conflicts.items()]
                            y_val = [val for _, val in conflicts.items()]

                            ax3.bar(x_pos, y_val)
                            ax3.set_xticks(x_pos)
                            ax3.set_xticklabels(x_label)
                            ax3.set_title(labeltext)

                            #"TTC < " + str(settings["plot_ttc_locations_max_seconds"])
                            ax3.set_ylabel(settings["plot_ttc_locations_y_label"])
                            ax3.set_ylim((0, settings["plot_ttc_locations_intervention_ylim"]))
                            ax3.grid(True)
                        
                    #seed for ##################################################################################

                ###################################################################################
                if func_enable["Critical"] == 1:
                    
                    plt.figure("Critical")

                    savePath = os.path.join(work_dir_figs[scenario], 'Critical_ttc_plot_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))
                    
                    if settings["plot_network_save_svg"] == "True":
                        plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                    plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                    plt.close()

                    qArray.put("    ** average number of critical TTC events: %s\n" %(np.mean(countList)))
                    qArray.put("\n")

                    if verbose:
                        if use_parallel:
                            q.put('Saving file:' + savePath)
                        else:
                            print('Saving file:', savePath)

                ###################################################################################3
                if func_enable["Density"] == 2:
                    # Caution: following line cuts data to x > uc__XLim[0]
                    if not df.empty:

                        plt.figure("Density")

                        with warnings.catch_warnings():
                            warnings.simplefilter("ignore", category = FutureWarning)

                            df = df[df.iloc[:,1] > ucXLim[0]]

                            x = df['x']
                            y = df['y']

                            #xy = np.vstack([x, y])
                            xy = pd.concat([x, y]) #Bill is it correct?
                            z = gaussian_kde(xy)(xy)

                            if verbose:
                                print('max z for colormap normalization: ', max(z)) ## to find out maximum for normalization --> adjust vmax after first run!

                            idx = z.argsort()
                            x, y, z = x[idx], y[idx], z[idx]

                            normalize = colors.Normalize(vmin = 0, vmax = 0.0028)
                            density = plt.scatter(x, y, c = z, s = 100, norm = normalize, edgecolor = '')

                            cb = plt.colorbar(density, pad = settings["plot_ttc_locations_right_bar_offset"])
                            cb.set_label(label = settings["plot_ttc_locations_critical_right_label"], size = settings["plot_ttc_locations_font_size"][3])
                            cb.ax.tick_params(labelsize = settings["plot_ttc_locations_font_size"][2])

                            plt.xticks(np.arange(ucXLim[0], ucXLim[1], step = settings["plot_ttc_locations_step_length"]), fontsize = settings["plot_ttc_locations_font_size"][2])
                            plt.yticks(np.arange(ucYLim[0], ucYLim[1], step = settings["plot_ttc_locations_step_height"]), fontsize = settings["plot_ttc_locations_font_size"][2])

                            plt.xlabel(settings["plot_ttc_locations_critical_labels"][0], fontsize = settings["plot_ttc_locations_font_size"][3])
                            plt.ylabel(settings["plot_ttc_locations_critical_labels"][1], fontsize = settings["plot_ttc_locations_font_size"][3])

                            plt.title(labeltextNoSuffix, fontsize = settings["plot_ttc_locations_font_size"][4], weight = 'bold')
                            plt.grid(True)

                            savePath = os.path.join(work_dir_figs[scenario], 'Density_plot_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))

                            if settings["plot_network_save_svg"] == "True":
                                plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                            plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                            plt.close()
    
                        if verbose:
                            if use_parallel:
                                q.put('Saving file:' + savePath)
                            else:
                                print('Saving file:', savePath)
                    else:
                         print("Density error : df data are empty")

                ############ Creating an aggregated 2D Histogram  --> adjust to specific UC #########################################
                if func_enable["Frequency"] == 1:
                    
                    if not df.empty:

                        plt.figure("Frequency")

                        x = df['x']
                        y = df['y']
                        nbins = tuple(settings["plot_ttc_locations_frequency_nbins"]) #[80, 40] # set number of bin to your specific uc
                        # 2D Histogram
                        my_cmap = plt.cm.jet
                        my_cmap.set_under('w', 1)
                         #set vmax to your specific UC for the maximum value of the colorbar !!! 
                        h = plt.hist2d(x, y, bins = nbins, cmap = my_cmap, vmin = settings["plot_ttc_locations_frequency_nbins_min_max"][0], vmax = settings["plot_ttc_locations_frequency_nbins_min_max"][1])

                        if verbose:
                            print('local maximum in hist: ', max(h[3]._A))

                        max_norm = []
                        max_norm.append(max(h[3]._A))

                        cb = plt.colorbar(h[3], pad = settings["plot_ttc_locations_right_bar_offset"])
                        cb.set_label(label = settings["plot_ttc_locations_critical_right_label"], size = settings["plot_ttc_locations_font_size"][3], weight = 'bold')
                        cb.ax.tick_params(labelsize = settings["plot_ttc_locations_font_size"][2])

                        plt.xticks(np.arange(ucXLim[0], ucXLim[1], settings["plot_ttc_locations_step_length"]), fontsize = settings["plot_ttc_locations_font_size"][2])
                        plt.yticks(np.arange(ucYLim[0], ucYLim[1], settings["plot_ttc_locations_step_height"]), fontsize = settings["plot_ttc_locations_font_size"][2])

                        plt.xlabel(settings["plot_ttc_locations_critical_labels"][0], fontsize = settings["plot_ttc_locations_font_size"][3], weight = 'bold')
                        plt.ylabel(settings["plot_ttc_locations_critical_labels"][1], fontsize = settings["plot_ttc_locations_font_size"][3], weight = 'bold')

                        #mid = plt.figure("Frequency").subplotpars.right / 2
                        #plt.suptitle(labeltextNoSuffix, x = mid, fontsize = settings["plot_ttc_locations_font_size"][4])

                        plt.title(labeltextNoSuffix, fontsize = settings["plot_ttc_locations_font_size"][4], weight = 'bold')
                        plt.grid(True)

                        savePath = os.path.join(work_dir_figs[scenario], 'Frequency_histogram_plot_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))

                        if settings["plot_network_save_svg"] == "True":
                            plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                        plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                        plt.close()

                        if verbose:
                            if use_parallel:
                                q.put('Saving file:' + savePath)
                            else:
                                print('Saving file:', savePath)
                    else:
                         print("Frequency error : df data are empty")

                ################################################################################
                if func_enable["Intervention"] == 1:
                
                    plt.figure("Intervention")

                    savePath = os.path.join(work_dir_figs[scenario], 'Intervention_plot_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))

                    if settings["plot_network_save_svg"] == "True":
                        plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                    plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                    plt.close()

                    if verbose:
                        if use_parallel:
                            q.put('Saving file:' + savePath)
                        else:
                            print('Saving file:', savePath)

                ################################################################################
                if func_enable["AverageIntervention"] == 1:

                    plt.figure("AverageIntervention")

                    x_pos = [i for i, _ in enumerate(conflictsSum)]
                    x_label = [key for key, _ in conflictsSum.items()]
                    y_val = [sum(val) for _, val in conflictsSum.items()]
                    y_variance = [np.std(val) for _, val in conflictsSum.items()]

                    plt.bar(x_pos, y_val)
                    plt.errorbar(x_pos, y_val, yerr = y_variance, capsize = settings["plot_ttc_locations_font_size"][0], ls = 'none', ecolor = "black", elinewidth = 5, mew = 4)
                    plt.yticks(fontsize = settings["plot_ttc_locations_font_size"][2])
                    plt.xticks(x_pos, x_label, fontsize = settings["plot_ttc_locations_font_size"][2])
                   
                    plt.ylabel(settings["plot_ttc_locations_y_label"], fontsize = settings["plot_ttc_locations_font_size"][3])
                    plt.xlabel(settings["plot_ttc_locations_average_intervention_x_label"], fontsize = settings["plot_ttc_locations_font_size"][3])

                    plt.title(labeltextNoSuffix, fontsize = settings["plot_ttc_locations_font_size"][4], weight = 'bold')

                    savePath = os.path.join(work_dir_figs[scenario], 'Average_intervention_plot_%s_%s' %(td_name, settings["traffic_mix_path"][tm_name]))
                    
                    if settings["plot_network_save_svg"] == "True":
                        plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

                    plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
                    plt.close()

                    if verbose:
                        if use_parallel:
                            q.put('Saving file:' + savePath)
                        else:
                            print('Saving file:', savePath)

                ###################################################################################
                if func_enable["AverageTTC"] == 1:
                    with warnings.catch_warnings():
                        warnings.simplefilter("ignore", category = RuntimeWarning)
                        ttc[td_name][tm_name].append(np.average(seedVal))

            df.drop(df.index, inplace = True)

    ############ Creating ttc average plot #########################################
    if func_enable["AverageTTC"] == 1:
        
        plt.style.use('ggplot')
        plt.figure("AverageTTC")

        trafficMixSize = len(trafficMix)
        x_label = []
        
         #TODO make this on the fly
        if trafficMixSize == 1:
            x_label = ["B/1", "C/1", "D/1"]
        elif trafficMixSize == 2:
            x_label = ["B/1", "B/2", "C/1", "C/2", "D/1", "D/2"]
        elif trafficMixSize == 3:
            x_label = ["B/1", "B/2", "B/3", "C/1", "C/2", "C/3", "D/1", "D/2", "D/3"]
        else:
            print("Error in trafficMixSize, trafficMix = :", trafficMix)
            sys.exit(1)

        x_pos = [i for i, _ in enumerate(x_label)]
        y_val = []
        #y_variance = []

        with warnings.catch_warnings():
            warnings.simplefilter("ignore", category = RuntimeWarning)
            
            for td_name in trafficDemand:
                for tm_name in trafficMix:
                    y_val.append(np.average(ttc[td_name][tm_name]))
        
        # plt.bar(x_pos, y_val, yerr = y_variance)
        plt.bar(x_pos, y_val)
        
        #if y_variance != []:
        #    plt.errorbar(x_pos, y_val, yerr = y_variance, capsize = 4.0, ls = 'none', ecolor = "black")
        plt.yticks(fontsize = settings["plot_ttc_locations_font_size"][2])
        plt.xticks(x_pos, x_label, fontsize = settings["plot_ttc_locations_font_size"][2])
        plt.title(settings["plot_ttc_locations_average_TTC_title"], fontsize = settings["plot_ttc_locations_font_size"][4], weight = 'bold')
        
        plt.ylabel(settings["plot_ttc_locations_y_label"], fontsize = settings["plot_ttc_locations_font_size"][3])
        plt.xlabel(settings["plot_ttc_locations_average_TTC_x_label"], fontsize = settings["plot_ttc_locations_font_size"][3])

        # plt.ylim((0, YLIM))
        savePath = os.path.join(work_dir_figs[scenario], 'Average_ttc_plot')

        if settings["plot_network_save_svg"] == "True":
            plt.savefig(savePath + ".svg", dpi = settings["plots_dpi"], bbox_inches = 'tight')

        plt.savefig(savePath + ".png", dpi = settings["plots_dpi"], bbox_inches = 'tight')
        plt.close()
        
        plt.style.use('classic')

        if verbose:
            if use_parallel:
                q.put('Saving file:' + savePath)
            else:
                print('Saving file:', savePath)

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n%s"%seperator)
    print("Data comparing TTC locations started")
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

            multiWork(work_dir[s], s, scenariosFix.index(s), use_parallel, qArray[s], q)
            
            print("\nScenario", s, "finished")
            print(seperator)

    outf = open("%s/critical_plot_ttc_statistics.csv"%dir_figs, 'w')
    for s in scenarios:
        while not qArray[s].empty():
            outf.write(qArray[s].get())
    
    outf.close()

    if use_parallel or use_threads:
        print("")
        while not q.empty():
            print(q.get())

    completedTime = time.time() - startTime
    print("Data comparing TTC locations process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
