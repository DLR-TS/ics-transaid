#!/usr/bin/python
import os, sys
import random
import shutil
import subprocess as sp
import time

# Number of simulations per Demand/Parameter configuration
Nsim = 1
# Number of lanes (regarding the LOS) in the current scenario
Nlanes = 1

# Number of restarts if a run fails
MAXTRIES = 10

# List of genral additional files to be loaded for all runs
generalAddFiles = ["../../../closeLanes.add.xml", "../../../shapes.add.xml"]

# Demand levels (levelID->veh/(h*l))
demand_urban = {"los_A":525, "los_B":825, "los_C":1155}
demand_urban = {"los_C":1155}

# levels to be used for simulations
demand_levels = demand_urban
demand_ID_map = {"los_A":0, "los_B":1, "los_C":2}

# Vehicle mix (mix_ID->class->share [in %])
veh_mixes = {"mix_0":{"LV":0.7, "CVToC":0.15, "CAVToC":0.15}, 
             "mix_1":{"LV":0.5, "CVToC":0.25, "CAVToC":0.25}, 
             "mix_2":{"LV":0.2, "CVToC":0.4, "CAVToC":0.4}}
veh_mixes = {"mix_2":{"LV":0.2, "CVToC":0.4, "CAVToC":0.4}}
            

# Parameter assumptions regarding (E)fficiency/(S)afety and (O)ptimism/(P)essimism
param_schemes = ["PS", "OS", "PE", "OE", "MSE"]
param_schemes = ["OS"]

# template file for the routes
routefile_template = "routes_template.rou.xml"

# config file
config_file = "sumo.cfg"

# runner script for single runs
runner_file = "runner.py"

def fillTemplate(template_file, out_file, content_dict):
    with open(template_file) as f:
        template = f.read()
        filled_template = template.format(**content_dict)
    with open(out_file, "w") as f:
        f.write(filled_template)
    
