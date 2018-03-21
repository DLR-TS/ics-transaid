# Protocol speed

Application to collect traffic information with a simple communication protocol.

### Structure
The application is divided in two main parts:
* The ics interface with has the task of communication with iCS and creates a base upon which to develop the actual communication application. It is inside divided in:
    * Communication with iCS. In the folder _ server _.
    * Node class abstraction. In the folder _ application _.
* The application that implements the communication protocol. It is inside the folder _ application/model _.  

To class that connects the two application parts is the __ iCSInterface __ inside the folder _ application/model _.

### Simulation

To run a simulation from a folder run  

    iCS -c itetris-config-file.xml
    
Example: run for the scenario __ A.Costa __ in the folder _ acosta _
    
    iCS -c itetris-config-file-ns3.20.xml
    
### Different scenarios

To use the application in a different scenario the configuration file _ protocol-config-file.xml _ has to be modified accordingly. In particular the tag __ <infrastructure> __ has to reflect the position of the RSUs and correctly specify the directions of the controlled roads.