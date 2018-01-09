/*
Copyright 2018 fuzz, Inc. All rights reserved. 
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
#include "ump/server/JobEnd.hpp"
#include "ump/server/JobGame.hpp"
#include "ump/server/JobKyoku.hpp"
#include "ump/server/JobReady.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
JobGame::JobGame(Game& game)
  : super(game)
{}
/***********************************************************************//**
	@brief 開始
***************************************************************************/
void JobGame::onBegin() {
  auto& game = getGame();
  auto command = game.createCommand(Command::TYPE_GAMESTART);
  command.setOption("id", game.getId());
  game.sendAll(command);
  game.beginJob(new JobReady(game));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobGame::onUpdate() {
  auto& game = getGame();
  if(!game.isFinish()) {
    game.beginJob(new JobKyoku(game));
    return this;
  }
  return new JobEnd(getGame());
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobGame::onEnd() {
  auto& game = getGame();
  auto players = game.getRanking();
  const auto& uma = getConfig()->getUma();
  for(size_t i = 0, n = players.size(); i < n; i++) {
    auto& player = players.at(i);
    auto point = player->getPoint();
    if(i == 0) {
      point += game.resetKyotaku();
    }
    if(i < uma.size()) {
      point += uma.at(i);
    }
    player->setPoint(point);
  }
  game.onEndGame(players);
  {
    auto command = game.createCommand(Command::TYPE_GAMEEND);
    for(auto& player : players) {
      command.append(player->getSeatString());
      command.append(player->getPoint().toString());
    }
    game.sendAll(command);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
