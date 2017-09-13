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
#include "AnkanTest.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
void AnkanTest::onRun() {
  const struct {
    const char* hais;
    const char* kanables;
  } TABLES[] = {
    { "1m2m3m4p5p6p7s8s1z1z1z2z2z", "1z" }, 
    { "2m2m2m3m3m4m4m4m5p6p7p2s2s", "" }, 
    { "2m2m2m3m3m5m5m5m5p6p7p2s2s", "2m5m" }, 
    { "1m2m7m7m2p3p4p4p4p4p1s2s3s", "4p" }, 
    { "1m1m1m2m3m4m4m4m1p2p3p1s2s", "" }, 
    { "1m1m1m3m4m4m5m6m7m8m9m9m9m", "1m" }, 
    { "2m2m2m3m4m4m5m5m6m6m9m9m9m", "9m" }, 
    { "4m6m6m8m8m6p6p8p8p6s6s4z4z", "" }
  };
  for(auto& table : TABLES) {
    shanten_.update(table.hais, true);
    TEST_MESSAGE(shanten_.getKanables().toString() == table.kanables, 
                 Format("%s => %s(%s)", table.hais, table.kanables, 
                        shanten_.getKanables().toString().c_str()));
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
