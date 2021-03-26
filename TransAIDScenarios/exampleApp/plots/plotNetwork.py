#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# Date: 2020/01/01
# Author: Leo
# Author: Dimitrios Koutras
# Author: Vasilios Karagounis

import os, sys, errno
from collections import defaultdict
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import pickle

import time
import json

from sys import platform as _platform
import matplotlib.font_manager as font_manager

plt.rcParams.update({'figure.max_open_warning': 0}) #explicitly closed and may consume too much memory. (To control this warning, see the rcParam `figure.max_open_warning`)

seperator = "---------------------------------------------------------------------"

verbose = False

scenarios = []
scenariosFix = []

legends = []

enables = []
settings = {}

work_dir_main = ""
work_dir_figs = ""

KPIField = {}

trafficDemand = {}
trafficMix = {}
driverBehaviourList = []

trafficMixSize = 0
trafficDemandSize = 0

LoS_Range_Str = ""
Mix_Range_Str = ""
losKeys = []

input_data_file = "network_aggregated"

#https://matplotlib.org/3.1.1/users/dflt_style_changes.html
#https://www.rgbtohex.net/

#plot_network_kpi_range: Y range for plots per KPI (may need to be adjusted per scenario)
#plot_network_fig_dimension: size [inches x inches] of produced figures (see output in ./figs/)

# Some infos about the batch dimensions (should match the numbers in DataTransformation.py)
#KPI = keep performance indicator

is_linux = 1
is_mac = 2
is_win = 3

OS_TYPE = 0
#---------------------------------------------------------------------------------------------------------------------------------------------
def find_platform():
    
    global OS_TYPE, use_threads

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

    if OS_TYPE == is_win:
        font_manager.FontProperties(fname='Arial Unicode.ttf')

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
def loadSettings():

    global work_dir_main, work_dir_figs, enables, scenarios, scenariosFix, settings 
    global driverBehaviourList, ttc_threshold, KPIField, trafficDemand, trafficMix, verbose, legends
    global LoS_Range_Str, Mix_Range_Str, losKeys, trafficMixSize, trafficDemandSize
    
    current = os.path.dirname(os.path.realpath(__file__))
    parent = os.path.abspath(os.path.join(current, os.pardir))
    
    work_dir_main =  os.path.join(parent, "results", "plots")
    settingsDir = os.path.join(current, "settings", "plots.json")
    
    loadFileSettings(settingsDir)

    work_dir_figs =  os.path.join(parent, "results", "plots", "figs", settings["folder_names"]["network"])

    if not os.path.exists(work_dir_figs):
        mkdir_p(work_dir_figs)

    scenariosFix = list(settings["scenarios"])
    legendsFix = list(settings["plot_network_legends"])

    if not enables:
        enables = list(settings["enables"])

    for id, enbl in enumerate(enables):
        if enbl == 1:
            scenarios.append(scenariosFix[id])
            legends.append(legendsFix[id])

    if not scenarios:
        print("Error: no scenarios found")
        sys.exit(1)

    print("Scenarios used:", scenarios)

    driverBehaviourList = settings["schemes"]
    trafficDemand = settings["level_Of_service"]

    for id, enbl in enumerate(enables):
        if enbl == 1:
            trafficMix = (settings["traffic_mix_" + str(id)])
            break
        
    trafficMixSize = len(trafficMix)
    trafficDemandSize = len(trafficDemand)

    for strLos in trafficDemand.keys():
        losKeys.append(strLos)

    LoS_Range_Str = "_LoS_" + losKeys[0][-1] + "-" + losKeys[trafficDemandSize - 1][-1] 

    mixKeys = []
    for numMix in trafficMix.values():
        mixKeys.append(str(numMix + int(settings["plot_network_png_mix_offset"])))

    Mix_Range_Str = "_Mix_" + mixKeys[0][-1] + "-" + mixKeys[trafficMixSize - 1][-1] 

    # TTC threshold for counting critical events (see DataTransformation for making alternative values available)
    ttc_threshold = settings["plot_network_ttc_threashold"]
    KPIField = settings["plot_network_kpi_field"]
    KPIField["SSM"] = KPIField["SSM"]%ttc_threshold

    if verbose:
        print("SSM filed:", KPIField["SSM"])

