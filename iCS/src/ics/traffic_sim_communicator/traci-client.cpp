/****************************************************************************/
/// @file    traci-client.cpp
/// @author  Julen Maneros
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

#include "traci-client.h"
#include "../ics.h"
#include "../itetris-node.h"
#include <utils/common/ToString.h>
#include "../../utils/ics/log/ics-log.h"
#include "../../utils/ics/iCStypes.h"

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
TraCIClient::TraCIClient() :
    m_tlsIDs_cached (false),
    m_socket(0),
    m_port(-1000)
{
}

TraCIClient::TraCIClient(int port, std::string host) :
    m_tlsIDs_cached (false),
    m_socket(0)
{
  m_port = port;
  m_host = host;
}

TraCIClient::~TraCIClient()
{
  delete m_socket;
  m_socket = 0;
}

bool TraCIClient::Connect()
{
  m_socket = new Socket(m_host, m_port);

  //m_socket->set_blocking(true);

  bool connected = false;
  for (int i = 0; i < 10; ++i)
  {
    try
    {
      cout << "iCS --> Trying " << i << " to connect SUMO on port " << m_port << "..." << endl;
      m_socket->connect();
      // subscribe departures and arrivals
      std: string objID;
      int noSubscribedVars = 2;
      tcpip::Storage outMsg, inMsg, tmp;
      outMsg.writeUnsignedByte(0);
      outMsg.writeInt(/*1 + 4 +*/5 + 1 + 4 + 8 + 8 + (int) objID.length() + 1 + noSubscribedVars);
      outMsg.writeUnsignedByte(CMD_SUBSCRIBE_SIM_VARIABLE); // command id
      outMsg.writeDouble(0.0); // begin time
      outMsg.writeDouble(86400.0); // end time
      outMsg.writeString(objID); // object id
      outMsg.writeUnsignedByte(noSubscribedVars); // variable number
      outMsg.writeUnsignedByte(VAR_DEPARTED_VEHICLES_IDS);
      outMsg.writeUnsignedByte(VAR_ARRIVED_VEHICLES_IDS);
      // send request message
      try
      {
        m_socket->sendExact(outMsg);
      } catch (SocketException &e)
      {
        cout << "Error while sending command: " << e.what();
        return false;
      }
      try
      {
        m_socket->receiveExact(inMsg);
        if (!ReportResultState(inMsg, CMD_SUBSCRIBE_SIM_VARIABLE))
        {
          return false;
        }
        vector<std::string> departed, arrived;
        processSubscriptions(inMsg, departed, arrived);
        return true;
      } catch (SocketException &e)
      {
        cout << "Error while receiving command: " << e.what();
        return false;
      }
    } catch (SocketException& e)
    {
      cout << "iCS --> No connection to SUMO; waiting..." << e.what() << endl;
      Sleep(3000);
    }
  }
  return false;
}

