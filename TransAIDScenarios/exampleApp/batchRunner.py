#!/usr/bin/python3

# Date: 2019/03/29
# Author: Leonhard Luecken
# Author: Vasilios Karagounis

# Configurable batch runner for baseline simulations covering
# a range of parameter combinations.
# Creates a subdirectory tree holding the custom config files
# and the generated output for combinations of demand level,
# traffic mix, and model parameter schemes

# Usage: ./batchRunner.py [options]

# Options (edit OPTIONS below if you add an option):
# "--sequential": Run sequentially
# "--gui": Run sequentially and with sumo-gui
# "--clean": Remove old results_ folder (in case that this is omitted, the new results_ eventually override old ones (or .gz.gz duplicates appear)
# "--no-gzip": Don't gzip all created files
# "--generateVTypes": Generate vType distribution files
# "--qt": 
# "--debug": 
# "--verbose": 
# "--json": load json_something vars from json file  

#---------------------------------------------------------------------------------------------------------------------------------------------

import os, sys, errno
import random
import shutil
import subprocess as sp
import time
import datetime
import functools
import csv
import json
import stat
import gzip
import ctypes
from sys import platform as _platform
from xml.dom import minidom
import pathlib

sys.dont_write_bytecode = True #avoid creating __pycache__

#---------------------------------------------------------------------------------------------------------------------------------------------
VERSION = '1.0.0' 
#1.0.0 initial version

i_CS = False

WORK_ON_CLUSTER = False
VERBOSE = False 
ALL_LOAD_FROM_JSON = False
NO_ARGS = False

RUN_QUICK_TEST = False
RUN_PARALLEL = True     # affected by options "--gui" and "--sequential"
RUN_SEQUENTIAL = False     
RUN_CLEAN = False       
RUN_WITH_GUI = False
RUN_WITH_DEBUG = False
RUN_WITH_NO_GZIP = False
RUN_WITH_LIGHTCOMM = False
RUN_ONLY_SUMO = False

SUMO_HOME = ""
TRANSAID_BIN = ""
SUMO_LIB = ""

PLATFORM_HOME = ""

USER = ""
OS_TYPE = 0

SPECIAL_ARGS = False

use_external_mix_file = False

is_linux = 1
is_mac = 2
is_win = 3

wdirMain = ""
wdir = ""
odir = ""
rdir = ""

times_dir = ""
cluster_scratch = ""
pythonStr = ""

scenarioString = ""
scenarioIndex = 0

net_type = ""

# nr of runs started from this script
runCount = 0

# Number of simulations per Demand/Parameter configuration
MAX_SIMULATION_SUPPORT = 10
sim_min = 0
sim_max = 10

# Number of lanes (regarding the LOS) in the current scenario
lanes = 0

# Number of restarts if a run fails
maxTries = 0

# Seed-map (maps run-number to according seed for the simulation)
seedMap = pythonStr = ""

results_dir = ""

# Directory for output files (relative path within directory tree's leaf
# corresponding to a specific parameter combinations!)
output_dir_rel = ""
# Top directory for all generated files (is reset dependent on the comm simulator below)
results_dir_rel = ""

# Directory holding itetris config templates (relative path wrt working dir!)
config_dir_rel = ""


# Directory holding SUMO config templates (relative path wrt working dir!)
# Note: it is assumed that the sumo directoy lies within the subtree of the itetris directory.
sumo_dir_rel = ""
ns3_dir_rel = ""


# Sumo config template (relative path wrt working dir!)

sumo_config_template_rel = ""
itetris_config_template_rel = ""

itetris_config_fn = ""

sumo_config_fn = "" # will be treated as global and set by createConfigFilesFromTemplates() below
# template file for the routes (relative path wrt working dir!)
routefile_template_rel = ""
# template file for the routes (relative path wrt working dir!)
vtypefile_template_dir_rel = ""
# detectors file template (relative path wrt working dir!)
detectors_template_rel = ""
# additional output file template (relative path wrt working dir!)
additionalsOutput_template_rel = ""
# Parameter assumptions regarding (E)fficiency/(S)afety and (O)ptimism/(P)essimism

config_dir = ""

sumo_config_template = ""
itetris_config_template = ""
ns3_config_template = ""

routefile_template = ""
detectors_template = ""
additionalsOutput_template = ""

schemes = []

demand_levels = {}
veh_mixes = {}
settings = {}

csv_time_logger = 0

#---------------------------------------------------------------------------------------------------------------------------------------------
def copytree(src, dst, symlinks = False, ignore = None):

    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                shutil.copy2(s, d)

#---------------------------------------------------------------------------------------------------------------------------------------------
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python > 2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

#---------------------------------------------------------------------------------------------------------------------------------------------
def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

#---------------------------------------------------------------------------------------------------------------------------------------------
def run_as_admin():
    
    global OS_TYPE

    if OS_TYPE == is_win:
        # Re-run the program with admin rights
        if not is_admin():
            ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, __file__, None, 1)

#---------------------------------------------------------------------------------------------------------------------------------------------
def find_platform():
    
    global OS_TYPE

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
def loadFileSettings(path):
    
    global settings

    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                settings = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            print("Json settings path :", path)        
    else:
         print("Json settings path not found :", path)   

