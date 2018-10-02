#!/bin/bash
# Bash script for the test run.

OLDDIR=$PWD
cd `dirname $0`
TRANSAID="$PWD/../.."
export TEXTTEST_HOME="$PWD"
export ICSAPP_BINARY="$PWD/../bin/iCS"
export SUMO_HOME="$TRANSAID/sumo"
export PATH=$SUMO_HOME/bin:$TRANSAID/bin:$PATH
export LD_LIBRARY_PATH=$TRANSAID/lib
cd $OLDDIR

if which texttest &> /dev/null; then
  texttest "$@"
else
  texttest.py "$@"
fi
