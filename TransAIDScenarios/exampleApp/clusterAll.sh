#!/bin/bash
#chmod u+x clusterAll.sh
#type ./clusterAll.sh to run (all scenarios get enabled)
# or
#type ./clusterAll.sh --en=0,0,0,1
#TIP: 1 scenarios * 3 los * 3 mix * 10 sim = 90 (cores needed)
#TIP: 4 scenarios * 3 los * 10 sim = 120 (cores needed)

#WARNING do not use 1 scenario for manual because it has only one mix

#node 17 and 18 are equal and they both have 64 cores and 128 gb of ram each

# Date: 2020/01/1
# Author: Vasilios Karagounis

#follow this example to create on the fly the run and the condor files
#https://gist.github.com/Tushar-N/11fc5b57d369af4d00a2c143492e0326
#http://www0.mi.infn.it/condor/manual/2_6Submitting_Job.html


WORK_DIR=$PWD
echo "${WORK_DIR}"

declare -a enables=()
declare -a enables_count=()

declare -a scenarios=()
declare -a demands=()
declare -a mixes=()

declare -a nodes=("1" "1" "2" "2") #0 = auto, 1 == 17, 2 == 18, 3 = empty 

declare -i count=0
declare -i countEnables=0

declare -i count_jobs=0

declare -i node=0

user=""

eval user="~$USER"
user="${user##*/}"

if [[ $user == "" ]]; then
	echo "Wrong user"
	sleep 3
	exit 1
fi


data_val=$(
/media/home/${user}/libs/bin/python3 << END
#!/usr/bin/python3
import os, sys, errno
import json

settings = {}
path = 'settings/batchRunner.json'

if os.path.exists(path):
    with open(path, 'r') as f:
        try:
            settings = json.load(f)
        except ValueError as e:
            sys.exit(1)

demands = str(settings['demands'])
print(demands.replace('[', '').replace(']', '').replace('\'', ''))

mixes = str(settings['traffic_mixes_clean'])
print(mixes.replace('[', '').replace(']', '').replace('\'', ''))

scenarios = str(settings['args_scenarios_clean'])
print(scenarios.replace('[', '').replace(']', '').replace('\'', ''))
END
)

#https://stackoverflow.com/questions/10586153/split-string-into-an-array-in-bash

data=($(echo "$data_val" | tr ',' '\n'))
#echo ${data[@]}

#split data
for dt in "${data[@]}"
do
	if [[ "$dt" == *"los"* ]]; then
		demands+=("$dt")
	elif [[ "$dt" == *"mix"* ]]; then
		mixes+=("$dt")
	else
		scenarios+=("$dt")
	fi
done

for i in "${scenarios[@]}"
do
    enables_count+=("1")
done

#look in arguments
for arg in "$@"
do

    key=$(echo $arg | cut -f1 -d=)
    temp=$(echo $arg | cut -f2 -d=)   

    if [[ $key == "--en" ]]; then
        enables=($(echo "$temp" | tr ',' '\n'))
        echo "enables: ${enables[@]}"
    fi
done

#set all enables to true since we didn't find any arguments
if [ "${#enables[@]}" == "0" ]; then
    enables=("${enables_count[@]}")
fi

#count how many enables are
declare -i count_enables=0

for i in "${enables[@]}"
do
    count_enables=count_enables+1
	
	if [[ $i == "1" ]]; then
		countEnables=countEnables+1
	fi

done