#---------------------------------------------------------------------------------------------------------------------------------------------
# load settings from json
def loadSettings():

    global ALL_LOAD_FROM_JSON, RUN_WITH_GUI, RUN_CLEAN, RUN_PARALLEL, RUN_SEQUENTIAL, RUN_QUICK_TEST,RUN_WITH_DEBUG, RUN_WITH_NO_GZIP
    
    global RUN_WITH_LIGHTCOMM, RUN_ONLY_SUMO, VERBOSE, i_CS

    global PLATFORM_HOME, USER, settings, wdirMain, wdir, odir, sim_min, sim_max, lanes, seedMap, maxTries, runner_fn, veh_mixes, demand_levels, sumo_dir_rel, ns3_dir_rel, schemes

    global rdir, config_dir, output_dir_rel, results_dir_rel, config_dir_rel, sumo_config_template, routefile_template, detectors_template
     
    global results_dir, routefile_template_rel, additionalsOutput_template, detectors_template_rel

    global sumo_config_template_rel, additionalsOutput_template_rel, vtypefile_template_dir_rel

    global scenarioString, scenarioIndex, SPECIAL_ARGS, NO_ARGS, use_external_mix_file

    try:
        PLATFORM_HOME = os.environ['HOME'] #sometimes this is failing
    except:
        PLATFORM_HOME = pathlib.Path.home()

    USER = os.path.basename(PLATFORM_HOME) #os.path.expanduser("~"))

    loadFileSettings("settings/batchRunner.json")

    if not settings:
        print("Error: Unable to read json settings file")
        print("path:", os.path.dirname(os.path.realpath(__file__)))
        sys.exit(1)

    #-------------------------------------------------------------------------------------------------------------
    if len(sys.argv) == 1:
        NO_ARGS = True

    if len(sys.argv) > 1:
        if "--json" in sys.argv:
            ALL_LOAD_FROM_JSON = True # meaning that all arguments are read from json (e.x. json_scenario="tm")

    # check given options
    SPECIAL_ARGS = False
    if not ALL_LOAD_FROM_JSON and len(sys.argv) > 1:
        for arg in sys.argv[1:]:
            if (arg not in settings["args"] 
                and arg not in settings["args_demands"]
                and arg not in settings["args_schemes"]
                and arg not in settings["args_mix"]
                and arg not in settings["args_scenarios"]
                ):
                argFound = False
                
                for special_arg in settings["args_special"]:
                    if arg.startswith(special_arg):
                        argFound = True
                        SPECIAL_ARGS = True

                if argFound == False:
                    print("Error: Unknown option '%s'"%arg)
                    sys.exit(1)

    if ALL_LOAD_FROM_JSON:
        RUN_WITH_GUI  = settings["json_gui"] == "True"
        RUN_CLEAN = settings["json_run_clean"] == "True"
        RUN_PARALLEL = settings["json_run_parallel"] == "True"
        RUN_SEQUENTIAL = settings["json_run_sequential"] == "True"
        RUN_QUICK_TEST = settings["json_quick_test"] == "True"
        RUN_WITH_DEBUG = settings["json_debug"] == "True"
        RUN_WITH_NO_GZIP = settings["json_no_gzip"] == "True"
        RUN_WITH_LIGHTCOMM = settings["json_lightcomm"] == "True"
        RUN_ONLY_SUMO = settings["json_only_sumo"] == "True"
    else:
        if "--gui" in sys.argv:
            RUN_WITH_GUI = True

        if "--debug" in sys.argv:
            RUN_WITH_DEBUG = True

        if "--clean" in sys.argv:
            RUN_CLEAN = True

        if "--sequential" in sys.argv:
            RUN_SEQUENTIAL = True    

        if "--qt" in sys.argv:
            RUN_QUICK_TEST = True

        if "--no-gzip" in sys.argv:
            RUN_WITH_NO_GZIP = True

        if "--lightcomm" in sys.argv:
            RUN_WITH_LIGHTCOMM = True

        if "--onlysumo" in sys.argv:
            RUN_ONLY_SUMO = True

    # don't run in parallel if gui is shown
    if RUN_WITH_GUI or RUN_SEQUENTIAL:
        RUN_PARALLEL = False

    # Working directory
    wdirMain = settings["json_work_path"]
    if not wdirMain:
        wdirMain = os.path.dirname(os.path.realpath(__file__))

    wdir = os.path.join(wdirMain, settings["results_dir"])

    find_sumo_home()
    
    find_python_path()

    handle_v_types()

    if WORK_ON_CLUSTER:
        if cluster_scratch:
            odir = cluster_scratch
        else:
            odir = wdir
    else:
        odir = wdir

    # Number of restarts if a run fails
    maxTries = settings["max_tries"]
    #-----------------------------------------------------------------------
    # Number of simulations per Demand/Parameter configuration
    if ALL_LOAD_FROM_JSON:
        sim_min = settings["json_sim_min"]
        sim_max = settings["json_sim_max"]
    elif SPECIAL_ARGS:
        for arg in sys.argv[1:]:
            if arg.startswith("--sim="):
               paramSim = arg.replace('--sim=', '')
               
               min_Max = paramSim.split(",")
               
               sim_min = int(min_Max[0])
               sim_max = int(min_Max[1])

               if sim_min >= MAX_SIMULATION_SUPPORT:
                    sim_min = MAX_SIMULATION_SUPPORT - 1
               if sim_min >= sim_max:
                    sim_min = 0
                    sim_max = MAX_SIMULATION_SUPPORT
               
    else:
        sim_min = settings["sim_min"]
        sim_max = settings["sim_max"]

    if sim_min == sim_max:
        print("Error: simulations are equal!")
        sys.exit(1)

    #-----------------------------------------------------------------------
    # Number of lanes (regarding the LOS) in the current scenario
    lanes = settings["lanes"]

    # Seed-map (maps run-number to according seed for the simulation)
    seedMap = dict([(i, i + settings["seed_start"]) for i in range(sim_min, sim_max, 1)])

    print("Seed map : ", seedMap)

    # Name of the runner script for individual simulation runs
    runner_fn = os.path.join(os.path.dirname(os.path.realpath(__file__)), settings["runner"])

    # Directory for output files (relative path within directory tree's leaf corresponding to a specific parameter combinations!)
    output_dir_rel = settings["output_dir"]
    # Top directory for all generated files (is reset dependent on the comm simulator below)
    results_dir_rel = settings["results_dir"]
    
    # Directory holding itetris config templates (relative path wrt working dir!)
    config_dir_rel = settings["config_dir"]

    if config_dir_rel == "":
        print("ERROR: 'config_dir' parameter is empty. Please type valid config file name!")
        sys.exit(1)

    # Directory holding SUMO config templates (relative path wrt working dir!)
    # Note: it is assumed that the sumo directoy lies within the subtree of the itetris directory.
    sumo_dir_rel = os.path.join(config_dir_rel, settings["sumo_folder"]) 
    ns3_dir_rel = os.path.join(config_dir_rel, settings["ns3_folder"]) 

    # Directory holding SUMO config templates (relative path wrt working dir!)
    sumo_config_template_rel = os.path.join(sumo_dir_rel, settings["sumo_config"])

    # detectors file template (relative path wrt working dir!)
    detectors_template_rel = os.path.join(sumo_dir_rel, settings["detectors"])

    # additional output file template (relative path wrt working dir!)
    additionalsOutput_template_rel = os.path.join(sumo_dir_rel, settings["additionals"])

    # template file for the routes (relative path wrt working dir!)
    vtypefile_template_dir_rel = os.path.join(sumo_dir_rel, settings["vtype_dir"])
    
    #-----------------------------------------------------------------------
    # Parameter assumptions regarding (E)fficiency/(S)afety and (O)ptimism/(P)essimism
    if ALL_LOAD_FROM_JSON:
        schemes = settings["json_schemes"]
    else:
         for arg in sys.argv[1:]:
            if arg in settings["args_schemes"]:
                tempScheme = arg.replace('-', '')
                schemes.append(settings["args_schemes_names"][tempScheme])

    if not schemes:
        schemes = settings["schemes"]

    if not schemes:
        print("ERROR: scheme options are missing, please provide a valid scheme options!")
        sys.exit(1) 

    #-----------------------------------------------------------------------
    if ALL_LOAD_FROM_JSON:
        VERBOSE = settings["json_verbose"] == "True"
    else:
        if "--verbose" in sys.argv:
            VERBOSE = True

    if ALL_LOAD_FROM_JSON:
        scenarioString = settings["json_scenario"]
    else:
        for idStr in settings["scenarios"]:
            scenario = "--" + idStr
            if scenario in sys.argv:
                scenarioString = idStr  
                break

    if scenarioString == "":
        tempDict = settings["scenarios"]
        tempList = list(tempDict.keys())
        scenarioString = tempList[0]
    
    scenarioIndex = settings["scenarios"][scenarioString]
    results_dir_rel = scenarioString

    if scenarioIndex == -1:
        print("ERROR: scenario option is missing, please provide a valid scenario option!")
        sys.exit(1)

    template = settings["template_" + str(scenarioIndex)]
    routefile_template_rel = os.path.join(sumo_dir_rel, template)

    #-----------------------------------------------------------------------
    #open, read and print edges
    if RUN_WITH_GUI:
        m_xml = minidom.parse(routefile_template_rel)
        edges = m_xml.getElementsByTagName("route")
        for ed in edges:
            print("Edges :", ed.getAttribute("edges"))

    #-----------------------------------------------------------------------
    # you can find an external_mix.py python file in UC_23_CRT use case 
    use_external_mix_file = settings["use_external_mix_file"] == "True"

    if use_external_mix_file:
        from external_mix import externalMixDict
    
    if RUN_QUICK_TEST:
        if use_external_mix_file:
            veh_mixes = externalMixDict(scenarioString, "", True, VERBOSE)
        else:
            veh_mixes = settings["traffic_mix_quick_test_" + str(scenarioIndex)]
    elif ALL_LOAD_FROM_JSON:
        veh_mixes = settings["json_mix"]
    
    # apply only specific mix here coming from args
    if not ALL_LOAD_FROM_JSON:
        mix_temp = {}
        only_some_mixes = []
    
        for idStr in settings["mix_names"]:
            mixStr = "--" + idStr
            if mixStr in sys.argv:
                only_some_mixes.append(settings["mix_names"][idStr])
        
        if only_some_mixes:
            for k in only_some_mixes:
                if use_external_mix_file:
                    mix_temp.update({k : externalMixDict(scenarioString, k, False, VERBOSE) })
                else:
                    mix_temp.update({k : settings["traffic_mix_" + str(scenarioIndex)][k] })
        
        if mix_temp:
            veh_mixes = mix_temp

    if not veh_mixes:
        if use_external_mix_file:
            veh_mixes = externalMixDict(scenarioString, "", False, VERBOSE)
        else:
            veh_mixes = settings["traffic_mix_" + str(scenarioIndex)]

        if not veh_mixes:
            print("Error: empty vehicle mix dictionary")
            exit(1)

    #-----------------------------------------------------------------------
    if RUN_QUICK_TEST:
        demand_levels = settings["demand_quick_test_" + str(scenarioIndex)]
    elif ALL_LOAD_FROM_JSON:
        demand_levels = settings["json_demands"]
    
    # apply only specific los here coming from args
    if not ALL_LOAD_FROM_JSON:
        demand_temp = {}
        only_some_demands = []

        for idStr in settings["demands_names"]:    
            demandStr = "--" + idStr
            if demandStr in sys.argv:
                only_some_demands.append(settings["demands_names"][idStr])

        if only_some_demands:
            for k in only_some_demands:
                demand_temp.update({k : settings["demand_level_" + str(scenarioIndex)][k] })

        if demand_temp:
            demand_levels = demand_temp
    
    if not demand_levels:
        demand_levels = settings["demand_level_" + str(scenarioIndex)]

    #-----------------------------------------------------------------------
    if WORK_ON_CLUSTER:
        results_dir = os.path.join(odir, results_dir_rel)
    else:
        results_dir = os.path.join(wdir, results_dir_rel)    

    if RUN_CLEAN:
        # remove existing temporary subdir structure
        try:
            shutil.rmtree(os.path.join(results_dir))
            print("removing dir %s"%d)
        except:
            pass

    #-----------------------------------------------------
    # Make config template paths absolute
    config_dir = os.path.join(wdirMain, config_dir_rel)
            
    sumo_config_template = os.path.join(wdirMain, sumo_config_template_rel)
    routefile_template = os.path.join(wdirMain, routefile_template_rel)

    detectors_template = os.path.join(wdirMain, detectors_template_rel)
    additionalsOutput_template = os.path.join(wdirMain, additionalsOutput_template_rel)

    

    # result output top level directory
    if WORK_ON_CLUSTER:
        rdir = os.path.join(odir, results_dir)
    else:
        rdir = os.path.join(wdir, results_dir)

