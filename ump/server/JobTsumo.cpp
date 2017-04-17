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
#include "ump/server/JobTsumo.hpp"
#include "ump/server/JobSutehai.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] game ゲーム
	@param[in] flag フラグ
***************************************************************************/
JobTsumo::JobTsumo(Game& game, unsigned int flag)
  : super(game), 
    flag_(flag)
{}
/***********************************************************************//**
	@copydoc Job::onUpdate
***************************************************************************/
Job* JobTsumo::onUpdate() {
  auto config = getConfig();
  auto& game = getGame();
  auto player = game.getTurnPlayer();
  auto hai = game.tsumo();
  auto rest = game.getRest();
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    Command command(Command::TYPE_TSUMO);
    command.append(player->getSeatString());
    command.append(rest);
    if(i == game.getTurn() || config->isOpen()) {
      command.append(hai->toString());
    }
    else {
      command.append(mj::Hai::GetUnknown()->toString());
    }
    if(isRinshan()) {
      command.append(Command::TYPE_RINSHAN);
    }
    game.getPlayer(i)->send(command);
  }
  return new JobSutehai(game, hai);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