int TraCIClient::Close()
{
  if (m_socket != 0)
  {
    m_socket->close();
    delete m_socket;
    m_socket = 0;
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

int TraCIClient::CommandSimulationStep(int time, std::vector<std::string> &departed, std::vector<std::string> &arrived)
{
  if (m_socket == 0)
  {
    cout << "iCS --> #Error while sending command: no connection to server SUMO" << endl;
    return EXIT_FAILURE;
  }

  tcpip::Storage outMsg;
  tcpip::Storage inMsg;
  outMsg.writeUnsignedByte(1 + 1 + 8); // command length
  outMsg.writeUnsignedByte(CMD_SIMSTEP2); // command id
  outMsg.writeDouble(double(time)/1000.); //time (in s)
  // send request message
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while sending command to SUMO: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command from SUMO: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  // validate result state
  if (!ReportResultState(inMsg, CMD_SIMSTEP2))
  {
    return EXIT_FAILURE;
  }
  // process subscriptions
  if (!processSubscriptions(inMsg, departed, arrived))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

bool TraCIClient::processSubscriptions(tcpip::Storage &inMsg, std::vector<std::string> &departed,
    std::vector<std::string> &arrived)
{
  try
  {
    int noSubscriptions = inMsg.readInt();
    for (int s = 0; s < noSubscriptions; ++s)
    {
      int respStart = inMsg.position();
      int extLength = inMsg.readUnsignedByte();
      if (extLength == 0)
        extLength = inMsg.readInt();
      int cmdId = inMsg.readUnsignedByte();
      if (cmdId < 0xe0 || cmdId > 0xef)
      {
        return false;
      }
      std::string objID = inMsg.readString();
      unsigned int varNo = inMsg.readUnsignedByte();
      for (unsigned int i = 0; i < varNo; ++i)
      {
        int varID = inMsg.readUnsignedByte();
        bool ok = inMsg.readUnsignedByte() == RTYPE_OK;
        int valueDataType = inMsg.readUnsignedByte();
        if (ok && cmdId == CMD_SUBSCRIBE_SIM_VARIABLE + 0x10 && varID == VAR_DEPARTED_VEHICLES_IDS)
        {
          departed = inMsg.readStringList();
          continue;
        }
        if (ok && cmdId == CMD_SUBSCRIBE_SIM_VARIABLE + 0x10 && varID == VAR_ARRIVED_VEHICLES_IDS)
        {
          arrived = inMsg.readStringList();
          continue;
        }
      }
    }
  } catch (invalid_argument& e)
  {
    cout << "#Error while reading message:" << e.what() << std::endl;
    return false;
  }
  return true;
}

bool TraCIClient::ReportResultState(tcpip::Storage& inMsg, int command)
{
  int cmdLength;
  int cmdId;
  int resultType;
  int cmdStart;
  std::string msg;

  try
  {
    cmdStart = inMsg.position();
    cmdLength = inMsg.readUnsignedByte();
    cmdId = inMsg.readUnsignedByte();
    if (cmdId != command)
    {
      cout << "SUMO --> iCS #Error: received status response to command: " << cmdId << " but expected: " << command
          << endl;
      return false;
    }
    resultType = inMsg.readUnsignedByte();
    msg = inMsg.readString();
  } catch (invalid_argument& e)
  {
    cout << "SUMO --> iCS #Error: an exception was thrown while reading result state message" << endl;
    return false;
  }

  switch (resultType)
  {
  case RTYPE_ERR:
  {
    cout << ".. SUMO answered with error to command (" << cmdId << "), [description: " << msg << "]" << endl;
    return false;
  }
  case RTYPE_NOTIMPLEMENTED:
    cout << ".. Sent command is not implemented (" << cmdId << "), [description: " << msg << "]" << endl;
    return false;
  case RTYPE_OK:
  {
#ifdef LOG_ON
     stringstream log;
     log << ".. Command acknowledged (" << cmdId << "), [description: " << msg << "]" << endl;
     IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
     break;
  }
  default:
    cout << ".. Answered with unknown result code(" << resultType << ") to command(" << cmdId << "), [description: "
        << msg << "]" << endl;
    return false;
  }
  if ((cmdStart + cmdLength) != inMsg.position())
  {
    cout << "SUMO --> iCS #Error: command at position " << cmdStart << " has wrong length" << endl;
    return false;
  }

  return true;
}

int TraCIClient::CommandClose()
{
  Storage outMsg;
  Storage inMsg;
  std::stringstream msg;

  if (m_socket == NULL)
  {
    cout << "iCS --> #Error while sending command: no connection to server" << endl;
    return EXIT_FAILURE;
  }

  // command length
  outMsg.writeUnsignedByte(1 + 1);
  // command id
  outMsg.writeUnsignedByte(CMD_CLOSE);

  // send request message
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  // validate result state
  if (!ReportResultState(inMsg, CMD_CLOSE))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

double
TraCIClient::getSimstepLength() {
    int commandID = CMD_GET_SIM_VARIABLE;
    tcpip::Storage inMsg;
    beginValueRetrieval("", VAR_DELTA_T, inMsg, commandID);
    const double simstepLength = inMsg.readDouble();
    return simstepLength;
}

int TraCIClient::CommandSetMaximumSpeed(const ITetrisNode &node, float maxSpeed)
{
  if (m_socket == 0)
  {
    cout << "iCS --> [ERROR] Socket is NULL" << endl;
    return EXIT_FAILURE;
  }

  Storage outMsg, inMsg, tmpMsg;
  tmpMsg.writeUnsignedByte(CMD_SET_VEHICLE_VARIABLE);    // command id
  tmpMsg.writeUnsignedByte(VAR_SPEED);    // variable id
  tmpMsg.writeString(node.m_tsId);    // object id
  tmpMsg.writeUnsignedByte(TYPE_DOUBLE); //data type
  tmpMsg.writeDouble((double) maxSpeed); // value

  outMsg.writeUnsignedByte(0); // command length -> extended
  outMsg.writeInt(1 + 4 + (int) tmpMsg.size());
  outMsg.writeStorage(tmpMsg);

  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return EXIT_FAILURE;
  }

#ifdef LOG_ON
  stringstream log;
  log << "Sending Maximum Speed of " << maxSpeed << " for node (SUMO ID): " << node.m_tsId;
  IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

float TraCIClient::GetSpeed(const ITetrisNode &node)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_SPEED, inMsg);
  return (float) inMsg.readDouble();
}

float TraCIClient::GetDirection(const ITetrisNode &node)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_ANGLE, inMsg);
  return (float) inMsg.readDouble();
}

