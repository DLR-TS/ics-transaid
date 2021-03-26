@echo off

 ECHO
 ECHO Start updating python libraries

 cmd.exe /c "python -m pip install --upgrade pip"

 cmd.exe /c  "pip3 install psutil numpy pandas matplotlib seaborn comtypes"

ECHO Finish updating python libraries

timeout 5 >nul
exit /B