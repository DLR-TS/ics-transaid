#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Evangelos Mintsis
# @author  Vasilios Karagounis
# @date    2020-6-30
# @version $Id$

import os
import sys
import time
import datetime
import random
import optparse
import json

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
# we need to import python modules from the $SUMO_HOME/tools directory
try:
    # sys.path.append(os.path.join(os.path.dirname(
    #     __file__), '..', '..', '..', '..', "tools"))  # tutorial in tests
    # sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
    #     os.path.dirname(__file__), "..", "..", "..")), "tools"))  # tutorial in docs
    # pdb.set_trace()
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME"), "tools"))
    from sumolib import checkBinary  # noqa
except ImportError:
    sys.exit("please declare environment variable 'SUMO_HOME' as the root directory of your sumo installation (it should contain folders 'bin', 'tools' and 'docs')")
#-------------------------------------------------------------------------------------------
import traci
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------

#1.0.0 initial version
VERSION = '1.0.0'
INFO = ""

step = 0
options = []
teleporters = set()

allScenarios = []
settings = {}

unique_array_of_veh = {} #enumerate the veh names adn show in debug

scenario = ""
wdir =  os.path.dirname(os.path.realpath(__file__))

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def readScenarios():
    
    global allScenarios

    path = os.path.join(wdir, "settings/batchRunner.json")

    if os.path.exists(path):
        with open(path, 'r') as f:
            with open(path, 'r') as f:
                try:
                    data = json.load(f)
                except ValueError as e:
                    print("Json error :", e)
                    sys.exit(1)
            
            allScenarios = data["args_scenarios"]
            print("Scenarios read from json file :", allScenarios)
    else:
         print("path not found (readScenarios) :", path)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def loadFileSettings():
    
    global settings, scenario

    path = os.path.join(wdir, "config/settings/runner-" + scenario + ".json")

    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                settings = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            print("Json settings path:", path)
    else:
        print("Path not found (loadFileSettings) :", path)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def loadSettings():

    global settings, scenario

    readScenarios()

    if len(sys.argv) > 1:
        for arg in sys.argv[1:]:
            if (arg in allScenarios):
                scenario = arg.replace('-', '')
                print("Current scenario :", scenario)
                break

    if not scenario:
        print("Error: scenario not found")
        sys.exit(2)

    loadFileSettings()

    if not settings:
        print("Error : Unable to read json settings file")
        print("Path :", wdir)
        sys.exit(3)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def getIdentifier(vehId, identifierList):
    for start_str in identifierList:
        if vehId.startswith(start_str):
            return start_str

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def debugPrint(allow, *args):
    if allow:
        print(*args)


#-------------------------------------------------------------------------------------------
# Tell traci to start tracking a vehicle at a specific edge or as soon it appears on net
#-------------------------------------------------------------------------------------------
class Views():
    def __init__(self):

        self.views = []
        self.vehicleStartedTracked = {}
        self.vehicleEndTracked = {}
        self.offset = []

    #-------------------------------------------------------------------------------------------
    def read(self, settings):
        
        if settings and options.gui:
            for view in settings:
                if ("view" in view):
                    self.views.append(view) 

            for id, v in enumerate(self.views):
                strView = "View #" + str(id)
                if traci.gui.hasView(strView):
                    offset = traci.gui.getOffset(strView)

                    self.offset.append(offset[0] + settings[v]["offsetX"])
                    self.offset.append(offset[1] + settings[v]["offsetY"])

                    traci.gui.setOffset(strView, self.offset[0], self.offset[1])
                    traci.gui.setZoom(strView, settings[v]["zoom"])
                    
                    if settings[v]["trackVehicle"]:
                        self.vehicleStartedTracked[v] = True
                        self.vehicleEndTracked[v] = False
                        print("Start tracking:", settings[v]["trackVehicle"])

                id+=1
                
    #-------------------------------------------------------------------------------------------
    def update(self, vehs):
        
        if not options.gui or not self.vehicleStartedTracked:
            return
        
        for id, v in enumerate(self.vehicleStartedTracked):
            if self.vehicleStartedTracked[v]:
                if settings[v]["trackVehicle"] in vehs:
                    strView = "View #" + str(id)
                    if settings[v]["trackEdge"]:
                        checkDistance = traci.vehicle.getDrivingDistance(settings[v]["trackVehicle"], settings[v]["trackEdge"], settings[v]["trackPos"])
                        if checkDistance <= 0.0:
                            traci.gui.trackVehicle(strView, settings[v]["trackVehicle"])
                            self.vehicleStartedTracked[v] = False
                            self.vehicleEndTracked[v] = True
                            debugPrint(options.debug, 'Traci start tracking :', settings[v]["trackVehicle"])
                    else:
                        traci.gui.trackVehicle(strView, settings[v]["trackVehicle"])
                        self.vehicleStartedTracked[v] = False
                        self.vehicleEndTracked[v] = True
                        debugPrint(options.debug, 'Traci start tracking :', settings[v]["trackVehicle"])
            else:
                if self.vehicleEndTracked[v]:
                    if settings[v]["trackVehicle"] not in vehs:
                        strView = "View #" + str(id)
                        self.vehicleEndTracked[v] = False
                        traci.gui.setOffset(strView, self.offset[0], self.offset[1])
        

