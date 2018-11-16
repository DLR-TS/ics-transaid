/****************************************************************************/
/// @file    RandHelper.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id: RandHelper.h 13811 2013-05-01 20:31:43Z behrisch $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RandHelper_h
#define RandHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <vector>
#include <random>

// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RandHelper
 * @brief Utility functions for using a global, resetable random number generator
 */
class RandHelper {
public:
    /// @brief Initialises the given options container with random number options
    static void insertRandOptions();

    /// @brief Reads the given random number options and initialises the random number generator in accordance
    static void initRandGlobal(std::default_random_engine* which = nullptr);

    /// @brief Returns a random real number in [0, 1)
    static inline SUMOReal rand() {
        return (SUMOReal) myRandomRealDistribution(myRandomNumberGenerator);
    }

    /// @brief Returns a random real number in [0, maxV)
    static inline SUMOReal rand(SUMOReal maxV) {
        return maxV * rand();
    }

    /// @brief Returns a random real number in [minV, maxV)
    static inline SUMOReal rand(SUMOReal minV, SUMOReal maxV) {
        return minV + (maxV - minV) * rand();
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline size_t rand(size_t maxV) {
        std::uniform_int_distribution<size_t> dist(0, maxV - 1);
        return dist(myRandomNumberGenerator);
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV) {
        std::uniform_int_distribution<int> dist(0, maxV - 1);
        return dist(myRandomNumberGenerator);
    }

    /// @brief Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV) {
        return minV + rand(maxV - minV);
    }

    /// @brief Access to a random number from a normal distribution
    static inline SUMOReal randNorm(SUMOReal mean, SUMOReal stddev, std::default_random_engine& rng = myRandomNumberGenerator) {
        std::normal_distribution<SUMOReal> dist(mean, stddev);
        return dist(rng);
    }

    /// @brief Returns a random element from the given vector
    template<class T>
    static inline T
    getRandomFrom(const std::vector<T>& v) {
        assert(v.size() > 0);
        return v[rand(v.size())];
    }

    /// @brief seeds the defautl random generator
    static inline void seedDefaultRNG(unsigned long s) {
        myRandomNumberGenerator.seed(s);
    }

protected:
    /// @brief the random number generator to use
    static std::default_random_engine myRandomNumberGenerator;
    static std::uniform_real_distribution<SUMOReal> myRandomRealDistribution;

};

#endif

/****************************************************************************/

