# to build this image run the following command
# $ docker build -t transaid --build-arg CREDS=<gitlab_user>:<gitlab_pass> - < Dockerfile
# to use it run (GUI applications won't work)
# $ docker run -it transaid bash
# now you have a bash inside a docker container and can for instance run
# $ cd /transaid/iTETRIS-Applications/traffic-monitor/acosta; iCS -c itetris-config-file-ns3.20.xml

FROM ubuntu:bionic

ARG CREDS

ENV SUMO_HOME=/transaid/sumo
ENV PATH=$SUMO_HOME/bin:/transaid/bin:$PATH
ENV LD_LIBRARY_PATH=/transaid/lib

RUN apt-get -y update
RUN apt-get -y install git libxerces-c-dev autoconf automake libtool libfox-1.6-dev libgl1-mesa-dev libglu1-mesa-dev libgdal-dev libproj-dev libgeographic-dev python-pip
RUN pip install texttest

RUN git clone --recursive https://$CREDS@gitlab.imet.gr/hit/transaid
RUN cd transaid; git checkout transaid-dev; git submodule update --init

RUN cd transaid/sumo; autoreconf -i; ./configure; make -j; export SUMO_HOME=$PWD

RUN cd transaid/ns-3.20; ./waf configure --prefix=$PWD/..; ./waf -j8; ./waf install

RUN cd transaid/iCS; autoreconf -i; ./configure; make -j; make install prefix=$PWD/..

RUN cd transaid/iTETRIS-Applications/traffic-monitor; autoreconf -i; ./configure; make -j; make install prefix=$PWD/../..
