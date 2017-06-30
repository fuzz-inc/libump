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
#include "ump/mj/Config.hpp"
#include "ump/mj/Hai.hpp"

//#define KAN_TEST

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Config::Config()
  : playerNum_(4), 
    haipaiNum_(13), 
    wanpaiNum_(14), 
    richiPoint_(1000), 
    renchanPoint_(100)
{
#if defined(KAN_TEST)
  for(int i = 1; i <= 9; i++) {
    setHaiNum(Hai::Get(Hai::COLOR_MANZU, i), 4 * 3);
  }
#else
  for(int color = Hai::COLOR_MANZU; color <= Hai::COLOR_SOUZU; color++) {
    for(int i = 1; i <= 9; i++) {
      setHaiNum(Hai::Get(color, i), 4);
    }
  }
#endif
  for(int i = 1; i <= 7; i++) {
    setHaiNum(Hai::Get(Hai::COLOR_ZIHAI, i), 4);
  }
}
/***********************************************************************//**
	@brief 牌の数をセットする
	@param[in] hai 牌
	@param[in] num 牌の数
	@return 自分自身
***************************************************************************/
Config& Config::setHaiNum(const Hai* hai, size_t num) {
  haiNums_[hai] = num;
  return *this;
}
/***********************************************************************//**
	@brief 牌の数を取得する
	@param[in] hai 牌
	@return haiの数
***************************************************************************/
size_t Config::getHaiNum(const Hai* hai) const {
  auto iter = haiNums_.find(hai);
  return (iter != haiNums_.end()) ? iter->second : 0;
}
/***********************************************************************//**
	@brief ドラ牌の枚数をセットする
	@param[in] hai 牌
	@param[in] num 枚数
	@return 自分自身
***************************************************************************/
Config& Config::setDoraHaiNum(const Hai* hai, size_t num) {
  hai = hai->getNormal();
  auto dora = hai->getDora();
  size_t sum = getHaiNum(hai) + getHaiNum(dora);
  if(num <= sum) {
    setHaiNum(hai, sum - num);
    setHaiNum(dora, num);
  }
  return *this;
}
/***********************************************************************//**
	@brief ドラ牌の枚数をセットする
	@param[in] num 枚数
	@param[in] number 数種
	@return 自分自身
***************************************************************************/
Config& Config::setDoraHaiNum(size_t num, int number) {
  setDoraHaiNum(Hai::Get(Hai::COLOR_MANZU, number), num);
  setDoraHaiNum(Hai::Get(Hai::COLOR_PINZU, number), num);
  setDoraHaiNum(Hai::Get(Hai::COLOR_SOUZU, number), num);
  return *this;
}
/***********************************************************************//**
	@brief すべての牌を数える
	@return すべての牌の数
***************************************************************************/
size_t Config::getHaiNum() const {
  size_t num = 0;
  for(auto& iter : haiNums_) {
    num += iter.second;
  }
  return num;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
