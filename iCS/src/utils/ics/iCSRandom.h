/****************************************************************************/
/// @file    iCSRandom.h
/// @author  Pasquale Cataldi
/// @date    Tue, 06.05.2010
/// @version $Id: iCSRandom.h $
///
//
/****************************************************************************/
// iTETRIS, see www.ict-itetris.eu/
// Copyright
/****************************************************************************/
//
//   This program is based on RandHelper.h implemented in SUMO
//   see http://sumo.sourceforge.net/
//
/****************************************************************************/
#ifndef iCSRandom_h
#define iCSRandom_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <random>


namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class iCSRandom
 * @brief Utility functions for using a global, resetable random number generator
 */
class iCSRandom {
public:

    /// Initializes the random object with a given seed
    static void seed(const unsigned long oneSeed);

    /// Returns a random real number in [0, 1)
    static inline float rand() {
        return (float) myRandomRealDistribution(myRandomNumberGenerator);
    }

    /// Returns a random real number in [0, maxV)
    static inline float rand(float maxV) {
        return maxV * rand();
    }

    /// Returns a random real number in [minV, maxV)
    static inline float rand(float minV, float maxV) {
        return minV + (maxV - minV) * rand();
    }

    /// Returns a random integer in [0, maxV-1]
    static inline size_t rand(size_t maxV) {
        std::uniform_int_distribution<size_t> dist(0, maxV - 1);
        return dist(myRandomNumberGenerator);
    }

    /// Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV) {
        std::uniform_int_distribution<int> dist(0, maxV - 1);
        return dist(myRandomNumberGenerator);
    }

    /// Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV) {
        return minV + rand(maxV - minV);
    }

    /// Access to a random number from a normal distribution
    static inline float randNorm(float mean, float stddev) {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(myRandomNumberGenerator);
    }

    /// Returns a random element from the given vector
    template<class T>
    static inline T
    getRandomFrom(const std::vector<T> &v) {
        return v[rand(v.size())];
    }

protected:
    /// the random number generator to use
    static std::default_random_engine myRandomNumberGenerator;
    static std::uniform_real_distribution<float> myRandomRealDistribution;

};

}

#endif

/****************************************************************************/

