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
#include "ump/server/JobNaki.hpp"
#include "ump/server/JobSutehai.hpp"
#include "ump/server/JobTsumo.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] game ゲーム
	@param[in] sutehai 捨て牌
***************************************************************************/
JobNaki::JobNaki(Game& game, const mj::Sutehai* sutehai)
  : super(game), 
    sutehai_(sutehai)
{
  if(sutehai->isRichi()) {
    richier_ = getPlayer();
  }
}
/***********************************************************************//**
	@copydoc Job::onBegin
***************************************************************************/
void JobNaki::onBegin() {
  auto& game = getGame();
  size_t playerNum = game.countPlayer();
  auto hai = sutehai_->getHai();
  for(size_t i = 1; i < playerNum; i++) {
    auto index = (game.getTurn() + i) % playerNum;
    auto player = game.getPlayer(index);
    if(player->isConnect()) {
      auto command = game.createCommand(Command::TYPE_NAKI_Q);
      command.append(hai->toString());
      if(player->canRon(hai)) {
        command.append(Command::TYPE_RON);
      }
      if(game.canKan() && player->canKan(hai)) {
        command.append(Command::TYPE_KAN);
      }
      if(player->canPon(hai)) {
        command.append(Command::TYPE_PON);
      }
      if(getConfig()->canChi() && i == 1 && player->canChi(hai)) {
        command.append(Command::TYPE_CHI);
      }
      if(command.countArg() > 1) {
        game.sendCommand(player, command);
        players_.push_back(player);
      }
    }
  }
  sleep(0.25f);
}
/***********************************************************************//**
	@copydoc Job::onUpdate
***************************************************************************/
Job* JobNaki::onUpdate() {
  auto& game = getGame();
  if(!players_.empty()) {
    if(!isOverTime(getConfig()->getNakiWait())) {
      for(auto player : players_) {
        if(!player->getReply().isExist()) {
          return this;
        }
      }
    }
    std::shared_ptr<Player> player;
    for(auto iter : players_) {
      if(!player ||
         iter->getReply().getType() > player->getReply().getType()) {
        player = iter;
      }
    }
    if(auto nextJob = doReply(player)) {
      return nextJob;
    }
  }
  game.nextTurn();
  return nullptr;
}
/***********************************************************************//**
	@copydoc Job::onEnd
***************************************************************************/
void JobNaki::onEnd() {
  auto& game = getGame();
  if(richier_) {
    auto richiPoint = getConfig()->getRichiPoint();
    addPoint(richier_->getSeat(), -richiPoint, Command::TYPE_RICHI);
    game.addKyotaku(richiPoint);
  }
  if(!isRon()) {
    game.onDiscarded(*getPlayer(), sutehai_->getHai());
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobNaki::doReply(std::shared_ptr<Player> player) {
  auto& game = getGame();
  auto& reply = player->getReply();
  auto type = reply.getType();
  auto hai = sutehai_->getHai();
  if(player->getCommand().hasArg(type)) {
    if(type == Command::TYPE_RON && player->updateAgari(hai)) {
      richier_.reset();
      setRon(true);
      return new JobAgari(game, player->getSeat());
    }
    else if(reply.countArg() > 0) {
      mj::HaiArray hais(reply.getArg(0).c_str());
      if((type == Command::TYPE_KAN &&
          game.canKan() && player->canKan(hais, hai)) || 
         (type == Command::TYPE_PON && player->canPon(hais, hai)) || 
         (getConfig()->canChi() && 
          type == Command::TYPE_CHI && player->canChi(hais, hai))) {
        openMentsu(player, type, hais, hai);
        game.setTurn(player->getSeat());
        if(type == Command::TYPE_KAN) {
          game.appendDora();
          return new JobTsumo(game, true);
        }
        else {
          return new JobSutehai(game);
        }
      }
    }
  }
  return nullptr;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
