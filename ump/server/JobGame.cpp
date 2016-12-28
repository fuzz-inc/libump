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
#include "ump/server/JobGame.hpp"
#include "ump/server/JobKyoku.hpp"
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
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getPlayer(i);
    Command command(Command::TYPE_GAMESTART);
    command.append(player->getSeatString());
    for(size_t i = 0; i < n; i++) {
      command.append(getPlayer(i)->getName());
    }
    command.setOption("id", getGame().getId());
    player->send(command);
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobGame::onUpdate() {
  if(isNextKyoku()) {
    getGame().beginJob(new JobKyoku(getGame()));
    return this;
  }
  return nullptr;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobGame::onEnd() {
  auto players(getGame().getPlayers());
  std::stable_sort(players.begin(), players.end(), 
                   [](const std::shared_ptr<mj::Player>& a, 
                      const std::shared_ptr<mj::Player>& b) {
                     return a->getPoint() > b->getPoint();
                   });
  const auto& uma = getConfig().getUma();
  Command command(Command::TYPE_GAMEEND);
  for(size_t i = 0, n = players.size(); i < n; i++) {
    auto& player = players.at(i);
    int point = player->getPoint();
    if(i == 0) {
      point += getGame().resetKyotaku();
    }
    if(i < uma.size()) {
      point += uma.at(i);
    }
    command.
      append(player->getSeatString()).
      append(point);
  }
  sendAll(command);
}
/***********************************************************************//**
	@brief 次の局に進むか調べる
	@return 次の局に進むとき真
***************************************************************************/
bool JobGame::isNextKyoku() const {
  const auto& game = getGame();
  const auto& config = game.getConfig();
  if(!config.isHakoshita()) {
    for(auto player : game.getPlayers()) {
      if(player->getPoint() < 0) {
        return false;
      }
    }
  }
  return game.getRound() < config.getRoundMax();
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
