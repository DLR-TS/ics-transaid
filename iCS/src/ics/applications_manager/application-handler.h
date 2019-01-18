/****************************************************************************/
/// @file    application-handler.h
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Jerome Haerri (jerome.haerri@eurecom.fr) and Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

#ifndef APP_HANDLER_H
#define APP_HANDLER_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <random>

#include <foreign/tcpip/socket.h>
#include "../../utils/ics/iCStypes.h"
#include "../sync-manager.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class AppMessageManager;
class Subscription;
class ITetrisNode;
class ResultContainer;
class V2xMessageManager;

// ===========================================================================
// struct definitions
// ===========================================================================
struct ServiceId {
    std::string unicastServiceId;
    std::string multicastServiceId;
    std::string broadcastServiceId;
    std::string geoBroadcastServiceId;
    std::string topoBroadcastServiceId;
};


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class ApplicationHandler
* @brief Manages the execution of the applications.
*/
class ApplicationHandler
{
private:
    /// @brief Counters for ID.
    static int m_idCounter;

public:

    /**
    * @brief Constructor.
    * @param[in] appHandler
    */
    ApplicationHandler(const ApplicationHandler& appHandler);

    /**
    * @brief Constructor
    * @param[in] syncManager Pointer to the synchronization manager.
    * @param[in] name Name that identifies the application.
    * @param[in] executable The command that launches the application in the Unix system.
    * @param[in] host Host in which the application is running.
    * @param[in] port Port used for the connection with the application.
    * @param[in] seed Used to generate random numbers.
    * @param[in] rate Penetration percentage of the application.
    * @param[in] resultType Type of result the application generates.
    * @param[in] serviceId The ID of the service to use for transmission in the wireless simulator.
    */
    ApplicationHandler(SyncManager* syncManager, std::string name, std::string executable, std::string host, int port, long seed, double rate, int resulType, ServiceId serviceIds);

    /// @brief Destructor.
    ~ApplicationHandler();

    /// @brief Identifier of the Application Handler.
    int m_id;

    /// @brief Name of the application the handler is pointing to.
    std::string m_name;

    /// @brief String to start the application in the system.
    std::string m_executable;

    /// @brief The address of the host the application is running on.
    std::string m_host;

    /// @brief The port number the application is lintening to.
    int m_port;

    /// @brief The random number generator for the app.
    std::mt19937 m_rng;

    /// @brief The penetration percentage of the application.
    double m_rate;

    /// @brief Indicates what type of result generates the application.
    int m_resultType;

    /// @brief The identifier of the service in the wireless communicator simulator.
    ServiceId m_serviceId;

    /// @brief The object to perform the connection with the application.
    AppMessageManager* m_appMessageManager;

    /// @brief Pointer to the application manager.
    SyncManager* m_syncManager;

    /**
    * @brief Asks a node if it wants more subscriptions.
    * @param[in] nodeId Node identifier.
    * @param[in] subscriptions Collection of subscriptions associated to the node.
    * @return True: If the function executes successfully.
    * @return False: If an error occurs.
    */
    bool AskForNewSubscriptions(int nodeId, std::vector<Subscription*> *subscriptions);

    /**
     * @brief Informs the applications of the creation of a new node.
     * @param[in] node The node created.
     * @return True If the function executes successfully, False otherwise.
     */
    bool CreateVehicleNodeApplication(VehicleNode *node);

    /**
    * @brief Asks a node if it wants to execute an unsubscription.
    * @param[in] nodeId iCS identifier of the node.
    * @param[in] subscriptions Collection of subscriptions associated to the node.
    * @return True: If it ends successfully.
    * @return False: If an error occurs.
    */
    bool AskForUnsubscriptions(int nodeId, std::vector<Subscription*>* subscriptions);

    /**
    * @brief Sends the information about the subscription to the application.
    * @param[in] nodeId Node identifier.
    * @param[in] subscription Subscription.
    * @param[in] nodes Collection of iTetris nodes.
    * @return EXIT_SUCCESS if the information is sent correctly, EXIT_FAILURE otherwise.
    */
    int SendSubscribedData(int nodeId, Subscription* subscription, NodeMap* nodes);

    /**
    * @brief Executes the application
    * @param[in] nodeId Node identifier.
    * @param[in] resultContainer Object that contain the result of the execution.
    * @return True: If the function finishes successfully.
    * @return False: If an error occurs
    */
    bool ExecuteApplication(int nodeId, ResultContainer* resultContainer);

    /**
    * @brief Tells the Application about the status of the messages delivering resutls.
    * @param[in] nodeId Node identifier.
    * @param[in] resultContainer Object that contain the result of the execution
    * @return EXIT_SUCCESS if the information is sent correctly, EXIT_FAILURE otherwise.
    */
    int SendMessageStatus(int nodeId, ResultContainer* result);
};

}

#endif
