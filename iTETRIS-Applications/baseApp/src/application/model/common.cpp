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

#include "common.h"
#include <sstream>

namespace baseapp
{
namespace application
{

bool IsVehicle(NodeType type)
{
  return type & NT_VEHICLE;
}

bool IsNodeType(NodeType lval, NodeType rval)
{
  if (rval == NT_ALL)
    return true;
  else if (rval == NT_RSU || rval == NT_VEHICLE)
    return lval & rval;
  else
    return lval == rval;
}

double AngleDifference(const double &angle1, const double &angle2)
{
  double difference = angle2 - angle1;
  while (difference < -180)
    difference += 360;
  while (difference > 180)
    difference -= 360;
  return difference;
}

double GetDistance(const Vector2D &pos1, const Vector2D &pos2)
{
  return std::sqrt(std::pow(pos2.x - pos1.x, 2) + std::pow(pos2.y - pos1.y, 2));
}

double NormalizeDirection(const double direction)
{
  double ret = direction;
  if (direction < -180)
    ret += 360;
  else if (direction > 180)
    ret -= 360;
  return ret;
}

std::string ToString(ProtocolId pid)
{
  switch (pid)
  {
  case PID_SPEED:
    return "Protocol Speed";
  default:
    return "???";
  }
}

std::string ToString(TypeBehaviour type)
{
  switch (type)
  {
  case TYPE_BEHAVIOUR:
    return "Behaviour";
  case TYPE_BEHAVIOUR_RSU:
    return "BehaviourRsu";
  case TYPE_BEHAVIOUR_NODE:
    return "BehaviourNode";
  case TYPE_DATA_MANAGER:
    return "DataManager";
  case TYPE_BEHAVIOUR_TEST_NODE:
    return "TestNode";
  case TYPE_BEHAVIOUR_TEST_RSU:
      return "TestRSU";
  case TYPE_BEHAVIOUR_UC1_NODE:
    return "UC1Node";
  case TYPE_BEHAVIOUR_UC1_RSU:
      return "UC1RSU";
  case TYPE_BEHAVIOUR_UC2_NODE:
      return "UC2Node";
  case TYPE_BEHAVIOUR_UC2_RSU:
      return "UC2RSU";
  case TYPE_BEHAVIOUR_UC3_NODE:
      return "UC3Node";
  case TYPE_BEHAVIOUR_UC3_RSU:
      return "UC3RSU";
  case TYPE_BEHAVIOUR_UC4_NODE:
      return "UC4Node";
  case TYPE_BEHAVIOUR_UC4_RSU:
      return "UC4RSU";
  case TYPE_BEHAVIOUR_UC5_NODE:
      return "UC5Node";
  case TYPE_BEHAVIOUR_UC5_RSU:
      return "UC5RSU";
  case TYPE_BEHAVIOUR_UC45_NODE:
      return "UC45Node";
  case TYPE_BEHAVIOUR_UC45_RSU:
      return "UC45RSU";
  default:
  {
      std::stringstream ss;
      ss << "<behavior type without known string conversion (type code: " << type << ")>";
      return ss.str();
  }
  }
}


std::string ToString(MessageType type)
{
  switch (type)
  {
  case MT_RSU_BEACON:
    return "MT_RSU_BEACON";
  case MT_BEACON_RESPONSE:
    return "MT_BEACON_RESPONSE";
  case MT_NO_LONGHER_CONFORMANT:
    return "MT_NO_LONGHER_CONFORMANT";
  case MT_RSU_TEST:
    return "MT_RSU_TEST";
  case MT_TEST_RESPONSE:
    return "MT_TEST_RESPONSE";
  case TRANSAID_CAM:
      return "TRANSAID_CAM";
  case TRANSAID_CPM:
    return "TRANSAID_CPM";
  case TRANSAID_MCM_VEHICLE:
      return "TRANSAID_MCM_VEHICLE";
  case TRANSAID_MAP:
      return "TRANSAID_MAP";
  case TRANSAID_DENM:
      return "TRANSAID_DENM";
  case TRANSAID_IVI:
      return "TRANSAID_IVI";
  case TRANSAID_MCM_RSU:
      return "TRANSAID_MCM_RSU";
  default:
  {
      std::stringstream ss;
      ss << "<behavior type without known string conversion (type code: " << type << ")>";
      return ss.str();
  }
  }
}

std::string ToString(VehicleMovement direction)
{
  switch (direction)
  {
  case APPROACHING:
    return "Approaching";
  case LEAVING:
    return "Leaving";
  default:
      return "UNKNOWN DIRECTION";

  }
}

const std::string VehicleDirection::getId() const
{
  std::ostringstream oss;
  oss << dir << ":";
  switch (vMov)
  {
  case LEAVING:
    oss << "l";
    break;
  case APPROACHING:
    oss << "a";
    break;
  }
  return oss.str();
}

bool VehicleDirection::operator==(const VehicleDirection &other) const
{
  return dir == other.dir && vMov == other.vMov;
}

bool VehicleDirection::operator!=(const VehicleDirection &other) const
{
  return !operator ==(other);
}

std::ostream& operator<<(std::ostream& os, const VehicleDirection& direction)
{
  os << direction.dir << " " << ToString(direction.vMov);
  return os;
}

} /* namespace application */
} /* namespace protocol */