#---------------------------------------------------------------------------------------------------------------------------------------------
def find_sumo_home():

    global OS_TYPE, PLATFORM_HOME, SUMO_HOME, TRANSAID_BIN, SUMO_LIB, USER, WORK_ON_CLUSTER, i_CS, wdirMain, cluster_scratch, settings

    if OS_TYPE == is_linux:
        
        if settings["cluster_home"] in wdirMain:
            WORK_ON_CLUSTER = True

            if settings["cluster_scratch_save"] == "True":
                cluster_scratch = os.path.join("/", "scratch", USER, settings["cluster_scratch_path"])
                if not os.path.exists(cluster_scratch):
                    mkdir_p(cluster_scratch)

            path = os.path.join(settings["cluster_home"], USER)

            for search in settings["linux_search_sumo"]:
                checkPath = os.path.join(path, search)
                if os.path.exists(checkPath):
                    SUMO_HOME = checkPath
                    return

        else:
            if settings["transaid_dir"] in wdirMain:
                for search in settings["linux_search_transaid"]:
                    checkPath = os.path.join(PLATFORM_HOME, search)
                    
                    if os.path.exists(checkPath):

                        if not RUN_ONLY_SUMO:
                            i_CS = True

                        SUMO_HOME = os.path.join(checkPath, "share/sumo")
                        SUMO_LIB = os.path.join(checkPath, "lib")
                        TRANSAID_BIN = os.path.join(checkPath, "bin")
                        return
            
            # if not a transaid folder exist look for just sumo...
            for search in settings["linux_search_sumo"]:
                checkPath = os.path.join(PLATFORM_HOME, search)
                if os.path.exists(checkPath):
                    SUMO_HOME = checkPath
                    return
        
    elif OS_TYPE == is_win:
        for search in settings["win_search_sumo"]:
            checkPath = os.path.join(PLATFORM_HOME, search)
            if os.path.exists(checkPath):
                SUMO_HOME = checkPath
                return
    
    print("ERROR: please provide a valid SUMO_HOME path!")
    sys.exit(1)

