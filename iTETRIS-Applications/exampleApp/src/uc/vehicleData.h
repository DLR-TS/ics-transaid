/*
 *  Created on: Mar 31, 2020
 *  Author: Vasilios Karagounis
 */

#ifndef VEHICLE_DATA_H_
#define VEHICLE_DATA_H_

#include <iostream>

namespace vehicleData
{
    struct general_t
    {
        bool optimizeForSpeed;
        
        bool debugMain;
        bool debugToc;
        
        bool debugCamMessages;
        bool debugDenmMessages;

        bool handleTeleported;
    };

    struct functions_t
    {
        bool isTocEnable;
        bool isExitEnable;
    };

    struct exit_network_t
    {
        std::string edge;
        double pos;
        double posBegin;
        bool done;
    };

    struct toc_t
    {
        double probability;
        double leadTime;
        double downwardPos;
        double downwardPosBegin;

        int downwardFromRsuDenm;

        bool downwardFromRsuDone;
        bool downwardDenmHighlight;
        bool downwardTOCDone;

        std::string downwardEdge;
    };
    
    struct tracked_vehicle_t
    {
        bool enable;
        double pos;
        double posBegin;
        std::string edge;
        std::string view;
        std::string vehName;

        tracked_vehicle_t()
        {
            enable = false;
            view = edge = vehName = "";
            pos = 0.0;
        }
    };
    
    struct execute_ids_t
    {
        int distanceExit;
        int distanceDownwardToc;
        int distanceTrackedVeh;

        void reset() { distanceExit = distanceDownwardToc = distanceTrackedVeh = -1; }
    };
} // namespace vehicleData

#endif /* VEHICLE_DATA_H_ */