#---------------------------------------------------------------------------------------------------------------------------------------------
def clearFiles():

    if settings["clean_old_data"] == "True":
        
        titles = settings["plot_network_kpi_titles"]

        try:
            count = 0
            for file in os.listdir(work_dir_figs):
                if file.endswith(".png") or file.endswith(".svg"):
                    path = os.path.join(work_dir_figs, file)
                    os.remove(path)
                    print("remove", path)
                    count+=1
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
def multiWork():

    resultFiles = {}
    resultList = []

    for sId, scenario in enumerate(scenarios):
        path = os.path.join(work_dir_main, scenarios[sId], '%s_%s'%(input_data_file,scenario)) + '.pickle'
        resultList.append(path)

    if not resultList:
        sys.exit(1)

    for lId, lSt in enumerate(resultList):
        resultFiles[scenarios[lId]] = resultList[lId]

    if verbose:
        print("Result files:", resultFiles)

    #----------------------------------------------------------
    # Load pickled contents from files
    #----------------------------------------------------------
    resultDicts = {}
    for batchID, fn in resultFiles.items():
        try:
            with open(fn, "rb") as f:
                resultDicts[batchID] = pickle.load(f)
        except Exception as e:
            print("Error when trying to open batch file '%s'"%fn)
            raise e
    
    titles = settings["plot_network_kpi_titles"]
       
    #----------------------------------------------------------
    # Make data frame of all data
    # ~ nBatches = len(resultDicts)
    #----------------------------------------------------------
    
    print("Working with data")

    func_enable = settings["plot_network_list_" +  str(scenariosFix.index(scenarios[0]))]
 
    #check if there is no reason to continue    
    count_kpi_that_we_need = 0
    for KPI in settings["plot_network_kpi_list"]:
        if func_enable[KPI] == 1:
            count_kpi_that_we_need+=1

    if count_kpi_that_we_need == 0:
        print("Can not find any usable KPI")
        return

    df = defaultdict(list)

    for KPI in settings["plot_network_kpi_list"]:
        if func_enable[KPI] == 1:
            
            for td_id in range(trafficDemandSize):
                for tm_id in range(trafficMixSize):
                    for db_id, db_name in enumerate(driverBehaviourList):

                        for batchID, resultDict in resultDicts.items():
                            matrix = resultDict[KPI]
                            stats = matrix[td_id][tm_id][db_id]
                            
                            for k,v in stats.items():
                                lenStats = len(v)
                                if not k in df.keys():
                                    # Ensure equal length of all columns for the DataFrame
                                    df[k] = [None]*len(df["batchID"])
                                df[k].extend(v)
                            lenStats = len(v)
                            df["batchID"].extend(lenStats * [batchID])
                            df["Level of Service"].extend(lenStats * [td_id])
                            df["Traffic Mix"].extend(lenStats * [tm_id])
                            df["scheme"].extend(lenStats * [db_name])
                            df["KPI"].extend(lenStats * [KPI])

    # Ensure equal length of all columns for the DataFrame
    dfLength = len(df["batchID"])
    for k, v in df.items():
        v.extend([None]*(dfLength - len(v)))
    
    df = pd.DataFrame(df)

    if verbose:
        df.info()

    ## Plotting
    # Set plot style
    sns.set()
     
    xTicks = ()
    xTickLabels = ()
    xTickLosLabels = () #("B","C","D")

    temp = []
    for strLos in losKeys:
        temp.append(strLos[-1])
    xTickLosLabels = tuple(temp)

    ParamCombiLabel = {}

    if trafficMixSize == 1:
        xTicks = (0)
        xTickLabels = ("1")
        ParamCombiLabel = {0:"B/1", 1:"C/1", 2:"D/1"}
    elif trafficMixSize == 2:
        xTicks = (0,1)
        xTickLabels = ("1","2")
        ParamCombiLabel = {0:"B/1", 1:"B/2", 2:"C/1", 3:"C/2", 4:"D/1", 5:"D/2"}
    elif trafficMixSize == 3:
        xTicks = (0,1,2)
        xTickLabels = ("1","2","3")
        ParamCombiLabel = {0:"B/1", 1:"B/2", 2:"B/3", 3:"C/1", 4:"C/2", 5:"C/3", 6:"D/1", 7:"D/2", 8:"D/3"}

    #################################################################################################
    # Plot with fixed mix
    #################################################################################################
    print("Ploting images with fixed traffic mix")

    for tm_id, tm_name in enumerate(trafficMix):
        for KPI in settings["plot_network_kpi_list"]:
            if func_enable[KPI] == 1:
                dfKPI = df[(df["KPI"] == KPI) & (df["Traffic Mix"] == tm_id)]
                #dfKPI.info()
                #print(dfKPI[KPIField[KPI]].describe())

                if verbose:
                    print(KPI)
                g = sns.catplot(data = dfKPI, x = "Level of Service", y = KPIField[KPI], hue = "batchID", ci = "sd", kind = "bar", capsize = 0.25, height = 5, aspect = 2, legend = False)
                g.set_ylabels(settings["plot_network_kpi_label"][KPI] + " " + settings["plot_network_kpi_units"][KPI], fontname="Arial Unicode")

                plt.gca().set_title(settings["traffic_mix_titles_big"][tm_name])
                if trafficMixSize > 1:
                    plt.gca().set_xticks(xTicks)
                plt.gca().set_xticklabels(xTickLosLabels)
                plt.ylim(settings["plot_network_kpi_range"][KPI])
                plt.gcf().set_size_inches(settings["plot_network_fig_dimension"])
                plt.tight_layout()
                leg  = plt.legend(labels = legends, loc = "best")
                for i, j in enumerate(leg.legendHandles):
                    j.set_color(settings["plot_network_legend_colors"][i])

                imageFNBase = os.path.join(work_dir_figs, titles[KPI].replace(" ", "_") + "_Mix_" + str(tm_id + int(settings["plot_network_png_mix_offset"])) + LoS_Range_Str)

                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(imageFNBase + ".svg")

                plt.savefig(imageFNBase + ".png", dpi = settings["plots_dpi"])
                plt.clf()
                plt.close()
    
    #################################################################################################
    # Plot with fixed LoS
    #################################################################################################
    print("Ploting images with fixed level of service")

    for td_id, td_name in enumerate(trafficDemand):
        for KPI in settings["plot_network_kpi_list"]:
            if func_enable[KPI] == 1:
                if verbose:
                    print(KPI)

                dfKPI = df[(df["KPI"] == KPI) & (df["Level of Service"] == td_id)]
                #dfKPI.info()
                #print(dfKPI[KPIField[KPI]].describe())

                g = sns.catplot(data = dfKPI, x = "Traffic Mix", y = KPIField[KPI], hue = "batchID", ci = "sd", kind = "bar", capsize = 0.25, height = 5, aspect = 2, legend = False)
                g.set_ylabels(settings["plot_network_kpi_label"][KPI] + " " + settings["plot_network_kpi_units"][KPI], fontname="Arial Unicode")
                plt.gca().set_title(settings["level_Of_service_titles_big"][td_name])
                if trafficMixSize > 1:
                    plt.gca().set_xticks(xTicks)
                plt.gca().set_xticklabels(xTickLabels)
                plt.ylim(settings["plot_network_kpi_range"][KPI])
                plt.gcf().set_size_inches(settings["plot_network_fig_dimension"])
                plt.tight_layout()
                leg  = plt.legend(labels = legends, loc = "best")
                for i, j in enumerate(leg.legendHandles):
                    j.set_color(settings["plot_network_legend_colors"][i])

                #g.despine(left = True)

                imageFNBase = os.path.join(work_dir_figs, titles[KPI].replace(" ", "_") + "_LoS_" + td_name[-1] + Mix_Range_Str)

                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(imageFNBase + ".svg")

                plt.savefig(imageFNBase + ".png", dpi = settings["plots_dpi"])
                plt.clf()
                plt.close()

    #################################################################################################
    # Plot all LoS/Mix combinations at once
    # add a new field (ternary code per combination)
    #################################################################################################
    print("Ploting all Los/Mix images")

    try:
        df["Parameter Combination (LoS/Mix)"] = df["Level of Service"] * 3 + df["Traffic Mix"]
    except:
        print("Unble to read df data")
    finally:    
        for KPI in settings["plot_network_kpi_list"]:
            if func_enable[KPI] == 1:
                if verbose:
                    print(KPI)

                dfKPI = df[(df["KPI"] == KPI)]
                #print(dfKPI[KPIField[KPI]].describe())
                g = sns.catplot(data = dfKPI, x = settings["plot_network_los_mix"], y = KPIField[KPI], hue = "batchID", ci = "sd", kind = "bar", capsize = 0.25, height = 5, aspect = 2, legend = False)
                g.set_ylabels(settings["plot_network_kpi_label"][KPI] + " " + settings["plot_network_kpi_units"][KPI],  fontname="Arial Unicode")
                plt.gca().set_xticks(list(ParamCombiLabel.keys()))
                plt.gca().set_xticklabels(list(ParamCombiLabel.values()))
                plt.ylim(settings["plot_network_kpi_range"][KPI])
                plt.gcf().set_size_inches((settings["plot_network_fig_dimension"][0] * 2, settings["plot_network_fig_dimension"][1]))
                plt.tight_layout()

                leg  = plt.legend(labels = legends, loc = "best")
                for i, j in enumerate(leg.legendHandles):
                    j.set_color(settings["plot_network_legend_colors"][i])

                #g.despine(left=True)
                imageFNBase = os.path.join(work_dir_figs, titles[KPI].replace(" ", "_") + LoS_Range_Str + Mix_Range_Str)

                if settings["plot_network_save_svg"] == "True":
                    plt.savefig(imageFNBase + ".svg", dpi = settings["plots_dpi"])

                plt.savefig(imageFNBase + ".png", dpi = settings["plots_dpi"])
                #plt.show()
                plt.clf()
                plt.close()

    #################################################################################################
    # Plot all LoS/Mix combinations at once for SpeedAvg
    # add a new field (ternary code per combination)
    #################################################################################################
    print("Ploting all Los/Mix images for Speed Avererage")
    if func_enable["SpeedAvg"] == 1:
        plotSpeedAvg(ParamCombiLabel, df, "box", True)
        plotSpeedAvg(ParamCombiLabel, df, "box", False)
        plotSpeedAvg(ParamCombiLabel, df, "violin", False)
   
    #################################################################################################    

