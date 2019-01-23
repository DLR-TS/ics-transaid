#! /bin/bash
# This builds and (locally) installs sumo, ns-3, iCS, LightComm, demoApp, baseApp and testApp

echo "## Setting up sumo..."
export SUMO_HOME=$PWD/share/sumo
mkdir sumo/build/cmake-build
cd sumo/build/cmake-build
cmake ../.. -DSUMO_UTILS=TRUE -DCMAKE_INSTALL_PREFIX=$PWD/../../..
make install -j
cd ../../..

echo "## Setting up ns-3..."
cd ns-3.20
./waf configure --prefix=$PWD/..
./waf -j
./waf install
cd ..

cd iCS
echo "## Setting up iCS..."
autoreconf -i
./configure --prefix=$PWD/..
make install -j
cd ..

echo "## Setting up demoApp..."
cd iTETRIS-Applications/traffic-monitor
autoreconf -i
./configure --prefix=$PWD/../..
make install -j
cd ../..

echo "## Setting up baseApp..."
cd iTETRIS-Applications/baseApp
autoreconf -i
./configure
make -j

echo "## Setting up testApp..."
cd ../testApp
autoreconf -i
./configure --prefix=$PWD/../..
make install -j
cd ../..

echo "## Setting up LightComm..."
cd LightCommSimulator
autoreconf -i
./configure --prefix=$PWD/..
make install -j
cd ..

export PATH=$PWD/bin:$PATH
export LD_LIBRARY_PATH=$PWD/lib