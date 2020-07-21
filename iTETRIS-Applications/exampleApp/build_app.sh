#!/bin/bash
#chmod u+x build_app.sh
#type ./build_app.sh to run

RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"

#set current working directory to the directory of the script
cd "${0%/*}"

echo ""
echo -e "${GREEN}build process started${NOCOLOR}"

autoreconf -i
wait
    echo -e "${GREEN}autoreconf finish${NOCOLOR}"
    echo ""

./configure --prefix="$PWD/../.."
wait
    echo -e "${GREEN}configure finish${NOCOLOR}"
    echo ""

make install
wait
    echo -e "${GREEN}build process finished${NOCOLOR}"