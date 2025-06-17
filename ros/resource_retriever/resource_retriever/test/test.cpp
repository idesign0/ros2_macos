// Copyright 2008, Willow Garage, Inc. All rights reserved.
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
//    * Neither the name of the  Willow Garage, Inc. nor the names of its
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

#include <string>
#include <exception>

#include "gtest/gtest.h"

#include "resource_retriever/retriever.hpp"

TEST(Retriever, getByPackage)
{
  try {
    resource_retriever::Retriever r;
    resource_retriever::MemoryResource res = r.get("package://resource_retriever/test/test.txt");

    ASSERT_EQ(res.size, 1u);
    ASSERT_EQ(*res.data, 'A');
  } catch (const std::exception & e) {
    FAIL() << "Exception caught: " << e.what() << '\n';
  }
}

TEST(Retriever, http)
{
  try {
    resource_retriever::Retriever r;
    resource_retriever::MemoryResource res = r.get("http://packages.ros.org/ros.key");

    ASSERT_GT(res.size, 0u);
  } catch (const std::exception & e) {
    FAIL() << "Exception caught: " << e.what() << '\n';
  }
}

TEST(Retriever, invalidFiles)
{
  resource_retriever::Retriever r;

  try {
    r.get("file://fail");
    FAIL();
  } catch (const std::exception & e) {
    (void)e;
  }

  try {
    r.get("package://roscpp");
    FAIL();
  } catch (const std::exception & e) {
    (void)e;
  }

  try {
    r.get("package://invalid_package_blah/test.xml");
    FAIL();
  } catch (const std::exception & e) {
    (void)e;
  }

  // Empty package name
  try {
    r.get("package:///test.xml");
    FAIL();
  } catch (const std::exception & e) {
    (void)e;
  }
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
