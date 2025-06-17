/*
 * Copyright (c) 2012, Willow Garage, Inc.
 * Copyright (c) 2016, Delft Robotics B.V.
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

#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "class_loader/class_loader.hpp"
#include "class_loader/multi_library_class_loader.hpp"

#include "base.hpp"

const std::string LIBRARY_1 = class_loader::systemLibraryFormat("class_loader_TestPlugins1");  // NOLINT
const std::string LIBRARY_2 = class_loader::systemLibraryFormat("class_loader_TestPlugins2");  // NOLINT

using class_loader::ClassLoader;

TEST(ClassLoaderUniquePtrTest, basicLoad) {
  try {
    ClassLoader loader1(LIBRARY_1, false);
    loader1.createUniqueInstance<Base>("Cat")->saySomething();  // See if lazy load works
    ASSERT_NO_THROW(class_loader::impl::printDebugInfoToScreen());
    SUCCEED();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }
}

TEST(ClassLoaderUniquePtrTest, basicLoadFailures) {
  ClassLoader loader1(LIBRARY_1, false);
  ClassLoader loader2("", false);
  loader2.loadLibrary();
  EXPECT_THROW(
    class_loader::impl::loadLibrary("LIBRARY_1", &loader1),
    class_loader::LibraryLoadException);
  EXPECT_THROW(
    class_loader::impl::unloadLibrary("LIBRARY_1", &loader1),
    class_loader::LibraryUnloadException);
}

TEST(ClassLoaderUniquePtrTest, MultiLibraryClassLoaderFailures) {
  class_loader::MultiLibraryClassLoader loader(true);
  loader.loadLibrary(LIBRARY_1);
  EXPECT_THROW(loader.createUniqueInstance<Base>("Cat2"), class_loader::ClassLoaderException);
}

TEST(ClassLoaderUniquePtrTest, LibrariesUsedByClassLoader) {
  try {
    ClassLoader loader1(LIBRARY_1, false);
    std::vector<std::string> v = class_loader::impl::getAllLibrariesUsedByClassLoader(&loader1);
    ASSERT_EQ(v.size(), 1u);
    SUCCEED();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }
}

TEST(ClassLoaderUniquePtrTest, correctLazyLoadUnload) {
  try {
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ClassLoader loader1(LIBRARY_1, true);
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      ClassLoader::UniquePtr<Base> obj = loader1.createUniqueInstance<Base>("Cat");
      ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
      ASSERT_TRUE(loader1.isLibraryLoaded());
    }

    // The library will unload automatically when the only plugin object left is destroyed
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    return;
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  } catch (...) {
    FAIL() << "Unhandled exception";
  }
}

TEST(ClassLoaderUniquePtrTest, nonExistentPlugin) {
  ClassLoader loader1(LIBRARY_1, false);

  try {
    ClassLoader::UniquePtr<Base> obj = loader1.createUniqueInstance<Base>("Bear");
    if (nullptr == obj) {
      FAIL() << "Null object being returned instead of exception thrown.";
    }

    obj->saySomething();
  } catch (const class_loader::CreateClassException &) {
    SUCCEED();
    return;
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

void wait(int seconds)
{
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void run(ClassLoader * loader)
{
  std::vector<std::string> classes = loader->getAvailableClasses<Base>();
  for (auto & class_ : classes) {
    loader->createUniqueInstance<Base>(class_)->saySomething();
  }
}

TEST(ClassLoaderUniquePtrTest, threadSafety) {
  ClassLoader loader1(LIBRARY_1);
  ASSERT_TRUE(loader1.isLibraryLoaded());

  // Note: Hard to test thread safety to make sure memory isn't corrupted.
  // The hope is this test is hard enough that once in a while it'll segfault
  // or something if there's some implementation error.
  try {
    std::vector<std::thread> client_threads;

    for (size_t c = 0; c < STRESS_TEST_NUM_THREADS; c++) {
      client_threads.emplace_back(std::bind(&run, &loader1));
    }

    for (auto & client_thread : client_threads) {
      client_thread.join();
    }

    loader1.unloadLibrary();
    ASSERT_FALSE(loader1.isLibraryLoaded());
  } catch (const class_loader::ClassLoaderException &) {
    FAIL() << "Unexpected ClassLoaderException.";
  } catch (...) {
    FAIL() << "Unknown exception.";
  }
}

TEST(ClassLoaderUniquePtrTest, loadRefCountingLazy) {
  try {
    ClassLoader loader1(LIBRARY_1, true);
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      ClassLoader::UniquePtr<Base> obj = loader1.createUniqueInstance<Base>("Dog");
      ASSERT_TRUE(loader1.isLibraryLoaded());
    }

    ASSERT_FALSE(loader1.isLibraryLoaded());

    loader1.loadLibrary();
    ASSERT_TRUE(loader1.isLibraryLoaded());

    loader1.loadLibrary();
    ASSERT_TRUE(loader1.isLibraryLoaded());

    loader1.unloadLibrary();
    ASSERT_TRUE(loader1.isLibraryLoaded());

    loader1.unloadLibrary();
    ASSERT_FALSE(loader1.isLibraryLoaded());

    loader1.unloadLibrary();
    ASSERT_FALSE(loader1.isLibraryLoaded());

    loader1.loadLibrary();
    ASSERT_TRUE(loader1.isLibraryLoaded());

    return;
  } catch (const class_loader::ClassLoaderException &) {
    FAIL() << "Unexpected exception.\n";
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

void testMultiClassLoader(bool lazy)
{
  try {
    class_loader::MultiLibraryClassLoader loader(lazy);
    loader.loadLibrary(LIBRARY_1);
    loader.loadLibrary(LIBRARY_2);
    for (int i = 0; i < 2; ++i) {
      loader.createUniqueInstance<Base>("Cat")->saySomething();
      loader.createUniqueInstance<Base>("Dog")->saySomething();
      loader.createUniqueInstance<Base>("Robot")->saySomething();
    }
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

TEST(MultiClassLoaderUniquePtrTest, lazyLoad) {
  testMultiClassLoader(true);
}
TEST(MultiClassLoaderUniquePtrTest, lazyLoadSecondTime) {
  testMultiClassLoader(true);
}
TEST(MultiClassLoaderUniquePtrTest, nonLazyLoad) {
  testMultiClassLoader(false);
}
TEST(MultiClassLoaderUniquePtrTest, noWarningOnLazyLoad) {
  try {
    ClassLoader::UniquePtr<Base> cat = nullptr, dog = nullptr, rob = nullptr;
    {
      class_loader::MultiLibraryClassLoader loader(true);
      loader.loadLibrary(LIBRARY_1);
      loader.loadLibrary(LIBRARY_2);

      cat = loader.createUniqueInstance<Base>("Cat");
      dog = loader.createUniqueInstance<Base>("Dog");
      rob = loader.createUniqueInstance<Base>("Robot");
    }
    cat->saySomething();
    dog->saySomething();
    rob->saySomething();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

// Run all the tests that were declared with TEST()
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
