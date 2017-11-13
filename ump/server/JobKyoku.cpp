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
#include "ump/server/JobHaipai.hpp"
#include "ump/server/JobKyoku.hpp"
#include "ump/server/JobRyukyoku.hpp"
#include "ump/server/JobTsumo.hpp"
#include "ump/server/Player.hpp"
#include "ump/server/Server.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
JobKyoku::JobKyoku(Game& game)
  : super(game)
{}
/***********************************************************************//**
	@brief 開始
***************************************************************************/
void JobKyoku::onBegin() {
  auto& game = getGame();
  game.beginKyoku();
  sendStart();
  sendPoint();
  sendDice();
  sendDora();
  game.beginJob(new JobHaipai(game));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobKyoku::onUpdate() {
  if(isEnd() || getGame().isAgari()) {
    return nullptr;
  }
  if(getGame().getRest() > 0) {
    getGame().beginJob(new JobTsumo(getGame(), false));
  }
  else {
    getGame().beginJob(new JobRyukyoku(getGame()));
    end();
  }
  return this;
}
/***********************************************************************//**
	@brief 終了
***************************************************************************/
void JobKyoku::onEnd() {
  auto& game = getGame();
  game.sendAll(game.createCommand(Command::TYPE_KYOKUEND));
  game.endKyoku();
  sleep(0.5f);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobKyoku::sendStart() {
  auto& game = getGame();
  auto command = game.createCommand(Command::TYPE_KYOKUSTART);
  command.append(game.getRound());
  command.append(Command::SeatToString(game.getOya()));
  command.append(game.getRenchan());
  command.append(game.getKyotaku().toString());
  command.append(game.getBakaze()->toString());
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    command.append(getPlayer(i)->getZikaze()->toString());
  }
  game.sendAll(command);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobKyoku::sendPoint() {
  auto& game = getGame();
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto command = game.createCommand(Command::TYPE_POINT);
    command.append(Command::SeatToString(i));
    command.append(std::string("=") + getPlayer(i)->getPoint().toString());
    game.sendAll(command);
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobKyoku::sendDice() {
  auto& game = getGame();
  auto command = game.createCommand(Command::TYPE_DICE);
  std::default_random_engine random((std::random_device())());
  std::uniform_int_distribution<int> dice(1, 6);
  command.append(dice(random));
  command.append(dice(random));
  game.sendAll(command);
  sleep(getConfig()->getDiceWait());
}
/***********************************************************************//**
	@brief ドラを送る
***************************************************************************/
void JobKyoku::sendDora() {
  for(size_t i = 0, n = getConfig()->getDoraNum(); i < n; i++) {
    getGame().appendDora();
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