#---------------------------------------------------------------------------------------------------------------------------------------------
def find_python_path():
    
    global OS_TYPE, WORK_ON_CLUSTER, pythonStr, settings

    pythonStr = "python3"

    if OS_TYPE == is_win:
       pythonStr = "python"

    if WORK_ON_CLUSTER:

        path = os.path.join(settings["cluster_home"], USER, "libs/bin")
        
        if not os.path.exists(path):
            print("ERROR: please provide a valid python3 path!")
            sys.exit(1)

        pythonStr =  os.path.join(path, "python3")

#---------------------------------------------------------------------------------------------------------------------------------------------
def handle_v_types():

    global pythonStr

    # remove vTypeDistribution files and create them again
    if ALL_LOAD_FROM_JSON:
        if settings["json_generateVTypes"] == "True":
            sp.call([pythonStr, settings["generateVTypes"]])
    else:
        if "--generateVTypes" in sys.argv:
            sp.call([pythonStr, settings["generateVTypes"]])

#---------------------------------------------------------------------------------------------------------------------------------------------
def fillTemplate(template_file, out_file, content_dict):
    
    global VERBOSE

    if VERBOSE:
        print("# fillTemplate():\n  template file:\n   %s\n  outfile\n   '%s'\n  format dict=\n   %s"%(template_file, out_file, content_dict))

    try:
        with open(template_file) as f:
            template = f.read()
            filled_template = template.format(**content_dict)
    except:
        print("Error in fillTemplate(): template_file '%s'\n"%(template_file))
        exit(0)

    try:
        with open(out_file, "w") as f:
            f.write(filled_template)
    except:
        print("Error in fillTemplate(): out_file '%s'\n"%(out_file))
        exit(0)
    
    if VERBOSE:
        print("# fillTemplate(): Wrote file '%s'"%(out_file))

