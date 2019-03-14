# to build this image run the following command
# $ docker build -t transaid --build-arg CREDS=<gitlab_user>:<gitlab_pass> - < Dockerfile
# to use it run (GUI applications won't work)
# $ docker run -it transaid bash
# now you have a bash inside a docker container and can for instance run
# $ cd /transaid/iTETRIS-Applications/traffic-monitor/acosta; iCS -c itetris-config-file-ns3.20.xml

FROM ubuntu:bionic

ARG CREDS

ENV SUMO_HOME=/transaid/share/sumo
ENV PATH=/transaid/bin:$PATH
ENV LD_LIBRARY_PATH=/transaid/lib

RUN apt-get -y update
RUN apt-get -y install psmisc vim git cmake autoconf automake libtool libxerces-c-dev libfox-1.6-dev libgl1-mesa-dev libglu1-mesa-dev libgdal-dev libproj-dev libgeographic-dev python-pip
RUN pip install texttest

RUN git clone --recursive https://$CREDS@gitlab.imet.gr/hit/transaid
RUN cd transaid; git checkout transaid-dev; git submodule update --init; ./build_all.sh
