@echo off

setlocal EnableDelayedExpansion

REM setlocal ENABLEDELAYEDEXPANSION

REM Date    2020/01/01
REM Author  Vasilios Karagounis

REM change to local dir
setlocal & cd /d %~dp0

set STARTTIME=%TIME%

set verbose=
set parallel=
set enables=
set readEnable=0

for %%x in (%*) do (
    
    if "!readEnable!" == "2" (
        set "enables=!enables!,%%~x" )

    if "!readEnable!" == "1" (
        set /A readEnable=2
        set "enables=--en=%%~x" )

    if "%%~x" == "--verbose" (
        set verbose=--verbose 
    ) else if "%%~x" == "--parallel" (
        set parallel=--parallel
    
    ) else if "%%~x" == "--en" (
        set /A readEnable=1 )
)

REM echo %verbose%
REM echo %parallel%
REM echo %enables%

python unzip.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data unziping process have finished!"

python generate.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data generation process have finished!"

python transform.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data transformation process have finished!"

python plotNetwork.py %verbose% %enables%
:waittofinish
   echo "Data plotting process have finished!"


python transformTrajectories.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data trajectories tranformation process have finished"

python plotTrajectories.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data plotting trajectories process have finished"


python transformDetectors.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data detector tranformation process have finished"

python plotDetectors.py %verbose% %parallel% %enables%
:waittofinish
   echo "Data detector plotting process have finished"


python plotTTCLocations.py %verbose% %parallel% %enables%
:waittofinish
  echo "Data plotting TTC locations process have finished"    

python plotLaneChangesLocations.py %verbose% %parallel% %enables%
:waittofinish
  echo "Data plotting lane changes locations process have finished"
 

set ENDTIME=%TIME%

rem convert STARTTIME and ENDTIME to centiseconds
set /A STARTTIME=(1%STARTTIME:~0,2%-100)*360000 + (1%STARTTIME:~3,2%-100)*6000 + (1%STARTTIME:~6,2%-100)*100 + (1%STARTTIME:~9,2%-100)
set /A ENDTIME=(1%ENDTIME:~0,2%-100)*360000 + (1%ENDTIME:~3,2%-100)*6000 + (1%ENDTIME:~6,2%-100)*100 + (1%ENDTIME:~9,2%-100)

rem calculating the duratyion is easy
set /A DURATION=%ENDTIME%-%STARTTIME%

rem we might have measured the time inbetween days
if %ENDTIME% LSS %STARTTIME% set set /A DURATION=%STARTTIME%-%ENDTIME%

rem now break the centiseconds down to hours, minutes, seconds and the remaining centiseconds
set /A DURATION_SECS=%DURATION% / 86400

rem some formatting
if %DURATIONH% LSS 10 set DURATIONH=0%DURATIONH%
if %DURATIONM% LSS 10 set DURATIONM=0%DURATIONM%
if %DURATIONS% LSS 10 set DURATIONS=0%DURATIONS%

rem outputing
echo STARTTIME: %STARTTIME% centiseconds
echo ENDTIME: %ENDTIME% centiseconds
echo DURATION: %DURATION% in centiseconds


