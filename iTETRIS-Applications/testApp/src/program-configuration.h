/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#ifndef PROGRAM_CONFIGURATION_H_
#define PROGRAM_CONFIGURATION_H_

#include <map>
#include <string>
#include "structs.h"
#include "vector.h"

namespace tinyxml2
{
class XMLElement;
}

namespace testapp
{

enum
{
  LOG_FILE = 0, DATA_FILE = 2, NS_LOG_FILE = 1
} typedef LogType;


enum
{
    TEST_CASE_NONE = 0, TEST_CASE_SETVTYPE, TEST_CASE_EXECUTE, TEST_CASE_ACOSTA, TEST_CASE_INDUCTIONLOOP
} typedef TestCase;


struct TLLane
{
  double dir;
  std::string controlledLane;
  std::string followingLane;
  std::string friendlyName;
};

struct Direction
{
  Direction()
  {
    leaving = false;
    approaching = true;
    direction = INVALID_DIRECTION;
    approachingTime = leavingTime = 0;
  }
  double direction;
  bool approaching;
  bool leaving;
  uint16_t approachingTime;
  uint16_t leavingTime;
};

struct RsuData
{
  int id;
  application::Vector2D position;
  std::vector<Direction> directions;
  std::vector<TLLane> lanes;
  Circle cam_area;
  Circle car_area;
}typedef RsuData;

class ProgramConfiguration
{
public:
  static int LoadConfiguration(const char* fileName);

  static int GetStartTime()
  {
    return m_start;
  }
  static int GetSocketPort()
  {
    return m_socket;
  }
  static unsigned GetMessageLifetime()
  {
    return m_messageLifetime;
  }

  static TestCase GetTestCase()
  {
    return m_testCase;
  }

  static void SetTestCase(TestCase tc)
  {
    m_testCase = tc;
  }

  static bool GetLogFileName(LogType type, std::string & fileName);
  static bool IsRsu(const int id);
  static const RsuData & GetRsuData(const int id);
private:
  ProgramConfiguration();
  ~ProgramConfiguration();
  static int ParseGeneral(tinyxml2::XMLElement * general);
  static int ParseInfrastructure(tinyxml2::XMLElement * infrastructure);
  static int ParseSetup(tinyxml2::XMLElement * setup);
  static int ParseOutput(tinyxml2::XMLElement * output);
  static int ParseNodeSampler(tinyxml2::XMLElement * nodeSampler);
  static void ParseLog(const tinyxml2::XMLElement * element,const LogType type);
private:
  static int m_start;
  static int m_socket;
  static unsigned m_messageLifetime;
  static TestCase m_testCase;
  static std::map<int, RsuData> m_rsus;
  static std::map<LogType, std::string> m_logs;
};

} /* namespace protocol */

#endif /* PROGRAM_CONFIGURATION_H_ */
