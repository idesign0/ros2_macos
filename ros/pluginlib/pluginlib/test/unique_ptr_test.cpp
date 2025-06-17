/*
 * Copyright (c) 2012, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <gtest/gtest.h>

#include <pluginlib/class_loader.hpp>

#include <test_base.h>

TEST(PluginlibUniquePtrTest, unknownPlugin) {
  pluginlib::ClassLoader<test_base::Fubar> test_loader("test_pluginlib", "test_base::Fubar");
  ASSERT_THROW(test_loader.createUniqueInstance("test_pluginlib/foobar"),
    pluginlib::LibraryLoadException);
}


TEST(PluginlibUniquePtrTest, misspelledPlugin) {
  pluginlib::ClassLoader<test_base::Fubar> bad_test_loader("test_pluginlib", "test_base::Fuba");
  ASSERT_THROW(bad_test_loader.createUniqueInstance(
      "test_pluginlib/foo"), pluginlib::LibraryLoadException);
}

TEST(PluginlibTest, brokenPlugin) {
  pluginlib::ClassLoader<test_base::Fubar> test_loader("test_pluginlib", "test_base::Fubar");
  ASSERT_THROW(test_loader.createUniqueInstance("test_pluginlib/none"), pluginlib::PluginlibException);
}

TEST(PluginlibUniquePtrTest, workingPlugin) {
  pluginlib::ClassLoader<test_base::Fubar> test_loader("test_pluginlib", "test_base::Fubar");

  try {
    pluginlib::UniquePtr<test_base::Fubar> foo = test_loader.createUniqueInstance("test_pluginlib/foo");
    foo->initialize(10.0);
    EXPECT_EQ(100.0, foo->result());
  } catch (pluginlib::PluginlibException & ex) {
    FAIL() << "Throwing exception: " << ex.what();
    return;
  } catch (...) {
    FAIL() << "Uncaught exception";
  }
}

TEST(PluginlibUniquePtrTest, createUniqueInstanceAndUnloadLibrary) {
  RCUTILS_LOG_INFO("Making the ClassLoader...");
  pluginlib::ClassLoader<test_base::Fubar> pl("test_pluginlib", "test_base::Fubar");

  RCUTILS_LOG_INFO("Instantiating plugin...");
  {
    pluginlib::UniquePtr<test_base::Fubar> inst = pl.createUniqueInstance("test_pluginlib/foo");
  }

  RCUTILS_LOG_INFO("Checking if plugin is loaded with isClassLoaded...");
  if (pl.isClassLoaded("test_pluginlib/foo")) {
    RCUTILS_LOG_INFO("Class is loaded");
  } else {
    FAIL() << "Library containing class should be loaded but isn't.";
  }

  RCUTILS_LOG_INFO("Trying to unload class with unloadLibraryForClass...");
  try {
    pl.unloadLibraryForClass("test_pluginlib/foo");
  } catch (pluginlib::PluginlibException & e) {
    FAIL() << "Could not unload library when I should be able to. " << e.what();
  }
  RCUTILS_LOG_INFO("Done.");
}

// Run all the tests that were declared with TEST()
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
