#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    runner.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import time
import random
import optparse
import xml.etree.ElementTree as ET
# we need to import python modules from the $SUMO_HOME/tools directory
try:
    sys.path.append(os.path.join(os.path.dirname(
        __file__), '..', '..', '..', '..', "tools"))  # tutorial in tests
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
        os.path.dirname(__file__), "..", "..", "..")), "tools"))  # tutorial in docs
    from sumolib import checkBinary  # noqa
except ImportError:
    sys.exit(
        "please declare environment variable 'SUMO_HOME' as the root directory of your sumo installation (it should contain folders 'bin', 'tools' and 'docs')")

import traci

# Vehicle ID start-string to identify vehicles that are informed by TransAID (none for baseline)
AV_identifiers = [] 
# Map from vehicle ID start-string to identify vehicles, that will perform a ToC in the scenario, 
#     to ToCLead time (in secs) for the corresponding vehicles 
ToC_lead_times = {"CAVToC.":10.0, "CVToC.":0.0}
# Probability, that a given CV or CAV has to perform a ToC
ToCprobability = 0.75
global options



def getIdentifier(fullID, identifierList):
    #~ print ("getIdentifier(%s, %s)"%(fullID, identifierList))
    for start_str in identifierList:
        if fullID.startswith(start_str):
            #~ print("found %s"%start_str)
            return start_str

def initToCs(vehSet, handledSet, edgeID, distance):
    ''' For all vehicles in the given set, check whether they passed the cross section, where a ToC should be triggered and trigger in case. 
    '''
    global options
    newTORs = []
    for vehID in vehSet:
        distToTOR = traci.vehicle.getDrivingDistance(vehID, edgeID, distance)
        if distToTOR < 0.:
            handledSet.add(vehID)
            ToCVehicleType = getIdentifier(vehID, ToC_lead_times.keys())
            newTORs.append(vehID)
            if ToCVehicleType is not None:
                # Request a ToC for the vehicle
                requestToC(vehID, ToC_lead_times[ToCVehicleType])
                if options.verbose:
                    t = traci.simulation.getCurrentTime() / 1000.
                    print("## Requesting ToC for vehicle '%s'!" % (vehID))
                    print("Requested ToC of %s at t=%s (until t=%s)" % (vehID, t, t + float(ToC_lead_times[ToCVehicleType])))
                    printToCParams(vehID, True)
                continue

            # No ToC vehicle
            # TODO: In non-baseline case add a check for the transAID-effectedness (see AV_identifiers)
            #~ TransAIDAffectedType = getIdentifier(vehID, AV_identifiers)
            #~ if TransAIDAffectedType is not None:
                #~ if options.verbose:
                    #~ print("## Informing AV '%s' about alternative path!" % (vehID))
                #~ # vClass change indicates that vehicle may run on restricted lane (UC1), which prohibits vClass custom1
                #~ traci.vehicle.setVehicleClass(vehID, "passenger")
    return newTORs
    

def outputNoToC(t, vehIDs, tocInfos):
    for vehID in vehIDs:
        el = ET.Element("noToC")
        el.set("time", str(t))
        el.set("vehID", vehID)
        tocInfos.append(el)
        

def outputTORs(t, vehIDs, tocInfos):
    for vehID in vehIDs:
        el = ET.Element("TOR")
        el.set("time", str(t))
        el.set("vehID", vehID)
        lastRouteEdgeID = traci.vehicle.getRoute(vehID)[-1]
        lastRouteEdgeLength = traci.lane.getLength(lastRouteEdgeID+"_0")
        distTillRouteEnd = str(traci.vehicle.getDrivingDistance(vehID, lastRouteEdgeID, lastRouteEdgeLength))
        el.set("remainingDist", distTillRouteEnd)
        tocInfos.append(el)
    
