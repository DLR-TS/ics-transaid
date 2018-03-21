/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          CBT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Julen Maneros <jmaneros@cbt.es>
 */
/****************************************************************************************
 * Edited by Andrea Cavallari
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#include "ns3-commands.h"

namespace ns3
{

Ns3Commands::Ns3Commands ()
{

}

double
Ns3Commands::RunCommandsUntilStep (double timeStep)
{
	// Edit by Andrea Cavallari
	Simulator::Stop(NanoSeconds(timeStep * 1000000) - Simulator::Now());
	Simulator::Run();
	Time t=Simulator::Now();
    return t.GetSeconds();
}

}
