import os
import subprocess
import sys
import json

typeList = [ 'LV' , 'LGV' , 'HGV' , 'CV' , 'CAVGA' , 'CAVGB' ]
driverBehaviourList = ['FSP']
sizeList = [ 2500 , 2500 , 2500 , 2500 , 2500 , 2500 ]

settings = {}
wdir= ""

verbose = False
#---------------------------------------------------------------------------------------------------------------------------------------------
def loadFileSettings():
    
    global settings, wdir

    path = os.path.join(wdir, "settings/distribution.json")
    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                settings = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            print("Json settings path:", path)        
    else:
         print("Json settings path not found :", path)   

#---------------------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    
    print("Distribution start")
    
    wdir = os.path.dirname(os.path.realpath(__file__))
    wdir_Actual = wdir.replace("/tools", "")
    wdir_Types = os.path.join(wdir_Actual, "config/vTypes")
    wdir_TypesConfigs = os.path.join(wdir_Actual, "config/vTypeDistConfigs")
    
    if verbose:
        print(wdir)
        print(wdir_Actual)
        print(wdir_Types)
        print(wdir_TypesConfigs)
        
    loadFileSettings()
    
    typeList = settings["typeList"]
    driverBehaviourList = settings["driverBehaviourList"]
    sizeList = settings["sizeList"]
    
    print("typeList:", typeList)
    for driverBehaviour in driverBehaviourList:
        for typeIdx, typeStr in enumerate(typeList):
            
            vTypes_fn = os.path.join(wdir_Types, "vTypes" + typeStr + "_" + driverBehaviour + ".add.xml")
            
            try:
                os.remove(vTypes_fn)
            except:
                pass
            
            command = ['python3' ,
                       os.path.join(wdir, 'createVehTypeDistribution.py'),
                       os.path.join(wdir_TypesConfigs, 'config' + typeStr + driverBehaviour + '.txt'),
                       '-s', str(sizeList[typeIdx]),
                       '-n', "veh" + typeStr + driverBehaviour,
                       '-o', vTypes_fn]
    
            print('\nGenerating distribution for :', typeStr)
            if verbose:
                print('\nCommand :', command)
    
            subprocess.call(command, stdout = sys.stdout, stderr = sys.stderr)
    
    print("\nDistribution process finished")
