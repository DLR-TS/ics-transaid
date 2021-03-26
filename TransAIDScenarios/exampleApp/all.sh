#!/bin/bash
#chmod u+x all.sh
#type ./all.sh to run

python3 batchRunner.py --manual
wait
    echo 'manual scenario have finished'

python3 batchRunner.py --baseline
wait
    echo 'baseline scenario have finished'

python3 batchRunner.py --dayonecits

wait
    echo 'dayonecits scenario have finished'

python3 batchRunner.py --tm

wait
    echo 'tm scenario have finished'