views = Views()

#-------------------------------------------------------------------------------------------
# Vehicle class
#-------------------------------------------------------------------------------------------
class Vehicle():
    def __init__(self, vehId, options):

        global settings, unique_array_of_veh

        debugPrint(options.debug, 'Init class vehicle :', vehId)
        
        self.options = options
        self.ID = vehId # this is a string

        self.isLV = False
        self.isCAV = False
        self.isCV_ToC = False
        self.isCAV_ToC = False

        if (self.ID.startswith('LV')):
            self.isLV = True
            if options.debug:
                unique_array_of_veh["LV"] = "1"

        elif (self.ID.startswith('CVToC')):
            self.isCV_ToC = True
            self.isCAV = True
            if options.debug:
                unique_array_of_veh["CVToC"] = "1"

        elif (self.ID.startswith('CAVToC')):
            self.isCAV_ToC = True
            self.isCAV = True
            if options.debug:
                unique_array_of_veh["CAVToC"] = "1"

        else :
            debugPrint(options.debug, "WARNING: vehicle id name unidentified :", self.ID)

        if options.debug:
            print(unique_array_of_veh)
        
        self.IDjson = vehId.split('.', 1)[0]

        self.handleTeleported = settings["handleTeleported"] == "True"
        self.tocDebug = settings["tocDebug"] == "True"       

        self.isTocEnable = settings[self.IDjson]["tocEnable"] == "True"
        self.isExitEnable = settings[self.IDjson]["exitEnable"] == "True"

        #-----------------------------
        # teleportation
        #-----------------------------
        self.isTeleported = False
        #-----------------------------
        # check exit point
        #-----------------------------
        self.exitEdge = ""
        self.exitPos = 0.0
        self.exitDone = False

        if self.isExitEnable:
            self.exitPos = settings[self.IDjson]["exitPos"]
            self.exitEdge = settings[self.IDjson]["exitEdge"]

        #-----------------------------
        # Transition of Control - toc - Take Over Control
        #-----------------------------
        self.tocProbability = 0.0
        self.tocLeadTime = 0.0 
        self.tocDownwardEdge = ""
        self.tocDownwardPos = 0.0
        self.downwardTOCDone = False

        if self.isTocEnable:
            self.tocProbability = settings[self.IDjson]["tocProbability"]
            self.tocLeadTime = settings[self.IDjson]["tocLeadTime"] 
            self.tocDownwardEdge = settings[self.IDjson]["tocDownwardEdge"]
            self.tocDownwardPos = settings[self.IDjson]["tocDownwardPos"]

    #-------------------------------------------------------------------------------------------
    #
    #-------------------------------------------------------------------------------------------
    def updateTeleportation(self):

        if not self.handleTeleported:
            return

        if (self.ID in teleporters):
            self.isTeleported = True
            debugPrint(options.debug, "Teleported vehicle :", self.ID)
        else:
            self.isTeleported = False

    #-------------------------------------------------------------------------------------------
    #
    #-------------------------------------------------------------------------------------------
    def updateExit(self):

        if not self.isExitEnable:
            return

        if not self.exitDone:
            checkDistance = traci.vehicle.getDrivingDistance(self.ID, self.exitEdge, self.exitPos)
            if checkDistance <= 0.0:
                self.exitDone = True
    
    #-------------------------------------------------------------------------------------------
    # perform "take over control" (TOC) downward or upward
    #-------------------------------------------------------------------------------------------
    def updateTOC(self):

        if not self.isTocEnable:
            return

        if not self.downwardTOCDone:
            checkDistance = traci.vehicle.getDrivingDistance(self.ID, self.tocDownwardEdge, self.tocDownwardPos)
            # enter in downward ToC area
            if checkDistance <= 0.0:
                self.downwardTOCDone = True

                if (random.random() < self.tocProbability):
                    traci.vehicle.setParameter(self.ID, "device.toc.requestToC", str(self.tocLeadTime)) # This vehicle has to perform a ToC
                    
                    debugPrint(self.tocDebug, 'Downward TOC done at vehicle :', self.ID)

                else:
                    # vehicle will manage situation without a ToC. There is no need to perform a upward TOC as well

                    debugPrint(self.tocDebug, 'Downward or updward TOC skipped due to probability, vehicle :', self.ID)

    #-------------------------------------------------------------------------------------------
    # call this in every frame for each vehicle class
    #-------------------------------------------------------------------------------------------
    def update(self):

        self.updateExit()

        self.updateTeleportation()

        self.updateTOC()

vehicles = {}