float TraCIClient::GetVehicleLength(const ITetrisNode &node)
{
//	Not working if the vehicle state is changed because the type changes
//  std: string type = GetVehicleType(node);
//  tcpip::Storage inMsg;
//  beginValueRetrieval(type, VAR_LENGTH, inMsg, CMD_GET_VEHICLETYPE_VARIABLE);
//  return (float) inMsg.readDouble();
	tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_LENGTH, inMsg);
  return (float) inMsg.readDouble();
}

float TraCIClient::GetVehicleWidth(const ITetrisNode &node)
{
//	Not working if the vehicles state is changed because the type changes
//  std: string type = GetVehicleType(node);
//  tcpip::Storage inMsg;
//  beginValueRetrieval(type, VAR_WIDTH, inMsg, CMD_GET_VEHICLETYPE_VARIABLE);
//  return (float) inMsg.readDouble();
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_WIDTH, inMsg);
  return (float) inMsg.readDouble();
}

std::pair<float, float> TraCIClient::GetPosition(const ITetrisNode &node)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_POSITION, inMsg);
  double x_double = inMsg.readDouble();
  double y_double = inMsg.readDouble();
  float x = (float) x_double;
  float y = (float) y_double;
  return make_pair(x, y);
}

bool TraCIClient::GetExteriorLights(const ITetrisNode &node)
{
  return false;
}

string TraCIClient::GetLane(const ITetrisNode &node)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_LANE_ID, inMsg);
  return inMsg.readString();
}

std::string TraCIClient::GetVehicleType(const ITetrisNode &node)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_TYPE, inMsg);
  return inMsg.readString();
}

int TraCIClient::SetVehicleToRunInLane(const ITetrisNode &node, string laneId)
{
  return 0;
}

int TraCIClient::SetVehicleToRunInLane(string laneId)
{
  return 0;
}

bool TraCIClient::ReRoute(const ITetrisNode &node, vector<string> route)
{
  if (m_socket == 0)
    return false;
  Storage outMsg, inMsg, tmpMsg;
  tmpMsg.writeUnsignedByte(CMD_SET_VEHICLE_VARIABLE); // command id
  tmpMsg.writeUnsignedByte(VAR_ROUTE); // variable id
  tmpMsg.writeString(node.m_tsId); // object id
  tmpMsg.writeUnsignedByte(TYPE_STRINGLIST); //data type
  tmpMsg.writeStringList(route); // value

  outMsg.writeUnsignedByte(0); // command length -> extended
  outMsg.writeInt(1 + 4 + tmpMsg.size());
  outMsg.writeStorage(tmpMsg);
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return false;
  }
#ifdef _DEBUG
    cout << "iCS --> Sending NewRoute for node (SUMO ID): " << node.m_tsId << endl;
#endif

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    return false;
  }
  return true;
}

