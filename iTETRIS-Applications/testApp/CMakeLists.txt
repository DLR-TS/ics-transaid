cmake_minimum_required(VERSION 3.13)
project(testApp)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/)
include_directories(src/test)

add_executable(testApp
        src/test/behaviour-test-factory.cpp
        src/test/behaviour-test-factory.h
        src/test/behaviour-test-node.cpp
        src/test/behaviour-test-node.h
        src/test/behaviour-test-rsu.cpp
        src/test/behaviour-test-rsu.h
        src/app-main.cpp
        src/Makefile
        src/Makefile.am
        src/Makefile.in
        testScenario/ns3/confDvbhBs.xml
        testScenario/ns3/confDvbhVehicle.xml
        testScenario/ns3/configGeneral.ns3.20.txt
        testScenario/ns3/configGeneral.txt
        testScenario/ns3/configTechnologies-ics.xml
        testScenario/ns3/confUmtsBs.xml
        testScenario/ns3/confUmtsVehicle.xml
        testScenario/ns3/confWaveRsu.xml
        testScenario/ns3/confWaveVehicle2.xml
        testScenario/ns3/confWifi.xml
        testScenario/ns3/confWimaxBs.xml
        testScenario/ns3/confWimaxVehicle.xml
        testScenario/sumo/net.net.xml
        testScenario/sumo/routes.rou.xml
        testScenario/sumo/standalone.sumocfg
        testScenario/sumo/test.sumocfg
        testScenario/sumo/tripinfo_output.xml
        testScenario/app-data.csv
        testScenario/app-data.txt
        testScenario/app-log.txt
        testScenario/app-ns-log.txt
        testScenario/application-config-file.xml
        testScenario/comm-protocol-parser.py
        testScenario/data-cleanout.sh
        testScenario/data-postprocess.sh
        testScenario/facilities-config-file.xml
        testScenario/ics-log-50.txt
        testScenario/itetris-config-file-gui-ns3.20.xml
        testScenario/itetris-config-file-ns3.20.xml
        testScenario/LDMrules-config-file.xml
        testScenario/lightcomm-config-file.xml
        testScenario/ns3-log-50.txt
        testScenario/stations-config-file.xml
        testScenario/test-config-file.xml
        Add_a_testApp_testcase.txt
        Makefile
        Makefile.am
        Makefile.in)
