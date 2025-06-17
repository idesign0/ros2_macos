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

#include <chrono>
#include <cstddef>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "class_loader/class_loader.hpp"
#include "class_loader/multi_library_class_loader.hpp"

#include "gtest/gtest.h"

#include "./base.hpp"

const std::string LIBRARY_1 = class_loader::systemLibraryFormat("class_loader_TestPlugins1");  // NOLINT
const std::string LIBRARY_2 = class_loader::systemLibraryFormat("class_loader_TestPlugins2");  // NOLINT

// These are loaded with dlopen() and RTLD_GLOBAL in loadUnloadLoadFromGraveyard and may cause
// unexpected side-effects if used elsewhere
const std::string GLOBAL_PLUGINS =  // NOLINT
  class_loader::systemLibraryFormat("class_loader_TestGlobalPlugins");

TEST(ClassLoaderTest, basicLoad) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    ASSERT_NO_THROW(class_loader::impl::printDebugInfoToScreen());
    loader1.createInstance<Base>("Cat")->saySomething();  // See if lazy load works
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

// Requires separate namespace so static variables are isolated
TEST(ClassLoaderUnmanagedTest, basicLoadUnmanaged) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    Base * unmanaged_instance = loader1.createUnmanagedInstance<Base>("Dog");
    ASSERT_NE(unmanaged_instance, nullptr);
    unmanaged_instance->saySomething();
    delete unmanaged_instance;
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

TEST(ClassLoaderUniquePtrTest, basicLoadFailures) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  class_loader::ClassLoader loader2("", false);
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

TEST(ClassLoaderTest, correctNonLazyLoadUnload) {
  try {
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_TRUE(loader1.isLibraryLoaded());
    loader1.unloadLibrary();
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_FALSE(loader1.isLibraryLoaded());
    return;
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  } catch (...) {
    FAIL() << "Unhandled exception";
  }
}

TEST(ClassLoaderTest, correctLazyLoadUnload) {
  try {
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    class_loader::ClassLoader loader1(LIBRARY_1, true);
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      std::shared_ptr<Base> obj = loader1.createInstance<Base>("Cat");
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

TEST(ClassLoaderTest, nonExistentPlugin) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);

  try {
    std::shared_ptr<Base> obj = loader1.createInstance<Base>("Bear");
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

TEST(ClassLoaderTest, nonExistentLibrary) {
  try {
    class_loader::ClassLoader loader1("libDoesNotExist.so", false);
  } catch (const class_loader::LibraryLoadException &) {
    SUCCEED();
    return;
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

class InvalidBase
{
};

TEST(ClassLoaderTest, invalidBase) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    if (loader1.isClassAvailable<InvalidBase>("Cat")) {
      FAIL() << "Cat should not be available for InvalidBase";
    } else if (loader1.isClassAvailable<Base>("Cat")) {
      SUCCEED();
      return;
    } else {
      FAIL() << "Class not available for correct base class.";
    }
  } catch (const class_loader::LibraryLoadException &) {
    FAIL() << "Unexpected exception";
  } catch (...) {
    FAIL() << "Unexpected and unknown exception caught.\n";
  }
}

void wait(int seconds)
{
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void run(class_loader::ClassLoader * loader)
{
  std::vector<std::string> classes = loader->getAvailableClasses<Base>();
  for (auto & class_ : classes) {
    loader->createInstance<Base>(class_)->saySomething();
  }
}

TEST(ClassLoaderTest, threadSafety) {
  class_loader::ClassLoader loader1(LIBRARY_1);
  ASSERT_TRUE(loader1.isLibraryLoaded());

  // Note: Hard to test thread safety to make sure memory isn't corrupted.
  // The hope is this test is hard enough that once in a while it'll segfault
  // or something if there's some implementation error.
  try {
    std::vector<std::thread *> client_threads;

    for (size_t c = 0; c < STRESS_TEST_NUM_THREADS; ++c) {
      client_threads.push_back(new std::thread(std::bind(&run, &loader1)));
    }

    for (auto & client_thread : client_threads) {
      client_thread->join();
    }

    for (auto & client_thread : client_threads) {
      delete (client_thread);
    }

    loader1.unloadLibrary();
    ASSERT_FALSE(loader1.isLibraryLoaded());
  } catch (const class_loader::ClassLoaderException &) {
    FAIL() << "Unexpected ClassLoaderException.";
  } catch (...) {
    FAIL() << "Unknown exception.";
  }
}

TEST(ClassLoaderTest, loadRefCountingNonLazy) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    ASSERT_TRUE(loader1.isLibraryLoaded());

    loader1.loadLibrary();
    loader1.loadLibrary();
    ASSERT_TRUE(loader1.isLibraryLoaded());

    loader1.unloadLibrary();
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

TEST(ClassLoaderTest, loadRefCountingLazy) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, true);
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      std::shared_ptr<Base> obj = loader1.createInstance<Base>("Dog");
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
      loader.createInstance<Base>("Cat")->saySomething();
      loader.createInstance<Base>("Dog")->saySomething();
      loader.createInstance<Base>("Robot")->saySomething();
    }
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

TEST(MultiClassLoaderTest, lazyLoad) {
  testMultiClassLoader(true);
}
TEST(MultiClassLoaderTest, lazyLoadSecondTime) {
  testMultiClassLoader(true);
}
TEST(MultiClassLoaderTest, nonLazyLoad) {
  testMultiClassLoader(false);
}
TEST(MultiClassLoaderTest, noWarningOnLazyLoad) {
  try {
    std::shared_ptr<Base> cat, dog, rob;
    {
      class_loader::MultiLibraryClassLoader loader(true);
      loader.loadLibrary(LIBRARY_1);
      loader.loadLibrary(LIBRARY_2);

      cat = loader.createInstance<Base>("Cat");
      dog = loader.createInstance<Base>("Dog");
      rob = loader.createInstance<Base>("Robot");
    }
    cat->saySomething();
    dog->saySomething();
    rob->saySomething();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

#ifndef _WIN32
// Not run on Windows because this tests dlopen-specific behavior

// This is a different class name so that static variables in ClassLoader are isolated
TEST(ClassLoaderGraveyardTest, loadUnloadLoadFromGraveyard) {
  // This first load/unload adds the plugin to the graveyard
  try {
    class_loader::ClassLoader loader(GLOBAL_PLUGINS, false);
    loader.createInstance<Base>("Kangaroo")->saySomething();
    loader.unloadLibrary();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  // Not all platforms use RTLD_GLOBAL as a default, and rcutils doesn't explicitly choose either.
  // In order to invoke graveyard behavior, this needs to be loaded first for global relocation.

  void * handle = dlopen(GLOBAL_PLUGINS.c_str(), RTLD_NOW | RTLD_GLOBAL);
  ASSERT_NE(handle, nullptr);

  // This load will cause system to use globally relocatable library.
  // For testing purposes, this will cause ClassLoader to revive the library from the graveyard.
  try {
    class_loader::ClassLoader loader(GLOBAL_PLUGINS, false);
    loader.createInstance<Base>("Panda")->saySomething();
    loader.unloadLibrary();

    loader.loadLibrary();
    loader.createInstance<Base>("Hyena")->saySomething();
    loader.unloadLibrary();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  dlclose(handle);
  // With all libraries closed, this should act like a normal load/unload.
  try {
    class_loader::ClassLoader loader(GLOBAL_PLUGINS, false);
    loader.createInstance<Base>("Alpaca")->saySomething();
    loader.unloadLibrary();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }
}

#endif  // ifndef _WIN32

// Run all the tests that were declared with TEST()
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