def outputToCs(t, vehIDs, tocInfos):
    for vehID in vehIDs:
        el = ET.Element("ToC")
        el.set("time", str(t))
        el.set("vehID", vehID)
        lastRouteEdgeID = traci.vehicle.getRoute(vehID)[-1]
        lastRouteEdgeLength = traci.lane.getLength(lastRouteEdgeID+"_0")
        distTillRouteEnd = str(traci.vehicle.getDrivingDistance(vehID, lastRouteEdgeID, lastRouteEdgeLength))
        el.set("remainingDist", distTillRouteEnd)
        ToCState = traci.vehicle.getParameter(vehID, "device.toc.state")
        if ToCState != "MRM":
            # vehicle is not performing an MRM
            el.set("MRM", str(0.))
        else:
            # vehicle was performing an MRM, determine the time for which it performed the MRM
            ToCVehicleType = getIdentifier(vehID, ToC_lead_times.keys())
            leadTime = ToC_lead_times[ToCVehicleType]
            MRMDuration = float(traci.vehicle.getParameter(vehID, "device.toc.responseTime")) - leadTime
            el.set("MRM", str(MRMDuration))
        tocInfos.append(el)

def run(downwardEdgeID, distance, tocInfos): #, upwardEdgeID, upwardDist):
    """execute the TraCI control loop
       tocInfos is an xml element tree to be used to output infos on the toc-processes.
    """
    # this is the list of vehicle states for the scenario, which each AV will traverse
    downwardToCPending = set(traci.vehicle.getIDList())
    downwardToCRequested = set()
    #~ downwardTocPerformed = set()
    #~ upwardToCPending = set()
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
        t = traci.simulation.getCurrentTime()/1000.
        step += 1
        if options.debug:
            print("\n---------------------------------\nsimstep: %s" % step)
        # Keep book of entered AVs
        arrivedVehs = [vehID for vehID in traci.simulation.getArrivedIDList() if getIdentifier(vehID, ToC_lead_times.keys()) is not None]
        downwardToCRequested.difference_update(arrivedVehs)
        downwardToCPending.difference_update(arrivedVehs)
        departedToCVehs = [vehID for vehID in traci.simulation.getDepartedIDList() if getIdentifier(vehID, ToC_lead_times.keys()) is not None]
        #~ if (len(traci.simulation.getDepartedIDList())>0):
            #~ print("departed = %s"%traci.simulation.getDepartedIDList())
            #~ print("departedToCVehs = %s"%departedToCVehs)
        noToC = []
        for vehID in departedToCVehs:
            # set ToC vehicle class to custom1
            #~ print("Departed ToC vehicle '%s'"%vehID)
            if (random.random() < ToCprobability):
                # This vehicle has to perform a ToC
                traci.vehicle.setVehicleClass(vehID, "custom1")
            else:
                # vehicle will manage situation witout a ToC
                noToC.append(vehID)
                
        departedToCVehs = [vehID for vehID in departedToCVehs if vehID not in noToC]
        downwardToCPending.update(departedToCVehs)
        
        # provide the ToCService at the specified cross section for informing the lane closure
        newTORs = initToCs(downwardToCPending, downwardToCRequested, downwardEdgeID, distance)
        downwardToCPending.difference_update(downwardToCRequested)

        # keep book on performed ToCs and trigger best lanes update by resetting the route
        downwardToCPerformed = set()
        for vehID in downwardToCRequested:
            if traci.vehicle.getVehicleClass(vehID) == "passenger":
                if options.debug:
                    print("Downward transition completed for vehicle '%s'" % vehID)
                downwardToCPerformed.add(vehID)
                traci.vehicle.updateBestLanes(vehID)
        downwardToCRequested.difference_update(downwardToCPerformed)
        #~ upwardToCPending.update(downwardTocPerformed) # no upwards ToC for baseline

        # add xml output
        outputNoToC(t, noToC, tocInfos.getroot())
        outputTORs(t, newTORs, tocInfos.getroot())
        outputToCs(t, downwardToCPerformed, tocInfos.getroot())
        
        #~ # provide ToCService to the upwardTransitions
        #~ upwardTocPerformed = set()
        #~ doToC(upwardToCPending, upwardTocPerformed, 0., upwardEdgeID, upwardDist)
        #~ upwardToCPending.difference_update(upwardTocPerformed)
        if options.debug:
            print("downwardToCRequested=%s" % downwardToCRequested)
            #~ print("Downward ToC performed: %s" % str(sorted(downwardTocPerformed)))
            #~ print("Upward ToC performed: %s" % str(sorted(upwardTocPerformed)))
            print("DownwardToCPending:%s" % str(sorted(downwardToCPending)))
            #~ print("upwardToCPending:%s" % str(sorted(upwardToCPending)))
        #~ upwardToCPending.difference_update(arrivedVehs)


