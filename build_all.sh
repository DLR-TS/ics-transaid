#! /bin/bash

# This builds or rebuilds sumo, ns-3, iCS, LightComm and all apps
# It understands the configurations "debug" and "profile" as parameter
# 
# Please note that you have to install additional dependencies before 
# the build will succeed. These are currently (Ubuntu 18.04):
# SUMO: cmake, libxerces-c-dev, libgdal-dev, libproj-dev, libfox-1.6-dev
# iTETRIS: autoconf, make and the Geographic Lib, which you need to install
# manually: Download the latest Linux tarball from 
#   https://sourceforge.net/projects/geographiclib/files/latest/download
# Unpack it and type ./configure && make -j8 && sudo make install

if test "$1" == "debug"; then
    cmake_opt="-DCMAKE_BUILD_TYPE=Debug"
    config_opt="--enable-debug"
    waf_opt="--build-profile=debug"
    waf_flags="-std=c++11 -g"
fi
if test "$1" == "profile"; then
    export CXXFLAGS=-pg
    config_opt="--enable-debug"
    cmake_opt="-DCMAKE_BUILD_TYPE=Debug -DPROFILING=ON"
    waf_flags="-std=c++11"
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
CXXFLAGS="$waf_flags" python2 ./waf configure $waf_opt --prefix=$PWD/..
if ! python2 ./waf -j8; then
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

cd iTETRIS-Applications

cd baseApp
echo "## Building baseApp in $PWD..."
autoreconf -i
./configure $config_opt --prefix=$PWD/../..
if ! make install -j8; then
    echo "### Build failed!"
    cd ../..
    exit 1
fi
cd ..

for app in *; do
    if test $app = "baseApp"; then
        continue
    fi
    cd $app
    if test -e configure.ac; then
        echo "## Building app in $PWD..."
        autoreconf -i
        ./configure $config_opt --prefix=$PWD/../..
        if ! make install -j8; then
            echo "### Build failed!"
            cd ../..
            exit 1
        fi
    fi
    cd ..
done
cd ..

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
