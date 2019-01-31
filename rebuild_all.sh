#! /bin/bash

# This rebuilds sumo, ns-3, iCS, LightComm, demoApp, baseApp and testApp (for initial install follow iTETRIS_quick_installation_ubuntu.txt)
# Intended to use for rebuild when checking out different commits. Therefore everything is reconfigured as well (comment out as required)

cd sumo/build/cmake-build
echo "## Rebuilding sumo in $PWD..."
cmake ../.. -DSUMO_UTILS=TRUE -DCMAKE_INSTALL_PREFIX=$PWD/../../..
make install -j8
cd ../../..

cd ns-3.20
echo "## Rebuilding ns-3 in $PWD..."
./waf configure --prefix=$PWD/..
./waf -j8
./waf install
cd ..

cd iCS
echo "## Rebuilding iCS in $PWD..."
autoreconf -i
./configure --prefix=$PWD/..
make install -j8
cd ..

cd iTETRIS-Applications/traffic-monitor
echo "## Rebuilding demoApp in $PWD..."
autoreconf -i
./configure --prefix=$PWD/../..
make install -j8
cd ../..

cd iTETRIS-Applications/baseApp
echo "## Rebuilding baseApp in $PWD..."
autoreconf -i
./configure
make -j8

cd ../testApp
echo "## Rebuilding testApp in $PWD..."
autoreconf -i
./configure --prefix=$PWD/../..
make install -j8
cd ../..

cd LightCommSimulator
echo "## Rebuilding LightComm in $PWD..."
autoreconf -i
./configure --prefix=$PWD/..
make install -j8
cd ..