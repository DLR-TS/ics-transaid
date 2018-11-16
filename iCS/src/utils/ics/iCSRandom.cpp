/****************************************************************************/
/// @file    iCSRandom.cpp
/// @author  Pasquale Cataldi
/// @date    Tue, 06.05.2010
/// @version $Id: RandHelper.cpp $
///
//
/****************************************************************************/
// iTETRIS, see www.ict-itetris.eu/
// Copyright
/****************************************************************************/
//
//   This program is based on RandHelper.cpp implemented in SUMO
//   see http://sumo.sourceforge.net/
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "iCSRandom.h"
#include <ctime>
#include <cmath>

namespace ics
{

// ===========================================================================
// static member variables
// ===========================================================================
std::default_random_engine  iCSRandom::myRandomNumberGenerator;
std::uniform_real_distribution<float> iCSRandom::myRandomRealDistribution(0,1);

void iCSRandom::seed(const unsigned long oneSeed) {
       iCSRandom::myRandomNumberGenerator.seed(oneSeed);
}

}
