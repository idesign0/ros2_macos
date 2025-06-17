/*
 * Copyright (c) 2012, Willow Garage, Inc.
 * All rights reserved.
 * Copyright (c) 2020, Open Source Robotics Foundation, Inc.
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
 *
 * 2020, Copied from plugins1.cpp and plugins2.cpp and changing class names
 */

#include <iostream>

#include "class_loader/class_loader.hpp"

#include "./base.hpp"

class Kangaroo : public Base
{
public:
  void saySomething()
  {
    printf("[Angry growl]\n");
  }
};

class Panda : public Base
{
public:
  void saySomething()
  {
    printf("[Excited squeaks!!!]\n");
  }
};

class Hyena : public Base
{
public:
  void saySomething()
  {
    printf("[Cackling laugh]\n");
  }
};

class Alpaca : public Base
{
public:
  void saySomething()
  {
    printf("hhhaaaaaaaaaa\n");
  }
};


CLASS_LOADER_REGISTER_CLASS(Kangaroo, Base)
CLASS_LOADER_REGISTER_CLASS(Panda, Base)
CLASS_LOADER_REGISTER_CLASS(Hyena, Base)
CLASS_LOADER_REGISTER_CLASS(Alpaca, Base)
