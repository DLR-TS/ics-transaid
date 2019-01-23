#! /bin/bash

# This rebuilds sumo, ns-3, iCS, LightComm, demoApp, baseApp and testApp with debug symbols
# Intended to use for rebuild when checking out different commits. Therefore everything is reconfigured as well (comment out as required)

echo "## Rebuilding sumo in $PWD..."
cd sumo/build/cmake-build
cmake ../.. -DSUMO_UTILS=TRUE -DCMAKE_INSTALL_PREFIX=$PWD/../../.. -DCMAKE_BUILD_TYPE=Debug
make install -j
cd ../../..

echo "## Rebuilding ns-3 in $PWD..."
cd ns-3.20
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

echo "## Rebuilding demoApp in $PWD..."
cd iTETRIS-Applications/traffic-monitor
autoreconf -i
./configure --enable-debug --prefix=$PWD/../..
make install -j
cd ../..

echo "## Rebuilding baseApp in $PWD..."
cd iTETRIS-Applications/baseApp
autoreconf -i
./configure --enable-debug
make -j

echo "## Rebuilding testApp in $PWD..."
cd ../testApp
autoreconf -i
./configure --enable-debug --prefix=$PWD/../..
make install -j
cd ../..

echo "## Rebuilding LightComm in $PWD..."
cd LightCommSimulator
autoreconf -i
./configure --enable-debug --prefix=$PWD/..
make install -j
cd ..