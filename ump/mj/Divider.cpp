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
#include "ump/mj/Divider.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Divider::Divider()
  : mentsuMax_(0)
{}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Divider::~Divider() {
}
/***********************************************************************//**
	@brief 
	@param[in] hais 牌の配列
***************************************************************************/
void Divider::set(const HaiArray& hais) {
  flag_.reset();
  hais_ = hais;
  hais_.sort();
  head_.clear();
  mentsus_.clear();
  tatsus_.clear();
  mentsuMax_ = 0;
}
/***********************************************************************//**
	@brief 頭に分解する
***************************************************************************/
void Divider::eachHead() {
  for(auto hai : hais_.getUnique()) {
    if(hais_.countSame(hai) >= 2) {
      head_.append(hais_.removeSame(hai));
      head_.append(hais_.removeSame(hai));
      onHead();
      pop(head_);
      head_.clear();
      if(isVeto()) {
        return;
      }
    }
  }
}
/***********************************************************************//**
	@brief 面子に分解する
***************************************************************************/
void Divider::eachMentsu(size_t index) {
  if(isVeto()) {
    return;
  }
  bool hasMentsu = false;
  const Hai* hai = 0;
  for(size_t i = index; i < hais_.size(); i++) {
    if(hai && hais_[i]->isSame(hai)) {
      continue;
    }
    hai = hais_[i];
    if(hais_.countSame(hai) >= 3) {
      pushMentsu(hai, hai, hai, i);
      hasMentsu = true;
    }
    if(!hai->isZihai() && hai->getNumber() <= 7) {
      auto a = hai->shift(1);
      auto b = hai->shift(2);
      if(hais_.isInclude(a) && hais_.isInclude(b)) {
        pushMentsu(hai, a, b, i);
        hasMentsu = true;
      }
    }
  }
  if(!hasMentsu) {
    if(mentsus_.size() >= mentsuMax_) {
      mentsuMax_ = mentsus_.size();
      onMentsu();
    }
  }
}
/***********************************************************************//**
	@brief ターツに分解する
***************************************************************************/
void Divider::eachTatsu(size_t index) {
  if(isVeto()) {
    return;
  }
  bool hasTatsu = false;
  const Hai* hai = 0;
  for(size_t i = index; i < hais_.size(); i++) {
    if(hai && hais_[i]->isSame(hai)) {
      continue;
    }
    hai = hais_[i];
    if(hais_.countSame(hai) >= 2) {
      pushTatsu(hai, hai, i);
      hasTatsu = true;
    }
    if(!hai->isZihai()) {
      if(auto a = hai->shift(1)) {
        if(hais_.isInclude(a)) {
          pushTatsu(hai, a, i);
          hasTatsu = true;
        }
      }
      if(auto a = hai->shift(2)) {
        if(hais_.isInclude(a)) {
          pushTatsu(hai, a, i);
          hasTatsu = true;
        }
      }
    }
  }
  if(!hasTatsu) {
    onTatsu();
  }
}
/***********************************************************************//**
	@brief すべてを分解できたか調べる
	@return すべてを分解できたとき真
***************************************************************************/
bool Divider::isAll() const {
  return hais_.empty();
}
/***********************************************************************//**
	@brief 頭が存在するか調べる
	@return 頭が存在するとき真
***************************************************************************/
bool Divider::hasHead() const {
  return !head_.empty();
}
/***********************************************************************//**
	@brief 残った牌を数える
	@return 残った牌の数
***************************************************************************/
size_t Divider::countHai() const {
  return hais_.size();
}
/***********************************************************************//**
	@brief 残った牌を取得する
	@param[in] インデックス
	@return インデックス位置の牌
***************************************************************************/
const Hai* Divider::getHai(size_t index) const {
  return hais_[index];
}
/***********************************************************************//**
	@brief 頭を取得する
	@return 頭
***************************************************************************/
const HaiArray& Divider::getHead() const {
  return head_;
}
/***********************************************************************//**
	@brief 牌を数える
	@param[in] hai 数える牌
	@return 牌の数
***************************************************************************/
size_t Divider::countHai(const Hai* hai) const {
  size_t num = getHead().countSame(hai) + hais_.countSame(hai);
  for(auto& mentsu : getMentsus()) {
    num += mentsu.countSame(hai);
  }
  for(auto& tatsu : getTatsus()) {
    num += tatsu.countSame(hai);
  }
  return num;
}
/***********************************************************************//**
	@brief 文字列に変換する
	@return 変換した文字列
***************************************************************************/
std::string Divider::toString() const {
  std::ostringstream result;
  result << "<" << head_.toString() << ">";
  for(auto& mentsu : mentsus_) {
    result << "<" << mentsu.toString() << ">";
  }
  for(auto& tatsu : tatsus_) {
    result << "<" << tatsu.toString() << ">";
  }
  result << hais_.toString();
  return result.str();
}
/***********************************************************************//**
	@brief 内容を表示する
***************************************************************************/
void Divider::dump() const {
  printf("%s\n", toString().c_str());
}
/***********************************************************************//**
	@brief 中断させる
***************************************************************************/
void Divider::veto() {
  flag_.set(FLAG_VETO);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Divider::onHead() {
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Divider::onMentsu() {
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Divider::onTatsu() {
}
/***********************************************************************//**
	@brief 中断させられたか調べる
	@return 中断させられたとき真
***************************************************************************/
bool Divider::isVeto() const {
  return flag_.test(FLAG_VETO);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Divider::pushMentsu(const Hai* a, const Hai* b, const Hai* c, 
                         size_t index) {
  HaiArray elem;
  elem.append(hais_.removeSame(a));
  elem.append(hais_.removeSame(b));
  elem.append(hais_.removeSame(c));
  mentsus_.push_back(elem);
  eachMentsu(index);
  mentsus_.pop_back();
  pop(elem);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Divider::pushTatsu(const Hai* a, const Hai* b, size_t index) {
  HaiArray elem;
  elem.append(hais_.removeSame(a));
  elem.append(hais_.removeSame(b));
  tatsus_.emplace_back(elem);
  eachTatsu(index);
  tatsus_.pop_back();
  pop(elem);
}
/***********************************************************************//**
	@brief 外していた牌を戻す
	@param[in] elem 戻す牌
***************************************************************************/
void Divider::pop(const HaiArray& elem) {
  hais_.append(elem);
  hais_.sort();
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
