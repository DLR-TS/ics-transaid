/****************************************************************************************
 * Copyright (c) 2020 Centre for Research and Technology-Hellas (CERTH)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * CERTH and its contributors''.
 * 4. Neither the name of the Centre nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Vasilios Karagounis
  ***************************************************************************************/

#ifndef VEHICLE_H
#define VEHICLE_H

#include "headers.h"
#include "UCConfig.h"
#include "vehicleData.h"
#include "ics-interface.h"
#include "uc/jsonReader.h"

using namespace baseapp;
using namespace baseapp::application;

using namespace vehicleData;

//using namespace ucapp;
//using namespace ucapp::application;

namespace ucapp
{
namespace application
{
//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
class Vehicle
{

public:
  Vehicle(const std::string &_vehID, iCSInterface *_interface, tracked_vehicle_t &_trackedData);
  virtual ~Vehicle(){};

  static const AutomationType deriveAutomationTypeFromID(const std::string &vehicleID) { return AutomationType::AT_UNKNOWN; }

  // @brief Decide whether AutomationType is a connected type
  static bool isConnected(AutomationType type);

  // @brief Get vehicle identifier (prefix)
  const std::string &getIdentifier() const;

  // @brief Return whether this vehicle is connected (CV or CAV)
  bool isConnected() const;

  //// getter and setter methods

  AutomationType getAutomationType() const { return automationType; }

  const std::string &getSumoId() const { return vehID; }

  bool isInitialised() const { return initialized; }

  void setTeleported(bool set) { teleported = set; }
  bool isTeleported() { return teleported; }

  bool isCAV() const { return is_CAV; }
  bool isCV() const { return is_CV; }

  void onAddSubscriptions();
  void execute();

  iCSInterface* getNodeInterface() {return nodeController; }
  
  bool setInfo(const int _nodeID, iCSInterface *_nodeController);
  int getNodeId() { return nodeID; }
  std::string & getVehId() { return vehID; }

  double getMobilitySpeed() { return useMobilityInfo ? currentSpeed : 0.0; }

  void receiveMessage(server::Payload *payload, double snr);
  
  std::string info();
private:
  void initialise();
  void initialiseData();

  // @brief Map SUMO ID of vehicle to AutomationType
  bool findTypesFromID(const std::string &vehicleID);

  void sendCommands();

  void updateExit();
  void setExit();

  void updateTeleportation();
  
  void updateTrackedVehicle();
  void setTrackedVehicle();

  void updateToC();
  void setDownwardToc();

  double randomDouble(double min, double max);

  void handle_CAM_message(TransaidHeader * header);
  void handle_DENM_message(TransaidHeader * header);

private:
  iCSInterface *iface;
  iCSInterface *nodeController;

  general_t general;
  functions_t functions;
  exit_network_t exit_n;
  toc_t toc;

  execute_ids_t executeId;
  tracked_vehicle_t trackedData;

  std::string vehID;
  AutomationType automationType;
  VehicleType vehicleType;

  std::string vehIdentifier;
  std::string vehIdName;

  bool initialized;
  bool teleported;

  bool is_CAV;
  bool is_CV;

  bool useMobilityInfo;
  bool useNS3Messages;

  double currentXposBegin;
  double currentSpeed;
  int currentLaneIndex;

  int nodeID;
};

} /* namespace application */
} // namespace ucapp

#endif /* VEHICLE_H */
