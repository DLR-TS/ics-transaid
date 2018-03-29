/****************************************************************************/
/// @file    subs-app-control-traci.cpp
/// @author  Jerome Haerri (EURECOM)
/// @date    March 31st, 2015
/// @version $Id:
///

/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Jerome Haerri (jerome.haerri@eurecom.fr) and Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <cstring>

#include "subs-app-control-traci.h"
#include "subscriptions-helper.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../itetris-node.h"
#include "app-commands-subscriptions-constants.h"

namespace ics
{

// ===========================================================================
// Constants
// ===========================================================================


// ===========================================================================
// static member definitions
// ===========================================================================
int SubsAppControlTraci::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
{
  if (subscriptions == NULL)
  {
    return EXIT_FAILURE;
  }

  vector<Subscription*>::iterator it;
  for (it = subscriptions->begin(); it < subscriptions->end(); it++)
  {
    Subscription* sub = *it;
    const type_info& typeinfo = typeid(sub);
    if (typeinfo == typeid(SubsAppControlTraci*))
    {
      SubsAppControlTraci* subsAppControlTraci = static_cast<SubsAppControlTraci*>(sub);
      if (subsAppControlTraci->m_nodeId == stationID)
      {
        delete subsAppControlTraci;
        delete sub;
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_SUCCESS;
}

// ===========================================================================
// member method definitions
// ===========================================================================

SubsAppControlTraci::SubsAppControlTraci(int appId, ics_types::stationID_t stationId, SyncManager* synchManager, unsigned char* msg,
    int msgSize) :
    Subscription(stationId), out_msg(msg, msgSize)
{
  // Read parameters
  m_id = ++m_subscriptionCounter;

  m_name = "SEND a Command to TraCI";

  m_appId = appId;

  pull(synchManager);
}

SubsAppControlTraci::~SubsAppControlTraci()
{
    //TODO manage memory
    //delete in_msg;
    //delete out_msg;
}

void SubsAppControlTraci::pull(SyncManager* syncManager)
{

  syncManager->m_trafficSimCommunicator->controlTraCI(in_msg, out_msg);

}


int SubsAppControlTraci::InformApp(AppMessageManager* messageManager)
{
  if (in_msg.size() > 0)
  {
    if (messageManager->CommandSendSubscriptionControlTraCI(m_id, in_msg, m_nodeId) == EXIT_FAILURE)
    {
      IcsLog::LogLevel("SubsAppControlTraci::InformApp() Could not send the result of the subscription",
          kLogLevelError);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

void SubsAppControlTraci::getMsg(tcpip::Storage* outMsg){

	if ((int)out_msg.size()>0){
		unsigned char* packet = NULL;
		packet = new unsigned char[(int)out_msg.size()];
		for (unsigned int i = 1; i <= (int)out_msg.size(); i++)
		{
			unsigned char type = out_msg.readChar();
			packet[i-1] = type;
			outMsg->writeChar(type);
		}
		delete[] packet;
	}
}

std::pair<float, float>  SubsAppControlTraci::getPositionFromMsg()
{

	float x = -101.0;
	float y = -101.0;

	if ((int)out_msg.size()>0){
		int msgLength = (int)out_msg.size();

		if ((msgLength>0) && (msgLength<255)){

			//Parsing the received message to get the appropriate informations
			out_msg.readUnsignedByte(); //length

			int cmdVariable =  out_msg.readUnsignedByte();
			//tmpMsg.writeUnsignedByte(CMD_SET_VEHICLE_VARIABLE);// command id
			int varMoveTo =  out_msg.readUnsignedByte();
			//tmpMsg.writeUnsignedByte(VAR_MOVE_TO_VTD);// variable id

			//get node's info only if message is Move_TO_VTD
		    if ((cmdVariable==CMD_SET_VEHICLE_VARIABLE)&& (varMoveTo==VAR_MOVE_TO_VTD)){
		    	out_msg.readString();
		    	//tmpMsg.writeString(node->tsId);// object id
		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_COMPOUND); //data type
		    	out_msg.readInt();
		    	//tmpMsg.writeInt(4);
		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_STRING);
		    	out_msg.readString();
		    	//tmpMsg.writeString(node->tsId);//Edge

		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_INTEGER);
		    	out_msg.readInt();
		    	//tmpMsg.writeInt(std::stoi(node->lane, nullptr)); //convert from string to int

		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_DOUBLE);
		    	x = (float)out_msg.readDouble();
		    	//tmpMsg.writeDouble(node->x + 1.0);
		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_DOUBLE);
		    	y = (float)out_msg.readDouble();
		    	//tmpMsg.writeDouble(node->y );
		    }
		}
		else if (msgLength>255){

			//Parsing the received message to get the appropriate informations
			out_msg.readUnsignedByte(); //length
			out_msg.readInt(); //length
			int cmdVariable =  out_msg.readUnsignedByte();
			//tmpMsg.writeUnsignedByte(CMD_SET_VEHICLE_VARIABLE);// command id
			int varMoveTo =  out_msg.readUnsignedByte();
			//tmpMsg.writeUnsignedByte(VAR_MOVE_TO_VTD);// variable id

			//get node's info only if message is Move_TO_VTD
		    if ((cmdVariable==CMD_SET_VEHICLE_VARIABLE)&& (varMoveTo==VAR_MOVE_TO_VTD)){
		    	out_msg.readString();
		    	//tmpMsg.writeString(node->tsId);// object id
		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_COMPOUND); //data type
		    	out_msg.readInt();
		    	//tmpMsg.writeInt(4);
		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_STRING);
		    	out_msg.readString();
		    	//tmpMsg.writeString(node->tsId);//Edge

		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_INTEGER);
		    	out_msg.readInt();
		    	//tmpMsg.writeInt(std::stoi(node->lane, nullptr)); //convert from string to int

		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_DOUBLE);
		    	x = (float)out_msg.readDouble();
		    	//tmpMsg.writeDouble(node->x + 1.0);
		    	out_msg.readUnsignedByte();
		    	//tmpMsg.writeUnsignedByte(TYPE_DOUBLE);
		    	y = (float)out_msg.readDouble();
		    	//tmpMsg.writeDouble(node->y );
		    }
		}
	}

	return make_pair(x, y);
}