#---------------------------------------------------------------------------------------------------------------------------------------------
def makeOutputSuffix(demandLevel, trafficMix, driverBehaviour, run_nr):
    """ (int, int, str, int) -> string
        Returns the suffix for output data (not including the extension)
        for identification of parameters for the corresponding simulation run.
    """
    return "TD_" + str(demandLevel) + "_TM_"+ str(trafficMix) +  "_DB_" + driverBehaviour + "_seed_" + str(run_nr)

#---------------------------------------------------------------------------------------------------------------------------------------------
def createConfigFilesFromTemplates(target_dir, demandLevel, trafficMix,  driverBehaviour, run_nr, seed, scenarioStr):
    
    global RUN_CLEAN, RUN_ONLY_SUMO, i_CS, RUN_WITH_LIGHTCOMM, results_dir, sumo_config_template, sumo_config_fn, routefile_template, detectors_template, additionalsOutput_template, itetris_config_fn

    global lanes, veh_mixes, sumo_dir_rel, itetris_config_template_rel, itetris_config_template, ns3_config_template, settings

    # Run identifier for labeling output
    run_id = makeOutputSuffix(settings["demands_map"][demandLevel], int(trafficMix[-1]), driverBehaviour, run_nr)

    # Names of custom config files for this run
    sumo_config_fn = os.path.join(target_dir, sumo_dir_rel, "sumoConfig_" + run_id + ".cfg.xml")
    addOut_fn_rel = "additionalsOutput_" + run_id + ".add.xml"

    formatDict = {}
    
    if i_CS:
        itetris_config_template_rel = os.path.join(results_dir_rel, settings["itetris_config_" + scenarioStr])

        itetris_config_template = os.path.join(config_dir, settings["itetris_config_" + scenarioStr])
        ns3_config_template = os.path.join(wdirMain, ns3_dir_rel, settings["ns3_cfg"])

        itetris_config_fn = os.path.join(target_dir, ns3_dir_rel, "itetrisConfig_" + run_id + ".cfg.xml")
        ns3_config_fn = os.path.join(target_dir, ns3_dir_rel, "configTechnologies-ics_" + run_id + ".xml")

        sumo_binary = "sumo-gui" if "--gui" in sys.argv else "sumo"
        comm_binary = "lightcomm" if RUN_WITH_LIGHTCOMM else "main-inci5"
        formatDict = {"sumoConfig" : sumo_config_fn, "sumoBinary" : sumo_binary, "ns3ConfigTechnologiesFile" : ns3_config_fn, "commBinary" : comm_binary}

        fillTemplate(itetris_config_template, itetris_config_fn, formatDict)

        # Fill and copy ns3 technologies config
        formatDict = {"KPIFilePrefix" :"../" + output_dir_rel + "/" + run_id}
        fillTemplate(ns3_config_template, ns3_config_fn, formatDict)

    # Fill and copy run specific routes file
    # vTypes
    formatDict = dict([(vType + "type", "veh" + vType + p) for vType in veh_mixes[trafficMix]])
    # Demands
    formatDict.update(dict([(vType + "prob", lanes * percentage * demand_levels[demandLevel] / 3600.) for (vType, percentage) in veh_mixes[m].items()]))
    # SSM output file
    outputSSM_fn = os.path.join(target_dir, output_dir_rel, "outputSSM_" + run_id + ".xml")
    formatDict.update({"outputSSM" : outputSSM_fn})
    # target file
    routes_fn = os.path.join(target_dir, sumo_dir_rel, "routes_" + run_id + ".rou.xml")

    #print(routefile_template, routes_fn, formatDict)
    fillTemplate(routefile_template, routes_fn, formatDict)

    # Fill and copy vType files
    vTypeDir = os.path.join(target_dir, vtypefile_template_dir_rel)
    
    # NOTE/FIXME: This could be done less complicated by including the toc output file as a global value  in the sumo config
    # Templates found in directory (files with extension .tpl.xml, see script createVTypeTemplates.py)

    vTypeTemplates = [os.path.join(vTypeDir, fn) for fn in os.listdir(vTypeDir) if fn[-14:] == driverBehaviour[-2:] + ".add.tpl.xml"]

    if VERBOSE:
        print ("Filling vType templates \n %s"%str(vTypeTemplates))
    
    # Names of filled templates
    vTypeFns = [os.path.join(vTypeDir, fn[:-12] + "_" + run_id + ".add.xml") for fn in vTypeTemplates]
    # Fill all vType templates
    tocFn = os.path.join(pdir, output_dir_rel, "output_" + run_id + ".xml")
    formatDict = {"tocFile" : tocFn}
    for tpl, fn in zip(vTypeTemplates, vTypeFns):
        fillTemplate(tpl, fn, formatDict)
    
    # Create vTypeFnDict for use in sumo config template
    vTypeIDs = [os.path.basename(fn)[:-(13 + len(driverBehaviour))] for fn in vTypeTemplates]
    vTypeFnDict = dict(zip(vTypeIDs, vTypeFns))
    
    # Add add non-templated vType files
    for vt in settings["non_templated_vtypes"]:
        typeStr = "vTypes" + vt
        vTypeFnDict[typeStr] = os.path.join(vTypeDir, typeStr + "_" + driverBehaviour + ".add.xml")

    if VERBOSE:
        print ("vTypeIDs =\n %s"%str(vTypeIDs))
        print ("vTypeFnDict =\n %s"%str(vTypeFnDict))

    # Fill and copy run specific detector configuration file
    formatDict = {"detectorOutput": os.path.join(target_dir, output_dir_rel, "detectors_" + run_id + ".xml")}

    detectors_fn = os.path.join(target_dir, sumo_dir_rel, "detectors_" + run_id + ".add.xml")
    fillTemplate(detectors_template, detectors_fn, formatDict)

    # Fill and copy run specific aggregated output additional file
    addOut_fn = os.path.join(target_dir, sumo_dir_rel, addOut_fn_rel)
    emission_fn = os.path.join(target_dir, output_dir_rel, "outputEmission_" + run_id + ".xml")
    meandata_fn = os.path.join(target_dir, output_dir_rel, "outputMeandata_" + run_id + ".xml")
    
    fillTemplate(additionalsOutput_template, addOut_fn, {"outputEmission" : emission_fn, "outputMeandata" : meandata_fn})

    # Fill and copy sumo config
    formatDict = {
        "outputLaneChanges" : os.path.join(target_dir, output_dir_rel, "outputLaneChanges_" + run_id + ".xml"),
        "outputSummary" : os.path.join(target_dir, output_dir_rel, "outputSummary_" + run_id + ".xml"),
        "outputQueue" : os.path.join(target_dir, output_dir_rel, "outputQueue_" + run_id + ".xml"),
        "outputTripinfos" : os.path.join(target_dir, output_dir_rel, "outputTripinfos_" + run_id + ".xml"),
        "fcdTrajectories" : os.path.join(target_dir, output_dir_rel, "trajectories_" + run_id + ".xml"),
        "additionalsOutput" : addOut_fn,
        "routesFile" : routes_fn,
        "detectors" : detectors_fn,
        "seed" : str(seed)
        }

    formatDict.update(vTypeFnDict)

    if settings["net_file_use"] == "True":
        net_file = os.path.join(target_dir, sumo_dir_rel, settings["net_file_" + str(scenarioIndex)])
        formatDict.update({"netFile" : net_file})
    
    fillTemplate(sumo_config_template, sumo_config_fn, formatDict)

