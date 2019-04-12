/*
 * TMCBehaviourTest.h
 *
 *  Created on: Mar 26, 2019
 *      Author: Leonhard Luecken
 */


#ifndef SRC_TEST_TMCBEHAVIOURTEST_H_
#define SRC_TEST_TMCBEHAVIOURTEST_H_

#include "application/model/TMCBehaviour.h"

namespace baseapp {
namespace application {
class iCSInterface;
}
}

using namespace baseapp;
using namespace baseapp::application;

namespace testapp {
namespace application {

class TMCBehaviourTest : public TMCBehaviour
{
public:
    TMCBehaviourTest();

    virtual ~TMCBehaviourTest();

    /// @brief To be called, when a message is received at an RSU
    /// @note  The payload pointer will be deleted externally after this call.
    void ReceiveMessage(int rsuID, server::Payload * payload, double snr, bool mobileNode=false);

    /// @brief Add a new RSU to be controlled by this TMC
    void Execute();

    /// @brief Add a new RSU to be controlled by this TMC
    void OnAddSubscriptions();

private:

    bool isActive();

};

} /* namespace application */
} /* namespace testapp */

#endif /* SRC_TEST_TMCBEHAVIOURTEST_H_ */
