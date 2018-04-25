#!/bin/bash
# Bash script for the test run.

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME="$PWD"
export ICSAPP_BINARY="$PWD/../bin/iCS"
cd $OLDDIR

if which texttest &> /dev/null; then
  texttest "$@"
else
  texttest.py "$@"
fi
