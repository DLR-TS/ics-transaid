#!/bin/bash
# Bash script for the test run.

OLDDIR=$PWD
cd `dirname $0`

export TEXTTEST_HOME="$PWD"

if test x"$ICS_BIN" = x; then
  cd ..
  ICS_BIN="$PWD/bin"
  echo 'no path to iCS-binary given, assuming I am in the right directory '
fi
cd $OLDDIR

export ICSAPP_BINARY="$ICS_BIN/iCS"

if which texttest &> /dev/null; then
  texttest "$@"
else
  texttest.py "$@"
fi
