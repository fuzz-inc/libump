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
#include "ump/mj/Pattern.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 配牌を引く
	@param[in] hais 牌列
***************************************************************************/
void Hand::drawHaipai(const HaiArray& hais) {
  menzen_.append(hais);
  for(auto hai : hais) {
    if(!hai->isUnknown()) {
      onShowHai(hai);
    }
  }
}
/***********************************************************************//**
	@brief 門前部分をソートする
	@return 自分自身
***************************************************************************/
Hand& Hand::sort() {
  menzen_.sort();
  return *this;
}
/***********************************************************************//**
	@brief 初期化
***************************************************************************/
void Hand::reset() {
  menzen_.clear();
  mentsus_.clear();
  kawa_.clear();
}
/***********************************************************************//**
	@brief 晒す
	@param[in] pt 晒した牌を表す文字列
	@param[in] hai 鳴いた牌(鳴きじゃないときは0)
***************************************************************************/
void Hand::open(const char* pt, const Hai* hai) {
  while(pt) {
    switch(auto c = *pt) {
    case '(':
    case '<':
      {
        HaiArray hais;
        pt = hais.parseString(pt + 1);
        auto cp = *pt++;
        assert((c == '(' && cp == ')') || (c == '<' && cp == '>'));
        openMentsu(hais, hai);
        hai = nullptr;
      }
      break;
    case '[':
      {
        HaiArray hais;
        pt = hais.parseString(pt + 1);
        auto cp = *pt++;
        assert(cp == ']');
        kakan(hais, hai);
        hai = nullptr;
      }
      break;
    case '\0':
      pt = nullptr;
      break;
    default:
      menzen_.clear();
      pt = menzen_.parseString(pt);
      onOpen(menzen_);
      break;
    }
  }
}
/***********************************************************************//**
	@brief 牌を河に捨てる
	@param[in] sutehai 捨て牌
	@return 河の捨て牌
***************************************************************************/
Sutehai* Hand::sutehai(const Sutehai& sutehai) {
  drop(sutehai.getHai());
  return &kawa_.append(sutehai);
}
/***********************************************************************//**
	@brief 面前か調べる
	@return	面前のとき真
***************************************************************************/
bool Hand::isMenzen() const {
  for(auto& mentsu : mentsus_) {
    if(!mentsu->isMenzen()) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief 伏せ牌が含まれているか調べる
	@return 伏せ牌が含まれているとき真
***************************************************************************/
bool Hand::hasUnknown() const {
  return getMenzen().isIncludeUnknown();
}
/***********************************************************************//**
	@brief すべてが伏せ牌か調べる
	@return すべてが伏せ牌のとき真
***************************************************************************/
bool Hand::isAllUnknown() const {
  for(auto hai : getMenzen()) {
    if(!hai->isUnknown()) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief 文字列を返す
	@return 手牌を表す文字列
***************************************************************************/
std::string Hand::toString() const {
  std::string str;
  //str.append(getBakaze()->toString()).append(getZikaze()->toString());
  str.append(menzen_.toString());
  for(auto& mentsu : mentsus_) {
    str.append(mentsu->toString());
  }
  return str;
}
/***********************************************************************//**
	@brief 面子を晒す
	@param[in] hais 晒す牌
	@param[in] hai 鳴いた牌
***************************************************************************/
void Hand::openMentsu(const HaiArray& hais, const Hai* hai) {
  drop(hais);
  std::shared_ptr<Mentsu> mentsu;
  if(hai) {
    mentsu = std::make_shared<Mentsu>(HaiArray(hais).append(hai), false);
  }
  else {
    mentsu = std::make_shared<Mentsu>(hais, true);
  }
  mentsus_.push_back(mentsu);
  onOpenMentsu(mentsu, hais, hai);
}
/***********************************************************************//**
	@brief 加槓
	@param[in] hais 加槓される牌列
	@param[in] hai 加槓する牌
***************************************************************************/
void Hand::kakan(const HaiArray& hais, const Hai* hai) {
  auto mentsu = findMentsu(hais);
  assert(mentsu);
  drop(hai);
  mentsu->kakan(hai);
  onKakan(mentsu, hai);
}
/***********************************************************************//**
	@brief 晒した面子の数を返す
	@return	晒した面子の数
***************************************************************************/
size_t Hand::countMentsu() const {
  return mentsus_.size();
}
/***********************************************************************//**
	@brief 晒した面子を取得する
	@param index インデックス
	@return 面子
***************************************************************************/
const Mentsu& Hand::getMentsu(size_t index) const {
  return *mentsus_.at(index);
}
/***********************************************************************//**
	@brief 面子を探す
	@param[in] hais 面子の牌列
	@return 見つかった面子(もしくはnullptr)
***************************************************************************/
std::shared_ptr<Mentsu> Hand::findMentsu(const HaiArray& hais) const {
  auto found = std::find_if(mentsus_.begin(), mentsus_.end(), 
                            [&](const std::shared_ptr<Mentsu>& mentsu) {
                              return mentsu->isEqual(hais);
                            });
  return (found != mentsus_.end()) ? *found : nullptr;
}
/***********************************************************************//**
	@brief ポンできる組み合わせを取得する
	@param[in] hai ポンする牌
	@return 組み合わせ
***************************************************************************/
Pattern Hand::getPonPattern(const Hai* hai) const {
  HaiArray hais;
  for(auto iter : menzen_) {
    if(iter->isSame(hai)) {
      hais.append(iter);
    }
  }
  Pattern pattern;
  size_t n = hais.size();
  if(n >= 2) {
    for(size_t i = 0; i < n - 1; i++) {
      for(size_t j = i + 1; j < n; j++) {
        pattern.append(HaiArray().append(hais.at(i)).append(hais.at(j)));
      }
    }
  }
  return pattern;
}
/***********************************************************************//**
	@brief チーできる組み合わせを取得する
	@param[in] hai チーする牌
	@return 組み合わせ
***************************************************************************/
Pattern Hand::getChiPattern(const Hai* hai) const {
  HaiArray hais;
  for(auto iter : menzen_) {
    if(iter->getColor() == hai->getColor() &&
       iter->getNumber() >= hai->getNumber() - 2 && 
       iter->getNumber() <= hai->getNumber() + 2) {
      hais.append(iter);
    }
  }
  Pattern pattern;
  size_t n = hais.size();
  if(n >= 2) {
    for(size_t i = 0; i < n - 1; i++) {
      for(size_t j = i + 1; j < n; j++) {
        HaiArray tatsu;
        tatsu.append(hais.at(i)).append(hais.at(j));
        if(Mentsu(tatsu + hai).isShuntsu()) {
          pattern.append(tatsu);
        }
      }
    }
  }
  return pattern;
}
/***********************************************************************//**
	@brief カンできる組み合わせを取得する
	@return 組み合わせ
***************************************************************************/
Pattern Hand::getKanPattern() const {
  Pattern pattern;
  auto& menzen = getMenzen();
  for(auto hai : menzen) {
    size_t num = menzen.countSame(hai);
    if(num >= 4) {
      HaiArray hais;
      auto clone(menzen);
      for(size_t i = 0; i < num; i++) {
        hais.append(clone.removeSame(hai));
      }
      pattern.parse(hais, 4);
    }
    for(size_t i = 0, n = countMentsu(); i < n; i++) {
      auto& mentsu = getMentsu(i);
      if(mentsu.isKotsu() && mentsu.at(0)->isSame(hai)) {
        pattern.append(HaiArray().append(hai));
      }
    }
  }
  return pattern;
}
/***********************************************************************//**
	@brief カンできる組み合わせを取得する
	@param[in] hai カンする牌
	@return 組み合わせ
***************************************************************************/
Pattern Hand::getKanPattern(const Hai* hai) const {
  Pattern pattern;
  HaiArray hais;
  for(auto iter : getMenzen()) {
    if(iter->isSame(hai)) {
      hais.append(iter);
    }
  }
  pattern.parse(hais, 3);
  return pattern;
}
/***********************************************************************//**
	@brief 牌を面前に追加する
	@param[in] hai 追加する牌
***************************************************************************/
void Hand::append(const Hai* hai) {
  menzen_.append(hai);
}
/***********************************************************************//**
	@brief 牌を捨てる
	@param[in] hai 捨てる牌
***************************************************************************/
void Hand::drop(const Hai* hai) {
  if(!menzen_.removeEqual(hai)) {
    if(!menzen_.removeEqual(Hai::GetUnknown())) {
      assert(0);
    }
    onShowHai(hai);
  }
}
/***********************************************************************//**
	@brief 複数の牌を捨てる
	@param[in] hais 捨てる牌列
***************************************************************************/
void Hand::drop(const HaiArray& hais) {
  for(auto hai : hais) {
    drop(hai);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
