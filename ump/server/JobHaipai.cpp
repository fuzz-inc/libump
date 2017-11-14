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
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] game ゲーム
***************************************************************************/
JobHaipai::JobHaipai(Game& game)
  : super(game), 
    num_(0)
{
}
/***********************************************************************//**
	@copydoc Job::onBegin
***************************************************************************/
void JobHaipai::onBegin() {
  num_ = getConfig()->getHaipaiNum();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobHaipai::onUpdate() {
  auto& game = getGame();
  auto config = getConfig();
  if(num_ > 0) {
    size_t num = std::min<size_t>(num_, 4);
    for(size_t i = 0, n = countPlayer(); i < n; i++) {
      drawHaipai(game.getTurn(), game.drawHaipai(num));
      sleep(config->getHaipaiWait());
      game.nextTurn();
    }
    num_ -= num;
    return this;
  }
  return nullptr;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobHaipai::drawHaipai(size_t seat, const mj::HaiArray& hais) {
  auto& game = getGame();
  auto config = getConfig();
  mj::HaiArray fusehai;
  for(size_t i = 0, n = hais.size(); i < n; i++) {
    fusehai.append(mj::Hai::GetUnknown());
  }
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    bool isTurn = (i == seat);
    auto command = game.createCommand(Command::TYPE_HAIPAI);
    command.append(Command::SeatToString(seat));
    if(isTurn) {
      command.append(hais.toString());
      getPlayer(i)->drawHaipai(hais);
    }
    else if(config->isOpen()) {
      command.append(hais.toString());
    }
    else {
      command.append(fusehai.toString());
    }
    game.sendCommand(getPlayer(i), command, isTurn);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
