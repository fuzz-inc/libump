/*
Copyright 2016 fuzz, Inc. All rights reserved. 
   http://www.fuzz.co.jp

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of fuzz, Inc. nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUZZ, INC. BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/***********************************************************************//**
	@file
***************************************************************************/
#include "Test.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
Test::Test()
  : testNum_(0), 
    failNum_(0)
{
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Test::run() {
  onRun();
  std::cout << "test failed " << failNum_ << "/" << testNum_ << std::endl;
  return failNum_ == 0;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::string Test::Format(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char buff[256];
  vsnprintf(buff, sizeof(buff), format, args);
  va_end(args);
  return std::string(buff);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Test::run(Test& test, const std::string& message) {
  std::cout << "<< " << message << std::endl;
  if(!test.run()) {
    failNum_++;
  }
  testNum_++;
  std::cout << ">> " << message << std::endl;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Test::test(bool value, const std::string& message) {
  std::cout <<  (value ? "[OK]" : "[NG]") << message << std::endl;
  testNum_++;
  if(!value) {
    failNum_++;
    //assert(0);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
