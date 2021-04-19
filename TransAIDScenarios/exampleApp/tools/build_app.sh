#!/bin/bash
#chmod u+x build_app.sh
#type ./build_app.sh to run

RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"

app_path=""
configure=true
debug=false
config_debug=""

echo ""
echo -e "${GREEN}build process started${NOCOLOR}"

for arg in "$@"
do

    key=$(echo $arg | cut -f1 -d=)
    temp=$(echo $arg | cut -f2 -d=)   

    if [[ $key == "--path" ]]; then
        app_path=($(echo "$temp" | tr ',' '\n'))
    fi

    if [[ $key == "--noconf" ]]; then
        configure=false
    fi

    if [[ $key == "--debug" ]]; then
        debug=true
    fi
done

if [ -z "$app_path" ]; then
    #set current working directory to the directory of the script
    cd "${0%/*}"
else
    cd $app_path
fi

if [[ $debug == true ]]; 
then
    config_debug="--enable-debug"
fi

if [[ $configure == true ]]; 
then
    #-i, –install : Copy missing auxiliary files
    #create automatically buildable source code for Unix-like systems
    autoreconf -i
    wait
        echo -e "${GREEN}autoreconf finish${NOCOLOR}"
        echo ""

    ./configure $config_debug --prefix="$PWD/../.."
    wait
        echo -e "${GREEN}configure finish${NOCOLOR}"
        echo ""
fi

make install
wait
    echo -e "${GREEN}build process finished${NOCOLOR}"