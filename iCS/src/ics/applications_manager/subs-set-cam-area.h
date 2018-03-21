/****************************************************************************/
/// @file    subs-set-cam-area.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef SUBSCRIPTION_SET_CAM_AREA_H
#define SUBSCRIPTION_SET_CAM_AREA_H

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

#include <vector>

#include "subscription.h"
#include "../../utils/ics/iCStypes.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class ITetrisNode;
class SyncManager;

// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * @enum nodeStatusInArea_t
 * @todo To be commented
 */
enum nodeStatusInArea_t {
    JustArrivedInArea,
    JustLeftArea,
    StillInsideArea,
    StillOutsideArea,
    UnknownStatus
};

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct nodeStatusInArea_t
 * @todo To be commented
 */
struct GeneralCamSubscription {
    stationID_t stationId;
    float maxFrequency;
    unsigned int maxPayloadLength;
    ITetrisNode* node;
} typedef TGeneralCamSubscription;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubsSetCamArea
 * @brief Commands vehicles in the defined area to start sending CAM messages.
 */
class SubsSetCamArea : public Subscription
{
public:
    /**
     * @brief Constructor
     * @param[in] appId The identifier of the subscription.
     * @param[in] stationId Station that owns the subscription.
     * @param[in] baseX The X value of the base point defined by the Application.
     * @param[in] baseY The Y value of the base point defined by the Application.
     * @param[in] radius The value that started from the base point defines the sending CAM area defined by the Application.
     * @param[in] frequency The frequency in which the sender will send the CAM messages defined by the Application.
     * @param[in] infoType The frequency in which the sender will send the CAM messages defined by the Application.
     */
    SubsSetCamArea(int appId, ics_types::stationID_t stationId, float baseX, float baseY, float radius, float frequency, int infoType);

    /// @brief Destructor.
    ~SubsSetCamArea();

    /**
     * @brief Updates the member containing the nodes that are within the limits of the area.
     * @param[in] syncManager Pointer to the sync manager to get the pointer of the stations in the area.
     * @return EXIT_SUCCESS if the area was processed correctly, EXIT_FAILURE otherwise.
     */
    int ScanArea(SyncManager* syncManager);

    /**
     * @brief Returns the nodes that are in the CAM sending area
     * @return The sending frequency of the messages for this area
     */
    unsigned int GetFrequency();

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] baseX The X value of the base point defined by the application.
    * @param[in] baseY The Y value of the base point defined by the application.
    * @param[in] radius The radius value of the base point defined by the application.
    * @param[in] frequency The frequency value for the CAM defined by the application.
    */
    static int Delete(float baseX, float baseY, float radius, float frequency, std::vector<Subscription*>* subscriptions);

    /**
    * @brief
    * @param[in] node
    * @todo TO BE COMMENTED
    */
    void RemoveNodeFromVector(ITetrisNode* node); //Added by Sendoa

    /**
    * @brief
    * @param[in] node
    * @todo TO BE COMMENTED
    */
    void AddNodeToVector(ITetrisNode* node); //Added by Pasquale Cataldi

    /**
    * @brief
    * @param[in] node
    * @todo TO BE COMMENTED
    */
    bool IsNodeInVector(ITetrisNode* node); //Added by Pasquale Cataldi

    /// @todo To be commented.
    int ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager);

    /// @todo To be commented.
    int ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message);

    /**
     * @brief      Check if the node is inside the area or outside and if it just arrived or left.
     * @param[in]  node iTetris node whose position is to be checked with respect to the current area.
     * @param[out] status Status of the node with respect the area.
     */
    nodeStatusInArea_t checkNodeStatus(ITetrisNode* node);

    /**
     * @brief      Returns the nodes in the area
     * @param[out] m_nodesInArea.
     */
    std::vector<ITetrisNode*>* getNodesInArea();

    /**
     * @brief      Vector that considers all the CAM subscriptions to set the maximum frequency the CAM should be generated by a node.
     */
    static std::vector<TGeneralCamSubscription> generalCamSubscriptions;

    /**
     * @brief      Check if the node has already subscribed a Cam transmission.
     * @param[in]  node iTetris node whose subscription will be checked.
     * @param[out] status Status of the Cam transmission.
     */
    static bool isNodeInGeneralCamSubscriptionVector(ITetrisNode* node);

    /**
     * @brief      Get the max frequency that the CAM messages are sent for the input node.
     * @param[in]  node iTetris node whose subscription property will be checked.
     * @param[out] maxFrequency Max frequency of the Cam sent.
     */
    static float getCamFrequencyFromCamSubscriptionVector(ITetrisNode* node);

    /**
     * @brief      Get the payload length for the CAM messages sent by the input node.
     * @param[in]  node iTetris node whose subscription property will be checked.
     * @param[out] payloadLength Max length of the Cam sent.
     */
    static unsigned int getCamPayloadLengthFromCamSubscriptionVector(ITetrisNode* node);

    /**
     * @brief      Set the new max frequency that the CAM messages are sent for the input node.
     * @param[in]  node iTetris node whose subscription property will be changed.
     * @param[in]  newFrequency Frequency of the CAM message transmission.
     */
    static void setNewCamFrequencyInCamSubscriptionVector(ITetrisNode* node, float newFrequency);

    /**
     * @brief      Set the new max payload length for the CAM messages that are sent by the input node.
     * @param[in]  node iTetris node whose subscription property will be changed.
     * @param[in]  newPayloadLength Length of the payload of the CAM messages.
     */
    static void setNewPayloadLengthInCamSubscriptionVector(ITetrisNode* node, float newFrequency);

    /**
     * @brief      Remove the input node from the general Cam subscription vector.
     * @param[in]  node iTetris node whose subscription property will be deleted.
     * @param[out] True if removed. False if not found.
     */
    static bool removeNodeFromCamSubscriptionVector(stationID_t nodeId);

    /**
     * @brief      Add the input node to the general Cam subscription vector.
     * @param[in]  node iTetris node whose subscription property will be created.
     * @param[in]  newFrequency Frequency of the CAM message transmission.
     */
    static void addNodeToCamSubscriptionVector(ITetrisNode* node, float maxFrequency, unsigned int maxPayloadLength);


    static float        getMaxFrequencyFromCamSubscriptionVector();        // TODO: remove when the CommandStartSendingCam() is updated
    static unsigned int getMaxPayloadLengthFromCamSubscriptionVector();    // TODO: remove when the CommandStartSendingCam() is updated

private:

    /// @brief Nodes that are currently in the area.
    std::vector<ITetrisNode*>* m_nodesInArea;

    /// @brief The X value of the base point.
    float m_baseX;

    /// @brief The Y value of the base point.
    float m_baseY;

    /// @brief The radius value of the base point.
    float m_radius;

    /**
     * @brief The frequency the sender will send the CAM messages
     * The number of messages to be sent per second
     */
    unsigned int m_frequency;

};

}

#endif
