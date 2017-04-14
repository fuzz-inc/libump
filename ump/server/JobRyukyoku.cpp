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
#include "ump/server/JobReady.hpp"
#include "ump/server/JobRyukyoku.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
JobRyukyoku::JobRyukyoku(Game& game)
  : super(game), 
    num_(game.countPlayer())
{}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobRyukyoku::onBegin() {
  getGame().resetTurn();
  askTenpai();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobRyukyoku::onUpdate() {
  if(isEnd()) {
    return new JobReady(getGame());
  }
  if(waitTenpai()) {
    sayTenpai();
    askTenpai();
  }
  return this;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobRyukyoku::onEnd() {
  sendAll(Command(Command::TYPE_RYUKYOKU));
  int tenpaiNum = 0;
  int notenNum = 0;
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    if(getPlayer(i)->isSayTenpai()) {
      tenpaiNum++;
    }
    else {
      notenNum++;
    }
  }
  if(tenpaiNum > 0 && notenNum > 0) {
    auto notenBappu = getGame().getConfig().getNotenBappu();
    auto add = notenBappu / tenpaiNum;
    auto sub = notenBappu / notenNum;
    for(size_t i = 0, n = countPlayer(); i < n; i++) {
      addPoint(i, getPlayer(i)->isSayTenpai() ? add : -sub);
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobRyukyoku::askTenpai() {
  while(num_ > 0) {
    auto player = getGame().getTurnPlayer();
    if(player->isTenpai() && !player->isRichi()) {
      Command command(Command::TYPE_TENPAI_Q);
      player->send(command);
      return;
    }
    else {
      if(player->isRichi()) {
        player->sayTenpai();
      }
      sayTenpai();
    }
  }
  end();
}
/***********************************************************************//**
	@brief テンパイ宣言を待つ
***************************************************************************/
bool JobRyukyoku::waitTenpai() {
  auto player = getGame().getTurnPlayer();
  const Command reply = player->getReply();
  if(reply.isExist()) {
    if(reply.getType() == Command::TYPE_TENPAI) {
      player->sayTenpai();
    }
    return true;
  }
  return isOverTime(getConfig().getTenpaiWait());
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobRyukyoku::sayTenpai() {
  auto& game = getGame();
  Command command(Command::TYPE_SAY);
  command.append(Command::SeatToString(game.getTurn()));
  if(game.getTurnPlayer()->isSayTenpai()) {
    openHand(game.getTurn());
    command.append(Command::TYPE_TENPAI);
  }
  else {
    command.append(Command::TYPE_NOTEN);
  }
  sayAll(command);
  game.nextTurn();
  num_--;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
