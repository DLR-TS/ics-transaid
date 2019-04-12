/*
 * TMCBehaviour.h
 *
 *  Created on: Mar 25, 2019
 *      Author: Leonhard Luecken
 */

#ifndef SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_
#define SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_

#include <map>
#include "node-handler.h"

namespace baseapp {

namespace server {
class Payload;
}

namespace application {

class TMCBehaviour : public server::NodeHandler::MessageReceptionListener {
public:
    TMCBehaviour();
    virtual ~TMCBehaviour();

    /// @brief Add a new RSU to be controlled by this TMC, pointer deletion responsibility lies at caller side
    /// @note  On the first RSU registration, some provisions are taken - the first added RSU's controller will serve
    ///        as interface to the generic application functionalities.
    /// @todo: If it is deleted before the TMCBehaviour, the RSU should be removed
    void addRSU(iCSInterface* rsu);

    /// @brief To be called, when a message is received at an RSU
    /// @note  The payload pointer will be deleted externally after this call.
    virtual void ReceiveMessage(int rsuID, server::Payload * payload, double snr, bool mobileNode = false) = 0;

    /// @brief Add a new RSU to be controlled by this TMC
    /// @brief Execute() is called once per simulation step, when the last RSU has been executed.
    ///        The TMC is allowed to use the rsu's interface for issuing of general subscriptions.
    virtual void Execute() = 0;

    /// @brief OnAddSubscriptions() is called once per simulation step, when the first RSU gets
    ///        its turn to issue iCS subscriptions. The TMC is allowed to use the rsu's interface for general
    ///        subscriptions.
    virtual void OnAddSubscriptions() = 0;

protected:

    /// @brief RSU interfaces controllable by the TMC
    std::map<int, iCSInterface*> m_RSUController;

    /// @brief Controller (iCSInterface) of an arbitrary RSU (currently the first RSU added to the TMC).
    /// @note Should be used for generic interactions with the application/iCS
    /// @todo Handle rsu-deletion
    iCSInterface * iface;
};

} /* namespace application */
} /* namespace baseapp */

#endif /* SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_ */