#---------------------------------------------------------------------------------------------------------------------------------------------
def copyStaticConfigFiles(target_dir):
    """ Copy all config files needed for the simulation to the target directory.
        This just places copies all contents of the config directory in the target
        directory.
    """
    global OS_TYPE, settings, config_dir, sumo_config_template_rel, routefile_template_rel, detectors_template_rel, additionalsOutput_template_rel

    # First copy everything to target_directory (assuming all is contained within the config_dir)
    copytree(config_dir, os.path.join(target_dir, settings["config_dir"]))

    # Remove template files from target directory
    if i_CS:
        template_fns = [itetris_config_template_rel, sumo_config_template_rel, routefile_template_rel, detectors_template_rel, additionalsOutput_template_rel]
    else:
        template_fns = [sumo_config_template_rel, routefile_template_rel, detectors_template_rel, additionalsOutput_template_rel]
        
    
    for fn_rel in template_fns:
        path = os.path.join(target_dir, fn_rel)
        if os.path.exists(path):
            if OS_TYPE == is_linux:
                sp.call(["rm", path])
            elif OS_TYPE == is_win:
                os.remove(path)

#---------------------------------------------------------------------------------------------------------------------------------------------
def timeCsv():

    global WORK_ON_CLUSTER, odir, settings, csv_time_logger

    # csv time log file
    if settings["times_enable"] == "True":
        times_dir = os.path.join(odir, settings["times_path"])
        mkdir_p(times_dir)

        path = os.path.join(times_dir, settings["times_csv"])

        csv_file = open(path, "a+")
        csv_time_logger = csv.writer(csv_file, delimiter = ',', quotechar = '"', quoting = csv.QUOTE_MINIMAL)

        fieldnames = ['Results', 'Scheme', 'Demand', 'Mix', 'Runs', 'Seconds', "Start", "End"]
        csv_time_logger = csv.DictWriter(csv_file, fieldnames = fieldnames)

        if os.path.getsize(path) <= 0:
            csv_time_logger.writeheader()

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n-----------------------------------------------------------------------")
    print ("BatchRunner version : %s "%(VERSION))

    find_platform()

    run_as_admin() #if is windows

    loadSettings()
    
    timeCsv()

    print("Sumo home :", SUMO_HOME)

    if i_CS:
        print("Sumo lib :", SUMO_LIB)
        print("Transaid bin :", TRANSAID_BIN)

    print("Operating system :", _platform, ", User:", USER, ", Cluster:", str(WORK_ON_CLUSTER))

    if NO_ARGS:
        print("Scenario used :", scenarioString, " -First found- ")
    else:
        print("Scenario used :", scenarioString)

    print("Vehicle mixes :", veh_mixes)
    print("Schemes :", schemes)
    print("Demand levels :", demand_levels)
    print("Simulation min - max : %s-%s:"%(sim_min,sim_max))

    if RUN_SEQUENTIAL:
        print("Sequential :",  "True")
    else:
        print("Sequential :",  "False")

    if RUN_PARALLEL:
        print("Parallel :",  "True")
    else:
        print("Parallel :",  "False")

    if RUN_CLEAN:
        print("Clean old data :",  "True")
    else:
        print("Clean old data :",  "False")

    if RUN_WITH_NO_GZIP:
        print("Zip data :",  "False")
    else:
        print("Zip data :",  "True")

    if RUN_WITH_LIGHTCOMM and not RUN_ONLY_SUMO:
        print("CommBinary :",  "lightcomm")
    else:
        print("CommBinary :",  "main-inci5")

    if RUN_ONLY_SUMO:
        print("Only SUMO :",  "true")
    else:
        print("Only SUMO :",  "false")

    if RUN_QUICK_TEST:
        print("Quick test :",  "True")
    else:
        print("Quick test :",  "False")

    if ALL_LOAD_FROM_JSON:
        print("Json  parameters :",  "True")
    else:
        print("Json  parameters :",  "False")        

    if VERBOSE:
        print("scenarioIndex", scenarioIndex)
        print("Working dir = %s"%wdir)
        print("Output dir = %s"%odir)

    # Create subdirectory structure: params/mix/demand/; fail if it exist and start simulations
    start_time = time.time()
    start_dateTime = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")

    # Total number of run-groups (Nrun runs per parameter combination) to be conducted (for ETA calculation)
    NRunsTotal = len(demand_levels) * len(veh_mixes) * len(schemes)
    NRunsTotalRemaining = NRunsTotal
    runsCount = 0
    print("Total parameter-combinations to be scanned : %s"%NRunsTotal)
    print("-----------------------------------------------------------------------\n")

    for d in demand_levels:
        print("Demand : %s"%d)
        ddir = os.path.join(rdir, d)

        for m in veh_mixes:
            print("Mix : %s"%m)
            demand_time = datetime.datetime.now()

            mdir = os.path.join(ddir, m)
            runsCount = 0
            for p in schemes:
                print("Scheme : %s"%p)
                pdir = os.path.join(mdir, p)
                
                # Create output directory for the following runs
                output_dir = os.path.join(pdir, output_dir_rel)
                # ~ sp.call(["mkdir", "-p", output_dir])
                mkdir_p(output_dir)

                # copy vType files corresponding to the parameter scheme
                copyStaticConfigFiles(pdir)
                # run simulations
                working_set = {}

                for (i, seed) in seedMap.items():
                    runsCount += 1
                    print ("Run : %s, Seed : %s"%(i, seed))
                    os.chdir(pdir)

                    # Copy custom config files to the directory for this parameter combination
                    createConfigFilesFromTemplates(pdir, d, m, p, i, seed, scenarioString)

                    # Call to start simulation:
                    run_id = makeOutputSuffix(int(m[-1]), settings["demands_map"][d], p, i)

                    # Environment variables
                    env = dict(os.environ)
                    iCSCall = []

                    if i_CS:
                        iCSCall = ["iCS", "-c", itetris_config_fn]

                        env.update({
                            "SUMO_HOME" : os.path.join(SUMO_HOME),
                            "PATH" : os.path.join(TRANSAID_BIN),
                            "LD_LIBRARY_PATH" : os.path.join(SUMO_LIB)
                            })
                    else:
                        env.update({"SUMO_HOME" : os.path.join(SUMO_HOME)})

                    argv = [pythonStr, runner_fn, "-v", "-c", sumo_config_fn, "--seed", str(seed)]

                    if RUN_WITH_GUI:
                        argv.append("--gui")

                    if RUN_WITH_DEBUG:
                        argv.append("--debug")

                    argv.append("--" + scenarioString)
                    argv.append("--info=Run_{}->{}->{}->{}".format(i, d, m, p))

                    # execute iCS within the itetris directory
                    os.chdir(os.path.join(pdir, config_dir_rel))

                    if VERBOSE:
                        print("Calling: %s"%" ".join(argv))

                    proccess = 0
                    if RUN_PARALLEL:
                        ## Call in separate processes, writing to output file
                        out_fn = os.path.join(pdir, output_dir_rel, "stdout_" + run_id + ".txt")
                        
                        if VERBOSE:
                            print("Writing output to file '%s'"%out_fn)

                        outfile = open(out_fn, "w") #w

                        if i_CS:
                            working_set[i] = (sp.Popen(iCSCall, stdout = outfile, stderr = outfile, env = env), outfile, iCSCall)
                        else:
                            working_set[i] = (sp.Popen(argv, stdout = outfile, stderr = outfile, env = env), outfile, argv)
                        
                        runCount += 1
                    else:
                        ## Call one at a time writing to stdout (e.g. for testing)
                        Ntries = 0
                        done = False
                        while not done:
                            Ntries += 1

                            if i_CS:
                                returnCode = sp.call(iCSCall, env = env)
                            else:
                                returnCode = sp.call(argv, env = env)
                            
                            if (returnCode != 0):
                                print ("Run %s failed with code %s. Restarting..."%(i, returnCode))
                                done = Ntries >= maxTries
                            else:
                                done = True

                        print("Run %s completed...  (%s->%s->%s)"%(i, d, m, p))
                        runCount += 1

                print("\nWaiting for all simulations for %s->%s->%s to complete...\n"%(d, m, p))
                
            #   '/home/vkaragounis/Desktop/transaid/TransAIDScenarios/UC_42_GUI/Motorway/results/manual/los_B/mix_0/FSP/config/sumo/additionalsOutput_TD_1_TM_0_DB_FSP_seed_0.add.xml'!

                # wait for all run to end, restart failed calls
                # (FIXME: reason for failing is unclear and fails are not reliably reproducible)
                # -> Might be resolved
                Ntries = 0
                while working_set:
                    i, v = working_set.popitem()
                    waitProcess, outfile, argv = v
                    returnCode = waitProcess.poll()

                    if returnCode is None:
                        # process not yet finished, push it back
                        working_set[i] = v
                        continue

                    #returnCode = waitProcess.wait()

                    # use this instead of wait
                    res = waitProcess.communicate()
                    returnCode = waitProcess.returncode

                    outfile.close()

                    if returnCode > 1 and Ntries < maxTries:

                        print("res =", res)

                        # restart process
                        print ("Run %s failed with code %s. (outfile='%s') Restarting..."%(i, returnCode, outfile))
                        Ntries += 1
                        out_fn = outfile.name
                        outfile = open(out_fn, "w")

                        if i_CS:
                            working_set[i] = (sp.Popen(iCSCall, stdout = outfile, stderr = outfile, env = env), outfile, iCSCall)
                        else:
                            working_set[i] = (sp.Popen(argv, stdout = outfile, stderr = outfile, env = env), outfile, argv)
                    else:
                        if not RUN_WITH_GUI:
                            out_fn = outfile.name
                            outfile = open(out_fn, "r")
                            data = outfile.readlines()
                            outfile.close()
                            for line in data:
                                print(line, end = " ")

                        print("Run %s completed..."%i)

                if RUN_WITH_NO_GZIP == False:
                    # gzip all output for this run
                    print("Gzipping all files in " + pdir + "...")
                    
                    time.sleep(3.0) # don't gzip config files before read

                    if OS_TYPE == is_linux:
                        callList=["gzip", "-r9", "-f"] + [os.path.join(pdir, fn) for fn in os.listdir(pdir)]
                        sp.call(callList)
                    elif OS_TYPE == is_win:
                         for root, dirs, files in os.walk(pdir):
                            for file in files:
                                path = os.path.join(root, file)
                                with open(path, 'rb') as f_in:
                                    with gzip.open(path + ".gz", 'wb') as f_out:
                                        shutil.copyfileobj(f_in, f_out)
                                os.remove(path)

                # Report ETA
                NRunsTotalRemaining -= 1
                elapsed = time.time() - start_time
                estimatedPerRun = elapsed/(NRunsTotal - NRunsTotalRemaining)
                estimatedRemaining = estimatedPerRun * NRunsTotalRemaining
                ETA = time.gmtime(time.time() + estimatedRemaining)
                print ("Estimated time remaining : %s h. (ETA: %s)\n"%(estimatedRemaining/3600., str(ETA.tm_mon) + "-" + str(ETA.tm_mday) + ", " + str(ETA.tm_hour + 1) + ":" + str(ETA.tm_min) + ":" + str(ETA.tm_sec)))

                if csv_time_logger:
                    demand_elapsed_time = datetime.datetime.now() - demand_time
                    mix_num_str = m
                    mix_num_str = mix_num_str.replace('mix_', '')
                    res_csv = results_dir_rel.replace('results_', '')
                    csv_time_logger.writerow({'Results': res_csv, 'Scheme': p, 'Demand': str(d), 'Mix': mix_num_str, 'Runs': str(runsCount), 'Seconds': str(demand_elapsed_time.seconds), 'Start': str(start_dateTime), 'End': str(datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))})

    # report elapsed time
    end_time = time.time()
    elapsed = end_time - start_time
    print ("Elapsed time : %s (for %s runs)\n"%(elapsed, runCount))
    print("-----------------------------------------------------------------------\n")