#-------------------------------------------------------------------------------------------
# find all teleported vehicles in flow
#-------------------------------------------------------------------------------------------
def findTeleporters():

    # returns a list of IDs of vehicles which started to teleport in this time step_.
    teleporters.update(traci.simulation.getStartingTeleportIDList())

    #returns a list of IDs of vehicles which ended to be teleported in this time step_.
    teleporters.difference_update(traci.simulation.getEndingTeleportIDList())

#-------------------------------------------------------------------------------------------
# find all vehicles in flow
#-------------------------------------------------------------------------------------------
def findVehicles():

    global options
    for vehID in traci.simulation.getDepartedIDList():
        if vehID not in vehicles:
            vehicles[vehID] = Vehicle(vehID, options) # Add new vehicles arrived
            debugPrint(options.debug, "Vehicle added :", vehID)

    for vehID in traci.simulation.getArrivedIDList():
        if vehID in vehicles:
            vehicles.pop(vehID, None)   # Remove vehicles leaved
            debugPrint(options.debug, "Vehicle removed :", vehID)

            #vehiclesSort.pop(vehID, None)
    
    views.update(vehicles)

#-------------------------------------------------------------------------------------------
# change position or dissapear some pois
#-------------------------------------------------------------------------------------------
def changeScene():

    global settings

    for rmItem in settings["removeItems"]:
        traci.poi.remove(rmItem)
        debugPrint(options.debug, "Item removed :", rmItem)

    for rposItem in settings["rePositionItems"]:
        traci.poi.setPosition(rposItem, settings["rePositionItems"][rposItem]["xPos"], settings["rePositionItems"][rposItem]["yPos"])
        debugPrint(options.debug, 'Item repositioned :', rposItem, ", xPos :", settings["rePositionItems"][rposItem]["xPos"], ", yPos :", settings["rePositionItems"][rposItem]["yPos"])

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def run():

    global step

    while traci.simulation.getMinExpectedNumber() > 0:

        findVehicles()

        findTeleporters()

        for vehID in vehicles:
            vehicles[vehID].update()

        traci.simulationStep()

        step += 1


#---------------------------------------------------------------------------------------------------------------------------------------------
def readOptions():

    global allScenarios, options, INFO

    optParser = optparse.OptionParser()

    optParser.add_option("-c", dest = "sumocfg", help = "sumo config file")
    optParser.add_option("--suffix", dest = "suffix", help = "suffix for output filenames")
    optParser.add_option("--seed", dest = "seed", help = "seed value")
    optParser.add_option("--additionals", dest = "additionals", help = "filename list for additional files to be loaded")
    optParser.add_option("--routes", dest = "routes", help = "filename list of route files to be loaded")
    optParser.add_option("-v", "--verbose", action = "store_true", dest = "verbose", default = False, help = "tell me what you are doing")
    optParser.add_option("-d", "--debug", action = "store_true", dest = "debug", default = False, help = "debug messages")
    optParser.add_option("--gui", action = "store_true", dest = "gui", default = False, help = "run the commandline version of sumo")
    optParser.add_option("--qt", action = "store_true", dest = "qt", default = False, help = "run a quick test")
    optParser.add_option("--info=", dest = "info", help = "get info from butch runner")

    #optParser.add_option("--lanechange-output.xy", dest = "", help = "")

    for arg in allScenarios:
        optParser.add_option(arg, action = "store_true", dest = arg.replace('-', ''),  default = False, help = "")

    for arg in sys.argv[1:]:
        if arg.startswith("--info="):
            INFO = arg.replace('--info=', '')

    options, args = optParser.parse_args()
    args = 0

#---------------------------------------------------------------------------------------------------------------------------------------------
# this is the main entry point of this script
#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    print("\n-----------------------------------------------------------------------")
    print ("Runner version : %s"%(VERSION))

    loadSettings()

    readOptions()

    # this script has been called from the command line. It will start sumo as a server, then connect and run
    if  options.gui:
        sumoBinary = checkBinary('sumo-gui')
    else:
        sumoBinary = checkBinary('sumo')

    sumo_args = [sumoBinary, "-c", options.sumocfg
                 # "--seed", options.seed,
                 # "--summary", "outputSummary%s"%options.suffix,
                 # "--device.toc.file", "outputToC%s"%options.suffix,
                 # "-a", options.additionals,
                 # "-r", options.routes,
                 # "--lanechange-output", "outputLaneChanges%s"%options.suffix,
                 # "--queue-output", "outputQueue%s.xml"%options.suffix
                 # "--lanechange-output.xy"
                 ]

    #TODO" --lanechange-output.xy" working with args...not from cfg

    print("Info :", INFO)
    print("\nStarting sumo")

    start_time = datetime.datetime.now()    

    traci.start(sumo_args)
    
    changeScene()

    #setViews()
    views.read(settings)

    while traci.simulation.getMinExpectedNumber() == 0:
        traci.simulationStep()
        step += 1

    run()

    traci.close()

    elapsed_time = datetime.datetime.now() - start_time
    
    print('Steps:', step, ' seconds:', elapsed_time.seconds)
    print('Ending sumo')
    print("-----------------------------------------------------------------------\n")
    
    sys.stdout.flush()
   
