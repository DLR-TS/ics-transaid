/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
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
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
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
 * Author Federico Caselli <f.caselli@unibo.it>
 * Author Enrico Zamagni
 * University of Bologna
 ***************************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#include "vector.h"
#include "cmath"
#include <stdint.h>

namespace testapp
{
namespace application
{

//TODO add other vehicle types
typedef enum
{
  NT_RSU = 0x01,
  NT_VEHICLE = 0x02,
  NT_VEHICLE_FULL = 0x06,
  NT_VEHICLE_MEDIUM = 0x0A,
  NT_ALL = 0xFF,
  NT_VEHICLE_SHADOW = 0x00
} NodeType;

typedef enum
{
  PID_SPEED = 0x01
} ProtocolId;

enum Type_Behaviour
{
  TYPE_BEHAVIOUR, TYPE_BEHAVIOUR_RSU, TYPE_BEHAVIOUR_NODE, TYPE_DATA_MANAGER

}typedef TypeBehaviour;

typedef enum
{
  MT_RSU_BEACON = 0x01, MT_BEACON_RESPONSE = 0x02, MT_NO_LONGHER_CONFORMANT = 0x03
} MessageType;

typedef enum
{
  APPROACHING = 0x01, LEAVING = 0x02
} VehicleMovement;

const unsigned int ID_ALL = 0xffffffff;
const double DIR_INVALID = 1000;
const double SPD_INVALID = -1000;

struct VehicleDirection
{
  VehicleDirection()
  {
    dir = DIR_INVALID;
    vMov = APPROACHING;
    time = 0;
  }
  VehicleDirection(double direction, VehicleMovement movement) :
      dir(direction), vMov(movement)
  {
    time = 0;
  }
  VehicleDirection(double direction, VehicleMovement movement, uint16_t beaconTime) :
      dir(direction), vMov(movement), time(beaconTime)
  {
  }
  VehicleDirection(const VehicleDirection & other)
  {
    dir = other.dir;
    vMov = other.vMov;
    time = other.time;
  }
  double dir;
  VehicleMovement vMov;
  uint16_t time;
  const std::string getId() const;
  bool operator==(const VehicleDirection &other) const;
  bool operator!=(const VehicleDirection &other) const;
};
std::ostream& operator<<(std::ostream& stream, const VehicleDirection& direction);

struct NodeInfo
{
  NodeInfo()
  {
  	nodeId = lastSeen = totalTime = -1;
  	lastMessage = toRemove = false;
  	direction = DIR_INVALID;
  	distance = currentSpeed = avgSpeedHigh = avgSpeedSmall = 0;
  }
  NodeInfo(int id, Vector2D pos, double dir, VehicleDirection cDir, double cSpd, double avgSpdS, double avgSpdH,
      int last, bool lMess) :
      nodeId(id), position(pos), direction(dir), conformantDirection(cDir), currentSpeed(cSpd), avgSpeedSmall(avgSpdS), avgSpeedHigh(
          avgSpdH), lastSeen(last), lastMessage(lMess), totalTime(0), distance(0)
  {
  	toRemove = false;
  }
  int nodeId;
  int lastSeen;
  int totalTime;
  Vector2D position;
  VehicleDirection conformantDirection;
  double direction;
  double currentSpeed;
  double avgSpeedSmall;
  double avgSpeedHigh;
  double distance;
  bool lastMessage;
  bool toRemove;
};

bool IsVehicle(NodeType type);
bool IsNodeType(NodeType lval, NodeType rval);
double AngleDifference(const double &angle1, const double &angle2);
double GetDistance(const Vector2D &pos1, const Vector2D &pos2);
double NormalizeDirection(const double direction);
std::string ToString(ProtocolId pid);
std::string ToString(TypeBehaviour type);
std::string ToString(VehicleMovement direction);

} /* namespace application */
} /* namespace protocol */

#endif /* COMMON_H_ */
