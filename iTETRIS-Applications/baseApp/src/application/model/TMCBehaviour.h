/*
 * TMCBehaviour.h
 *
 *  Created on: Mar 25, 2019
 *      Author: Leonhard Lücken
 */

#ifndef SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_
#define SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_

#include "ics-interface.h"

namespace baseapp {
namespace application {

class TMCBehaviour {
public:
    TMCBehaviour();
    virtual ~TMCBehaviour();

    /// @brief To be called, when a message is received at an RSU
    virtual void ReceiveMessage(int rsuID, server::Payload * payload, double snr) = 0;

    /// @brief Add a new RSU to be controlled by this TMC
    virtual void addRSU(iCSInterface* rsu) {
        m_RSUController.insert(std::make_pair(rsu->GetId(), rsu));
    };

    /// @brief Add a new RSU to be controlled by this TMC
    virtual void Execute();

private:
    /// @brief RSU interfaces controllable by the TMC
    std::map<int, iCSInterface*> m_RSUController;

    /// @brief To be called by an RSU, when a message is received
    virtual void ReceiveMessage() = 0;
};

} /* namespace application */
} /* namespace baseapp */

#endif /* SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_ */
