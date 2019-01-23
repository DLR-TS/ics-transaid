#! /bin/bash

# This rebuilds sumo, ns-3, iCS, LightComm, demoApp, baseApp and testApp with debug symbols
# Intended to use for rebuild when checking out different commits. Therefore everything is reconfigured as well (comment out as required)

<<<<<<< 66d2f40258ba9b54b5a1b2011b8f18a6c3e92528
cd sumo/build/cmake-build
echo "## Rebuilding sumo in $PWD..."
=======
echo "## Rebuilding sumo in $PWD..."
cd sumo/build/cmake-build
>>>>>>> Added rebuild script with debug configuration
cmake ../.. -DSUMO_UTILS=TRUE -DCMAKE_INSTALL_PREFIX=$PWD/../../.. -DCMAKE_BUILD_TYPE=Debug
make install -j
cd ../../..

<<<<<<< 66d2f40258ba9b54b5a1b2011b8f18a6c3e92528
cd ns-3.20
echo "## Rebuilding ns-3 in $PWD..."
=======
echo "## Rebuilding ns-3 in $PWD..."
cd ns-3.20
>>>>>>> Added rebuild script with debug configuration
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

<<<<<<< 66d2f40258ba9b54b5a1b2011b8f18a6c3e92528
cd iTETRIS-Applications/traffic-monitor
echo "## Rebuilding demoApp in $PWD..."
=======
echo "## Rebuilding demoApp in $PWD..."
cd iTETRIS-Applications/traffic-monitor
>>>>>>> Added rebuild script with debug configuration
autoreconf -i
./configure --enable-debug --prefix=$PWD/../..
make install -j
cd ../..

<<<<<<< 66d2f40258ba9b54b5a1b2011b8f18a6c3e92528
cd iTETRIS-Applications/baseApp
echo "## Rebuilding baseApp in $PWD..."
=======
echo "## Rebuilding baseApp in $PWD..."
cd iTETRIS-Applications/baseApp
>>>>>>> Added rebuild script with debug configuration
autoreconf -i
./configure --enable-debug
make -j

<<<<<<< 66d2f40258ba9b54b5a1b2011b8f18a6c3e92528
cd ../testApp
echo "## Rebuilding testApp in $PWD..."
=======
echo "## Rebuilding testApp in $PWD..."
cd ../testApp
>>>>>>> Added rebuild script with debug configuration
autoreconf -i
./configure --enable-debug --prefix=$PWD/../..
make install -j
cd ../..

<<<<<<< 66d2f40258ba9b54b5a1b2011b8f18a6c3e92528
cd LightCommSimulator
echo "## Rebuilding LightComm in $PWD..."
=======
echo "## Rebuilding LightComm in $PWD..."
cd LightCommSimulator
>>>>>>> Added rebuild script with debug configuration
autoreconf -i
./configure --enable-debug --prefix=$PWD/..
make install -j
cd ..