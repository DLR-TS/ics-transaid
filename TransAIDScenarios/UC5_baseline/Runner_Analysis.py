import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import seaborn as sns
import os
#import pyexcel as pe

#upper bound (speed [Km/h]) to be printed in y axis (Time - Speed per Edge)
yLimSpeed = 145.0
# For UC5.1 probably you need the following parameter
# laneChangeInputStr = 'lane_changes_'
# endingXMLLaneChange = '.xml.xml'
# endingXMLSummary = '.xml.xml'

# For every other UC use these
laneChangeInputStr = 'outputLaneChanges'
endingXMLLaneChange = '.xml.xml'
endingXMLSummary = '.xml.xml'

driverBehaviourList = ['PE','PS','MSE','OE','OS']
seedSize = 10
dictLOS={'Level_of_Service_A':0,
         'Level_of_Service_B':1,
         'Level_of_Service_C':2
         }
dictMIX={'Traffic_Mix_1':0,
         'Traffic_Mix_2':1,
         'Traffic_Mix_3':2
         }
trafficMixSize = len(dictMIX)
trafficDemandSize = len(dictLOS)
showCrpt = True

def meanValueEmmisions(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand ->',trafficDemand)
        for trafficMix in range(trafficMixSize):
            print('trafficMix',trafficMix)
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                CO2 = 0.0
                numFiles =0

                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ 'outputEmission',
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + '.xml')
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    #print(root.findall('*/edge'))
                    numFiles += 1

                    for edge in root.findall('*/edge'):
                        #print(edge.get('CO2_abs'))
                        #devided by 10^6 [mg]->[tones]
                        CO2 += float( edge.get('CO2_abs') ) / 1000000
                    #normalized
                    #CO2_perEdge /= len( root.findall('*/edge') )

                if (numFiles>0.0): CO2 /= numFiles
                print('CO2 = ',CO2,'[Kg] parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = CO2

    return rtn

def meanValueEmmisionsPerVeh(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand ->',trafficDemand)
        for trafficMix in range(trafficMixSize):
            print('trafficMix',trafficMix)
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                CO2output = 0.0
                numFiles =0

                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    emissionList = [ 'outputEmission',
                                     'trafficMix' , str( trafficMix  ),
                                     'trafficDemand' , str( trafficDemand),
                                     'driverBehaviour' , driverBehaviour,
                                     'seed' , str( seed )
                    ]
                    emissionStr = '_'.join( emissionList )

                    summaryList = [ 'outputSummary',
                                    'trafficMix' , str( trafficMix  ),
                                    'trafficDemand' , str( trafficDemand),
                                    'driverBehaviour' , driverBehaviour,
                                    'seed' , str( seed )
                    ]
                    summaryListStr = '_'.join( summaryList )

                    meanDataList = [ 'outputMeandata',
                                     'trafficMix' , str( trafficMix  ),
                                     'trafficDemand' , str( trafficDemand),
                                     'driverBehaviour' , driverBehaviour,
                                     'seed' , str( seed )
                    ]
                    meanDataListStr = '_'.join( meanDataList )

                    try:
                        emissionTree = ET.parse( emissionStr + '.xml')
                        emissionRoot = emissionTree.getroot()
                    except:
                        if (showCrpt):print('file -> ', emissionStr,' is corrupted or missing ', file = f )
                        print(emissionStr)
                        continue

                    try:
                        meanDataTree = ET.parse(meanDataListStr + '.xml')
                        meanDataRoot = meanDataTree.getroot()
                    except :
                        if (showCrpt):print('file -> ', meanDataListStr,' is corrupted or missing ', file = f )
                        print(meanDataListStr)
                        continue

                    try:
                        summaryTree = ET.parse( summaryListStr + endingXMLSummary )
                        summaryRoot = summaryTree.getroot()
                    except :
                        if (showCrpt):print('file -> ', summaryListStr,' is corrupted or missing ', file = f )
                        print(summaryListStr + '.xml')
                        continue

                    #print(root.findall('*/edge'))
                    numFiles += 1
                    #print('success')

                    # aggregate total CO2 emissions [g] for a specific simulation
                    CO2 = 0.0
                    for edge in emissionRoot.findall('*/edge'):
                        #devided by 10^3 [mg]->[g]
                        #print('    CO2 ->',CO2)
                        CO2 += float( edge.get('CO2_abs') ) / 1000
                        #print('CO2_perVeh ->', edge.get('CO2_perVeh') )
                    #print('CO2 ->',CO2)

                    # the total number of vehicles (exited the network and still in the network)
                    step = summaryRoot.findall('step')
                    vehEnded = float( step[-1].get('ended') )
                    vehRunning = float( step[-1].get('running') )
                    totalVeh = vehEnded + vehRunning
                    #print('totalVeh ->',totalVeh)

                    totalDistance=0.0
                    for edge in meanDataRoot.findall('*/edge'):
                        totalDistance += float( edge.get('sampledSeconds') ) * float( edge.get('speed') )
                    totalDistance /= 1000
                    #print('total Distance', totalDistance)

                    CO2perVehPerKm = CO2 / totalDistance
                    #print('CO2perVehPerKm ->',CO2perVehPerKm)
                    CO2output += CO2perVehPerKm
                if (numFiles>0.0): CO2output /= numFiles
                print('CO2 = ',CO2output,'[g/veh/Km] parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = CO2output

    return rtn

def meanTotalDistanceTraveled(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand ->',trafficDemand)
        for trafficMix in range(trafficMixSize):
            print('trafficMix',trafficMix)
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                meanTotalDistanceTraveled = 0.0
                numFiles =0

                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )

                    meanDataList = [ 'outputMeandata',
                                     'trafficMix' , str( trafficMix  ),
                                     'trafficDemand' , str( trafficDemand),
                                     'driverBehaviour' , driverBehaviour,
                                     'seed' , str( seed )
                    ]
                    meanDataListStr = '_'.join( meanDataList )


                    try:
                        meanDataTree = ET.parse(meanDataListStr + '.xml')
                        meanDataRoot = meanDataTree.getroot()
                    except :
                        if (showCrpt):print('file -> ', meanDataListStr,' is corrupted or missing ', file = f )
                        print(meanDataListStr)
                        continue

                    #print(root.findall('*/edge'))
                    numFiles += 1
                    #print('success')

                    totalDistance=0.0
                    for edge in meanDataRoot.findall('*/edge'):
                        totalDistance += float( edge.get('sampledSeconds') ) * float( edge.get('speed') )
                    totalDistance /= 1000
                    #print('total Distance', totalDistance)
                    meanTotalDistanceTraveled += totalDistance

                if (numFiles>0.0): meanTotalDistanceTraveled /= numFiles
                print('total distance traveled = ',meanTotalDistanceTraveled,'[Km] parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = meanTotalDistanceTraveled

    return rtn

def meanValueSpeed(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand -> ',trafficDemand)

        for trafficMix in range(trafficMixSize):
            # XXX: dictTimeSpeed first dim is the seed, dim2 is the scheme, dim3 is timeStep and speed
            dictTimeSpeed = {}
            for seedHelper in range(seedSize):
                dictTimeSpeed[ str(seedHelper) ] = {}
                for i in driverBehaviourList:
                    dictTimeSpeed[ str(seedHelper) ][ i ] = {}

            print('trafficMix ->',trafficMix)

            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                speed = 0.0
                numFiles = 0
                #print(dictTimeSpeed)
                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ 'outputSummary',
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + endingXMLSummary)
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    #print(root.findall('step'))
                    numFiles += 1

                    speed_perTimeStep = 0.0

                    for edge in root.findall('step'):
                        #print(edge.get('meanSpeed'))
                        speed_perTimeStep += float( edge.get('meanSpeed') )*3.6
                        #TODO: cut speed<0.0 (in the first time steps -no vehicles in the network-)
                        if ( float(edge.get('time')) < 3600.0 ):
                            dictTimeSpeed[ str(seed) ][ driverBehaviour ][edge.get('time')] = float( edge.get('meanSpeed') )*3.6
                    #normazied
                    if ( len(root.findall('step')) > 0.0 ):speed_perTimeStep /= len(root.findall('step'))

                    speed += speed_perTimeStep

                if (numFiles>0.0): speed /= numFiles
                print('speed = ',speed,'[Km/h] parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = speed

            # #XXX: per seed time-speed and scheme
            # if trafficMix == 2 and trafficDemand == 2:
            #     nameStr = 'SpeedPerSeed'
            #     #print(' dictTimeSpeed ',dictTimeSpeed['0']['PS'])
    return rtn

def meanTroughtput(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand -> ',trafficDemand)

        for trafficMix in range(trafficMixSize):
            # XXX: dictTimeSpeed first dim is the seed, dim2 is the scheme, dim3 is timeStep and speed
            dictTimeSpeed = {}
            for seedHelper in range(seedSize):
                dictTimeSpeed[ str(seedHelper) ] = {}
                for i in driverBehaviourList:
                    dictTimeSpeed[ str(seedHelper) ][ i ] = {}

            print('trafficMix ->',trafficMix)

            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                troughputTotal = 0.0
                numFiles = 0
                #print(dictTimeSpeed)
                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ 'outputSummary',
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + endingXMLSummary)
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    #print(root.findall('step'))
                    numFiles += 1

                    step = root.findall('step[@time="3600.00"]')
                    #print('step',step)
                    vehEnded = float( step[0].get('ended') )
                    #throughput_perSeed = vehEnded / 3600

                    troughputTotal += vehEnded

                if (numFiles>0.0): troughputTotal /= numFiles
                print('throughput = ',troughputTotal,'[#] parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = troughputTotal

            # #XXX: per seed time-speed and scheme
            # if trafficMix == 2 and trafficDemand == 2:
            #     nameStr = 'SpeedPerSeed'
            #     #print(' dictTimeSpeed ',dictTimeSpeed['0']['PS'])

    return rtn

def meanNumberLaneChanges(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ None ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]


    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand -> ',trafficDemand)
        for trafficMix in range(trafficMixSize):
            print('trafficMix ->',trafficMix)
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                numberLaneChanges = 0.0
                numFiles = 0

                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ laneChangeInputStr,
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + endingXMLLaneChange)
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    numberLaneChanges += len(root.findall('change'))
                    numFiles += 1

                if (numFiles>0): numberLaneChanges /= numFiles
                print('NLC = ',numberLaneChanges,' parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = numberLaneChanges

    return rtn

def meanSecureGap(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand ->',trafficDemand)
        for trafficMix in range(trafficMixSize):
            print('trafficMix ->',trafficMix)
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                secureGap = 0.0
                numFiles = 0

                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ laneChangeInputStr,
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )
                    try:
                        tree = ET.parse( cmdStr + endingXMLLaneChange)
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    #print(root.findall('change'))
                    numFiles += 1

                    secureGap_perLC = 0.0

                    for edge in root.findall('change'):
                        #print(edge.get('meanSpeed'))
                        if ( edge.get('leaderSecureGap') != 'None' ): secureGap_perLC += float( edge.get('leaderSecureGap') )
                    #normazied if there are any lanechanges
                    if ( len(root.findall('change'))>0.0 ): secureGap_perLC /= len(root.findall('change'))

                    secureGap += secureGap_perLC

                if (numFiles>0): secureGap /= numFiles
                print('secureGap = ',secureGap,'[m] parametrization scheme = ',driverBehaviour)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = secureGap

    return rtn

def meanSSM(f):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    #rtn = [ [ [ [] ]*dim3  ]*dim2 ]*dim1
    rtn = [[[0 for col in range(dim3)]for row in range(dim2)] for x in range(dim1)]

    # #XXX: for createing the XMLS Sven asked
    # dictionary={}

    for trafficDemand in range(trafficDemandSize):
        print('trafficDemand -> ',trafficDemand)
        for trafficMix in range(trafficMixSize):
            print('trafficMix ->',trafficMix)
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                tmp=[]
                TTC = 0
                numFiles = 0

                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ 'outputSSM',
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + '.xml')
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr + '.xml' ,' is corrupted or missing ', file = f )
                        continue

                    numFiles += 1
                    instances = 0
                    dictConflictType={}
                    for conflictType in range(20):
                        dictConflictType[ 'type' + str(conflictType) ] = 0

                    for element in root.findall('*/minTTC'):
                        #print(edge.get('meanSpeed'))
                        if (element.get('value') != 'NA'):
                            #print(cmdStr)
                            #print( element.get('value') )
                            tmp.append( float( element.get('value') ) )
                            if ( float( element.get('value') ) < 3.5 ): dictConflictType[ 'type' + element.get('type') ] += 1
                            if ( float( element.get('value') ) < 3.5 ): instances += 1

                if ( len(tmp)>0.0 ): TTC = sum(tmp) / len(tmp)
                if (numFiles >0.0 ): instancesAverage = instances/numFiles
                #print('TTC = ',TTC,'[s] parametrization scheme = ',driverBehaviour)
                print('instancesAverage = ',instancesAverage,'[#] parametrization scheme = ',driverBehaviour)
                #print('Conflict Type ',dictConflictType)
                rtn[trafficDemand][trafficMix][driverBehaviourIdx] = instancesAverage

                #XXX: for the xmls Sven asked
                #dictionary[cmdStr] = tmp
    #XXX: for the xmls Sven asked
    # sheet = pe.get_sheet(adict = dictionary)
    # sheet.save_as('graphs/output.xls')

    return rtn

def generateGraph(nameKPI, valueKPI, namePRM):
    Data = {}

    fig = plt.figure()
    ax = fig.add_subplot(111)

    #print('valueKPI',valueKPI)

    for driverBehaviourIdx, driverBehaviour in enumerate(driverBehaviourList):
        Data[driverBehaviour] = [valueKPI[i][driverBehaviourIdx] for i in range(trafficMixSize)]
        #print( Data.get(driverBehaviour) )

    # the width of each bar is 10% of the max height
    width = 0.1 * max( max(valueKPI) )

    # the space between each traffic mix groupLength
    space = 2.5 * width

    # each traffic mix group has a total length of:
    groupLength = 5*width + space

    # ind -> where each group starts [0 length 2*length]
    ind = [ i*groupLength for i in range(trafficMixSize) ]

    colorBar = sns.color_palette('deep',len(driverBehaviourList))

    rect = [None] * len(driverBehaviourList)
    # the bars
    for driverBehaviourIdx, driverBehaviour in enumerate(driverBehaviourList):
        rect[driverBehaviourIdx] = ax.bar([ ind[i] + driverBehaviourIdx*width  for i in range(trafficMixSize)],
                                          Data.get(driverBehaviour),
                                          width,
                                          color = colorBar[driverBehaviourIdx]
                                         )
    yLabelUnit = ''
    if 'Speed' in nameKPI:
        yLabelUnit = ' [Km/h]'
    elif 'TTC' in nameKPI:
        yLabelUnit = ' [sec]'
    elif 'Throughput' in nameKPI:
        yLabelUnit = ' [veh/h]'
    elif 'Total_CO2' in nameKPI:
        yLabelUnit = ' [kg]'
    elif 'CO2' in nameKPI:
        yLabelUnit = ' [gr/Km]'
    elif 'Distance' in nameKPI:
        yLabelUnit = ' [Km]'
    elif 'Gap' in nameKPI:
        yLabelUnit = ' [m]'
    #the labels
    #ax.set_xlim(-width, 2.8*groupLength)
    ax.set_ylabel( nameKPI.replace('_',' ')+yLabelUnit )

    xLabelStr=''
    if 'Level_of_Service' in namePRM:
        xLabelStr = 'Traffic Mix'
    elif 'Traffic_Mix' in namePRM:
        xLabelStr = 'Level of Service'

    ax.set_xlabel( xLabelStr )


    ax.set_title(  namePRM.replace('_',' ') )

    if 'Level_of_Service' in namePRM:
        xTickMarks = [i for i in ['1','2','3']]
    elif 'Traffic_Mix' in namePRM:
        xTickMarks = [i for i in ['A','B','C']]

    ax.set_xticks([ ind[i]+2*width for i in range(trafficMixSize) ])
    xtickNames = ax.set_xticklabels(xTickMarks)
    plt.setp(xtickNames, fontsize=10)

    ## add a legend
    ax.legend( (rect[0][0], rect[1][0], rect[2][0], rect[3][0], rect[4][0]),
               ('PE','PS','MSE','OE','OS') , loc = 'upper left',
               bbox_to_anchor=(1.04,1) )

    cwd = os.getcwd()

    changeToDir(nameKPI=nameKPI)
    fig.savefig(namePRM+'_'+nameKPI+'.jpg', bbox_inches="tight")
    returnToCurrentWorkingDir(cwd)
    #plt.show()
    plt.close('all')

def generateSpeedTimePerEdge(edgeName):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    for trafficDemand in range(trafficDemandSize ):
        print('trafficDemand -> ',trafficDemand)

        for trafficMix in range(trafficMixSize):
            print('trafficMix ->',trafficMix)

            #plt.gca().set_prop_cycle( sns.color_palette('deep',len(driverBehaviourList)) )
            speedFig = plt.figure(1); plt.clf()
            inflowFig = plt.figure(2); plt.clf()
            outflowFig = plt.figure(3); plt.clf()
            
            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                speed = []
                inflow = []
                outflow = []
                numFiles = 0
                #print(dictTimeSpeed)
                maxHelper = 0.0
                maxInflow = 0
                maxOutflow = 0
                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ 'outputMeandata',
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + '.xml')
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    #print(cmdStr)
                    #print(root.findall('step'))
                    numFiles += 1

                    time=[]
                    for interval in root.findall('interval'):
                        if (float(interval.get('begin'))<3800.00): time.append( float(interval.get('begin')) )

                    #for param in root.findall(".//*[@key='device.ssm.file']")
                    for edge in root.findall('*//edge'):
                        if edge.get('id') == edgeName:
                            speed.append( float(edge.get('speed'))*3.6 )
                            inflow.append( int(edge.get('entered')))
                            outflow.append( int(edge.get('left')))


                # for i in range(numFiles):
                #     speed[i] = speed[i] / numFiles
                #print('time ->',len(time),'speed ->',len(speed))
                for i in range(len(speed) , len(time)*numFiles ):
                    #print('Before time ->',len(time),'speed ->',len(speed))
                    speed.append(0.0)
                    inflow.append(0)
                    outflow.append(0)
                    #print('After time ->',len(time),'speed ->',len(speed))


                speedHelper = [None]*len(time)
                inflowHelper = [None]*len(time)
                outflowHelper = [None]*len(time)
                for i in range( len(time) ):
                    speedSum = 0
                    inflowSum = 0
                    outflowSum = 0                    
                    for j in range(0+i, len(time)*numFiles, len(time)):
                        speedSum += speed[j]
                        outflowSum += outflow[j]
                        inflowSum += inflow[j]
                        #print(i,j)
                    speedHelper[i] = speedSum / numFiles
                    inflowHelper[i] = float(inflowSum) / numFiles
                    outflowHelper[i] = float(outflowSum) / numFiles

                #print('time',time,'speed',speedHelper)+

                plt.figure(speedFig.number)
                plt.plot(time,speedHelper,label=driverBehaviour)
                plt.figure(inflowFig.number)
                plt.plot(time,inflowHelper,label=driverBehaviour)
                plt.figure(outflowFig.number)
                plt.plot(time,outflowHelper,label=driverBehaviour)
                if max(speed) > maxHelper:
                    maxHelper = max(speed)
                if max(inflow) > maxInflow:
                    maxInflow = max(inflow)
                if max(outflow) > maxOutflow:
                    maxOutflow = max(outflow)


            # plot speed
            plt.figure(speedFig.number)
            plt.plot(time,speedHelper,label=driverBehaviour)
            plt.legend( loc = 'upper left', bbox_to_anchor=(1.04,1))
            plt.xlabel('time [s]')
            plt.ylabel('speed [Km/h]')
            plt.ylim(0.0, yLimSpeed)
            plt.xlim(0.0, 3600.0)
            graphNameList = [edgeName,
                             'TrafficMix',str(trafficMix),
                             'TrafficDemand',str(trafficDemand)]
            graphNameStr = '_'.join( graphNameList )

            if trafficDemand == 0:
                TDLoS = 'LoS A'
            elif trafficDemand == 1:
                TDLoS = 'LoS B'
            elif trafficDemand == 2:
                TDLoS = 'LoS C'
            graphTitleList = [edgeName,' - ',
                              'Traffic Mix_', str(trafficMix+1),' - ',
                              'Traffic Demand_', TDLoS
                              ]
            graphNameStr = ''.join(graphTitleList)
            plt.title(graphNameStr)

            cwd = os.getcwd()
            changeToDir(edgeName=edgeName, edgeKPI='Speed')
            print(graphNameStr+'.jpg')
            plt.savefig(graphNameStr+'.jpg', bbox_inches="tight")
            returnToCurrentWorkingDir(cwd)

            # plot inflow
            plt.figure(inflowFig.number)
            plt.plot(time,inflowHelper,label=driverBehaviour)
            plt.legend( loc = 'upper left', bbox_to_anchor=(1.04,1))
            plt.xlabel('time [s]')
            plt.ylabel('inflow [veh]')
            #~ plt.ylim(0.0, yLimSpeed)
            plt.xlim(0.0, 3600.0)
            plt.title(graphNameStr)
            
            changeToDir(edgeName=edgeName, edgeKPI='Inflow')
            plt.savefig(graphNameStr+'.jpg', bbox_inches="tight")
            returnToCurrentWorkingDir(cwd)

            # plot outflow
            plt.figure(outflowFig.number)
            plt.plot(time,outflowHelper,label=driverBehaviour)
            plt.legend( loc = 'upper left', bbox_to_anchor=(1.04,1))
            plt.xlabel('time [s]')
            plt.ylabel('outflow [veh]')
            #~ plt.ylim(0.0, yLimSpeed)
            plt.xlim(0.0, 3600.0)
            plt.title(graphNameStr)

            changeToDir(edgeName=edgeName, edgeKPI='Outflow')
            plt.savefig(graphNameStr+'.jpg', bbox_inches="tight")
            returnToCurrentWorkingDir(cwd)

            #plt.show()
            plt.close('all')
    return

def generateQueueTimePerEdge(edgeName):
    dim1 = trafficDemandSize
    dim2 = trafficMixSize
    dim3 = len(driverBehaviourList)

    for trafficDemand in range(trafficDemandSize ):
        print('trafficDemand -> ',trafficDemand)

        for trafficMix in range(trafficMixSize):
            print('trafficMix ->',trafficMix)

            #plt.gca().set_prop_cycle( sns.color_palette('deep',len(driverBehaviourList)) )

            for driverBehaviourIdx,driverBehaviour in enumerate( driverBehaviourList ):

                dictValue = {}
                numFiles = 0
                #print(dictTimeSpeed)
                for seed in range(seedSize):
                    #print(seed , trafficDemand , trafficMix , driverBehaviourIdx   )
                    cmdList = [ 'outputQueue',
                                'trafficMix' , str( trafficMix  ),
                                'trafficDemand' , str( trafficDemand),
                                'driverBehaviour' , driverBehaviour,
                                'seed' , str( seed )
                    ]

                    cmdStr = '_'.join( cmdList )

                    try:
                        tree = ET.parse( cmdStr + '.xml')
                        root = tree.getroot()
                    except:
                        if (showCrpt):print('file -> ', cmdStr,' is corrupted or missing ', file = f )
                        continue

                    #print(cmdStr)
                    #print(root.findall('step'))
                    numFiles += 1

                    for dataStep in root.findall('data'):
                        #TODO: arxikopoihsh dictionary
                        if not (dataStep.get('timestep') in dictValue): dictValue[dataStep.get('timestep')]=0.0

                        for lanesID in dataStep.findall('lanes'):

                            #TODO: mean values for all lanes
                            sumHelp = []
                            for lane in lanesID.findall('lane'):
                                if edgeName in lane.get('id'):
                                    sumHelp.append( float( lane.get('queueing_length_experimental') ) )
                            if len(sumHelp) > 0 :
                                mean = sum(sumHelp) / len(sumHelp)
                            else:
                                mean = 0.0
                            dictValue[dataStep.get('timestep')] += mean
                xAxis = []
                yAxis = []
                for key, value in dictValue.items():
                    xAxis.append( float(key) )
                    yAxis.append(value / numFiles)

                #print('##### xAxis ->',xAxis,'##### yAxis->',yAxis)
                #print('##### yAxis->',yAxis)

                #finding the mean value of many points
                secInterval = 150.0
                sumH = []
                xHelper = []
                yHelper = []
                for indx, x in enumerate(xAxis):
                    if ( x % secInterval) == 0.0:
                        xHelper.append(x)
                        mean = sum(sumH) / len(sumH) if len(sumH) > 0 else 0.0
                        yHelper.append(mean)
                        sumH = []
                    else:
                        sumH.append( yAxis[indx] )

                plt.plot(xHelper,yHelper,label=driverBehaviour)
            plt.legend( loc = 'upper left', bbox_to_anchor=(1.04,1))
            plt.xlabel('time [s]')
            plt.xlim(0.0, 3600.0)
            plt.ylabel('Queue Length [m]')
            graphNameList = [edgeName,
                             'TrafficMix',str(trafficMix),
                             'TrafficDemand',str(trafficDemand)]
            graphNameStr = '_'.join( graphNameList )

            if trafficDemand == 0:
                TDLoS = 'LoS A'
            elif trafficDemand == 1:
                TDLoS = 'LoS B'
            elif trafficDemand == 2:
                TDLoS = 'LoS C'
            graphTitleList = [edgeName,' - ',
                              'Traffic Mix:', str(trafficMix+1),' - ',
                              'Traffic Demand:', TDLoS
                              ]
            graphNameStr = ''.join(graphTitleList)
            plt.title(graphNameStr)
            cwd = os.getcwd()

            changeToDir(edgeName=edgeName, edgeKPI='Queue')
            plt.savefig(graphNameStr+'.jpg', bbox_inches="tight")
            returnToCurrentWorkingDir(cwd)
            #plt.show()
            plt.close('all')
    return

def changeToDir(nameKPI='None', parentDir="Analysis", edgeName='None', edgeKPI='None' ):
    if not( parentDir in os.listdir() ): os.mkdir(parentDir)
    os.chdir(parentDir)

    if edgeName == 'None':

        if not( nameKPI.replace('_',' ') in os.listdir() ): os.mkdir( nameKPI.replace('_',' ') )
        os.chdir( nameKPI.replace('_',' ') )

    else:

        if not( 'Edges' in os.listdir() ): os.mkdir( 'Edges' )
        os.chdir( 'Edges' )

        if not( edgeKPI in os.listdir() ): os.mkdir( edgeKPI )
        os.chdir( edgeKPI )

        if not( edgeName in os.listdir() ): os.mkdir( edgeName )
        os.chdir( edgeName )


#returns to the directory where the output files are located
def returnToCurrentWorkingDir(dirGoal):
    os.chdir(dirGoal)

# this is the main entry point of this script
if __name__ == "__main__":

    f = open('corruptedFiles.txt','w')

    dictKPI = {}

    #~ emissionsMeanVal = meanValueEmmisions(f)
    #~ dictKPI['Total_CO2_Emissions'] = emissionsMeanVal

    #~ speedMeanVal = meanValueSpeed(f)
    #~ dictKPI['Average_Network_Speed'] = speedMeanVal

    #~ totalLaneChanges = meanNumberLaneChanges(f)
    #~ dictKPI['Number_of_Lane_Changes'] = totalLaneChanges
    #~ print('dictKPI[Number_of_Lane_Changes]',dictKPI['Number_of_Lane_Changes'])

    #~ averageSecureGap = meanSecureGap(f)
    #~ dictKPI['Secure_Gap'] = averageSecureGap

    #~ averageSSM = meanSSM(f)
    #~ dictKPI['TTC_<_3.0'] = averageSSM

    #~ emissionsPerVeh = meanValueEmmisionsPerVeh(f)
    #~ dictKPI['CO2_Emissions'] = emissionsPerVeh

    #~ averageTroughput = meanTroughtput(f)
    #~ dictKPI['Throughput'] = averageTroughput

    #~ averageTotalDistanceTravelled = meanTotalDistanceTraveled(f)
    #~ dictKPI['Total_Distance_Travelled'] = averageTotalDistanceTravelled

    #~ for nameKPI, valueKPI in dictKPI.items():
        #~ for nameLOS, valueLOS in dictLOS.items():
            #~ generateGraph(nameKPI, valueKPI[valueLOS], nameLOS)

    #~ for nameKPI, valueKPI in dictKPI.items():
        #~ for nameMIX, valueMIX in dictMIX.items():
            #~ valueKPIHelper = [ valueKPI[i][valueMIX] for i in range(trafficDemandSize)]
            #~ generateGraph(nameKPI, valueKPIHelper, nameMIX)

    # UC2.1 edgeNameList = ['longEdge3' ,'longEdge2','longEdge1' ,'onramp','entry','exit']
    # UC5.1 edgeNameList = ['e0' ,'entry']
    # UC1.1 edgeNameList = ['approach1' ,'approach2','end','leave',
    #                 'safetyzone1_1', 'safetyzone1_2',
    #                 'safetyzone2_1', 'safetyzone2_2',
    #                 'start','workzone']
    # UC4.2 edgeNameList = ['approach1' ,'approach2','end','leave',
    #                 'safetyzone1', 'safetyzone2',
    #                 'start','workzone']
    # UC3.1 ['leave_area' ,'merge_area','start_north','start_south']
    edgeNameList = ['e0' ,'entry']

    for edgeName in edgeNameList:
        print('edgeName ->',edgeName)
        generateSpeedTimePerEdge(edgeName)

    for edgeName in edgeNameList:
        print('edgeName ->',edgeName)
        generateQueueTimePerEdge(edgeName)


    f.close()
