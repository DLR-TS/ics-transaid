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

#include "rng-seed-manager.h"

namespace ns3 {

uint64_t RngSeedManager::NextStreamIndex = 0;
uint32_t RngSeedManager::RngSeed = 1;
uint64_t RngSeedManager::RngRun = 1;

uint32_t RngSeedManager::GetSeed(void) {
    return RngSeed;
}
void RngSeedManager::SetSeed(uint32_t seed) {
    RngSeed = seed;
}

void RngSeedManager::SetRun(uint64_t run) {
    RngRun = run;
}

uint64_t RngSeedManager::GetRun() {
    return RngRun;
}

uint64_t RngSeedManager::GetNextStreamIndex(void) {
    return NextStreamIndex++;
}

} // namespace ns3
