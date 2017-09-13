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
#include "ShantenTest.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
void ShantenTest::onRun() {
  const struct {
    const char* hais;
    int shanten;
    const char* richi;
  } TABLES[] = {
    { "4m5m8m6p6p7p1s1s6s6s8s8s5z5z", 1, nullptr }, 
    { "4m5m8m6p6p6p1s1s6s6s8s8s5z5z", 1, nullptr }, 
    { "4m4m6p6p6p6p1s1s6s6s8s8s5z5z", 1, nullptr }, 

    { "1m5m9m1p9p1s9s1z2z3z4z5z6z6z", 0, nullptr }, 
    { "1m5m9m1p9p1s9s1z2z3z5z5z6z6z", 1, nullptr }, 

    { "1m2m4m5m5p6p7p1s2s3s5s6s8s9s", 2, nullptr }, 
    { "1m2m4m5m5p6p7p1s2s3s5s6s8s8s", 1, nullptr }, 
    { "1m2m3m5m6m7m5p5p3s4s5s7s9s5z", 0, nullptr }, 

    { "3m4m5m1p2p4s5s6s6s6s7s7s8s8s", 0, "1p2p" }, 
    { "8m3p4p4p5p5p5p6p6p7p4s6s7s8s", 0, "8m4s" },
    { "2m3m4m6m7m8m4p5p6p3z3z3z6z7z", 0, "6z7z" }, 

    { "6m6m7m7m7m8m8m9m9m2s2s4s4s7s", 0, "7m" }, 
    { "1m1m1m5m5m3p3p8p8p9s9s6z6z6z", 1, nullptr }
  };
  for(auto& table : TABLES) {
    shanten_.update(table.hais, true);
    if(table.richi) {
      TEST_MESSAGE(shanten_.getShanten() == 0 && 
                   shanten_.getRichi().toString() == table.richi, 
                   Format("%s => %s(%s)", table.hais, table.richi, 
                          shanten_.getRichi().toString().c_str()));
    }
    else {
      TEST_MESSAGE(shanten_.getShanten() == table.shanten, 
                   Format("%s => %d", table.hais, table.shanten));
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
