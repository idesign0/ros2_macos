// Copyright (c) 2022, Open Source Robotics Foundation, Inc.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef BONDCPP__VISIBILITY_CONTROL_HPP_
#define BONDCPP__VISIBILITY_CONTROL_HPP_

/*! \file visibility_control.hpp
  * \brief Macros for controlling visibilty of exported iterfaces.
  *
  * This logic was borrowed (then namespaced) from the examples on the gcc wiki:
  *     https://gcc.gnu.org/wiki/Visibility
  */
/**
  * \def BONDCPP_EXPORT
  * \brief Exposes the function with its decorated name in the compiled library object.
  */
/**
  * \def BONDCPP_IMPORT
  * \brief On Windows declares a function will be imported from a dll, otherwise it is empty
  */
/**
  * \def BONDCPP_PUBLIC
  * \brief Declares symbols and functions will be visible for export.
  */
/**
  * \def BONDCPP_PUBLIC_TYPE
  * \brief On Windows, this is a replica of BONDCPP_PUBLIC, otherwise it is empty.
  */
/**
  * \def BONDCPP_LOCAL
  * \brief Declares symbols cannot be exported from the dll.
  */

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define BONDCPP_EXPORT __attribute__ ((dllexport))
    #define BONDCPP_IMPORT __attribute__ ((dllimport))
  #else
    #define BONDCPP_EXPORT __declspec(dllexport)
    #define BONDCPP_IMPORT __declspec(dllimport)
  #endif
  #ifdef BONDCPP_BUILDING_LIBRARY
    #define BONDCPP_PUBLIC BONDCPP_EXPORT
  #else
    #define BONDCPP_PUBLIC BONDCPP_IMPORT
  #endif
  #define BONDCPP_PUBLIC_TYPE BONDCPP_PUBLIC
  #define BONDCPP_LOCAL
#else
  #define BONDCPP_EXPORT __attribute__ ((visibility("default")))
  #define BONDCPP_IMPORT
  #if __GNUC__ >= 4
    #define BONDCPP_PUBLIC __attribute__ ((visibility("default")))
    #define BONDCPP_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define BONDCPP_PUBLIC
    #define BONDCPP_LOCAL
  #endif
  #define BONDCPP_PUBLIC_TYPE
#endif

#endif  // BONDCPP__VISIBILITY_CONTROL_HPP_
