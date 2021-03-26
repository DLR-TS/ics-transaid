#!/usr/bin/env python

# Date: 2020/01/01
# Author: Vasilios Karagounis

import os
import sys
import stat

import subprocess
from subprocess import Popen

from tkinter import *
from tkinter.ttk import * 
from tkinter import messagebox

import signal
import psutil
import json
from xml.dom import minidom
import pathlib
from datetime import datetime
import time

import ctypes
from sys import platform as _platform
from pathlib import Path
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
VERSION = '1.0.0'

scenarioDefault = 0
simulationDefault = "ALL"

#http://zetcode.com/tkinter/dialogs/

window = Tk()

wdir =  os.path.dirname(os.path.realpath(__file__))

batch_json = {}
plots_json = {}
distribution_json = {}

argsInt = []
argsDemandsInt = []
argsMixesInt = []
argsSchemesInt = []

vehiclesSizeInt = []
vehiclesSchemeInt = []

scenarioRun = IntVar()
proccessPid = None

argsMixesCheckButtons = []

PLATFORM_HOME = ""
sumo_path = ""
pythonRun = ""
terminal = ""
windows_sumo_version = ""

downloadProcess = False
rsync_proc = 0

plotEnableChkBtns = []

plotScenariosInt = []
plotParallel = BooleanVar()
plotVerbose = BooleanVar()

plotProcess = False
plot_proc = 0
plot_proc_pid = None

update_sumo_button = None
updateSumoProcess = False
update_sumo_proc = 0

update_linux_button = None
updateLinuxProcess = False
update_linux_proc = 0

update_python_button = None
updatePythonProcess = False
update_python_proc = 0

chatMessage = StringVar()
sumo_update_hash = StringVar()

fetch_button = None
run_batch_runner_button = None
stop_batch_runner_button = None
terminate_plot_button = None
compile_app_button = None

plot_buttons = {}

is_linux = 1
is_mac = 2
is_win = 3

OS_TYPE = 0

git_sumo_version = []
labels_git_version = []

net_files_clean = [] 
netFilesComboBox = None
createNewConsole = BooleanVar()
keepOpenNewConsole = BooleanVar()
verbose = BooleanVar()
buildCppWithDebug = BooleanVar()
buildCppWithConfig = BooleanVar()

compileApp = False
compile_app_proc = 0
compile_app_proc_pid = None

compileBaseApp = False
compile_base_app_proc = 0
compile_base_app_proc_pid = None

running_in_transaid = False

class ToolTip(object):

    def __init__(self, widget):
        self.widget = widget
        self.tipwindow = None
        self.id = None
        self.x = self.y = 0

    def showtip(self, text):
        "Display text in tooltip window"
        self.text = text
        if self.tipwindow or not self.text:
            return
        x, y, cx, cy = self.widget.bbox("insert")
        x = x + self.widget.winfo_rootx() + 57
        y = y + cy + self.widget.winfo_rooty() +27
        self.tipwindow = tw = Toplevel(self.widget)
        tw.wm_overrideredirect(1)
        tw.wm_geometry("+%d+%d" % (x, y))
        label = Label(tw, text=self.text, justify=LEFT,
                      background="#ffffe0", relief=SOLID, borderwidth=1,
                      font=("arial", "10", "normal"))
        label.pack(ipadx=1)

    def hidetip(self):
        tw = self.tipwindow
        self.tipwindow = None
        if tw:
            tw.destroy()

