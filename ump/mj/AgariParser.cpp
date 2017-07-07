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
#include "ump/mj/AgariParser.hpp"
#include "ump/mj/Player.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] player プレイヤー
***************************************************************************/
AgariParser::AgariParser(const Player& player)
  : player_(player), 
    agariHai_(nullptr)
{}
/***********************************************************************//**
	@brief 最高の和了を求める
	@param[in] ron ロン和了牌(ツモ和了のときはnullptr)
***************************************************************************/
const Agari& AgariParser::parse(const Hai* ron) {
  auto menzen = getPlayer().getMenzen();
  flag_.reset();
  if(ron) {
    flag_.set(FLAG_RON);
    agariHai_ = ron;
    menzen.append(ron);
  }
  else {
    agariHai_ = getPlayer().getTsumoHai();
    assert(agariHai_);
  }
  super::set(menzen);
  agari_.clear();
  update();
  return agari_;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariParser::onHead() {
  eachMentsu();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariParser::onMentsu() {
  if(!isAll()) {
    return;
  }
  Agari agari;
  for(size_t i = 0; i < getPlayer().countMentsu(); i++) {
    agari.appendMentsu(getPlayer().getMentsu(i));
  }
  agari.appendMentsu(Mentsu(getHead()));
  for(auto& mentsu : getMentsus()) {
    agari.appendMentsu(mentsu);
  }
  checkAgariHai(agari);
}
/***********************************************************************//**
	@brief ロン和了か調べる
	@return ロン和了のとき真
***************************************************************************/
bool AgariParser::isRon() const {
  return flag_.test(FLAG_RON);
}
/***********************************************************************//**
	@brief 更新
***************************************************************************/
void AgariParser::update() {
  if(getPlayer().isMenzen()) {
    if(isKokushi()) {
      agari_.appendYaku(Agari::YAKUMAN_KOKUSHI);
      return;
    }
    checkChitoi();
  }
  eachHead();
}
/***********************************************************************//**
	@brief 国士無双か調べる
	@return 国士無双のとき真
***************************************************************************/
bool AgariParser::isKokushi() const {
  HaiArray uniq = getPlayer().getMenzen().getUnique();
  if(uniq.size() != 13) {
    return false;
  }
  for(size_t i = 0, n = uniq.size(); i < n; i++) {
    if(!uniq[i]->isYaochu()) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief 七対子か調べる
***************************************************************************/
void AgariParser::checkChitoi() {
  HaiArray hais = getHais();
  HaiArray uniq = hais.getUnique();
  Agari agari;
  for(size_t i = 0, n = uniq.size(); i < n; i++) {
    auto hai = uniq.at(i);
    if(hais.countSame(hai) != 2) {
      return;
    }
    HaiArray elem;
    for(int i = 0; i < 2; i++) {
      elem.append(hais.removeSame(hai));
    }
    agari.appendMentsu(Mentsu(elem));
  }
  checkAgariHai(agari);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariParser::checkAgariHai(Agari& agari) {
  for(size_t i = 0, n = agari.countMentsu(); i < n; i++) {
    auto& mentsu = agari.getMentsu(i);
    if(mentsu.isMenzen() && mentsu.isInclude(agariHai_)) {
      mentsu.setAgariHai(agariHai_, isRon());
      agari.update(getPlayer());
      if(agari_ < agari) {
        agari_ = agari;
      }
      mentsu.clearAgariHai();
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
