/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
/*
 * Copyright (c) 2006 INRIA, 2010 NICTA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *         Quincy Tse <quincy.tse@nicta.com.au>
 */
#ifndef NS3_FATAL_ERROR_H
#define NS3_FATAL_ERROR_H

#include <iostream>
#include <exception>
#include <cstdlib>

/**
 * \ingroup debugging
 * \brief fatal error handling
 *
 * When this macro is hit at runtime, details of filename
 * and line number is printed to stderr, and the program
 * is halted by calling std::terminate(). This will
 * trigger any clean up code registered by
 * std::set_terminate (NS3 default is a stream-flushing
 * code), but may be overridden.
 *
 * This macro is enabled unconditionally in all builds,
 * including debug and optimized builds.
 */
#define NS_FATAL_ERROR_NO_MSG()                           \
  do                                                      \
    {                                                     \
      std::cerr << "file=" << __FILE__ << ", line=" <<    \
      __LINE__ << std::endl;                              \
      std::terminate ();                                  \
    }                                                     \
  while (false)

/**
 * \ingroup debugging
 * \brief fatal error handling
 *
 * \param msg message to output when this macro is hit.
 *
 * When this macro is hit at runtime, the user-specified
 * error message is printed to stderr, followed by a call
 * to the NS_FATAL_ERROR_NO_MSG() macro which prints the
 * details of filename and line number to stderr. The
 * program will be halted by calling std::terminate(),
 * triggering any clean up code registered by
 * std::set_terminate (NS3 default is a stream-flushing
 * code, but may be overridden).
 *
 * This macro is enabled unconditionally in all builds,
 * including debug and optimized builds.
 */
#define NS_FATAL_ERROR(msg)                             \
  do                                                    \
    {                                                   \
      std::cerr << "msg=\"" << msg << "\", ";           \
      NS_FATAL_ERROR_NO_MSG ();                         \
    }                                                   \
  while (false)

/**
 * \ingroup assert
 * \param condition condition to verify.
 *
 * At runtime, in debugging builds, if this condition is not
 * true, the program prints the source file, line number and
 * unverified condition and halts by calling std::terminate
 */
#define NS_ASSERT(condition)                                    \
  do                                                            \
    {                                                           \
      if (!(condition))                                         \
        {                                                       \
          std::cerr << "assert failed. cond=\"" <<              \
          # condition << "\", ";                                \
          NS_FATAL_ERROR_NO_MSG ();                             \
        }                                                       \
    }                                                           \
  while (false)

#endif /* FATAL_ERROR_H */
