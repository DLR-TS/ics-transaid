FROM ubuntu:latest

ARG CREDS

RUN apt-get -y update
RUN apt-get -y install git libxerces-c-dev autoconf automake libtool libfox-1.6-dev libgl1-mesa-dev libglu1-mesa-dev libgdal-dev libproj-dev libgeographic-dev python-pip
RUN pip install texttest

RUN git clone --recursive https://$CREDS@gitlab.imet.gr/hit/transaid
RUN cd transaid; git checkout transaid-dev; git submodule update --init

RUN cd transaid/sumo; autoreconf -i; ./configure; make -j; export SUMO_HOME=$PWD

RUN cd transaid/ns-3.20; ./waf configure --prefix=$PWD/..; ./waf -j8; ./waf install

RUN cd transaid/iCS; autoreconf -i; ./configure; make -j; make install prefix=$PWD/..

RUN cd transaid/iTETRIS-Applications/traffic-monitor; autoreconf -i; ./configure; make -j; make install prefix=$PWD/../..
