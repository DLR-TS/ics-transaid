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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "configuration-manager-xml.h"
#include "ns3/log.h"
#include "ns3/object-factory.h"

#include "iTETRISNodeManager.h"
#include "comm-module-installer.h"

#include "ns3/fatal-error.h"
#include "ns3/log.h"
#include "ns3/global-value.h"
#include "ns3/string.h"
#include "ns3/config.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

NS_LOG_COMPONENT_DEFINE("ConfigurationManagerXml");

namespace ns3 {

ConfigurationManagerXml::ConfigurationManagerXml(std::string fileName)
    : m_filename(fileName),
      m_seed(1),
      m_runNumber(1)
{}

ConfigurationManagerXml::~ConfigurationManagerXml()
{}

int ConfigurationManagerXml::GetSeed(void) {
    return m_seed;
}

int ConfigurationManagerXml::GetRunNumber(void) {
    return m_runNumber;
}

void
ConfigurationManagerXml::SetSeed(int seed) {
    m_seed = seed;
}

void ConfigurationManagerXml::SetRunNumber(int runNumber) {
    m_runNumber = runNumber;
}

void
ConfigurationManagerXml::ReadFile(iTETRISNodeManager* nodeManager) {
    std::string completePath;
    int index = 0;
    xmlTextReaderPtr reader = xmlNewTextReaderFilename(m_filename.c_str());
    if (reader == NULL) {
        NS_FATAL_ERROR("Error at xmlReaderForFile");
    }
    int rc;
    rc = xmlTextReaderRead(reader);
    while (rc > 0) {
        const xmlChar* tag = xmlTextReaderConstName(reader);
        if (tag == 0) {
            NS_FATAL_ERROR("Invalid type");
        }

        if (std::string((char*)tag) == "installer") {

            xmlChar* type = xmlTextReaderGetAttribute(reader, BAD_CAST "type");
            if (type == 0) {
                NS_FATAL_ERROR("Error getting attribute 'type'");
            }
            ObjectFactory factory;
            factory.SetTypeId((char*)type);
            Ptr<Object> object = factory.Create();
            Ptr<CommModuleInstaller> installer = DynamicCast<CommModuleInstaller>(object);

            xmlChar* name = xmlTextReaderGetAttribute(reader, BAD_CAST "name");
            if (name == 0) {
                NS_FATAL_ERROR("Error getting attribute 'name'");
            }
            nodeManager->AttachInstaller((char*)name, installer);

            NS_LOG_DEBUG("Installer type=" << type << " name=" << name);

            xmlChar* file = xmlTextReaderGetAttribute(reader, BAD_CAST "file");
            if (file == 0) {
                NS_LOG_DEBUG("'file' attribute has not been found for installer " << (char*)name);
            } else {
                completePath = m_filename;
                index = completePath.find("configTechnologies");
                completePath.replace(index, completePath.length() - 1, (char*)file);
                NS_LOG_DEBUG("CONFIGURATION-MANAGER.XML calling Pathfile= " << completePath);
                installer->Configure(completePath);
                completePath = "";
            }

            xmlChar* relatedInstaller = xmlTextReaderGetAttribute(reader, BAD_CAST "relatedInstaller");

            if (relatedInstaller != 0) {
                Ptr<CommModuleInstaller> relatedInstallerObject = nodeManager->GetInstaller((char*)relatedInstaller);
                NS_ASSERT_MSG(installer, "Installer has not been created");
                installer->RelateInstaller(relatedInstallerObject);
                NS_LOG_DEBUG("Installer " << (char*)name << " will be related with installer " << (char*)relatedInstaller);
            }
            xmlChar* def = xmlTextReaderGetAttribute(reader, BAD_CAST "default");
            if (def == 0) {
                NS_LOG_DEBUG("'default' attribute has not been found for installer " << (char*)name);
            } else {
                if (strcmp((char*)def, "true") == 0) {
                    nodeManager->SetDefaultModule((char*)name);
                    NS_LOG_DEBUG("Installer " << (char*)name << " will be installed by default in every new node");
                } else if (strcmp((char*)def, "false") != 0) {
                    NS_LOG_DEBUG("'default' attribute only accepts 'true' or 'false' values");
                }
            }

            xmlFree(type);
            xmlFree(name);
            xmlFree(file);
            xmlFree(def);

        } // installer

        if (std::string((char*)tag) == "randomGenerator") {
            xmlChar* seed = xmlTextReaderGetAttribute(reader, BAD_CAST "seed");
            if (seed == 0) {
                NS_FATAL_ERROR("Error getting attribute 'seed'");
            }

            SetSeed(atoi((char*)seed));

            xmlChar* runNumber = xmlTextReaderGetAttribute(reader, BAD_CAST "runNumber");
            if (runNumber == 0) {
                NS_FATAL_ERROR("Error getting attribute 'runNumber'");
            }

            SetRunNumber(atoi((char*)runNumber));
            xmlFree(seed);
            xmlFree(runNumber);
        } // randomGenerator


        if (std::string((char*)tag) == "KPIFilePrefix") {
            xmlChar* runID = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (runID == 0) {
                NS_FATAL_ERROR("Error getting attribute 'value' of element 'KPIFilePrefix'");
            }
            std::cout << " Parsed runID = " << (char*)runID << std::endl;
            nodeManager->SetKPIFilePrefix(std::string((char*)runID));
            xmlFree(runID);
        } // runID

        if (std::string((char*)tag) == "logKPIs") {
            xmlChar* value = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (value == 0) {
                NS_FATAL_ERROR("Error getting attribute 'value' of element 'logKPIs'");
            }
            std::string valueStr((char*)value);
            const bool logKPIs = (bool) atoi((char*)value);
            std::cout << " Parsed logKPIs = " << logKPIs << std::endl;
            nodeManager->SetKPILogging(logKPIs);
            xmlFree(value);
        }

        if (std::string((char*)tag) == "InitialXlogKPI") {
            xmlChar* cordIniX = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (cordIniX == 0) {
                NS_FATAL_ERROR("Error getting attribute 'value' of element 'InitialXlogKPI'");
            }
            nodeManager->SetInitialX(atoi((char*)cordIniX));
            xmlFree(cordIniX);
        }

        if (std::string((char*)tag) == "EndXlogKPI") {
            xmlChar* cordEndX = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (cordEndX == 0) {
                NS_FATAL_ERROR("Error getting attribute 'value' of element 'EndXlogKPI'");
            }
            nodeManager->SetEndX(atoi((char*)cordEndX));
            xmlFree(cordEndX);
        }

        if (std::string((char*)tag) == "InitialYlogKPI") {
            xmlChar* cordIniY = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (cordIniY == 0) {
                NS_FATAL_ERROR("Error getting attribute 'value' of element 'InitialXlogKPI'");
            }
            nodeManager->SetInitialY(atoi((char*)cordIniY));
            xmlFree(cordIniY);
        }

        if (std::string((char*)tag) == "EndYlogKPI") {
            xmlChar* cordEndY = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (cordEndY == 0) {
                NS_FATAL_ERROR("Error getting attribute 'value' of element 'EndYlogKPI'");
            }
            nodeManager->SetEndY(atoi((char*)cordEndY));
            xmlFree(cordEndY);
        }


        // enable comm result logging
        rc = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if (nodeManager->GetKPIFilePrefix() != "" && !nodeManager->KPILogOn()) {
        std::cerr << "ns3::ConfigurationManagerXml: Warning: Specified KPIFilePrefix but KPILog is off." << std::endl;
    }




}


} // namespace ns3
