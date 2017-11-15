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
#include "ump/server/Config.hpp"
#include "ump/server/Game.hpp"
#include "ump/server/JobAgari.hpp"
#include "ump/server/JobReady.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
JobAgari::JobAgari(Game& game, size_t player)
  : super(game), 
    player_(player)
{}
/***********************************************************************//**
	@copydoc Job::onBegin
***************************************************************************/
void JobAgari::onBegin() {
  say();
  agari();
}
/***********************************************************************//**
	@copydoc Job::onUpdate
***************************************************************************/
Job* JobAgari::onUpdate() {
  return new JobReady(getGame());
}
/***********************************************************************//**
	@copydoc Job::onEnd
***************************************************************************/
void JobAgari::onEnd() {
}
/***********************************************************************//**
	@brief 発声
***************************************************************************/
void JobAgari::say() {
  auto& game =getGame();
  auto command = game.createCommand(Command::TYPE_SAY);
  command.append(Command::SeatToString(player_));
  auto player = getPlayer(player_);
  if(player->getAgari().isRon()) {
    command.append(Command::TYPE_RON);
  }
  else {
    command.append(Command::TYPE_TSUMO);
  }
  sayAll(command);
}
/***********************************************************************//**
	@brief 和了
***************************************************************************/
void JobAgari::agari() {
  super::openHand(player_);
  sleep(getConfig()->getAgariWait());
  auto& game = getGame();
  auto player = getPlayer(player_);
  auto& agari = player->getAgari();
  if(player->isRichi()) {
    game.uraDora();
    agari.update(*player);
    sleep(getConfig()->getUraDoraWait());
  }
  auto point = agari.getPoint();
  auto text = agari.getText();
  if(getConfig()->isAotenjo() && !agari.isYakuman()) {
    point = agari.getFu() * 32;
    for(size_t i = 1, n = agari.getHan(); i < n; i++) {
      point *= 2;
    }
    text.clear();
  }
  if(player_ == game.getOya()) {
    point += point / 2;
  }
  {
    auto command = game.createCommand(Command::TYPE_AGARI);
    command.append(player->getSeatString());
    if(text.empty()) {
      command.append(mj::Agari::Ceil(point, 100).toString());
    }
    else {
      command.append(text);
    }
    command.append(agari.getSrcFu());
    for(int i = 0; i < mj::Agari::YAKU_MAX; i++) {
      int han = agari.getHan(i);
      if(han > 0) {
        command.append(mj::Agari::GetYakuName(i));
        command.append(han);
      }
    }
    if(!agari.isYakuman()) {
      size_t dora = agari.getDora();
      if(dora > 0) {
        command.append(u8"ドラ");
        command.append(dora);
      }
    }
    game.sendAll(command);
  }
  auto playerNum = countPlayer();
  if(playerNum > 1) {
    auto renchanPoint = game.getRenchan() * getConfig()->getRenchanPoint();
    if(player_ == game.getOya()) {
      playerNum--;
    }
    if(game.getTurn() == player_) {
      point /= playerNum;
      auto add = game.resetKyotaku();
      for(size_t i = 0, n = countPlayer(); i < n; i++) {
        if(i != player_) {
          auto sub = (i == game.getOya()) ? point * 2 : point;
          sub = mj::Agari::Ceil(sub, 100) + renchanPoint;
          addPoint(i, -sub);
          add += sub;
        }
      }
      addPoint(player_, add);
    }
    else {
      point = mj::Agari::Ceil(point, 100) + 
        renchanPoint * (countPlayer() - 1);
      addPoint(game.getTurn(), -point);
      addPoint(player_, point + game.resetKyotaku());
    }
  }
  player->agari();
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
