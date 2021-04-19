#!/bin/bash
#chmod u+x update_linux.sh
#type ./update_linux.sh --password=YOURPASSWORD to run


RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"

password=""

printf  "\n${GREEN}Linux update starting.${NOCOLOR}\n"

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

#test
#echo $password | sudo -S ls
#exit

echo
echo -e "step 1: ${GREEN}pre-configuring packages${NOCOLOR}"
echo $password | sudo -S dpkg --configure -a

echo
echo -e "step 2: ${GREEN}fix and attempt to correct a system with broken dependencies${NOCOLOR}"
echo $password | sudo -S apt-get install -f -y

echo
echo -e "step 3: ${GREEN}update apt cache${NOCOLOR}"
echo $password | sudo -S apt-get update -y

echo
echo -e "step 4: ${GREEN}upgrade packages${NOCOLOR}"
echo $password | sudo -S apt-get upgrade -y

#echo
#echo -e "step 5: ${GREEN}distribution upgrade${NOCOLOR}"
#echo $password | sudo apt-get dist-upgrade

echo
echo -e "step 5: ${GREEN}remove unused packages${NOCOLOR}"
echo $password | sudo -S apt-get --purge autoremove -y

echo
echo -e "step 6: ${GREEN}clean up${NOCOLOR}"
echo $password | sudo -S apt-get autoclean -y

printf  "\n${GREEN}Linux update finished.${NOCOLOR}\n"
