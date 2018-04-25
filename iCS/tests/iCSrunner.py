# @file    iCSrunner.py
# @author  Michael Behrisch
# @date    2018-04-25
# @version $Id$

import os
import subprocess
import sys

if os.path.exists("data"):
    os.chdir("data")
iCS = [os.environ.get('ICS_BINARY', 'iCS')]

subprocess.call(iCS + sys.argv[1:], env=os.environ,
                stdout=sys.stdout, stderr=sys.stderr)
