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
#include "ump/Command.hpp"
#include "ump/Version.hpp"
#include "ump/client/Bot.hpp"
#include "ump/mj/Player.hpp"

namespace ump {
namespace client {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Bot::Bot(std::shared_ptr<const mj::Config> config, 
         std::shared_ptr<socket::Socket> socket)
  : super(config, socket), 
    wait_(0)
{
}
/***********************************************************************//**
	@copydoc Client::onReceiveCommand
***************************************************************************/
void Bot::onReceiveCommand(const Command& command) {
  super::onReceiveCommand(command);
  switch(command.getType()) {
  case Command::TYPE_READY_Q:
    replyCommand(Command(Command::TYPE_YES), command);
    break;
  case Command::TYPE_TENPAI_Q:
    replyCommand(Command(Command::TYPE_YES), command);
    break;
  case Command::TYPE_NAKI_Q:
    replyNaki(command);
    break;
  case Command::TYPE_SUTEHAI_Q:
    replySutehai(command);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Bot::replySutehai(const Command& command) {
  std::this_thread::sleep_for(wait_);
  if(command.hasArg(Command::TYPE_TSUMO)) {
    replyCommand(Command(Command::TYPE_TSUMO), command);
  }
  else {
    auto player = getPlayer();
    auto menzen = player->getMenzen();
    const mj::Hai* best = nullptr;
    int score = 0;
    mj::Shanten shanten;
    for(auto hai : menzen.getUnique()) {
      menzen.removeSame(hai);
      shanten.update(menzen, player->isMenzen());
      if(!best || shanten.getShanten() < score) {
        best = hai;
        score = shanten.getShanten();
      }
      menzen.append(hai);
    }
    Command reply(Command::TYPE_SUTEHAI);
    reply.append(best->toString());
    if(player->getRichiableHai().isInclude(best)) {
      reply.append(Command::TYPE_RICHI);
    }
    replyCommand(reply, command);
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Bot::replyNaki(const Command& command) {
  if(command.hasArg(Command::TYPE_RON)) {
    replyCommand(Command(Command::TYPE_RON), command);
  }
  else {
    replyCommand(Command(Command::TYPE_NO), command);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
