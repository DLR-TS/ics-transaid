
#!/bin/bash
#chmod u+x get_sumo_version.sh
#type ./get_sumo_version.sh --path=SUMO_PATH to run

sumo_path=""

#look in arguments
for arg in "$@"
do

 case $arg in
    -e=*|--path=*)
    sumo_path="${arg#*=}"
    shift # past argument=value
    ;;
    *)
    ;;
esac

done

if [ -z "$sumo_path" ];
then
    echo "sudo path is missing: type --path="
    exit 1
fi

cd $sumo_path

hash=$(git rev-parse HEAD)
date=$(git log -1 --format=%cd --date=local)
author=$(git show -s --format='%ae' ${hash})

echo "${hash},${date},${author}"
