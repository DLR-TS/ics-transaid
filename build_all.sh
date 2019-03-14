#! /bin/bash

# This builds or rebuilds sumo, ns-3, iCS, LightComm, demoApp, baseApp and testApp
# It understands the configurations "debug" and "profile" as parameter

if test "$1" == "debug"; then
    cmake_opt="-DCMAKE_BUILD_TYPE=Debug"
    config_opt="--enable-debug"
    waf_opt="--build-profile=debug"
fi
if test "$1" == "profile"; then
    config_opt="CXXFLAGS=-pg --enable-debug"
    cmake_opt="-DCMAKE_BUILD_TYPE=Debug -DPROFILING=ON"
fi

mkdir -p sumo/build/cmake-build$1
cd sumo/build/cmake-build$1
echo "## Building sumo in $PWD..."
cmake ../.. -DSUMO_UTILS=TRUE $cmake_opt -DCMAKE_INSTALL_PREFIX=$PWD/../../..
if ! make install -j8; then
    echo "### Build failed!"
    cd ../../..
    exit 1
fi
cd ../../..

cd ns-3.20
echo "## Building ns-3 in $PWD..."
./waf configure $waf_opt --prefix=$PWD/..
if ! ./waf -j8; then
    echo "### Build failed!"
    cd ..
    exit 1
fi
./waf install
cd ..

cd iCS
echo "## Building iCS in $PWD..."
autoreconf -i
./configure $config_opt --prefix=$PWD/..
if ! make install -j8; then
    echo "### Build failed!"
    cd ..
    exit 1
fi
cd ..

cd iTETRIS-Applications/traffic-monitor
echo "## Building demoApp in $PWD..."
autoreconf -i
./configure $config_opt --prefix=$PWD/../..
if ! make install -j8; then
    echo "### Build failed!"
    cd ../..
    exit 1
fi
cd ../..

cd iTETRIS-Applications/baseApp
echo "## Building baseApp in $PWD..."
autoreconf -i
./configure $config_opt
if ! make install -j8; then
    echo "### Build failed!"
    cd ../..
    exit 1
fi

cd ../testApp
echo "## Building testApp in $PWD..."
autoreconf -i
./configure $config_opt --prefix=$PWD/../..
if ! make install -j8; then
    echo "### Build failed!"
    cd ../..
    exit 1
fi
cd ../..

cd LightCommSimulator
echo "## Building LightComm in $PWD..."
autoreconf -i
./configure $config_opt --prefix=$PWD/..
if ! make install -j8; then
    echo "### Build failed!"
    cd ..
    exit 1
fi
cd ..