#---------------------------------------------------------------------------------------------------------------------------------------------
def plotSpeedAvg(paramCombiLabel, data, plotType, showFliers):
    
    KPI = "SpeedAvg"
    titles = settings["plot_network_kpi_titles"]

    try:
        data["Parameter Combination (LoS/Mix)"] = data["Level of Service"] * 3 + data["Traffic Mix"]
    except:
        print("Unble to read df data")
    finally:
        
        dfKPI = data[(data["KPI"] == KPI)]
        
        flierprops = dict(marker = 'X', markerfacecolor = 'r', markersize = 0.1, linestyle = 'none') #, markeredgecolor = 'b'
        if plotType == "box":
            if showFliers == True:
                bp = sns.boxplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = "speedAvg", hue = "batchID", linewidth = 0.8, fliersize = 0, orient = 'v', flierprops = flierprops)
            else:
                bp = sns.boxplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = "speedAvg", hue = "batchID", linewidth = 0.8, fliersize = 0, orient = 'v')    
        elif plotType == "violin":
            if trafficMixSize > 1:
                bp = sns.violinplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = "speedAvg", hue = "batchID",  split = True, linewidth = 0.8, fliersize = 0, inner = "quartile")
            else:
                bp = sns.violinplot(data = dfKPI, x = "Parameter Combination (LoS/Mix)", y = "speedAvg", hue = "batchID",  split = False, linewidth = 0.8, fliersize = 0, inner = "quartile")
        
        bp.set(ylabel = (settings["plot_network_speed_avg"] + " " + settings["plot_network_kpi_units"][KPI]))

        handles, labels = bp.get_legend_handles_labels()
        labels = legends

        plt.legend(handles[0:2], labels[0:2], borderaxespad = 0., bbox_to_anchor = (0.25, 1.00, 0.45, 0.15),  ncol = 2, mode = "expand", frameon = False)
        
        plt.gca().set_xticks(list(paramCombiLabel.keys()))
        plt.gca().set_xticklabels(list(paramCombiLabel.values()))
        
        plt.ylim(settings["plot_network_kpi_range"][KPI])

        plt.gcf().set_size_inches((settings["plot_network_fig_dimension"][0] * 2, settings["plot_network_fig_dimension"][1]))
        plt.tight_layout()

        savePath = os.path.join(work_dir_figs, titles[KPI].replace(" ", "_"))
        
        if plotType == "box":
            savePath += "_boxplot"
        elif plotType == "violin":
            savePath += "_violin"
        
        if showFliers == True:
             savePath += "_fliers"

        savePath += LoS_Range_Str + Mix_Range_Str + ".png"
        
        if verbose:
            print(savePath)
            
        plt.savefig(savePath, dpi = settings["plots_dpi"], bbox_inches = 'tight')
        plt.clf()
        plt.close()   
        
        if verbose:
            print('Saving file:', savePath)

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    
    print("\n%s"%seperator)
    print("Data plotting proccess started")
    startTime = time.time()

    if "--verbose" in sys.argv:
        verbose = True

    for arg in sys.argv:
        if "--en=" in arg:
            temp = arg.replace("--en=", "")
            enables = [int(n) for n in temp.split(",")]
    
    find_platform()

    loadSettings()

    print("\n%s"%seperator)
    clearFiles()

    print("\n%s"%seperator)
    printDisabledFunctions()

    print("\n%s"%seperator)
    multiWork()

    print("\n%s"%seperator)
    completedTime = time.time()- startTime
    print("\nData plotting process completed after ", completedTime, " [sec]")
    print(seperator,"\n")
