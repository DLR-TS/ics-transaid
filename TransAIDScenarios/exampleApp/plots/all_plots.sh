#!/bin/bash
#chmod u+x all_plots.sh
#type ./all_plots.sh

# Date    2020/01/01
# Author  Vasilios Karagounis

#change to local dir
cd "$(dirname "$0")"

secs_to_human() {
    if [[ -z ${1} || ${1} -lt 60 ]] ;then
        min=0 ; secs="${1}"
    else
        time_mins=$(echo "scale=2; ${1}/60" | bc)
        min=$(echo ${time_mins} | cut -d'.' -f1)
        secs="0.$(echo ${time_mins} | cut -d'.' -f2)"
        secs=$(echo ${secs}*60|bc|awk '{print int($1+0.5)}')
    fi
    echo ""
    echo "---------------------------------------------------------"
    echo "Total Time Elapsed : ${min} minutes and ${secs} seconds."
    echo "---------------------------------------------------------"
}

start=$(date +%s)

verbose=""
parallel=""
enables=""

#look in arguments
for arg in "$@"
do
    if [[ $arg == "--verbose" ]]; then
        verbose=${arg}
    elif [[ $arg == "--parallel" ]];
	then
        parallel=${arg}
    fi

    key=$(echo $arg | cut -f1 -d=)

    if [[ $key == "--en" ]]; then
        enables=$arg
        echo "enables: ${enables[@]}"
    fi
done

python3 unzip.py $verbose $parallel $enables
wait
    echo 'Data unziping process have finished!'

python3 generate.py $verbose $parallel $enables
wait
    echo 'Data generation process have finished!'

{
    python3 transformTrajectories.py $verbose $parallel $enables
    wait
        echo 'Data trajectories tranformation process have finished'

    python3 plotTrajectories.py $verbose $parallel $enables
    wait
        echo 'Data plotting trajectories process have finished'
} &

{
    python3 transformDetectors.py $verbose $parallel $enables
    wait
        echo 'Data detector tranformation process have finished'

    python3 plotDetectors.py $verbose $parallel $enables
    wait
        echo 'Data detector plotting process have finished'
} &

{
    python3 transform.py $verbose $parallel $enables
    wait
        echo 'Data transformation process have finished!'

    #plot has no parallel processing feature(is too short procedure)
    python3 plotNetwork.py $verbose $enables
    wait
        echo 'Data plotting process have finished!'

    python3 plotTTCLocations.py $verbose $parallel $enables
    wait
        echo 'Data plotting TTC locations process have finished'    

    python3 plotLaneChangesLocations.py $verbose $parallel $enables
    wait
        echo 'Data plotting lane changes locations process have finished'
} &

wait

secs_to_human "$(($(date +%s) - ${start}))"