def CreateToolTip(widget, text):
    toolTip = ToolTip(widget)
    def enter(event):
        toolTip.showtip(text)
    def leave(event):
        toolTip.hidetip()
    widget.bind('<Enter>', enter)
    widget.bind('<Leave>', leave)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def find_platform():
    
    global OS_TYPE, pythonRun, terminal

    if _platform == "linux" or _platform == "linux2":
        OS_TYPE = is_linux
        pythonRun = "python3"
        terminal = "gnome-terminal"
    elif _platform == "darwin":
        OS_TYPE = is_mac
    elif _platform == "win32":
        OS_TYPE = is_win
        pythonRun = "python"
        terminal = "cmd"
    elif _platform == "win64":
        OS_TYPE = is_win
        pythonRun = "python"
        terminal = "cmd"
    else:
        print("Error : Unknown operating system")
        sys.exit(1)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def run_as_admin():
    
    global OS_TYPE

    if OS_TYPE == is_win:
        # Re-run the program with admin rights
        if not is_admin():
            ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, __file__, None, 1)
            sys.exit(0)
        else:
            myappid = 'mycompany.myproduct.subproduct.version' 
            ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid) 

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def debugPrint(*args):

    global chatMessage

    msg = ''.join(map(str, args)) 
    print(msg)
    chatMessage.set(msg)
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def readData():
    
    global wdir, batch_json, plots_json, distribution_json, sumo_path
    global net_files_clean

    path = os.path.join(wdir, "settings/batchRunner.json")

    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                batch_json = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            if verbose.get():
                print("Json settings path :", path)
    else:
         print("Path not found (readScenarios) :", path)

    path = os.path.join(wdir, "plots/settings/plots.json")

    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                plots_json = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            if verbose.get():
                print("Json plot settings path :", path)
    else:
         print("Path not found (readScenarios) :", path)

    path = os.path.join(wdir, "tools/settings/distribution.json")

    if os.path.exists(path):
        with open(path, 'r') as f:
            try:
                distribution_json = json.load(f)
            except ValueError as e:
                print("Json error :", e)
                sys.exit(1)
            if verbose.get():
                print("Json plot settings path:", path)
    else:
         print("Path not found (readScenarios) :", path)      

    if OS_TYPE == is_linux:

        #gedit ~/.profile
        sumo_path = os.environ.get("SUMO_HOME")
        #print(sumo_path)
        if not sumo_path or not os.path.exists(sumo_path):
            for search in batch_json["linux_search_sumo"]:  # try from json
                sumo_path = os.path.join(PLATFORM_HOME, search)
                if os.path.exists(sumo_path):
                    if verbose.get():
                        print("SUMO_HOME", sumo_path)
                    break    
    elif OS_TYPE == is_win:
        for search in batch_json["win_search_sumo"]:
            sumo_path = os.path.join(PLATFORM_HOME, search)
            if os.path.exists(sumo_path):
                if verbose.get():
                    print("SUMO_HOME", sumo_path)
                break

    for fileid, file in enumerate(batch_json["args_scenarios_clean"]):
        temp = batch_json["net_file_" + str(fileid)]
        if temp not in net_files_clean:
            net_files_clean.append(temp)

    
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def setPlotButtons(enable):

    for name in plot_buttons:

        if plots_json["data_buttons"][name] == 1:
            wd = plot_buttons[name]
            if enable:
                wd["state"] = "enabled"
            else:
                wd["state"] = "disabled"

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def runBatchRunner():

    #subprocess.Popen(args_, bufsize=0, executable=None, stdin=None, stdout=None, stderr=None, preexec_fn=None, close_fds=False, shell=False, 
    # cwd=None, env=None, universal_newlines=False, startupinfo=None, creationflags=0)
    global proccessPid, argsInt, argsDemandsInt, argsMixesInt, argsSchemesInt
    global scenarioRun, comboSimText, wdir, terminal, pythonRun
    global OS_TYPE, createNewConsole, keepOpenNewConsole

    newConsole = createNewConsole.get()
    keepConsole = keepOpenNewConsole.get()

    pathBatchRunner = ""
    args = []
    argsFinal = ""

    if OS_TYPE == is_win:
        pathBatchRunner = "\"" + os.path.join(wdir, "batchRunner.py\" ")
    elif OS_TYPE == is_linux:
        pathBatchRunner = os.path.join(wdir, "batchRunner.py ")

    #/k Run Command and then return to the CMD prompt
    #/c Run Command and then terminate 
    #-q removes the verbose output

    if proccessPid == None:
        if newConsole:
            if OS_TYPE == is_win:
                if keepConsole:
                    argsFinal = "start cmd /k " + pythonRun + " " + pathBatchRunner
                else:
                    argsFinal = "start cmd /c " + pythonRun + " " + pathBatchRunner
            elif OS_TYPE == is_linux:
                argsFinal = terminal + " -q -- bash -c \"" + pythonRun + " " + pathBatchRunner
        else:
            argsFinal = pythonRun + " " + pathBatchRunner
        
        index = 0
        for id in argsInt:
            if id.get() == 1:
                argsFinal += batch_json["args"][index] + " "
            index+=1

        if comboSimText.get() != simulationDefault:
            minVal = int(comboSimText.get()) - 1
            argsFinal += "--sim=" + str(minVal) + "," + comboSimText.get() + " "
            
        index = 0
        for id in argsDemandsInt:
            if id.get() == 1:
                argsFinal += batch_json["args_demands"][index] + " "
            index+=1

        index = 0
        for id in argsMixesInt:
            if id.get() == 1:
                argsFinal += batch_json["args_mix"][index] + " "
            index+=1

        index = 0
        for id in argsSchemesInt:
            if id.get() == 1:
                argsFinal += batch_json["args_schemes"][index] + " "
            index+=1

        argsFinal += batch_json["args_scenarios"][scenarioRun.get()]

        #remove exit parameter if u want to prevent new console from auto closing
        if newConsole:
            if OS_TYPE == is_linux:
                if keepConsole:
                    argsFinal +="; bash\""
                else:
                    argsFinal +="; exit bash\""

        ############check mixes##########
        count_mixes = len(batch_json["traffic_mix_" + str(scenarioRun.get())])

        mixesError = False
        index = 0
        for id in argsMixesInt:
            if id.get() == 1:
                if index >= count_mixes:
                    mixesError = True
            index+=1    

        if mixesError:
            debugPrint("Selected wrong traffic mix for this scenario. Max mixes are:", count_mixes)
            return
        ############
        if newConsole:
            if OS_TYPE == is_win:
                proccess = subprocess.Popen(argsFinal, shell = True)
            elif OS_TYPE == is_linux:
                proccess = subprocess.Popen(argsFinal, stdout = subprocess.PIPE, stderr = subprocess.PIPE, shell = True, env = os.environ)
        else:
            if OS_TYPE == is_win:
                proccess = subprocess.Popen(argsFinal, shell = True, env = os.environ)
            elif OS_TYPE == is_linux:
                args.append(argsFinal)
                proccess = subprocess.Popen(args, shell = True, env = os.environ)
        
        proccessPid = psutil.Process(proccess.pid)

        stop_batch_runner_button["state"] = "enabled"
        run_batch_runner_button["state"] = "disabled"

        if verbose.get():
            debugPrint("Run args: ", argsFinal)
            debugPrint("Process info: ", proccessPid)
    else:
        debugPrint("Process with pid: ", proccessPid, " is still opened")

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def killsubprocesses(parent_pid):
    '''kill parent and all subprocess using COM/WMI and the win32api'''
    try:
        import comtypes.client
    except ImportError:
        debugPrint("comtypes library is missing!")
        return

    # get pid and subprocess pids for all alive processes
    WMI = comtypes.client.CoGetObject('winmgmts:')
    processes = WMI.InstancesOf('Win32_Process')
    subprocess_pids = {} # parent pid -> list of child pids

    for process in processes:
        pid = process.Properties_('ProcessID').Value
        parent = process.Properties_('ParentProcessId').Value
        subprocess_pids.setdefault(parent, []).append(pid)
        subprocess_pids.setdefault(pid, [])

    processes_to_kill = []
    parent_processes = [parent_pid]
    while parent_processes:
        try:
            current_pid = parent_processes.pop()
            subps = subprocess_pids[current_pid]
            parent_processes.extend(subps)
            processes_to_kill.extend(subps)
        except:
            pass

    kernel32 = ctypes.windll.kernel32

    PROCESS_TERMINATE = 1

    for pid in processes_to_kill:
        hProcess = kernel32.OpenProcess(PROCESS_TERMINATE, FALSE, pid)
        if hProcess:
            kernel32.TerminateProcess(hProcess, 3)
            kernel32.CloseHandle(hProcess)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def findsubprocesses(parent_pid):

    if not parent_pid:
        return False
    try:
        import comtypes.client
    except ImportError:
        debugPrint("comtypes library is missing!")
        return

    # get pid and subprocess pids for all alive processes
    WMI = comtypes.client.CoGetObject('winmgmts:')
    processes = WMI.InstancesOf('Win32_Process')
    subprocess_pids = {} # parent pid -> list of child pids

    for process in processes:
        pid = process.Properties_('ProcessID').Value
        parent = process.Properties_('ParentProcessId').Value
        subprocess_pids.setdefault(parent, []).append(pid)
        subprocess_pids.setdefault(pid, [])

    processes_find = []
    parent_processes = [parent_pid]
    while parent_processes:
        try:
            current_pid = parent_processes.pop()
            subps = subprocess_pids[current_pid]
            parent_processes.extend(subps)
            processes_find.extend(subps)
        except:
            return False

    if processes_find:
        return True
    else: 
        return False
    
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def stopBatchRunner():
    
    global proccessPid, OS_TYPE
    
    if proccessPid:
        if OS_TYPE == is_linux:
            for proc in psutil.process_iter():
                if verbose.get():
                    debugPrint(proc)

                if proc.name() == "sh" and proc.pid >= proccessPid.pid:
                    if verbose.get():
                        debugPrint("Kill process with pid :", proc.pid)
                    proc.terminate()

                if proc.name() == pythonRun and proc.pid >= proccessPid.pid:
                    if verbose.get():
                        debugPrint("Kill process with pid :", proc.pid)
                    proc.terminate()
                
                if proc.name() == "sumo-gui" and proc.pid >= proccessPid.pid:
                    if verbose.get():
                        debugPrint("Kill process with pid :", proc.pid)
                    proc.terminate()

                if proc.name() == "iCS" and proc.pid >= proccessPid.pid:
                    if verbose.get():
                        debugPrint("Kill process with pid :", proc.pid)
                    proc.terminate()

        if OS_TYPE == is_win:
            killsubprocesses(proccessPid.pid)
            if verbose.get():
                debugPrint("Kill process with pid: ", proccessPid.pid)

        stop_batch_runner_button["state"] = "disabled"
        run_batch_runner_button["state"] = "enabled"
        proccessPid = None

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def runPlots(cmdId):

    global plot_proc, plot_proc_pid, plotProcess, plotParallel, plotVerbose, wdir
    global OS_TYPE, createNewConsole, keepOpenNewConsole

    if not plotProcess:
        
        extension = os.path.splitext(plots_json["data_python"][cmdId])[1]
        extension = extension.replace('.', '')
        pathPlot = os.path.join(wdir, "plots", plots_json["data_python"][cmdId])

        if not extension and plots_json["data_python"][cmdId] == "all_plots":
            if OS_TYPE == is_win:
                extension = "bat"
                pathPlot += ".bat"
                pathPlot = '"' + pathPlot + '"'

            elif OS_TYPE == is_linux:
                extension = "sh"
                pathPlot += ".sh"

        newConsole = createNewConsole.get() 
        keepConsole = keepOpenNewConsole.get()

        #/k Run Command and then return to the CMD prompt
        #/c Run Command and then terminate 
        #-q removes the verbose output
        
        if extension == "py":
            if newConsole:
                if OS_TYPE == is_win:
                    if keepConsole:
                        argsPlot = "start cmd /k " + pythonRun + " " + pathPlot
                    else:
                        argsPlot = "start cmd /c " + pythonRun + " " + pathPlot
                elif OS_TYPE == is_linux:
                    argsPlot = terminal + " -q -- bash -c \"" + pythonRun + " " + pathPlot
            else:
                argsPlot = pythonRun + " " + pathPlot
        elif extension == "sh":
            if newConsole:
                argsPlot = terminal + " -q -- bash -c \"" + pathPlot
            else:
                argsPlot = pathPlot
        elif extension == "bat":
            if newConsole:
                argsPlot = "start cmd /c " + pathPlot
            else:
                argsPlot = pathPlot

        if plotParallel.get():
            argsPlot += " --parallel"

        if plotVerbose.get():
            argsPlot += " --verbose"

        argsEnables = ""
        hasEnables = False
        for id in plotScenariosInt:
            if id.get() == 1:
                hasEnables = True
                break

        index = 0
        if hasEnables:
            argsEnables = " --en="
            for id in plotScenariosInt:
                if id.get() == 1:
                    argsEnables+="1"
                else:
                    argsEnables+="0"
                if index < len(plotScenariosInt) - 1:
                    argsEnables+=","
                index+=1

        argsPlot += argsEnables

        #remove exit parameter if u want to prevent new console from auto closing
        if newConsole:
            if OS_TYPE == is_linux:
                if keepConsole:
                    argsPlot +="; bash\""
                else:
                    argsPlot +="; exit bash\""

        if newConsole:
            if OS_TYPE == is_win:
                plot_proc = subprocess.Popen(argsPlot, stdin = subprocess.PIPE, stdout = subprocess.PIPE, shell = True, env = os.environ)
            elif OS_TYPE == is_linux:
                plot_proc = subprocess.Popen(argsPlot, stdout = subprocess.PIPE, stderr = subprocess.PIPE, shell = True, env = os.environ)
        else:
            plot_proc = subprocess.Popen(argsPlot, shell = True)

        plotProcess = True

        buttonName = ""
        for id, name in enumerate(plots_json["data_buttons"]):
            if id == cmdId:
                buttonName = name
                break 
        
        plot_proc_pid = psutil.Process(plot_proc.pid)
        
        window.after(1000, waitPlots)
        terminate_plot_button["state"] = "enabled"
        setPlotButtons(False)
        
        if verbose.get():
            debugPrint("Run args: ", argsPlot)
            debugPrint("Process info: ", plot_proc_pid)

        debugPrint("Process '%s' started at: %s"%(buttonName, datetime.now().strftime("%m/%d/%Y, %H:%M:%S")))
    else:
        debugPrint("Please wait until a plot process finishes!")
        return

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def waitPlots():

    global window, plot_proc, plotProcess, plot_proc_pid, OS_TYPE, createNewConsole
    
    if plotProcess:
        if OS_TYPE == is_win and createNewConsole.get():
            if plot_proc_pid and findsubprocesses(plot_proc_pid.pid):
                returnCode = None
            else:
                returnCode = 0
        else:
            returnCode = plot_proc.poll()
        
        if returnCode is None:
            window.after(1000, waitPlots)
        else:
            if verbose.get():
                print("return code from proc:", returnCode)

            if returnCode > 1:
                if OS_TYPE == is_win and returnCode == 3:
                    debugPrint ("Plot process exit with code %s."%returnCode)
                else:
                    debugPrint ("Plot process failed with code %s."%returnCode)

            terminate_plot_button["state"] = "disabled"
          
            setPlotButtons(True)

            plot_proc_pid = None
            plotProcess = False
            debugPrint("Plot process finished at: ", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def terminatePlots():

    global plot_proc_pid

    if plot_proc_pid:
        if OS_TYPE == is_linux:
            for proc in psutil.process_iter():
                if verbose.get():
                    debugPrint(proc)
                
                if proc.name() == 'sh' and proc.pid >= plot_proc_pid.pid:
                    if verbose.get():
                        debugPrint("Kill process with pid :", proc.pid)
                    proc.terminate()

                if proc.name() == pythonRun and proc.pid >= plot_proc_pid.pid:
                    if verbose.get():
                        debugPrint("Kill process with pid :", proc.pid)
                    proc.terminate()
        
        if OS_TYPE == is_win:
            killsubprocesses(plot_proc_pid.pid)

        debugPrint("Plotting process terminated!")
        plot_proc_pid = None

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def compileTetris_App():

    global wdir, compileApp, compile_app_proc, compile_app_proc_pid
    global OS_TYPE, createNewConsole, keepOpenNewConsole
 
    if not compileApp:
        
        newConsole = createNewConsole.get() 
        keepConsole = keepOpenNewConsole.get()
        
        tempdir = wdir[0 : wdir.find("transaid")]
        path_app = os.path.join(wdir, tempdir, "transaid", batch_json["itetris_app"])
        path_sh = os.path.join(wdir, "tools", "build_app.sh")

        #/k Run Command and then return to the CMD prompt
        #/c Run Command and then terminate 
        #-q removes the verbose output
        
        if newConsole:
            argsCompile = terminal + " -q -- bash -c \"" + path_sh
        else:
            argsCompile = path_sh

        argsCompile += " '--path=" + path_app + "'"

        if buildCppWithDebug.get():
            argsCompile += " --debug=true"

        if not buildCppWithConfig.get():
            argsCompile += " --noconf=true"

        if newConsole:
            if OS_TYPE == is_linux:
                if keepConsole:
                    argsCompile += "; bash\""
                else:
                    argsCompile += "; exit bash\""

        if newConsole:
            if OS_TYPE == is_win:
                compile_app_proc = subprocess.Popen(argsCompile, stdin = subprocess.PIPE, stdout = subprocess.PIPE, shell = True, env = os.environ)
            elif OS_TYPE == is_linux:
                compile_app_proc = subprocess.Popen(argsCompile, stdout = subprocess.PIPE, stderr = subprocess.PIPE, shell = True, env = os.environ)
        else:
            compile_app_proc = subprocess.Popen(argsCompile, shell = True)

        compileApp = True

        compile_app_proc_pid = psutil.Process(compile_app_proc.pid)
        
        window.after(1000, waitAppCompile)
        
        if verbose.get():
            debugPrint("Run args: ", argsCompile)
            debugPrint("Process info: ", compile_app_proc_pid)

        debugPrint("Process compile app started at: %s"%(datetime.now().strftime("%m/%d/%Y, %H:%M:%S")))
    else:
        debugPrint("Please wait until a compiling process finishes!")

#-------------------------------------------------------------------------------------------
def waitAppCompile():

    global window, compileApp, compile_app_proc, compile_app_proc_pid, OS_TYPE, createNewConsole
    
    if compileApp:
        if OS_TYPE == is_win and createNewConsole.get():
            if compile_app_proc_pid and findsubprocesses(compile_app_proc_pid.pid):
                returnCode = None
            else:
                returnCode = 0
        else:
            returnCode = compile_app_proc.poll()
        
        if returnCode is None:
            window.after(1000, waitAppCompile)
        else:
            if verbose.get():
                print("return code from proc:", returnCode)

            if returnCode > 1:
                if OS_TYPE == is_win and returnCode == 3:
                    debugPrint ("Compile app process exit with code %s."%returnCode)
                else:
                    debugPrint ("Compile app process failed with code %s."%returnCode)

            compile_app_proc_pid = None
            compileApp = False
            debugPrint("Compile app process finished at: ", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def compileBase_App():

    global wdir, compileBaseApp, compile_base_app_proc, compile_base_app_proc_pid
    global OS_TYPE, createNewConsole, keepOpenNewConsole
 
    if not compileBaseApp:
        
        newConsole = createNewConsole.get() 
        keepConsole = keepOpenNewConsole.get()
        
        tempdir = wdir[0 : wdir.find("transaid")]
        path_app = os.path.join(wdir, tempdir, "transaid", batch_json["itetris_base_app"])
        path_sh = os.path.join(wdir, "tools", "build_app.sh")

        #/k Run Command and then return to the CMD prompt
        #/c Run Command and then terminate 
        #-q removes the verbose output
        
        if newConsole:
            argsCompile = terminal + " -q -- bash -c \"" + path_sh
        else:
            argsCompile = path_sh

        argsCompile += " '--path=" + path_app + "'"

        if buildCppWithDebug.get():
            argsCompile += " --debug=true"

        if not buildCppWithConfig.get():
            argsCompile += " --noconf=true"

        if newConsole:
            if OS_TYPE == is_linux:
                if keepConsole:
                    argsCompile += "; bash\""
                else:
                    argsCompile += "; exit bash\""

        if newConsole:
            if OS_TYPE == is_win:
                compile_base_app_proc = subprocess.Popen(argsCompile, stdin = subprocess.PIPE, stdout = subprocess.PIPE, shell = True, env = os.environ)
            elif OS_TYPE == is_linux:
                compile_base_app_proc = subprocess.Popen(argsCompile, stdout = subprocess.PIPE, stderr = subprocess.PIPE, shell = True, env = os.environ)
        else:
            compile_base_app_proc = subprocess.Popen(argsCompile, shell = True)

        compileBaseApp = True

        compile_base_app_proc_pid = psutil.Process(compile_base_app_proc.pid)
        
        window.after(1000, waitBaseAppCompile)
        
        if verbose.get():
            debugPrint("Run args: ", argsCompile)
            debugPrint("Process info: ", compile_app_proc_pid)

        debugPrint("Process compile base app started at: %s"%(datetime.now().strftime("%m/%d/%Y, %H:%M:%S")))
    else:
        debugPrint("Please wait until a compiling process finishes!")

#-------------------------------------------------------------------------------------------
def waitBaseAppCompile():

    global window, compileBaseApp, compile_base_app_proc, compile_base_app_proc_pid, OS_TYPE, createNewConsole
    
    if compileBaseApp:
        if OS_TYPE == is_win and createNewConsole.get():
            if compile_base_app_proc_pid and findsubprocesses(compile_base_app_proc_pid.pid):
                returnCode = None
            else:
                returnCode = 0
        else:
            returnCode = compile_base_app_proc.poll()
        
        if returnCode is None:
            window.after(1000, waitBaseAppCompile)
        else:
            if verbose.get():
                print("return code from proc:", returnCode)

            if returnCode > 1:
                if OS_TYPE == is_win and returnCode == 3:
                    debugPrint ("Compile base app process exit with code %s."%returnCode)
                else:
                    debugPrint ("Compile base app process failed with code %s."%returnCode)

            compile_base_app_proc_pid = None
            compileBaseApp = False
            debugPrint("Compile base app process finished at: ", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))            

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def openFolder(folderName):
    
    path = ""
    
    if folderName == "results":
        path = os.path.join(wdir, "results")
    elif folderName == "config":
        path = os.path.join(wdir, "config")
    elif folderName == "figs":
        path = os.path.join(wdir, "results", "plots", "figs")

    if os.path.exists(path):
        if OS_TYPE == is_linux:
            path = "xdg-open " + path
        if OS_TYPE == is_win:
            path = "explorer " + path

        subprocess.Popen(path, shell = True, env = os.environ)
    else:
        debugPrint(folderName, " folder does not exist!")

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def runNetEdit():

    global wdir, sumo_path, net_files_clean, netFilesComboBox
    global OS_TYPE

    net_file_path = os.path.join(wdir, "config", net_files_clean[netFilesComboBox.current()])
   
    if not os.path.exists(net_file_path):
         debugPrint("File not found:", net_file_path) 

    if OS_TYPE == is_linux:
        print(sumo_path)
        path = os.path.join(sumo_path, "bin", "netedit")
        command = [path, net_file_path]
    elif OS_TYPE == is_win:
        path = os.path.join(sumo_path, "bin", "netedit.exe")
        command = [path, net_file_path]
    
    if OS_TYPE == is_linux:
        subprocess.run(command, stdout=subprocess.PIPE)
    elif OS_TYPE == is_win:
        si = subprocess.STARTUPINFO()
        si.dwFlags = subprocess.CREATE_NEW_CONSOLE | subprocess.NORMAL_PRIORITY_CLASS
        subprocess.Popen(command, startupinfo=si)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def runDistribution():

    global wdir

    msgBox = messagebox.askokcancel("Run distribution!", 'Are you sure?')

    if msgBox:
        path = os.path.join(wdir, "tools/distribution.py")
        command = [pythonRun , path]
        subprocess.call(command, stdout = sys.stdout, stderr = sys.stderr)

        path = os.path.join(wdir, "tools/createVTypeTemplates.py")
        command = [pythonRun , path]
        subprocess.call(command, stdout = sys.stdout, stderr = sys.stderr)

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def setDefaultDistribution():

    varId = 0
    for vehType in batch_json["vehicle_types"]:
        vehiclesSizeInt[varId].set(0)

        if vehType in distribution_json["typeList"]:
            id = distribution_json["typeList"].index(vehType)
            vehiclesSizeInt[varId].set(distribution_json["sizeListDefault"][id])

        varId+=1
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def saveDistribution():

    global wdir, distribution_json
    msgBox = messagebox.askokcancel("Save distribution data!", 'Are you sure?')

    if msgBox:
        path = os.path.join(wdir, "tools/settings/distribution.json")

        distribution_json["sizeList"] = [] #reset

        index = 0
        for vehType in batch_json["vehicle_types"]:
            if vehType in distribution_json["typeList"]:
                if vehiclesSizeInt[index].get() > 0:
                    distribution_json["sizeList"].append(vehiclesSizeInt[index].get())
            index+=1

        distribution_json["driverBehaviourList"] = [] #reset
        for index, item in enumerate(batch_json["args_schemes_clean"]):
            if vehiclesSchemeInt[index].get() == 1:
                distribution_json["driverBehaviourList"].append(item)

        with open(path, 'w') as fp:
            json.dump(distribution_json, fp, indent = 2)

        if verbose.get():
            debugPrint("Distribution saved : ", distribution_json["sizeList"], ", ", distribution_json["driverBehaviourList"]) 

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def fetchResults(parentWindow):

    global rsync_proc, downloadProcess

    if downloadProcess:
        debugPrint("Please wait until a process finishes!")
        return

    username = ""
    password = ""

    #-----------------------------------------
    if batch_json["cluster_user"]:
        username = batch_json["cluster_user"]
    else:
        username = credentialsForm(parentWindow, "Enter User Name", False)

    if not username:
        debugPrint("Please enter a user name")
        return

    #-----------------------------------------
    if batch_json["cluster_password"]:
        password = batch_json["cluster_password"]
    else:    
        password = credentialsForm(parentWindow)

    if not password:
        debugPrint("Please enter a password")
        return

    destinationPath = os.path.dirname(os.path.realpath(__file__))

    sourcePath = batch_json["cluster_path"] + "/results"

    if OS_TYPE == is_win:
        if sourcePath[0:2]  == "~/":
           length = len(sourcePath)
           sourcePath = sourcePath[2:length]

        cmd  = 'pscp -pw' + ' ' + password + ' -r -q ' + username + batch_json["cluster_server_name"] + ":" + sourcePath + ' "' + destinationPath + '"'
    elif OS_TYPE == is_linux:
        cmd  = 'sshpass -p' + ' ' + password + ' ' +  'rsync -r' + ' ' + username + batch_json["cluster_server_name"] + ":" + sourcePath + ' ' + destinationPath
    
    rsync_proc = subprocess.Popen(cmd, shell = True, stdin = subprocess.PIPE, stdout = subprocess.PIPE )
    
    if verbose.get():
        debugPrint("Command: ", cmd)

    if OS_TYPE == is_win:
        rsync_proc.stdin.write(b"y\n")
        rsync_proc.stdin.flush()

    fetch_button["state"] = "disabled"

    downloadProcess = True
    debugPrint("Download process started at :", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))
    window.after(1000, waitResults)
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def waitResults():

    global window, rsync_proc, downloadProcess
    
    if downloadProcess:
        returnCode = rsync_proc.poll()
        if returnCode is None:
            window.after(1000, waitResults)
        else:
            if returnCode > 1:
                debugPrint ("Download process failed with code %s."%returnCode)
            
            fetch_button["state"] = "enabled"
            downloadProcess = False
            debugPrint("Download process finished at :", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))


