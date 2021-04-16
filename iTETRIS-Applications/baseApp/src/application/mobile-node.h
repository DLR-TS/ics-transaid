/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#ifndef MOBILE_NODE_H_
#define MOBILE_NODE_H_

#include "node.h"
#include "structs.h"

namespace baseapp {
namespace application {
class BehaviourFactory;

class MobileNode: public Node {
public:
    MobileNode(int id, BehaviourFactory* factory);
    MobileNode(MobilityInfo* info, BehaviourFactory* factory);
    MobileNode(const int nodeId, const int ns3NodeId, const std::string& sumoNodeId,
               const std::string& sumoType, const std::string& sumoClass, BehaviourFactory* factory);
    virtual ~MobileNode();

    void updateMobilityInformation(MobilityInfo* info);
    Vector2D getPosition() const;
    Vector2D getVelocity() const;
    int getLaneIndex() const;
    float getSpeed() const;
    float getAcceleration() const;
    double getDirection() const;


    /// @brief helper function that adds SetCAMArea subscription
    void subscribeSendingCAMs();

private:
    MobilityInfo* m_position;
    void selectNodeType();
};

} /* namespace application */
} /* namespace protocol */

#endif /* MOBILE_NODE_H_ */