if __name__ == "__main__":
    wdir = os.path.dirname(os.path.realpath(__file__))
    print("Working dir = %s"%wdir)
    # nr of runs started from this script
    runCount = 0
    # remove vTypeDistribution files and create them again
    if "--generateVTypes" in sys.argv: sp.call(["python", "distribution.py"])
    
    # remove existing subdir structure
    for d in demand_levels:
        try:
            shutil.rmtree(os.path.join(wdir, d))
            print("removing dir %s"%d)
        except:
            pass

    # Create subdirectory structure: params/mix/demand/; fail if it exists
    # ... and start simulations
    start_time = time.time()
    for d in demand_levels:
        print("Demand: %s"%d)
        ddir = os.path.join(wdir, d)
        os.mkdir(ddir)
        for m in veh_mixes:
            print("Mix: %s"%m)
            mdir = os.path.join(ddir, m)
            os.mkdir(mdir)
            for p in param_schemes:
                print("Scheme: %s"%p)
                pdir = os.path.join(mdir, p)
                os.mkdir(pdir)
                
                # fill and copy sumo config
                addFileList = ["vTypes"+s+"_"+p+".add.xml" for s in veh_mixes[m].keys()]
                #~ addFileDict = {"addFiles": ", ".join(addFileList)}
                #~ print("addFileDict: %s"%addFileDict)
                config_fn = os.path.join(pdir, config_file)
                #~ fillTemplate(os.path.join(wdir, config_file), config_fn, addFileDict)
                fillTemplate(os.path.join(wdir, config_file), config_fn, {})
                
                # copy vType files
                for fn in addFileList:
                    shutil.copyfile(os.path.join(wdir, fn), os.path.join(pdir, fn))
        
                # copy runner script
                runner_fn = os.path.join(pdir, runner_file)
                shutil.copyfile(os.path.join(wdir, runner_file), os.path.join(pdir, runner_fn))
                
                # run simulations
                working_set = {}
                for i in xrange(Nsim):
                    os.chdir(pdir)
                    
                    output_suffix = "_trafficMix_%s_trafficDemand_%s_driverBehaviour_%s_seed_%s.xml"%(m[-1], demand_ID_map[d], p, i)
                    
                    # Fill and copy run specific routes file
                    route_dict = dict([(vType+"prob", Nlanes*percentage*demand_levels[d]/3600.) for (vType, percentage) in veh_mixes[m].iteritems()])
                    route_dict.update(dict([(vType+"type", "veh"+vType+p) for vType in veh_mixes[m]]))
                    ssmOutFile = "outputSSM" + output_suffix
                    route_dict.update({"ssmOutFile" : os.path.join(pdir,  ssmOutFile)})
                    routes_fn = "routes" + output_suffix
                    routes_fn = os.path.join(pdir, routes_fn)
                    fillTemplate(os.path.join(wdir, routefile_template), routes_fn, route_dict)
                    print("wrote file '%s'\nrouteDict=%s"%(os.path.join(pdir, routes_fn), route_dict))
                    
                    # Fill and copy run specific aggregated output additional file
                    output_addFileTemplate = "additionalOutput_template.add.xml"
                    output_addFile = "additionalsOutput" + output_suffix
                    emissionOutputFile = os.path.join(pdir, "outputEmission" + output_suffix)
                    meandataOutputFile = os.path.join(pdir, "outputMeandata" + output_suffix)
                    fillTemplate(os.path.join(wdir, output_addFileTemplate), os.path.join(pdir, output_addFile), {"emissionOutfile":emissionOutputFile, "meandataOutfile":meandataOutputFile})
                    allAddFiles = ", ".join(addFileList+[output_addFile]+generalAddFiles)
                    
                    ## Run with fixed seeds (for reproducibility)
                    argv = ["python", runner_fn, "-c", config_fn, "--additionals", allAddFiles, "--routes", routes_fn, "--seed", str(42+i), "--gui", "--suffix", output_suffix]
                    ## Run with random seeds
                    #~ argv = ["python", runner_fn, "-c", config_fn, "--additionals", allAddFiles, "--routes", routes_fn,  "--seed", random.randint(0,100000000000000)]), "--suffix", str(i)]
                    ## run with gui (for testing)
                    #~ argv = ["python", runner_fn, "-c", config_fn, "--gui", "--seed", str(42+i), "--additionals", allAddFiles, "--routes", routes_fn,  "--verbose", "--debug", "--suffix", str(i)]
                    #~ argv = ["python", runner_fn, "-c", config_fn, "--gui", "--seed", str(42+i), "--additionals", allAddFiles, "--routes", routes_fn,  "--suffix", str(i)]
                    print("Calling: %s"%argv)
                    
                    
                    ## Call one at a time writing to stdout (for testing)
                    Ntries = 0
                    while(sp.call(argv) != 0 and Ntries < MAXTRIES):
                        Ntries += 1
                    
                    ## Call in separate processes, writing to output file
                    #~ out_fn=os.path.join(pdir, "output_"+str(i)+".txt")
                    #~ print("Writing output to file '%s'"%out_fn)
                    #~ outfile = open(out_fn, "w")
                    #~ working_set[i] = (sp.Popen(argv, stdout=outfile, stderr=outfile), outfile, argv)
                    runCount += 1
                    
                
                print("\nWaiting for all simulations for %s->%s->%s to complete...\n"%(d, m, p))
                
                # wait for all run to end, restart failed calls (FIXME: reason for failing is unclear and fails are not reliably reproducible) 
                Ntries = 0
                while any(working_set):
                    i, v = working_set.popitem()
                    waitProcess, outfile, argv = v
                    returnCode = waitProcess.poll()
                    if returnCode is None:
                        # process not yet finished, push it back
                        working_set[i]=v
                        #time.sleep(0.1)
                        continue
                        
                    returnCode = waitProcess.wait()
                    outfile.close()
                    if returnCode != 0 and Ntries < MAXTRIES:
                        # restart process
                        print ("Run %s failed. Restarting..."%i)
                        Ntries += 1
                        out_fn=outfile.name
                        outfile = open(out_fn, "w")
                        working_set[i] = (sp.Popen(argv, stdout=outfile, stderr=outfile), outfile, argv)
                    else:
                        print ("Run %s completed..."%i)
                
                
                if not "--no-gzip" in sys.argv: 
                    # gzip all output for this run
                    callList=["gzip", "-r9"] + [os.path.join(pdir, fn) for fn in os.listdir(pdir)]
                    print("\ngzipping all files in "+pdir+"...\n")
                    sp.call(callList)
                
                
    # report elapsed time
    end_time = time.time()
    elapsed = end_time-start_time
    print ("\nDone.\n\nElapsed time: %s (for %s runs)\n"%(elapsed, runCount))
    #~ # compress output
    #~ try: shutil.rmtree(os.path.join(wdir, "output.zip"))
    #~ except: pass
    #~ zip_command = ["zip", os.path.join(wdir,"output.zip"), os.path.join(wdir,"los_*"), "-r"]
    #~ print("Calling "+" ".join(zip_command))
    #~ sp.call(["zip", os.path.join(wdir,"output.zip"), os.path.join(wdir,"los_*"), "-r"])
                
                