#check length
if [[ $count_enables != ${#enables_count[@]} ]]; then
	echo "wrong len of enables"
	exit 1
fi

echo "countEnables" $countEnables	

#-----------------------------------------------------------------------------------------------------------------
#condor stuff
#-----------------------------------------------------------------------------------------------------------------
#1,2,3,4  = user, scenarios, demand, node
condor_make_files () {

	local runCluster=runCluster_${2}_${3}.sh
	local condorFile=condor_${2}_${3}.condor
	local user=${1}

	local batchRunnerPath="/media/home/${1}/libs/bin/python3 ${WORK_DIR}/batchRunner.py --${2} --${3}"

	#local subName="_${2}_${3}"

	#file runCluster---------------------------------------------
	cat>"$WORK_DIR/$runCluster"<<- "EOF"
	#!/bin/bash
	EOF
	echo "SUMO_HOME=/media/home/${1}/SUMO/sumo" >> "$WORK_DIR/$runCluster"
	echo "export SUMO_HOME=/media/home/${1}/SUMO/sumo" >> "$WORK_DIR/$runCluster"
	echo "$batchRunnerPath" >> "$WORK_DIR/$runCluster"
	
	chmod -R 777 $runCluster

	#file runCluster---------------------------------------------
	cat>"$WORK_DIR/$condorFile"<<- "EOF"

	+AccountingGroup = "group_uwicore"
	EOF
	#echo "User	= $user" >> "$WORK_DIR/$condorFile"
	echo "Universe = vanilla" >> "$WORK_DIR/$condorFile"
	echo "Request_memory = 20000" >> "$WORK_DIR/$condorFile"
	echo "Request_cpus = 10" >> "$WORK_DIR/$condorFile"

	if [ ${4} == "0" ];
	then
		echo "Requirements = Machine == \"compute-0-17.local\" || Machine == \"compute-0-18.local\"" >> "$WORK_DIR/$condorFile"  
	elif [ ${4} == "1" ];
	then
		echo "Requirements = Machine == \"compute-0-17.local\"" >> "$WORK_DIR/$condorFile" 
	elif [ ${4} == "2" ];
	then
		echo "Requirements = Machine == \"compute-0-18.local\"" >> "$WORK_DIR/$condorFile" 
	fi

	#echo "Request_disk = 2000M" >> "$WORK_DIR/$condorFile"
	#echo "InitialDir = \$(InputDir)" >> "$WORK_DIR/$condorFile"
	#echo "InputDir      = /media/home/\$(User)/" >> "$WORK_DIR/$condorFile"
	#echo "Arguments  =" >> "$WORK_DIR/$condorFile"
	#echo "OutputDir  = /media/home/$user/" >> "$WORK_DIR/$condorFile"

	echo "OutputDir  = $WORK_DIR/" >> "$WORK_DIR/$condorFile"

	echo "Error      = \$(OutputDir)/err.\$(Process)" >> "$WORK_DIR/$condorFile"
	echo "Log        = \$(OutputDir)/log.\$(Process)" >> "$WORK_DIR/$condorFile"
	echo "Output     = \$(OutputDir)/out.\$(Process)" >> "$WORK_DIR/$condorFile"

	echo "Rank	= Mips" >> "$WORK_DIR/$condorFile"
	echo "Executable = $WORK_DIR/$runCluster" >> "$WORK_DIR/$condorFile"
	echo "Queue" >> "$WORK_DIR/$condorFile"

	chmod -R 777 $condorFile

	echo "$condorFile"
}

#-----------------------------------------------------------------------------------------------------------------
#1,2,3,4,5,6 = user, scenarios, demand, mix, sim, node
condor_make_files_special () {

	local runCluster=runCluster_${2}_${3}.sh
	local condorFile=condor_${2}_${3}.condor
	local user=${1}

	local batchRunnerPath="/media/home/${1}/libs/bin/python3 ${WORK_DIR}/batchRunner.py --${2} --${3} --${4} --${5}"

	#local subName="_${2}_${3}"

	#file runCluster---------------------------------------------
	cat>"$WORK_DIR/$runCluster"<<- "EOF"
	#!/bin/bash
	EOF
	echo "SUMO_HOME=/media/home/${1}/SUMO/sumo" >> "$WORK_DIR/$runCluster"
	echo "export SUMO_HOME=/media/home/${1}/SUMO/sumo" >> "$WORK_DIR/$runCluster"
	echo "$batchRunnerPath" >> "$WORK_DIR/$runCluster"
	
	chmod -R 777 $runCluster

	#file runCluster---------------------------------------------
	cat>"$WORK_DIR/$condorFile"<<- "EOF"

	+AccountingGroup = "group_uwicore"
	EOF
	#echo "User	= $user" >> "$WORK_DIR/$condorFile"
	echo "Universe = vanilla" >> "$WORK_DIR/$condorFile"
	echo "Request_memory = 20000" >> "$WORK_DIR/$condorFile"
	echo "Request_cpus = 10" >> "$WORK_DIR/$condorFile"

	if [ ${6} == "0" ];
	then
		echo "Requirements = Machine == \"compute-0-17.local\" || Machine == \"compute-0-18.local\"" >> "$WORK_DIR/$condorFile"  
	elif [ ${6} == "1" ];
	then
		echo "Requirements = Machine == \"compute-0-17.local\"" >> "$WORK_DIR/$condorFile" 
	elif [ ${6} == "2" ];
	then
		echo "Requirements = Machine == \"compute-0-18.local\"" >> "$WORK_DIR/$condorFile" 
	fi

	#echo "Request_disk = 2000M" >> "$WORK_DIR/$condorFile"
	#echo "InitialDir = \$(InputDir)" >> "$WORK_DIR/$condorFile"
	#echo "InputDir      = /media/home/\$(User)/" >> "$WORK_DIR/$condorFile"
	#echo "Arguments  =" >> "$WORK_DIR/$condorFile"
	#echo "OutputDir  = /media/home/$user/" >> "$WORK_DIR/$condorFile"

	echo "OutputDir  = $WORK_DIR/" >> "$WORK_DIR/$condorFile"

	echo "Error      = \$(OutputDir)/err.\$(Process)" >> "$WORK_DIR/$condorFile"
	echo "Log        = \$(OutputDir)/log.\$(Process)" >> "$WORK_DIR/$condorFile"
	echo "Output     = \$(OutputDir)/out.\$(Process)" >> "$WORK_DIR/$condorFile"

	echo "Rank	= Mips" >> "$WORK_DIR/$condorFile"
	echo "Executable = $WORK_DIR/$runCluster" >> "$WORK_DIR/$condorFile"
	echo "Queue" >> "$WORK_DIR/$condorFile"

	chmod -R 777 $condorFile

	echo "$condorFile"
}

#-----------------------------------------------------------------------------------------------------------------
condor_run () {
	condor_submit $1
}

#-----------------------------------------------------------------------------------------------------------------
#https://pwiki.pic.es/index.php?title=HTCondor_User_Guide
#https://research.cs.wisc.edu/htcondor/manual/v7.7/condor_wait.html

#-----------------------------------------------------------------------------------------------------------------
condor_wait_finish () {

	condor_wait ${WORK_DIR}/log.0
}

#-----------------------------------------------------------------------------------------------------------------
condor_display_jobs () {

	condor_q -allusers -nobatch
}

#-----------------------------------------------------------------------------------------------------------------
send_msg_to_slack () {
	/media/home/${1}/libs/bin/python3 "${WORK_DIR}/slack.py"
}

#-----------------------------------------------------------------------------------------------------------------
retval=""

echo "Clearing condor jobs"
condor_rm $user

echo "Deleting run data"
rm err.0
rm log.0
rm out.0
rm runCluster_*
rm condor_*
#rm -rf results

if [ ${countEnables} == "1" ]; then
	echo "using sim to core technique"
	
	for i in "${scenarios[@]}"
	do
		if [ ${enables[$count]} == "1" ]; then
		 	for j in "${demands[@]}"
		 	do
			 	for k in "${mixes[@]}"
				do
					for l in {0..9}
					do
			 			count_jobs=count_jobs+1
						retval=$(condor_make_files_special $user $i $j ${nodes[node]})
						echo "now running for: $user, $i, $j, $k, $l, ${nodes[node]}, $retval" 
						condor_run $retval
					done

					if [[ $node == 0 ]]; 
					then
						node=2
					else
						node=0
					fi
				done
			done
		fi
	count=count+1
	done
else
	#-----------------------------------------------------------------------------------------------------------------
	echo "using old technique"

	for i in "${scenarios[@]}"
	do
		if [ ${enables[$count]} == "1" ]; then
		 	for j in "${demands[@]}"
		 	do
			 	count_jobs=count_jobs+1
				retval=$(condor_make_files $user $i $j ${nodes[count]})
				echo "now running for: $user, $i, $j, ${nodes[count]}, $retval" 
				condor_run $retval
			done
		fi
		count=count+1
	done

	echo "please wait for 10 seconds..."
	sleep 10
fi

#'&' symbol sends the command to background running asychronously
{
	condor_wait_finish $count_jobs
	send_msg_to_slack $user
} &