#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def permissionsFile(path):
    f = Path(path)
    f.chmod(f.stat().st_mode | stat.S_IEXEC)
    
#---------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def updateSumo(parentWindow):

    global updateSumoProcess, plotProcess, OS_TYPE, update_sumo_proc, updateLinuxProcess, updatePythonProcess
    global update_sumo_button, update_linux_button, update_python_button

    if plotProcess or updateSumoProcess or updateLinuxProcess or updatePythonProcess:
        debugPrint("A process has not finished yet!")
        return

    if OS_TYPE == is_win:
        path = os.path.join(wdir, "tools/update_sumo.bat")
        subprocess.call([path])
    elif OS_TYPE == is_linux:

        password = credentialsForm(parentWindow)

        if password:
            path = os.path.join(wdir, "tools/update_sumo.sh")

            permissionsFile(path)

            command = ['%s --path=%s --password=%s --hash=%s' %(path, sumo_path, password, sumo_update_hash.get())]
            update_sumo_proc = subprocess.Popen(command, shell = True)

            updateSumoProcess = True
            window.after(1000, waitUpdateSumo)
            update_sumo_button["state"] = "disabled"
            update_linux_button["state"] = "disabled"
            update_python_button["state"] = "disabled"


#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def waitUpdateSumo():

    global window, update_sumo_proc, updateSumoProcess
    global update_sumo_button, update_linux_button, update_python_button
    
    if updateSumoProcess:
        returnCode = update_sumo_proc.poll()

        if returnCode is None:
            window.after(1000, waitUpdateSumo)
        else:
            if returnCode > 1:
                debugPrint ("Update SUMO process failed with code %s."%returnCode)

            updateSumoProcess = False

            update_sumo_button["state"] = "enabled"
            update_linux_button["state"] = "enabled"
            update_python_button["state"] = "enabled"

            debugPrint("Update SUMO process finished at :", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))

            getSUMOVersion()

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def updateLinux(parentWindow):

    global downloadProcess, plotProcess, updateSumoProcess, updateLinuxProcess, updatePythonProcess, update_linux_proc
    global update_sumo_button, update_linux_button, update_python_button

    if downloadProcess or plotProcess or updateSumoProcess or updateLinuxProcess or updatePythonProcess:
        debugPrint("A process has not finished yet!")
        return
    
    password = credentialsForm(parentWindow)

    if password:
        path = os.path.join(wdir, "tools/update_linux.sh")

        permissionsFile(path)

        command = ['%s --password=%s' %(path, password)]
        update_linux_proc = subprocess.Popen(command, shell = True)

        updateLinuxProcess = True
        window.after(1000, waitUpdateLinux)

        update_sumo_button["state"] = "disabled"
        update_linux_button["state"] = "disabled"
        update_python_button["state"] = "disabled"


