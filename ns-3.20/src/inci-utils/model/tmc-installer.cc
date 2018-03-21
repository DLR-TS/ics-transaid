/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Jerome Haerri <haerri@eurecom.fr>
 */

#include "tmc-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"


// Includes for facilties
#include "ns3/mw-facilities-helper.h"
#include "ns3/MWFacilities.h"

// Specific objects for RsuVehicleStas
#include "ns3/ip-base-sta-mgnt.h"

NS_LOG_COMPONENT_DEFINE ("TmcInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (TmcInstaller);

TypeId TmcInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TmcInstaller")
    .SetParent<CommModuleInstaller> ()
    .AddConstructor<TmcInstaller> ()
    ;
  return tid;
}
    
void 
TmcInstaller::Install (NodeContainer container)
{
  NS_LOG_INFO ("TmcInstaller");
  uint32_t index = 0;
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
	  // JHNote(22/01/2011) : a TMC not having any net devices, it also does not have any manager
      // Check if the RSU has the object RsuStaMgnt already installed
     /* Ptr<IpBaseStaMgnt> ipStaMgnt = (*i)->GetObject <IpBaseStaMgnt> ();
      if (ipStaMgnt == NULL)
	{
    	  ipStaMgnt = CreateObject <IpBaseStaMgnt> ();
    	  ipStaMgnt->SetNode (*i);
          (*i)->AggregateObject (ipStaMgnt);
          NS_LOG_INFO ("The object IpStaMgnt has been installed in the TMC");
        }*/

      // Check if the vehicle has the Facilities already installed
      Ptr<MWFacilities> facilities = (*i)->GetObject <MWFacilities> ();
      if (facilities == NULL)
	{
    	  MWFacilitiesHelper facilitiesHelper;
          //facilitiesHelper.AddDefaultServices (m_servListHelper); // services not required on the TMC
          facilitiesHelper.Install (*i);
          NS_LOG_INFO ("The object MWFacilities has been installed in the TMC");
        }
      /*else  // Service not required on the TMC
        {
    	  MWFacilitiesHelper  facilitiesHelper;
          facilitiesHelper.SetServiceListHelper (m_servListHelper);
          facilitiesHelper.AddServices (facilities, *i);
          NS_LOG_INFO ("New services have been installed in the TMC");
        }*/

      index ++;
    }

}

} // namespace ns3
