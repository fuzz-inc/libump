/*
Copyright 2017 fuzz, Inc. All rights reserved. 
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
#include "ump/server/JobTenpai.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief 
***************************************************************************/
JobTenpai::JobTenpai(Game& game)
  : super(game), 
    state_(STATE_NULL)
{
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobTenpai::onBegin() {
  auto& game = getGame();
  auto player = game.getTurnPlayer();
  if(player->isRichi()) {
    state_ = STATE_TENPAI;
  }
  else if(player->isTenpai()) {
    game.sendCommand(player, game.createCommand(Command::TYPE_TENPAI_Q));
  }
  else {
    state_ = STATE_NOTEN;
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobTenpai::onUpdate() {
  auto& game = getGame();
  auto player = game.getTurnPlayer();
  if(state_ == STATE_NULL) {
    auto reply = player->getReply();
    if(reply.isExist()) {
      state_ = (reply.getType() == Command::TYPE_TENPAI)
        ? STATE_TENPAI
        : STATE_NOTEN;
    }
    else if(isOverTime(getConfig()->getTenpaiWait())) {
      state_ = STATE_NOTEN;
    }
    else {
      return this;
    }
  }
  return nullptr;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobTenpai::onEnd() {
  auto& game = getGame();
  auto command = game.createCommand(Command::TYPE_SAY);
  command.append(Command::SeatToString(game.getTurn()));
  if(state_ == STATE_TENPAI) {
    openHand(game.getTurn());
    command.append(Command::TYPE_TENPAI);
    game.getTurnPlayer()->sayTenpai();
  }
  else {
    command.append(Command::TYPE_NOTEN);
  }
  sayAll(command);
  game.nextTurn();
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
