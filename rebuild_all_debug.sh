#! /bin/bash

# This rebuilds sumo, ns-3, iCS, LightComm, demoApp, baseApp and testApp with debug symbols
# Intended to use for rebuild when checking out different commits. Therefore everything is reconfigured as well (comment out as required)

cd sumo/build/cmake-build
echo "## Rebuilding sumo in $PWD..."
cmake ../.. -DSUMO_UTILS=TRUE -DCMAKE_INSTALL_PREFIX=$PWD/../../.. -DCMAKE_BUILD_TYPE=Debug
make install -j
cd ../../..

cd ns-3.20
echo "## Rebuilding ns-3 in $PWD..."
./waf configure --enable-debug --prefix=$PWD/..
./waf -j
./waf install
cd ..

cd iCS
echo "## Rebuilding iCS in $PWD..."
autoreconf -i
./configure --enable-debug --prefix=$PWD/..
make install -j
cd ..

cd iTETRIS-Applications/traffic-monitor
echo "## Rebuilding demoApp in $PWD..."
autoreconf -i
./configure --enable-debug --prefix=$PWD/../..
make install -j
cd ../..

cd iTETRIS-Applications/baseApp
echo "## Rebuilding baseApp in $PWD..."
autoreconf -i
./configure --enable-debug
make -j

cd ../testApp
echo "## Rebuilding testApp in $PWD..."
autoreconf -i
./configure --enable-debug --prefix=$PWD/../..
make install -j
cd ../..

cd LightCommSimulator
echo "## Rebuilding LightComm in $PWD..."
autoreconf -i
./configure --enable-debug --prefix=$PWD/..
make install -j
cd ..