bool TraCIClient::ReRoute(const ITetrisNode &node)
{
  if (m_socket == 0)
    return false;
  Storage outMsg, inMsg, tmpMsg;
  tmpMsg.writeUnsignedByte(CMD_SET_VEHICLE_VARIABLE); // command id
  tmpMsg.writeUnsignedByte(CMD_REROUTE_TRAVELTIME); // variable id
  tmpMsg.writeString(node.m_tsId); // object id
  tmpMsg.writeUnsignedByte(TYPE_COMPOUND); //data type
  tmpMsg.writeInt(0); // value

  outMsg.writeUnsignedByte(0); // command length -> extended
  outMsg.writeInt(1 + 4 + tmpMsg.size());
  outMsg.writeStorage(tmpMsg);
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return false;
  }
#ifdef _DEBUG
    cout << "iCS --> Sending NewRoute for node (SUMO ID): " << node.m_tsId << endl;
#endif

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    return false;
  }
  return true;
}

bool TraCIClient::ChangeTrafficLightStatus(string trafficLightId, string lightStates)
{
  if (m_socket == 0)
    return false;

  Storage outMsg, inMsg, tmpMsg;

  // Storage outMsg, inMsg;
  tmpMsg.writeUnsignedByte(CMD_SET_TL_VARIABLE);    // command id
  tmpMsg.writeUnsignedByte(TL_RED_YELLOW_GREEN_STATE);    // variable id
  tmpMsg.writeString(trafficLightId);    // object id
  tmpMsg.writeUnsignedByte(TYPE_STRING); //data type
  tmpMsg.writeString(lightStates); // value

  outMsg.writeUnsignedByte(0); // command length -> extended
  outMsg.writeInt(1 + 4 + tmpMsg.size());
  outMsg.writeStorage(tmpMsg);

  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return false;
  }
#ifdef _DEBUG
     cout << "iCS --> Sending signal phase" << lightStates << " for tls (SUMO ID): " << trafficLightId << endl;
#endif

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    return false;
  }
  return true;
}

bool TraCIClient::ChangeEdgeWeight(const ITetrisNode &node, string edgeId, float weight)
{
  if (m_socket == 0)
    return false;
  Storage outMsg, inMsg, tmpMsg;
  tmpMsg.writeUnsignedByte(CMD_SET_VEHICLE_VARIABLE); // command id
  tmpMsg.writeUnsignedByte(VAR_EDGE_TRAVELTIME); // variable id
  tmpMsg.writeString(node.m_tsId); // object id
  tmpMsg.writeUnsignedByte(TYPE_COMPOUND); //data type
  tmpMsg.writeUnsignedByte(TYPE_INTEGER);
  tmpMsg.writeInt(2);
  tmpMsg.writeUnsignedByte(TYPE_STRING); //edge id
  tmpMsg.writeString(edgeId);
  tmpMsg.writeUnsignedByte(TYPE_DOUBLE); //edge id
  tmpMsg.writeDouble((double) weight);

  outMsg.writeUnsignedByte(0); // command length -> extended
  outMsg.writeInt(1 + 4 + tmpMsg.size());
  outMsg.writeStorage(tmpMsg);
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return false;
  }
#ifdef _DEBUG
    cout << "iCS --> Sending NewRoute for node (SUMO ID): " << node.m_tsId << endl;
#endif

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    return false;
  }
  return true;
}

bool TraCIClient::SetEdgeWeight(string edgeId, float weight)
{
  if (m_socket == 0)
    return false;
  Storage outMsg, inMsg, tmpMsg;
  tmpMsg.writeUnsignedByte(CMD_SET_EDGE_VARIABLE); // command id
  tmpMsg.writeUnsignedByte(VAR_EDGE_TRAVELTIME); // variable id
  tmpMsg.writeString(edgeId); // edge ID only in string
  tmpMsg.writeUnsignedByte(TYPE_COMPOUND); //data type
  tmpMsg.writeInt(3); // begin, end and value fields // LG WAS 1
  tmpMsg.writeUnsignedByte(TYPE_INTEGER); // start validity
  tmpMsg.writeInt((int) (0 * 1000));
  tmpMsg.writeUnsignedByte(TYPE_INTEGER); // end validity
  tmpMsg.writeInt((int) (2000 * 1000));
  tmpMsg.writeUnsignedByte(TYPE_DOUBLE); // value
  tmpMsg.writeFloat((double) weight);

  outMsg.writeUnsignedByte(0); // command length -> extended
  outMsg.writeInt(1 + 4 + tmpMsg.size());
  outMsg.writeStorage(tmpMsg);
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> Error while sending command: " << e.what() << endl;
    return false;
  }
