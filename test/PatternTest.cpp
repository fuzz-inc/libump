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
#include "PatternTest.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
void PatternTest::onRun() {
  const struct {
    const char* hais;
    const char* hai;
    const char* pattern;
  } TABLES[] = {
    { "4m5m5m5M6m7m8m", "6m", "4m5m|4m5M|5m7m|5M7m|7m8m" }
  };
  for(auto& table : TABLES) {
    ump::mj::Hand hand;
    hand.drawHaipai(ump::mj::HaiArray(table.hais));
    auto pattern = hand.getChiPattern(ump::mj::Hai::Get(table.hai));
    TEST_MESSAGE(pattern.toString() == table.pattern, 
                 Format("%s %s => %s(%s)", table.hais, table.hai, 
                        pattern.toString().c_str(), 
                        table.pattern));
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
