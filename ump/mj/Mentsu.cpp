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
#include "ump/mj/Mentsu.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] hais 牌の配列
	@param[in] isMenzen 面前のとき真
***************************************************************************/
Mentsu::Mentsu(const HaiArray& hais, bool isMenzen)
  : super(hais), 
    type_(TYPE_NULL), 
    fu_(0), 
    machiFu_(0)
{
  sort();
  flag_.set(FLAG_MENZEN, isMenzen);
  switch(size()) {
  case 2:
    if(at(1)->isSame(at(0))) {
      setType(TYPE_TOITSU);
    }
    break;
  case 3:
    if(at(1)->isSame(at(0)) &&
       at(2)->isSame(at(0))) {
      setType(TYPE_KOTSU);
    }
    else if(at(1)->isSame(at(0)->shift(1)) && 
            at(2)->isSame(at(0)->shift(2))) {
      setType(TYPE_SHUNTSU);
    }
    break;
  case 4:
    if(at(1)->isSame(at(0)) &&
       at(2)->isSame(at(0)) &&
       at(3)->isSame(at(0))) {
      setType(TYPE_KANTSU);
    }
    break;
  default:
    break;
  }
  clearAgariHai();
}
/***********************************************************************//**
	@brief 符を取得する
	@return 符
***************************************************************************/
int Mentsu::getFu() const {
  return (isAnko() ? fu_ * 2 : (fu_ + machiFu_));
}
/***********************************************************************//**
	@brief 面前か調べる
	@return 面前のとき真
***************************************************************************/
bool Mentsu::isMenzen() const {
    return flag_.test(FLAG_MENZEN);
}
/***********************************************************************//**
	@brief ロン和了か調べる
	@return ロン和了のとき真
***************************************************************************/
bool Mentsu::isRon() const {
  return flag_.test(FLAG_RON);
}
/***********************************************************************//**
	@brief 対子か調べる
	@return 対子のとき真
***************************************************************************/
bool Mentsu::isToitsu() const {
  return type_ == TYPE_TOITSU;
}
/***********************************************************************//**
	@brief 順子か調べる
	@return 順子のとき真
***************************************************************************/
bool Mentsu::isShuntsu() const {
  return type_ == TYPE_SHUNTSU;
}
/***********************************************************************//**
	@brief 刻子のとき真を返す
	@return 刻子のとき真
***************************************************************************/
bool Mentsu::isKotsu() const {
  return type_ == TYPE_KOTSU;
}
/***********************************************************************//**
	@brief 槓子か調べる
	@return 槓子のとき真
***************************************************************************/
bool Mentsu::isKantsu() const {
  return type_ == TYPE_KANTSU;
}
/***********************************************************************//**
	@brief 暗刻か調べる
	@return 暗刻のとき真
***************************************************************************/
bool Mentsu::isAnko() const {
  return isMenzen() && (isKotsu() || isKantsu()) && !isRon();
}
/***********************************************************************//**
	@brief 平和として許される面子か調べる
	@return 平和として許される面子のとき真を返す
***************************************************************************/
bool Mentsu::isPinfu() const {
  return getFu() == 0;
}
/***********************************************************************//**
	@brief 和了牌をクリアする
***************************************************************************/
void Mentsu::clearAgariHai() {
  flag_.reset(FLAG_RON);
  machiFu_ = 0;
}
/***********************************************************************//**
	@brief 和了牌をセットする
	@param[in] hai 和了牌
	@param[in] isRon ロン和了のとき真
***************************************************************************/
void Mentsu::setAgariHai(const Hai* hai, bool isRon) {
  clearAgariHai();
  flag_.set(FLAG_RON, isRon);
  if(isToitsu() ||
     (isShuntsu() &&
      !((hai->isSame(at(0)) && hai->getNumber() < 7) ||
        (hai->isSame(at(2)) && hai->getNumber() > 3)))) {
    machiFu_ = 2;
  }
}
/***********************************************************************//**
	@brief 加槓できるか調べる
	@param[in] hai 牌
***************************************************************************/
bool Mentsu::canKakan(const Hai* hai) const {
  return isKotsu() && !isMenzen() && at(0)->isSame(hai);
}
/***********************************************************************//**
	@brief 加槓
	@param[in] hai 牌
***************************************************************************/
void Mentsu::kakan(const Hai* hai) {
  assert(canKakan(hai));
  push_back(hai);
  setType(TYPE_KANTSU);
}
/***********************************************************************//**
	@brief 文字列に変換する
	@return	面子を表す文字列
***************************************************************************/
std::string Mentsu::toString() const {
  std::string str;
  if(isMenzen()) {
    str.append("(").append(super::toString()).append(")");
  }
  else {
    str.append("<").append(super::toString()).append(">");
  }
  return str;
}
/***********************************************************************//**
	@brief 種別を設定する
	@param[in] type 種別
***************************************************************************/
void Mentsu::setType(Type type) {
  type_ = type;
  switch(type) {
  case TYPE_KOTSU:
    fu_ = 2;
    break;
  case TYPE_KANTSU:
    fu_ = 8;
    break;
  case TYPE_TOITSU:
  case TYPE_SHUNTSU:
    fu_ = 0;
    break;
  default:
    assert(0);
    break;
  }
  if(fu_ > 0 && at(0)->isYaochu()) {
    fu_ *= 2;
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
