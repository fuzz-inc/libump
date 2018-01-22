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
#include "ump/mj/Hand.hpp"
#include "ump/mj/Shanten.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Shanten::Shanten(const Hand& hand)
  : hand_(hand), 
    mentsuMax_(0), 
    shanten_(std::numeric_limits<int>::max())
{}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Shanten::~Shanten() {
}
/***********************************************************************//**
	@brief シャンテンを計算する
	@return シャンテン数
***************************************************************************/
int Shanten::update() {
  return update(hand_.getMenzen());
}
/***********************************************************************//**
	@brief シャンテンを計算する
	@param[in] hais 門前の牌列
	@return シャンテン数
***************************************************************************/
int Shanten::update(const HaiArray& hais) {
  super::set(hais.getNormal());
  mentsuMax_ = hais.size() / 3;
  shanten_ = std::numeric_limits<int>::max();
  flag_.reset(FLAG_TENPAI);
  richi_.clear();
  kanables_.clear();
  for(auto hai : hais.getUnique()) {
    if(hais.countSame(hai) >= 3) {
      kanables_.push_back(hai);
    }
  }
  if(hand_.isMenzen()) {
    checkKokushi();
    checkChitoi();
  }
  eachHead();
  eachMentsu();
  if(getShanten() < 0) {
    richi_ = getHais();
  }
  richi_.unique();
  return getShanten();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::onHead() {
  eachMentsu();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::onMentsu() {
  eachTatsu();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::onTatsu() {
  //dump();
  auto mentsuNum = getMentsus().size();
  auto tatsuNum = countTatsu();
  if(mentsuNum + tatsuNum > mentsuMax_) {
    tatsuNum = mentsuMax_ - mentsuNum;
  }
  int shanten = int(mentsuMax_ - mentsuNum) * 2 - int(tatsuNum);
  if(hasHead()) {
    shanten--;
  }
  updateShanten(shanten);
  if(shanten == 0) {
    if(countHai() == 0 && countTatsu() == 1) {
      richi_.append(getTatsu(0));
    }
    else {
      richi_.append(getHais());
    }
    updateKanables();
  }
}
/***********************************************************************//**
	@brief シャンテン数を更新する
***************************************************************************/
void Shanten::updateShanten(int shanten) {
  if(shanten < shanten_) {
    shanten_ = shanten;
  }
}
/***********************************************************************//**
	@brief 国士無双のチェック
***************************************************************************/
void Shanten::checkKokushi() {
  auto hais(getHais());
  HaiArray elem;
  for(size_t i = 0; i < HaiArray::YAOCHU.size(); i++) {
    auto hai = hais.removeSame(HaiArray::YAOCHU[i]);
    if(hai) {
      elem.append(hai);
    }
  }
  int shanten = 13 - int(elem.size());
  if(hais.isIncludeYaochu()) {
    shanten--;
  }
  updateShanten(shanten);
  if(shanten == 0) {
    for(auto hai : hais) {
      if(hai->isYaochu()) {
        richi_.append(hai);
      }
      else {
        richi_.clear();
        richi_.append(hai);
        break;
      }
    }
  }
}
/***********************************************************************//**
	@brief 七対子のチェック
***************************************************************************/
void Shanten::checkChitoi() {
  const auto& hais = getHais();
  auto uniq = hais.getUnique();
  int shanten = (int(hais.size()) - 1) / 2;
  HaiArray single;
  HaiArray triple;
  for(auto hai : uniq) {
    auto num = hais.countSame(hai);
    if(num < 2) {
      single.push_back(hai);
    }
    else {
      shanten--;
      if(num > 2) {
        triple.push_back(hai);
      }
    }
  }
  if(uniq.size() < 7) {
    shanten += 7 - uniq.size();
  }
  updateShanten(shanten);
  if(shanten == 0) {
    if(triple.empty()) {
      richi_.append(single);
    }
    else {
      richi_.append(triple);
    }
  }
}
/***********************************************************************//**
	@brief 暗槓可能牌を更新する
***************************************************************************/
void Shanten::updateKanables() {
  for(auto iter = kanables_.begin(); iter != kanables_.end();) {
    if(isKotsu(*iter)) {
      iter++;
    }
    else {
      //std::cerr << (*iter)->toString() << ":" << toString() << std::endl;
      iter = kanables_.erase(iter);
    }
  }
}
/***********************************************************************//**
	@brief 刻子か調べる
	@param[in] hai 牌
	@return 牌が刻子のとき真
***************************************************************************/
bool Shanten::isKotsu(const Hai* hai) const {
  for(auto& mentsu : getMentsus()) {
    if(mentsu.isKotsu() && mentsu.at(0)->isSame(hai)) {
      return true;
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::dump() const {
  std::cout << "(" << getHead().toString() << ")";
  for(auto& mentsu : getMentsus()) {
    std::cout << "(" << mentsu.toString() << ")";
  }
  for(size_t i = 0, n = countTatsu(); i < n; i++) {
    std::cout << "(" << getTatsu(i).toString() << ")";
  }
  std::cout << getHais().toString() << std::endl;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
