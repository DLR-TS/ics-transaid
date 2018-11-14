import os
import subprocess
import sys

typeList = [ 'LV' , 'CAVToC' , 'CVToC' ]
driverBehaviourList = ['PS','PE','MSE','OE','OS']
sizeList = [ 1000 , 1000 , 1000 ]

if __name__ == "__main__":
    wdir = os.path.dirname(os.path.realpath(__file__))
    for driverBehaviour in driverBehaviourList:
        for typeIdx,typeStr in enumerate(typeList):
            print('\n\n#### Generating Distribution For ',type,'\n')
            vTypes_fn=os.path.join(wdir,"vTypes"+typeStr+"_"+driverBehaviour+".add.xml")
            try:
                os.remove(vTypes_fn)
            except:
                pass
            command = ['python' , 'createVehTypeDistribution.py' ,
                       os.path.join("vTypeDistConfigs", 'config' + typeStr + driverBehaviour + '.txt') ,
                       '-s' , str( sizeList[typeIdx] ),
                       '-n' , "veh"+typeStr+driverBehaviour,
                       '-o' , vTypes_fn
            ]

            print ('in createVehTypeDistribution, printing cmd',command)

            subprocess.call( command, stdout=sys.stdout, stderr=sys.stderr )
