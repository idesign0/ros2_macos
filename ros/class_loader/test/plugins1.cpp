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

#include <iostream>

#include "class_loader/class_loader.hpp"

#include "./base.hpp"

class Dog : public Base
{
public:
  void saySomething()
  {
    printf("Bark\n");
  }
};

class Cat : public Base
{
public:
  void saySomething()
  {
    printf("Meow\n");
  }
};

class Duck : public Base
{
public:
  void saySomething()
  {
    printf("Quack\n");
  }
};

class Cow : public Base
{
public:
  void saySomething()
  {
    printf("Moooo\n");
  }
};

class Sheep : public Base
{
public:
  void saySomething()
  {
    printf("Baaah\n");
  }
};

CLASS_LOADER_REGISTER_CLASS(Dog, Base)
CLASS_LOADER_REGISTER_CLASS(Cat, Base)
CLASS_LOADER_REGISTER_CLASS(Duck, Base)
CLASS_LOADER_REGISTER_CLASS(Cow, Base)
CLASS_LOADER_REGISTER_CLASS(Sheep, Base)
