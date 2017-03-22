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
#pragma once

#include "ump/mj/Config.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief 設定
***************************************************************************/
class Config
  : public mj::Config
{
  typedef mj::Config super;

 private:
  enum {
    FLAG_OPEN, 
    FLAG_HAKOSHITA, 
    FLAG_MAX
  };

 private:
  std::bitset<FLAG_MAX> flag_;
  int point_;
  int notenBappu_;
  size_t roundMax_;
  size_t doraNum_;
  std::vector<int> uma_;
  int deltaTime_;
  float speed_;
  float diceWait_;
  float haipaiWait_;
  std::chrono::seconds sutehaiWait_;
  std::chrono::seconds nakiWait_;
  std::chrono::seconds tenpaiWait_;
  std::chrono::seconds readyWait_;
  std::chrono::seconds sayWait_;
  std::string logPrefix_;

 public:
  Config();
  ~Config() = default;

  /**
   * 手牌オープンモード
   */
  UMP_BIT_ACCESSOR(Open, flag_, FLAG_OPEN);

  /**
   * 箱下続行
   */
  UMP_BIT_ACCESSOR(Hakoshita, flag_, FLAG_HAKOSHITA);

  UMP_ACCESSOR(Point, point_);
  UMP_ACCESSOR(NotenBappu, notenBappu_);
  UMP_ACCESSOR(RoundMax, roundMax_);
  UMP_ACCESSOR(DoraNum, doraNum_);
  UMP_ACCESSOR(Uma, uma_);

  UMP_ACCESSOR(DeltaTime, deltaTime_);

  UMP_ACCESSOR(Speed, speed_);
  UMP_ACCESSOR(DiceWait, diceWait_);
  UMP_ACCESSOR(HaipaiWait, haipaiWait_);
  UMP_ACCESSOR(SutehaiWait, sutehaiWait_);
  UMP_ACCESSOR(NakiWait, nakiWait_);
  UMP_ACCESSOR(TenpaiWait, tenpaiWait_);
  UMP_ACCESSOR(ReadyWait, readyWait_);
  UMP_ACCESSOR(SayWait, sayWait_);

  UMP_ACCESSOR(LogPrefix, logPrefix_);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
