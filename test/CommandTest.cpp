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
#include "CommandTest.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
void CommandTest::onRun() {
  TEST(ump::Command::Encode("タンヤオ") == 
       "%E3%82%BF%E3%83%B3%E3%83%A4%E3%82%AA");
  {
    ump::Command command;
    TEST(command.parse("1 naki? 7m chi"));
    TEST(command.hasArg(ump::Command::TYPE_CHI));
  }
  /*
  {
    ump::Command command(ump::Command::TYPE_AGARI);
    command.append(ump::mj::Agari::GetYakuName(ump::mj::Agari::YAKU_TANYAO));
    command.append(1);
    printf("%s\n", command.toString(false).c_str());
  }
  {
    ump::Command command;
    command.parse("77 agari A0 0 %E5%B9%B3%E5%92%8C 1 %E9%96%80%E5%89%8D%E8%87%AA%E6%91%B8 1");
    printf("%d %s\n", command.countArg(), command.toString(false).c_str());
    for(int i = 0, num = command.countArg(); i < num; i++) {
      printf("<%s>", command.getArg(i).c_str());
    }
    printf("\n");
  }
  */
}
/***********************************************************************//**
	$Id$
***************************************************************************/
