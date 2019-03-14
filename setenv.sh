# This sets the required environment variable and is supposed to be called like this: ". setenv.sh"

export SUMO_HOME=$PWD/share/sumo
export PATH=$PWD/bin:$PATH
export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
# this sets the prefix for the profiling outputs
export GMON_OUT_PREFIX=gmon
