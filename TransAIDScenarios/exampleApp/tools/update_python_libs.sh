#!/bin/bash
#chmod u+x update_python_libs.sh
#type ./update_python_libs.sh --password=YOURPASSWORD to run


RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"

password=""

printf  "\n${GREEN}Python libraries update starting.${NOCOLOR}\n"

for arg in "$@"
do

    key=$(echo $arg | cut -f1 -d=)
    temp=$(echo $arg | cut -f2 -d=)   
    if [[ $key == "--password" ]]; then
        password=($(echo "$temp" | tr ',' '\n'))
    fi
done

if [[ $password == "" ]];
then
    echo -e "${RED}sudo password is missing: type --password=${NOCOLOR}"
    exit 1
fi

echo
echo -e "step 1: ${GREEN}update apt cache${NOCOLOR}"
echo $password | sudo -S apt-get update -y

echo
echo -e "step 2: ${GREEN}upgrade pip3 package${NOCOLOR}"
echo $password | sudo -S -H pip3 install --upgrade pip

echo
echo -e "step 2: ${GREEN}upgrade python package${NOCOLOR}"
echo $password | sudo -S pip3 install --upgrade psutil numpy pandas matplotlib seaborn

echo
echo -e "step 3: ${GREEN}update tkinter package${NOCOLOR}"
echo $password | sudo -S apt-get install python3-tk

echo
echo -e "step 4: ${GREEN}remove unused packages${NOCOLOR}"
echo $password | sudo -S apt-get --purge autoremove -y

printf  "\n${GREEN}Python libraries update finished${NOCOLOR}\n"
