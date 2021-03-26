
#!/bin/bash
#chmod u+x update_sumo.sh
#type ./update_sumo.sh --path=SUMO_PATH --password=LINUX_PASS --hash=SUMO_GIT_VERSION to run

#1. Open SUMO folder
#2. Open terminal inside the opened folder
#3. git pull --rebase     
#4. mkdir build/cmake-build
#5. cd build/cmake-build
#6. make -j 8
#7. sudo make install


RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"

sumo_build_folder="build/cmake-build"

sumo_path=""
password=""
specific_hash=""

old_hash=""
new_hash=""

printf  "\n${GREEN}SUMO update starting.${NOCOLOR}\n"

#hash tester
#e6fab6edfa6fa0b028d35a1913f42a7dcdcd7ca2

#look in arguments
for arg in "$@"
do

 case $arg in
    -e=*|--path=*)
    sumo_path="${arg#*=}"
    shift # past argument=value
    ;;
    -e=*|--password=*)
    password="${arg#*=}"
    shift # past argument=value
    ;;
    -e=*|--hash=*)
    specific_hash="${arg#*=}"
    shift # past argument=value
    ;;    
    *)
    ;;
esac
done

if [ -z "$password" ]; then
    echo "${RED}sudo password is missing: type --password=${NOCOLOR}"
    exit 1
fi

if [ -z "$sumo_path" ]; then
    echo "${RED}SUMO path is missing: type --path=${NOCOLOR}"
    exit 1
fi

cd $sumo_path

echo $sumo_path

git config --global advice.detachedHead false

old_hash=$(git rev-parse HEAD)

if [ -z "$specific_hash" ]; then
    echo "${GREEN}Updating SUMO to the latest version.${NOCOLOR}"
    echo $password | sudo -S -H chmod g+w .git -R
    git pull --rebase

    new_hash=$(git rev-parse HEAD)

    if [ $new_hash != $old_hash ]; then
        echo "${GREEN}Building updated SUMO version.${NOCOLOR}"
        mkdir -p build/cmake-build
        cd build/cmake-build
        make -j 8
        echo $password | sudo -S make install
    else
        echo "${GREEN}SUMO version is the same.${NOCOLOR}"    
    fi

else
    if [ $specific_hash != $old_hash ]; then

        #echo "${GREEN}Updating SUMO to specific version.${NOCOLOR}"
        #echo
        #echo -e "step 2: ${GREEN}Removing sumo folder if exist.${NOCOLOR}"
        #rm -rf $sumo_path
#
        #echo
        #echo -e "step 3: ${GREEN}Updating linux OS.${NOCOLOR}"
        #sudo apt-get update
#
        #echo
        #echo -e "step 4: ${GREEN}Installing SUMO libraries.${NOCOLOR}"
        #sudo apt-get install cmake python3 g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev swig
#
        #echo
        #echo -e "step 5: ${GREEN}Cleaning linux OS.${NOCOLOR}"
        #sudo apt autoremove
#
        #echo
        #echo -e "step 7: ${GREEN}Cloning SUMO git.${NOCOLOR}"
        #git clone --recursive https://github.com/eclipse/sumo.git
#
        #echo
        #echo -e "step 8: ${GREEN}Entering sumo folder.${NOCOLOR}"
        #eval cd $sumo_path
        #echo "Current working directory:"$PWD

        #echo
        #echo -e "step 9: ${GREEN}Exporting SUMO path.${NOCOLOR}"
        #export SUMO_HOME="$PWD"
#
        #echo
        #echo -e "step 10: ${GREEN}Creating SUMO build folder.${NOCOLOR}"
        #mkdir -p $sumo_build_folder
        #eval cd $sumo_build_folder
        #echo "Current working directory:"$PWD
#
        #echo
        #echo -e "step 11: ${GREEN}Producing makefiles of SUMO.${NOCOLOR}"
        #cmake ../..
#
        #echo
        #echo -e "step 12: ${GREEN}Building SUMO code.${NOCOLOR}"
        #make -j 8 #$(nproc)

        echo "${GREEN}Updating SUMO to specific version.${NOCOLOR}"
        echo
        git reset --hard $specific_hash

        echo "${GREEN}Building specific SUMO version.${NOCOLOR}"
        mkdir -p build/cmake-build
        cd build/cmake-build
        make -j 8
        echo $password | sudo -S make install

    else
        echo "${GREEN}SUMO version is the same.${NOCOLOR}"    
    fi
fi  

cd $sumo_path

hash=$(git rev-parse HEAD)
date=$(git log -1 --format=%cd --date=local)
author=$(git show -s --format='%ae' ${hash})

echo "${hash}"
echo "${date}"
echo "${author}"

printf  "\n${GREEN}SUMO update finished${NOCOLOR}\n"