#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def waitUpdateLinux():

    global window, update_linux_proc, updateLinuxProcess
    global update_sumo_button, update_linux_button, update_python_button

    if updateLinuxProcess:
        returnCode = update_linux_proc.poll()

        if returnCode is None:
            window.after(1000, waitUpdateLinux)
        else:
            if returnCode > 1:
                debugPrint ("Update linux process failed with code %s."%returnCode)

            updateLinuxProcess = False
            
            update_sumo_button["state"] = "enabled"
            update_linux_button["state"] = "enabled"
            update_python_button["state"] = "enabled"

            debugPrint("Update linux process finished at :", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def updatePythonLibs(parentWindow):

    global downloadProcess, plotProcess, updateSumoProcess, OS_TYPE, updatePythonProcess, update_python_proc, updateLinuxProcess
    global update_sumo_button, update_linux_button, update_python_button

    if downloadProcess or plotProcess or updateSumoProcess or updatePythonProcess or updateLinuxProcess:
        debugPrint("A process has not finished yet!")
        return

    if OS_TYPE == is_win:
        path = os.path.join(wdir, "tools/update_python_libs.bat")
        subprocess.call([path])
    elif OS_TYPE == is_linux:

        password = credentialsForm (parentWindow)

        if password:
            path = os.path.join(wdir, "tools/update_python_libs.sh")

            permissionsFile(path)

            command = ['%s --password=%s' %(path, password)]
            update_python_proc = subprocess.Popen(command, shell = True)

            updatePythonProcess = True
            window.after(1000, waitUpdatePython)
            
            update_sumo_button["state"] = "disabled"
            update_linux_button["state"] = "disabled"
            update_python_button["state"] = "disabled"


#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def waitUpdatePython():

    global window, update_python_proc, updatePythonProcess
    global update_sumo_button, update_linux_button, update_python_button

    if updatePythonProcess:
        returnCode = update_python_proc.poll()

        if returnCode is None:
            window.after(1000, waitUpdatePython)
        else:
            if returnCode > 1:
                debugPrint ("Update python process failed with code %s."%returnCode)

            updatePythonProcess = False

            update_sumo_button["state"] = "enabled"
            update_linux_button["state"] = "enabled"
            update_python_button["state"] = "enabled"

            debugPrint("Update python process finished at :", datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))


