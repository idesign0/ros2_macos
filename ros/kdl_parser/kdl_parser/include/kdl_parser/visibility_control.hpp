/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2017, Open Source Robotics Foundation, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the copyright holder nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* This header must be included by all kdl_parser headers which declare symbols
 * which are defined in the kdl_parser library. When not building the kdl_parser
 * library, i.e. when using the headers in other package's code, the contents
 * of this header change the visibility of certain symbols which the kdl_parser
 * library cannot have, but the consuming code must have inorder to link.
 */

#ifndef KDL_PARSER__VISIBILITY_CONTROL_HPP_
#define KDL_PARSER__VISIBILITY_CONTROL_HPP_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define KDL_PARSER_EXPORT __attribute__ ((dllexport))
    #define KDL_PARSER_IMPORT __attribute__ ((dllimport))
  #else
    #define KDL_PARSER_EXPORT __declspec(dllexport)
    #define KDL_PARSER_IMPORT __declspec(dllimport)
  #endif
  #ifdef KDL_PARSER_BUILDING_DLL
    #define KDL_PARSER_PUBLIC KDL_PARSER_EXPORT
  #else
    #define KDL_PARSER_PUBLIC KDL_PARSER_IMPORT
  #endif
  #define KDL_PARSER_PUBLIC_TYPE KDL_PARSER_PUBLIC
  #define KDL_PARSER_LOCAL
#else
  #define KDL_PARSER_EXPORT __attribute__ ((visibility("default")))
  #define KDL_PARSER_IMPORT
  #if __GNUC__ >= 4
    #define KDL_PARSER_PUBLIC __attribute__ ((visibility("default")))
    #define KDL_PARSER_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define KDL_PARSER_PUBLIC
    #define KDL_PARSER_LOCAL
  #endif
  #define KDL_PARSER_PUBLIC_TYPE
#endif

#endif  // KDL_PARSER__VISIBILITY_CONTROL_HPP_