def requestToC(vehID, timeUntilMRM):
    traci.vehicle.setParameter(vehID, "device.toc.requestToC", str(timeUntilMRM))


def printToCParams(vehID, only_dynamic=False):
    holder = traci.vehicle.getParameter(vehID, "device.toc.holder")
    manualType = traci.vehicle.getParameter(vehID, "device.toc.manualType")
    automatedType = traci.vehicle.getParameter(vehID, "device.toc.automatedType")
    responseTime = traci.vehicle.getParameter(vehID, "device.toc.responseTime")
    recoveryRate = traci.vehicle.getParameter(vehID, "device.toc.recoveryRate")
    initialAwareness = traci.vehicle.getParameter(vehID, "device.toc.initialAwareness")
    mrmDecel = traci.vehicle.getParameter(vehID, "device.toc.mrmDecel")
    currentAwareness = traci.vehicle.getParameter(vehID, "device.toc.currentAwareness")
    state = traci.vehicle.getParameter(vehID, "device.toc.state")
    speed = traci.vehicle.getSpeed(vehID)

    print("time step %s" % traci.simulation.getCurrentTime())
    print("ToC device infos for vehicle '%s'" % vehID)
    if not only_dynamic:
        print("Static parameters:")
        print("  holder = %s" % holder)
        print("  manualType = %s" % manualType)
        print("  automatedType = %s" % automatedType)
        print("  responseTime = %s" % responseTime)
        print("  recoveryRate = %s" % recoveryRate)
        print("  initialAwareness = %s" % initialAwareness)
        print("  mrmDecel = %s" % mrmDecel)
        print("Dynamic parameters:")
    print("  currentAwareness = %s" % currentAwareness)
    print("  currentSpeed = %s" % speed)
    print("  state = %s" % state)


def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("-c", dest="sumocfg", help="sumo config file")
    optParser.add_option("--suffix", dest="suffix", help="suffix for output filenames")
    optParser.add_option("--seed", dest="seed", help="seed value")
    optParser.add_option("--additionals", dest="additionals", help="filename list for additional files to be loaded")
    optParser.add_option("--routes", dest="routes", help="filename list of route files to be loaded")
    optParser.add_option("--gui", action="store_true",
                         default=False, help="run the commandline version of sumo")
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-d", "--debug", action="store_true", dest="debug",
                         default=False, help="debug messages")
    options, args = optParser.parse_args()
    return options


# this is the main entry point of this script
if __name__ == "__main__":
    options = get_options()
    downwardEdgeID = None
    distance = None
    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run
    if options.gui:
        sumoBinary = checkBinary('sumo-gui')
    else:
        sumoBinary = checkBinary('sumo')

    traci.start([sumoBinary, "-c", options.sumocfg, "--seed", options.seed, 
    "--summary", "outputSummary%s.xml"%options.suffix, "-a", options.additionals, "-r", options.routes,
    "--lanechange-output", "outputLaneChanges%s.xml"%options.suffix,
    "--queue-output", "outputQueue%s"%options.suffix])
    
    # provide etree for ToC infos
    tocInfos = ET.ElementTree(ET.Element("tocInfos"))

    downwardEdgeID = "e0"
    distance = 2300. 
    #~ upwardEdgeID = "e0"
    #~ upwardDist = 3500.0

    run(downwardEdgeID, distance, tocInfos) #, upwardEdgeID, upwardDist)

    traci.close()
    tocInfos.write("outputToCs%s"%options.suffix)
    sys.stdout.flush()
    
    
