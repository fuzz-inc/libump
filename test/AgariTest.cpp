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
#include "AgariTest.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
AgariTest::AgariTest()
  : config_(std::make_shared<ump::mj::Config>()), 
    game_(std::make_shared<ump::mj::Game>()), 
    player_(std::make_shared<ump::mj::Player>()), 
    parser_(*player_)
{
  game_->setConfig(config_);
  game_->setPlayer(0, player_);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariTest::onRun() {
  const struct {
    const char* hais;
    int tsumoHan;
    int ronHan;
  } TABLES[] = {
    { "6m6m6m2p3p4p5p6p6p6p7p2s3s4s", 2, 1 }, 
    { "2m2m9m9m3p3p1s1s7s7s1z1z5z5z", 3, 2 }, 	// 七対子
    { "1m2m3m2p3p4p3s4s5s7s8s4z4z9s", 2, 1 }, 	// 平和
    { "1m2m3m2p3p4p3s4s5s7s8s1z1z9s", 1, 0 }, 	// 平和ではない
    { "1m2m3m2p3p4p3s4s5s9s9s1z1z1z", 3, 2 }, 	// ダブ東
    { "1m1m2m2m4m5m6m7m7m7m9m9m9m2m", 9, 6 }, 
    { "2m3m4m1p1p3p4p5p6p7p3s4s5s2p", 2, 1 }
  };
  for(auto& table : TABLES) {
    {
      auto& agari = parse(table.hais, false);
      TEST_MESSAGE(agari.getHan() == table.tsumoHan, 
                   Format("%s(%d)", table.hais, agari.getHan()));
    }
    {
      auto& agari = parse(table.hais, true);
      TEST_MESSAGE(agari.getHan() == table.ronHan, 
                   Format("%s(%d)", table.hais, agari.getHan()));
    }
  }
  {
    resetHand("1p1p1p3p4p2s2s8s8s7z7z7z");
    openMentsu("8s8s", "8s");
    openMentsu("1p1p1p", "1p");
    openMentsu("7z7z7z", "7z");
    player_->tsumo(ump::mj::Hai::Get("2p"), true);
    auto& agari = player_->getAgari();
    TEST(agari.isInclude(ump::mj::Agari::YAKU_RINSHAN));
  }
  testYakuman();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariTest::testYakuman() {
  using Agari = ump::mj::Agari;
  static const struct {
    const char* hand;
    const char* tsumo;
    int yakuman;
  } TABLES[] = {
    { "1m9m1p9p1s9s1z2z3z4z5z6z7z", "4z", Agari::YAKUMAN_KOKUSHI }, 
    { "4m5m6m2p2p5z5z5z6z6z7z7z7z", "6z", Agari::YAKUMAN_DAISANGEN }, 
    { "1m1m9m9m9m1p1p1p1s1s9s9s9s", "1s", Agari::YAKUMAN_CHINROUTOU }, 
    { "5m5m5m7p7p9p9p1s1s1s3s3s3s", "7p", Agari::YAKUMAN_SUANKO }, 
    { "2m3m4m1z1z1z2z2z2z3z3z3z4z", "4z", Agari::YAKUMAN_SHOSUSHI }, 
    { "3m3m1z1z1z2z2z2z3z3z3z4z4z", "4z", Agari::YAKUMAN_DAISUSHI }, 
    { "1z1z1z2z2z4z4z4z5z5z5z7z7z", "2z", Agari::YAKUMAN_TSUISOU }, 
    { "2s2s3s3s4s4s6s6s8s8s8s6z6z", "6s", Agari::YAKUMAN_RYUISOU }, 
    { "1m1m1m2m3m4m5m6m7m8m9m9m9m", "2m", Agari::YAKUMAN_CHUREN }, 
    { "1m1m2m2m2m3m3m3m4m4m4m7m7m", "1m", Agari::YAKUMAN_SUANKO }
  };
  for(auto& table : TABLES) {
    resetHand(table.hand);
    auto& agari = tsumo(table.tsumo);
    TEST_MESSAGE(agari.isInclude(table.yakuman), agari.toString());
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
const ump::mj::Agari& AgariTest::parse(const char* str, bool isRon) {
  ump::mj::HaiArray hais(str);
  auto hai = hais.removeSame(hais.back());
  player_->reset();
  player_->setBakaze(ump::mj::Hai::Get("1z"));
  player_->setZikaze(ump::mj::Hai::Get("1z"));
  player_->drawHaipai(hais);
  player_->resetFirst();
  //player_->setRichi(true);
  if(isRon) {
    /*
    TEST_MESSAGE(player_->canRon(hai), 
                 Format("%s %s", player_->toString().c_str(), 
                        hai->toString()));
    */
    return parser_.parse(hai);
  }
  player_->tsumo(hai, false);
  return parser_.parse(nullptr);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariTest::resetHand(const char* hais) {
  player_->reset();
  player_->setBakaze(ump::mj::Hai::Get("1z"));
  player_->setZikaze(ump::mj::Hai::Get("1z"));
  player_->drawHaipai(ump::mj::HaiArray(hais));
  player_->resetFirst();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariTest::openMentsu(const char* hais, const char* hai) {
  player_->openMentsu(ump::mj::HaiArray(hais), 
                      hai ? ump::mj::Hai::Get(hai) : nullptr);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
const ump::mj::Agari& AgariTest::tsumo(const char* hai, bool rinshan) {
  player_->tsumo(ump::mj::Hai::Get(hai), rinshan);
  return player_->getAgari();
}
/***********************************************************************//**
	$Id$
***************************************************************************/