#-------------------------------------------------------------------------------------------
#https://github.com/eclipse/sumo.git
#check commit 90e5e0373660a27113a1342230fb79018112463b
#-------------------------------------------------------------------------------------------
def getSUMOVersion():

    global OS_TYPE, sumo_path, git_sumo_version
    
    if OS_TYPE == is_win:
        windows_sumo_version = "Not found"
        path = os.path.join(sumo_path, "bin", "sumo.exe")
        if os.path.exists(path):
            #windows_sumo_version = time.ctime(os.path.getctime(path)) #metadata change time of a file
            windows_sumo_version = time.ctime(os.path.getmtime(path)) #modification time of a file 

        print("SUMO version:", windows_sumo_version)
        labels_git_version[0].configure(text = windows_sumo_version)

    elif OS_TYPE == is_linux:
        
        path = os.path.join(wdir, "tools/get_sumo_version.sh")

        permissionsFile(path)

        command = ['%s --path=%s' %(path, sumo_path)]
        answer = subprocess.check_output(command, shell = True)

        temp = answer.decode("utf-8")

        temp = temp.rstrip("\n")

        git_sumo_version = temp.split(',')

        if len(git_sumo_version) == 3:
            print("SUMO version:", git_sumo_version[0])
            print("SUMO date:", git_sumo_version[1])
            print("SUMO author:", git_sumo_version[2])

            labels_git_version[0].configure(text = git_sumo_version[0])
            labels_git_version[1].configure(text = git_sumo_version[1])
            labels_git_version[2].configure(text = git_sumo_version[2])

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def close():
    global window, downloadProcess, plotProcess, updateSumoProcess, updateLinuxProcess, updatePythonProcess

    if downloadProcess or plotProcess or updateSumoProcess or updateLinuxProcess or updatePythonProcess:
        msgBox = messagebox.askyesno("A process has not finished yet! Do you want to exit?", 'Are you sure?')
        if msgBox == False:
            return

    #stop download and plot processes here (kill)
    terminatePlots()
    stopBatchRunner()
    window.destroy()

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def credentialsForm(parentWindow, title = "Enter Password", isPassword = True):
       
    userData = StringVar()

    width = 280
    height = 80
    posX = parentWindow.winfo_x() + int(parentWindow.winfo_width() / 2 - width / 2)
    posY = parentWindow.winfo_y() + int(parentWindow.winfo_height() / 3 - height / 2)
    geoStr = str(width) + "x" + str(height) + "+" + str(posX) + "+" + str(posY)

    form = Toplevel(parentWindow)
    form.attributes('-topmost', 'true')
    form.resizable(FALSE, FALSE)
    form.geometry(geoStr)
    form.title(title)

    if isPassword:
        e = Entry(form, textvariable = userData, show = "*")
    else:
        e = Entry(form, textvariable = userData)

    e.grid(padx = 58, pady = 10, sticky = (W))
    e.focus_set()

    b = Button(form, text = "Ok", command = form.destroy)
    b.grid(padx = 92, pady = 3, sticky = (W))
    
    form.wait_window()

    return userData.get()
    

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def make_label(master, x, y, w, h, *args, **kwargs):
    
    f = Frame(master, width = w, height = h, borderwidth = 2, relief=SUNKEN)

    f.pack(fill = BOTH, expand = True)
    #f.pack_propagate(False) # don't shrink

    f.place(x = x, y = y, width = w, height = h)

    label = Label(f, *args, **kwargs)
    label.pack(fill = BOTH, expand = True)
    return label

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def make_button(master, x, y, w, h, *args, **kwargs):
    
    f = Frame(master, width = w, height = h, borderwidth = 2, relief=GROOVE)
    
    f.pack_propagate(0) # don't shrink
    f.place(x = x, y = y)

    button = Button(f, *args, **kwargs)
    button.pack(fill = BOTH, expand = True)
    return button    