#ifdef _DEBUG
     cout << "iCS --> Changing EdgeTravelTime weight for edge: " << edgeId << endl;
#endif

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
    cout << "iCS --> #Error while receiving command in TraCIClient::SetEdgeWeight(): " << e.what() << endl;
    return false;
  }
  return true;
}

float TraCIClient::GetEdgeWeight(std::string edgeID)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(edgeID, VAR_EDGE_TRAVELTIME, inMsg, CMD_GET_EDGE_VARIABLE);
  return (float) inMsg.readDouble();
}

std::vector<std::string> TraCIClient::GetRouteEdges(const ITetrisNode &node)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(node.m_tsId, VAR_EDGES, inMsg);
  return inMsg.readStringList();
}

std::vector<std::string> TraCIClient::GetRouteEdges(std::string routeID)
{
  tcpip::Storage inMsg;
  beginValueRetrieval(routeID, VAR_EDGES, inMsg, CMD_GET_ROUTE_VARIABLE);
  return inMsg.readStringList();
}

void TraCIClient::beginValueRetrieval(const std::string &objID, int varID, tcpip::Storage &inMsg, int commandID)
{
  if (m_socket == 0)
  {
    cerr << "#Error while sending command: no connection to server";
    throw SocketException("Socket is closed");
  }

  tcpip::Storage outMsg;
  outMsg.writeUnsignedByte(1 + 1 + 1 + 4 + (int) objID.length()); // command length
  outMsg.writeUnsignedByte(commandID); // command id
  outMsg.writeUnsignedByte(varID); // variable id
  outMsg.writeString(objID); // object id

  // send request message
  try
  {
    m_socket->sendExact(outMsg);
  } catch (SocketException& e)
  {
  	cerr << "iCS --> #Error while sending command: " << e.what() << endl;
    throw SocketException("Socket is closed");
  }

  // receive answer message
  try
  {
    m_socket->receiveExact(inMsg);
  } catch (SocketException& e)
  {
  	cerr << "iCS --> #Error while receiving command: " << e.what() << endl;
  }

  if (!ReportResultState(inMsg, commandID))
  {
    throw SocketException("");
  }
  // validate result state
  int respStart = inMsg.position();
  int extLength = inMsg.readUnsignedByte();
  if (extLength == 0)
    extLength = inMsg.readInt();

  //int respLength = inMsg.readInt();
  int cmdId = inMsg.readUnsignedByte();
  if (cmdId != (commandID + 0x10))
  {
    throw SocketException(
        "#Error: received response with command id: " + toString(cmdId) + "but expected: " + toString(commandID + 0x10));
  }
  inMsg.readUnsignedByte(); // variable id
  inMsg.readString(); // object id
  inMsg.readUnsignedByte(); // value type
}


void
TraCIClient::controlTraCI(tcpip::Storage &inMsg, tcpip::Storage &outMsg)
{
    if (m_socket == 0) {
        cout << "#Error while sending command: no connection to server" ;
        throw SocketException("Socket is closed");
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);// no interpretation, just forward
    } catch (SocketException& e) {
        cout << "iCS --> #Error while sending command: " << e.what() << endl;
        throw SocketException("Socket is closed");
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what() << endl;
    }
}


int
TraCIClient::GetTrafficLights(vector<trafficLightID_t>& trafficLigthIds)
{
    if (!m_tlsIDs_cached) {
        string objID;
        tcpip::Storage inMsg;
        beginValueRetrieval(objID, ID_LIST, inMsg, CMD_GET_TL_VARIABLE);
        trafficLigthIds = inMsg.readStringList(); // traffic light IDs
        m_tlsIDs = trafficLigthIds;
        m_tlsIDs_cached = true;
    } else {
        trafficLigthIds = m_tlsIDs;
    }
	return EXIT_SUCCESS;
}

