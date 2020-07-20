#!/bin/bash
#chmod u+x run_app.sh
#type ./run_app.sh to run

RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"

echo ""
echo -e "${GREEN}running process started${NOCOLOR}"

cd ../../TransAIDScenarios/exampleApp/config

WORK_DIR=$PWD
echo "Working path : ${WORK_DIR}"

iCS -c itetris_cfg_template_tm.xml

wait
    echo -e "${GREEN}running process finished${NOCOLOR}"