#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
def make_checkbox(master, x, y, w, h, *args, **kwargs):
    
    f = Frame(master, width = w, height = h, borderwidth = 2, relief=GROOVE)
    
    f.pack_propagate(0) # don't shrink
    f.place(x = x, y = y)

    button = Checkbutton(f, *args, **kwargs)
    button.pack(fill = BOTH, expand = True)
    return button 
#-------------------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------------------
class ResizingCanvas(Canvas):
    def __init__(self,parent,**kwargs):
        Canvas.__init__(self,parent,**kwargs)
        self.bind("<Configure>", self.on_resize)
        self.height = self.winfo_reqheight()
        self.width = self.winfo_reqwidth()

    def on_resize(self,event):
        # determine the ratio of old width/height to new width/height
        wscale = float(event.width)/self.width
        hscale = float(event.height)/self.height
        self.width = event.width
        self.height = event.height
        # resize the canvas 
        self.config(width=self.width, height=self.height)
        # rescale all the objects tagged with the "all" tag
        self.scale("all",0,0,wscale,hscale)

#-------------------------------------------------------------------------------------------
# this is the main entry point of this script
#-------------------------------------------------------------------------------------------
if __name__ == "__main__":

    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    if "transaid" in wdir:
        running_in_transaid = True

    find_platform()

    run_as_admin() #windows

    print("\n-----------------------------------------------------------------------")
    print ("python gui version : %s "%(VERSION))

    try:
        PLATFORM_HOME = os.environ['HOME'] #sometimes this is crashing
    except:
        PLATFORM_HOME = pathlib.Path.home()

    #USER = os.path.basename(PLATFORM_HOME) #os.path.expanduser("~"))

    readData()

    #window = Tk()

    for i in range(len(batch_json["args"])):
        argsInt.append(IntVar())

    argsInt[len(batch_json["args"]) - 4].set(1) #set gui to true

    for i in range(len(batch_json["args_demands"])):
        argsDemandsInt.append(IntVar())

    for i in range(len(batch_json["args_mix"])):
        argsMixesInt.append(IntVar())

    for i in range(len(batch_json["args_schemes"])):
        argsSchemesInt.append(IntVar())    

    for i in range(len(batch_json["vehicle_types"])):
        vehiclesSizeInt.append(IntVar())    

    for i in range(len(batch_json["args_schemes_clean"])):
        vehiclesSchemeInt.append(IntVar())    

    if scenarioDefault >=0 and scenarioDefault < len(batch_json["scenarios_names"]):
        scenarioRun.set(scenarioDefault)
    
    comboSimText = StringVar()
    comboSimText.set(simulationDefault)

    for i in range(len(batch_json["scenarios_names"])):
        plotScenariosInt.append(IntVar())
  
    window.title("Transaid Gui Manager, version " + VERSION + " - " + batch_json["use_case"] + " - " +  batch_json["network"])
    window.geometry('1024x768')
    window.columnconfigure(0, weight=1)
    window.columnconfigure(1, weight=1)
    window.columnconfigure(2, weight=1)
    window.columnconfigure(3, weight=1)
    window.columnconfigure(4, weight=1)
    window.columnconfigure(5, weight=1)
    
    window.iconphoto(True, PhotoImage(file = os.path.join(wdir, "settings/gui.png")))
    window.resizable(FALSE, FALSE)

    s = Style()
    s.configure('TFrame', background='grey')
    s.configure('TCheckbutton', background='grey')
    s.configure('TRadiobutton', background='grey')

    s.configure('TCheckbutton', font=('', '11'))
    s.configure('TButton', font=('', '11'))

    s_button_red = Style()
    s_button_green = Style() 
    
    s_button_green.configure('G.TButton', font = ('Arial', '12', 'bold'), foreground = 'green')
    s_button_red.configure('R.TButton', font = ('Arial', '12', 'bold'), foreground = 'red')

    mainframe = ResizingCanvas(window, width = 1024, height = 768, bg = "grey", highlightthickness = 1)
    mainframe.pack(fill = BOTH, expand = True)
    
    nb = Notebook(mainframe) #window
    nb.pack()
    nb.config( height = 580 )

    page_Sumo = Frame(nb, relief=SUNKEN)
    page_Plots = Frame(nb, relief=SUNKEN)
    page_Tools = Frame(nb, relief=SUNKEN)

    nb.add(page_Sumo, text=" -TRANSAID- ")
    nb.add(page_Plots, text=' -PLOTS- ')
    nb.add(page_Tools, text=' -TOOLS- ')

    nb.pack(fill = 'both', padx = 10, pady = 10)
    
    nbTools = Notebook(page_Tools)
    nbTools.config(width = 300, height = 540 )

    page_sub_other_tools = Frame(nbTools, relief=SUNKEN)
    page_sub_distribution = Frame(nbTools, relief=SUNKEN)
    page_sub_cluster_server = Frame(nbTools, relief=SUNKEN)
    page_sub_sumo_platform = Frame(nbTools, relief=SUNKEN)
    
    nbTools.add(page_sub_other_tools, text=' -OTHER TOOLS- ')
    nbTools.add(page_sub_distribution, text=' -DISTRIBUTION- ')
    nbTools.add(page_sub_cluster_server, text=' -CLUSTER SERVER- ')
    
    if OS_TYPE == is_linux:
        nbTools.add(page_sub_sumo_platform, text=' -SUMO LINUX- ')
    elif OS_TYPE == is_win:
        nbTools.add(page_sub_sumo_platform, text=' -SUMO WINDOWS- ')

    nbTools.pack(fill = 'both', padx = 10, pady = 10)
    
    nbTools.columnconfigure(0, weight=1)
    nbTools.columnconfigure(1, weight=1)
    nbTools.columnconfigure(2, weight=1)
    nbTools.columnconfigure(3, weight=1)
    nbTools.columnconfigure(4, weight=1)
    nbTools.columnconfigure(5, weight=1)

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    # SUMO
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    lb1 = Label(page_Sumo, text = "Simulation Seed", relief = "solid")
    lb1.grid(column = 1, row = 1, padx=10, pady=10, sticky = W)
    lb1.config(width = 18, anchor = CENTER)

    cb1 = Combobox(page_Sumo, textvariable = comboSimText, state = "readonly", values = ("1","2","3","4","5","6","7","8","9","10", simulationDefault))
    cb1.grid(column = 2, row = 1, sticky = W)
    cb1.config(width = 5)

    #----------------------------------------------------------------------------------------------------------------------------------------------------

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    lb = Label(page_Sumo, text = "Functionalities", relief = "solid")
    lb.grid(column = 1, row = 3, padx = 10, pady = 10, sticky = W)
    lb.config(width = 15, anchor = CENTER)

    lb = Label(page_Sumo, text = "Traffic Demand", relief = "solid")
    lb.grid(column = 2, row = 3, padx = 10, pady = 10, sticky = W)
    lb.config(width = 15, anchor = CENTER)

    lb = Label(page_Sumo, text = "Traffic Mix", relief = "solid")
    lb.grid(column = 3, row = 3, padx = 10, pady = 10, sticky = W)
    lb.config(width = 15, anchor = CENTER)

    lb = Label(page_Sumo, text = "Parametrization", relief = "solid")
    lb.grid(column = 4, row = 3, padx = 10, pady = 10, sticky = W)
    lb.config(width = 15, anchor = CENTER)

    lb = Label(page_Sumo, text = "Scenarios", relief = "solid")
    lb.grid(column = 5, row = 3, padx = 10, pady = 10, sticky = W)
    lb.config(width = 15, anchor = CENTER)

    #----------------------------------------------------------------------------------------------------------------------------------------------------

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    # add check buttons for args
    rowId = 0
    for text in batch_json["args_names"]:
        wd = Checkbutton(page_Sumo, text = text, variable = argsInt[rowId])
        wd.grid(column = 1, row = 4 + rowId, padx = 10, pady = 10, sticky = W)
        wd.config(width = 15)
        CreateToolTip(wd, batch_json["args_tooltips"][rowId])

        rowId += 1
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    demandId = 0
    rowId = 0
    for text in batch_json["args_demands_names"]:
        if text in batch_json["demands"]:
            wd = Checkbutton(page_Sumo, text = batch_json["args_demands_names"][text], variable = argsDemandsInt[demandId])
            wd.grid(column = 2, row = 4 + rowId, padx = 10, pady = 10, sticky = W)
            wd.config(width = 8)
            rowId += 1
        demandId += 1
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    rowId = 0
    for text in batch_json["args_mix_names"]:
        if text in batch_json["traffic_mixes"]:
           wd = Checkbutton(page_Sumo, text = batch_json["args_mix_names"][text], variable = argsMixesInt[rowId])
           wd.grid(column = 3, row = 4 + rowId, padx = 10, pady = 10, sticky = W)
           wd.config(width = 8)
           argsMixesCheckButtons.append(wd)
           rowId += 1

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    mixId = 0
    rowId = 0
    for text in batch_json["args_schemes_names"]:
        upperTxt = batch_json["args_schemes_names"][text]
        if upperTxt in batch_json["schemes"]:
            wd = Checkbutton(page_Sumo, text = upperTxt, variable = argsSchemesInt[rowId])
            wd.grid(column = 4, row = 4 + rowId, padx = 10, pady = 10, sticky = W)
            wd.config(width = 8)
            rowId += 1
        mixId += 1

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    rowId = 0
    for text in batch_json["scenarios_names"]:
        Radiobutton(page_Sumo, text = text, variable = scenarioRun, value = rowId).grid(column = 5, row = 4 + rowId, padx=10, pady=10, sticky = W)
        rowId += 1
    
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    run_batch_runner_button = make_button(page_Sumo, 220, 400, 160, 30, text = "Start batchRunner", style = 'G.TButton', command = runBatchRunner)
    
    stop_batch_runner_button = make_button(page_Sumo, 600, 400, 160, 30, text = "Stop batchRunner", style = 'R.TButton', command = stopBatchRunner)
    stop_batch_runner_button["state"] = "disabled"

    if running_in_transaid:
        compile_app_button = make_button(page_Sumo, 220, 460, 160, 30, text = "Compile c++ app", style = 'G.TButton', command = compileTetris_App)
        compile_base_app_button = make_button(page_Sumo, 220, 510, 160, 30, text = "Compile c++ base", style = 'G.TButton', command = compileBase_App)

        buildCppWithDebug.set(False)
        check_button1 = make_checkbox(page_Sumo, 420, 460, 160, 30, text = "Debug build", variable = buildCppWithDebug)

        buildCppWithConfig.set(False)
        check_button = make_checkbox(page_Sumo, 420, 510, 160, 30, text = "Configure code", variable = buildCppWithConfig)

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    # Plots
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    rowId = 0
    for buttonName in plots_json["data_buttons"]:
        wd = Button(page_Plots, text = buttonName, command = lambda i = rowId: runPlots(i) )
        wd.grid(column = 1, row = 3 + rowId, padx = 10, pady = 10, sticky = S)
        wd.config(width = 30)

        plot_buttons[buttonName] = wd

        if plots_json["data_buttons"][buttonName] == 0:
            wd["state"] = "disabled"

        rowId += 1

    #TODO checkboxes. Send "func enables=1,1,1,1,1,1,1,1,1,1" in all_plots.sh --
    '''rowId = 0
    for buttonName in plots_json["data_buttons"]:
        if rowId < len(plots_json["data_buttons"]) - 1:
            plotEnableChkBtns.append(BooleanVar())
            plotEnableChkBtns[rowId].set(True)
            
            wd = Checkbutton(page_Plots, text = "", command = lambda i = rowId: plotEnableChkBtns[rowId] )
            wd.grid(column = 2, row = 3 + rowId, padx = 2, pady = 10, sticky = S)
            wd.config(width = 2)

            #wd = make_checkbox(page_Plots, 300, 9 + rowId * 49, 40, 30, text = "", variable = plotEnableChkBtns[rowId])

            if plots_json["data_buttons"][buttonName] == 0:
                wd["state"] = "disabled"

            rowId += 1
    '''


    wd = Checkbutton(page_Plots, text = "Parallel", variable = plotParallel, onvalue = 1, offvalue = 0)
    wd.grid(column = 3, row = 3, padx = 10, pady = 10, sticky = W)
    wd.config(width = 10)

    wd = Checkbutton(page_Plots, text = "Verbose", variable = plotVerbose, onvalue = 1, offvalue = 0)
    wd.grid(column = 3, row = 4, padx = 10, pady = 10, sticky = W)
    wd.config(width = 10)

    rowId = 0
    for text in batch_json["scenarios_names"]:
        Checkbutton(page_Plots, text = text, variable = plotScenariosInt[rowId]).grid(column = 4, row = 3 + rowId, padx = 10, pady = 10, sticky = W)
        rowId += 1
    
    terminate_plot_button = Button(page_Plots, text = "Terminate process", command = terminatePlots, style = 'W.TButton')
    terminate_plot_button.grid(column = 7, row = 3, padx = 20, pady = 10, sticky = S)
    terminate_plot_button.config(width = 20)
    terminate_plot_button["state"] = "disabled"

    wd = Button(page_Plots, text = "Open results folder", command = lambda name = "results": openFolder(name))
    wd.grid(column = 7, row = 4, padx = 20, pady = 10, sticky = E)
    wd.config(width = 20)

    wd = Button(page_Plots, text = "Open figs folder", command = lambda name = "figs": openFolder(name))
    wd.grid(column = 7, row = 5, padx = 20, pady = 10, sticky = E)
    wd.config(width = 20)

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    #Other tools
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    bt = Button(page_sub_other_tools, text = "Run NetEdit", command = runNetEdit)
    bt.grid(column = 1, row = 3, padx = 10, pady = 10, sticky = (W,EW,E))
    bt.config(width = 16)

    netFilesComboBox = Combobox(page_sub_other_tools,  values=net_files_clean)
    netFilesComboBox.grid(column = 2, row = 3, padx = 10, pady = 10, sticky = (W,EW,E))
    netFilesComboBox.config(width = 32)

    netFilesComboBox.current(0)
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    #Distribution
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    varId = 0
    rowId = 0
    columnId = 0

    for vehType in batch_json["vehicle_types"]:

        wd = Label(page_sub_distribution, text = vehType, anchor = CENTER)
        wd.config(width = 12, font = (None, 13))
        wd.grid(column = columnId, row = 3 + rowId, padx = 2, pady = 8, sticky = (E))
        
        ed = Entry(page_sub_distribution, textvariable = vehiclesSizeInt[varId])
        ed.config(width = 10, font = (None, 12))
        ed.grid(column = columnId + 1, row = 3 + rowId, padx = 2, pady = 8, sticky = (E))

        if vehType in distribution_json["typeList"]:
            id = distribution_json["typeList"].index(vehType)
            vehiclesSizeInt[varId].set(distribution_json["sizeList"][id])

        varId+=1
        rowId += 1
        if rowId == 8:
            rowId = 0
            columnId = columnId + 2
    
    rowId = 0
    columnId = 5
    for behaviourType in batch_json["args_schemes_clean"]:
        wd = Checkbutton(page_sub_distribution, text = behaviourType, variable = vehiclesSchemeInt[rowId], onvalue = 1, offvalue = 0)
            
        wd.grid(column = columnId, row = 3 + rowId, padx = 10, pady = 8, sticky = (W,E))
        wd.config(width = 10)

        if behaviourType in distribution_json["driverBehaviourList"]:
            vehiclesSchemeInt[rowId].set(1)

        rowId += 1
        if rowId == 8:
            rowId = 0
    
    #sp = Separator(page_sub_distribution, orient="horizontal")
    #sp.grid(columnspan=7, sticky = (W,E))

    bt = Button(page_sub_distribution, text = "Save", command = saveDistribution)
    bt.grid(column = 1, row = 15, padx = 0, pady = 40, sticky = (W,E))
    bt.config(width = 16)

    bt = Button(page_sub_distribution, text = "Run", command = runDistribution)
    bt.grid(column = 3, row = 15, padx = 0, pady = 40, sticky = (W,E))
    bt.config(width = 16)

    bt = Button(page_sub_distribution, text = "Set default", command = setDefaultDistribution)
    bt.grid(column = 6, row = 15, padx = 0, pady = 40, sticky = (W,E))
    bt.config(width = 16)
    
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    #Cluster server
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    fetch_button = Button(page_sub_cluster_server, text = "Fetch results", command = lambda: fetchResults(window))
    fetch_button.grid(column = 1, row = 3, padx = 10, pady = 10, sticky = (W,EW,E))
    fetch_button.config(width = 16)

    #----------------------------------------------------------------------------------------------------------------------------------------------------
    #Sumo-Linux Windows
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    if OS_TYPE == is_linux:
        update_linux_button = Button(page_sub_sumo_platform, text = "Update Linux", command = lambda: updateLinux(window))
        update_linux_button.grid(column = 1, row = 3, padx = 15, pady = 10)
        update_linux_button.config(width = 24)

        update_sumo_button = Button(page_sub_sumo_platform, text = "Update sumo", command = lambda: updateSumo(window))
        update_sumo_button.grid(column = 2, row = 3, padx = 60, pady = 10)
        update_sumo_button.config(width = 24)

        update_python_button = Button(page_sub_sumo_platform, text = "Update python libraries", command = lambda: updatePythonLibs(window))
        update_python_button.grid(column = 1, row = 4, padx = 15, pady = 10)
        update_python_button.config(width = 24)
    
        ################
        l = Label(page_sub_sumo_platform, text = "Sumo version", anchor = CENTER)
        l.grid(column = 2, row = 4, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 24)

        l = Label(page_sub_sumo_platform, text = "Date", anchor = CENTER)
        l.grid(column = 2, row = 5, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 24)

        l = Label(page_sub_sumo_platform, text = "Author", anchor = CENTER)
        l.grid(column = 2, row = 6, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 24)

        l = Label(page_sub_sumo_platform, text="")
        l.grid(column = 3, row = 4, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 60)
        labels_git_version.append(l)

        l = Label(page_sub_sumo_platform, text="")
        l.grid(column = 3, row = 5, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 60)
        labels_git_version.append(l)

        l = Label(page_sub_sumo_platform, text="")
        l.grid(column = 3, row = 6, padx = 2, pady = 2, sticky = (W,EW,E))
        l.config(width = 60)
        labels_git_version.append(l)

        ed = Entry(page_sub_sumo_platform, textvariable = sumo_update_hash)
        ed.config(width = 80)
        ed.grid(column = 3, row = 3, padx = 2, pady = 10)

    if OS_TYPE == is_win:
        bt = Button(page_sub_sumo_platform, text = "Update python libraries", command = lambda: updatePythonLibs(window))
        bt.grid(column = 1, row = 8, padx = 10, pady = 10, sticky = (W,EW,E))
        bt.config(width = 24)
    
        l = Label(page_sub_sumo_platform, text = "Sumo version", anchor = CENTER)
        l.grid(column = 2, row = 8, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 24)

        l = Label(page_sub_sumo_platform, text="")
        l.grid(column = 3, row = 8, padx = 2, pady = 0, sticky = (W,EW,E))
        l.config(width = 60)
        labels_git_version.append(l)


    #----------------------------------------------------------------------------------------------------------------------------------------------------
    #
    #----------------------------------------------------------------------------------------------------------------------------------------------------
    chatMessage.set("Welcome to SUMO gui application")

    make_label(mainframe, 1, 624, 1022, 76, textvariable = chatMessage, anchor = CENTER, background='grey', borderwidth=2)
    make_button(mainframe, 432, 710, 160, 30, text = "    Exit gui    ", command = close)
   
    createNewConsole.set(True)
    make_checkbox(mainframe, 50, 710, 140, 30, text = "New console", variable = createNewConsole)

    keepOpenNewConsole.set(False)
    make_checkbox(mainframe, 220, 710, 120, 30, text = "Keep open", variable = keepOpenNewConsole)

    verbose.set(False)
    make_checkbox(mainframe, 780, 710, 130, 30, text = "Verbose gui", variable = verbose)

    mainframe.addtag_all("all")

    getSUMOVersion()

    window.mainloop()


    print("\n-----------------------------------------------------------------------")

#---------------------------------------------------------------------------------------------------------------------------------------------