int TraCIClient::GetTrafficLightStatus(ics_types::trafficLightID_t trafficLightId, std::string &state)
{
	tcpip::Storage inMsg;
	beginValueRetrieval(trafficLightId, TL_RED_YELLOW_GREEN_STATE, inMsg, CMD_GET_TL_VARIABLE);
	state = inMsg.readString(); // traffic light state
	return EXIT_SUCCESS;
}

int TraCIClient::GetTrafficLightControlledLanes(ics_types::trafficLightID_t trafficLightId,	std::vector<std::string> &lanes)
{
	tcpip::Storage inMsg;
	beginValueRetrieval(trafficLightId, TL_CONTROLLED_LANES, inMsg, CMD_GET_TL_VARIABLE);
	lanes = inMsg.readStringList(); // traffic light state
	return EXIT_SUCCESS;
}

int TraCIClient::GetTrafficLightControlledLinks(ics_types::trafficLightID_t trafficLightId, std::vector<std::vector<std::string> > &lanes)
{
	tcpip::Storage inMsg;
	beginValueRetrieval(trafficLightId, TL_CONTROLLED_LINKS, inMsg, CMD_GET_TL_VARIABLE);
	inMsg.readInt();
	inMsg.readUnsignedByte(); // value type
	int num = inMsg.readInt();
	for (int i = 0; i < num; ++i)
	{
		inMsg.readUnsignedByte(); // value type
		int links = inMsg.readInt();
		for (int j = 0; j < links; ++j)
		{
			inMsg.readUnsignedByte(); // value type
			vector<string> link = inMsg.readStringList();
			lanes.push_back(link);
		}
	}
	return EXIT_SUCCESS;
}

int TraCIClient::GetLaneLinksConsecutiveLane(std::string laneId, std::vector<std::pair<std::string, std::string> > &consecutiveLanes)
{
	tcpip::Storage inMsg;
	beginValueRetrieval(laneId, LANE_LINKS, inMsg, CMD_GET_LANE_VARIABLE);
	inMsg.readInt();
	inMsg.readUnsignedByte(); // value type
	int num = inMsg.readInt();
	for (int i = 0; i < num; ++i)
	{
		inMsg.readUnsignedByte(); // value type
		string s1 = inMsg.readString();
		inMsg.readUnsignedByte(); // value type
		string s2 = inMsg.readString();
		inMsg.readUnsignedByte(); // value type
		inMsg.readUnsignedByte();
		inMsg.readUnsignedByte(); // value type
		inMsg.readUnsignedByte();
		inMsg.readUnsignedByte(); // value type
		inMsg.readUnsignedByte();
		inMsg.readUnsignedByte(); // value type
		inMsg.readString();
		inMsg.readUnsignedByte(); // value type
		inMsg.readString();
		inMsg.readUnsignedByte(); // value type
		inMsg.readDouble();
		consecutiveLanes.push_back(make_pair(s1, s2));
	}
	return EXIT_SUCCESS;
}

float TraCIClient::GetLaneMaxSpeed(std::string laneId)
{
	tcpip::Storage inMsg;
	beginValueRetrieval(laneId, VAR_MAXSPEED, inMsg, CMD_GET_LANE_VARIABLE);
	return static_cast<float>(inMsg.readDouble());
}

std::string TraCIClient::GetVehicleClass(const ITetrisNode &node)
{
	tcpip::Storage inMsg;
	beginValueRetrieval(node.m_tsId, VAR_VEHICLECLASS, inMsg);
	return inMsg.readString();
}

int TraCIClient::TraciCommand(tcpip::Storage & command,tcpip::Storage & result)
{
	if (m_socket == 0)
	{
		cerr << "#Error while sending command: no connection to server";
		throw SocketException("Socket is closed");
	}
	// send request message
	try
	{
		m_socket->sendExact(command);
	} catch (SocketException& e)
	{
		cerr << "iCS --> #Error while sending command: " << e.what() << endl;
		throw SocketException("Error while sending command");
	}

	// receive answer message
	try
	{
		m_socket->receiveExact(result);
	} catch (SocketException& e)
	{
		cerr << "iCS --> #Error while receiving command: " << e.what() << endl;
		throw SocketException("Error while receiving command");
	}
	return EXIT_SUCCESS;
}
} //sumoclient