void SubsAppControlTraci::printGetSpeedMessage()
{
	std::cout<<"[iCS] [SubsAppControlTraci::printGetSpeedMessage]"<<std::endl;
	if ((int)out_msg.size()>0){
		int msgLength = (int)out_msg.size();
			//Parsing the received message to get the appropriate informations
			int length = out_msg.readUnsignedByte(); //length
			std::cout<<"[iCS] [SubsAppControlTraci][printGetSpeedMessage] command length: "<<length<<std::endl;

			int cmdVariable =  out_msg.readUnsignedByte();
			std::cout<<"[iCS] [SubsAppControlTraci][printGetSpeedMessage] command id: "<<cmdVariable<<std::endl;
			//tmpMsg.writeUnsignedByte(CMD_GET_VEHICLE_VARIABLE);// command id
			int varSpeed =  out_msg.readUnsignedByte();
			std::cout<<"[iCS] [SubsAppControlTraci][printGetSpeedMessage] VarId: "<<varSpeed<<std::endl;
			//tmpMsg.writeUnsignedByte(VAR_SPEED);// variable id

			//get node's info only if message is Move_TO_VTD
		    if ((cmdVariable==CMD_GET_VEHICLE_VARIABLE)&& (varSpeed==VAR_SPEED)){
		    	//tmpMsg.writeString(node->tsId);// object id
		    	std::cout<<"[iCS] [SubsAppControlTraci::printGetSpeedMessage] node->tsID"<<out_msg.readString()<<std::endl;
		    	//tmpMsg.writeDouble(node->y );
		    }
	}
}


tcpip::Storage& SubsAppControlTraci::returnValues()
{
  return in_msg;
}

} // end namespace ics
