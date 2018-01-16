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
#include "ump/server/JobKakan.hpp"
#include "ump/server/JobNaki.hpp"
#include "ump/server/JobSutehai.hpp"
#include "ump/server/JobTsumo.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] game ゲーム
	@param[in] tsumoHai ツモ牌(鳴いた後はnullptr)
***************************************************************************/
JobSutehai::JobSutehai(Game& game, const mj::Hai* tsumoHai)
  : super(game), 
    tsumoHai_(tsumoHai), 
    command_(game.createCommand(Command::TYPE_SUTEHAI_Q))
{
}
/***********************************************************************//**
	@copydoc Job::onBegin
***************************************************************************/
void JobSutehai::onBegin() {
  auto& game = getGame();
  auto player = game.getTurnPlayer();
  if(tsumoHai_) {
    if(player->canAgari()) {
      command_.append(Command::TYPE_TSUMO);
    }
    if(player->canRichi()) {
      command_.setOption(Command::ToString(Command::TYPE_RICHI), 
                         player->getRichiableHai().toString());
    }
    if(game.canKan()) {
      if(player->canAnkan()) {
        command_.append(Command::TYPE_ANKAN);
      }
      if(player->canKakan()) {
        command_.append(Command::TYPE_KAKAN);
      }
    }
  }
  game.sendCommand(player, command_);
  sleep(0.5f);
}
/***********************************************************************//**
	@copydoc Job::onUpdate
***************************************************************************/
Job* JobSutehai::onUpdate() {
  auto& game = getGame();
  auto player = game.getTurnPlayer();
  Command reply = player->getReply();
  if(player->isConnect() && 
     !isOverTime(getConfig()->getSutehaiWait()) &&
     !reply.isExist()) {
    return this;
  }
  if(reply.isExist()) {
    auto type = reply.getType();
    if(command_.hasArg(type)) {
      switch(type) {
      case Command::TYPE_TSUMO:
        if(player->canAgari()) {
          return new JobAgari(game, game.getTurn());
        }
        break;
      case Command::TYPE_ANKAN:
        if(reply.countArg() > 0) {
          mj::HaiArray hais(reply.getArg(0).c_str());
          if(player->canAnkan(hais)) {
            return kan(type, hais);
          }
        }
        break;
      case Command::TYPE_KAKAN:
        if(reply.countArg() > 0) {
          if(auto hai = mj::Hai::Get(reply.getArg(0))) {
            if(auto mentsu = player->findKakanMentsu(hai)) {
              openMentsu(player, type, *mentsu, hai);
              return new JobKakan(game, hai);
            }
          }
        }
        break;
      default:
        break;
      }
    }
  }
  else {
    player->stop();
  }
  return sutehai(reply);
}
/***********************************************************************//**
	@brief 捨て牌
***************************************************************************/
Job* JobSutehai::sutehai(const Command& reply) {
  auto& game = getGame();
  auto player = game.getTurnPlayer();
  mj::Sutehai request;
  if(reply.countArg() > 0) {
    request.setHai(mj::Hai::Get(reply.getArg(0)));
  }
  request.setTsumogiri(reply.hasArg(Command::TYPE_TSUMOGIRI));
  request.setRichi(reply.hasArg(Command::TYPE_RICHI));
  auto sutehai = game.sutehai(*player, request);
  if(sutehai->isRichi()) {
    auto command = game.createCommand(Command::TYPE_SAY);
    command.append(player->getSeatString());
    command.append(Command::TYPE_RICHI);
    sayAll(command);
  }
  auto command = game.createCommand(Command::TYPE_SUTEHAI);
  command.append(player->getSeatString());
  command.append(sutehai->getHai()->toString());
  if(sutehai->isTsumogiri()) {
    command.append(Command::TYPE_TSUMOGIRI);
  }
  if(sutehai->isRichi()) {
    command.append(Command::TYPE_RICHI);
  }
  game.sendAll(command);
  sleep(0.25f);
  return new JobNaki(game, sutehai);
}
/***********************************************************************//**
	@brief 槓
	@param[in] type 種別
	@param[in] hais 晒す牌
	@param[in] hai 鳴いた牌
	@return 次のジョブ
***************************************************************************/
Job* JobSutehai::kan(Command::Type type, 
                     const mj::HaiArray& hais, 
                     const mj::Hai* hai) {
  auto& game = getGame();
  openMentsu(game.getTurnPlayer(), type, hais, hai);
  game.appendDora();
  return new JobTsumo(game, true);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
