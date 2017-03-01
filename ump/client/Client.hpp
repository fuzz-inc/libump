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
#pragma once

#include "ump/Command.hpp"
#include "ump/mj/Game.hpp"
#include "ump/mj/HaiArray.hpp"
#include "ump/mj/Pattern.hpp"

namespace ump {
namespace client {
/***********************************************************************//**
	@brief UMPクライアント基底クラス
***************************************************************************/
class Client
  : public mj::Game
{
  typedef mj::Game super;

 private:
  enum {
    FLAG_GAME_END, 
    FLAG_MAX
  };

 private:
  std::shared_ptr<socket::Socket> socket_;
  std::unique_ptr<std::thread> thread_;
  Command hello_;
  std::shared_ptr<mj::Player> player_;
  size_t rest_;
  mj::Pattern ponPattern_;
  mj::Pattern chiPattern_;
  mj::Pattern kanPattern_;
  std::map<const mj::Hai*, size_t> hideHaiNums_;
  std::bitset<FLAG_MAX> flag_;

 public:
  Client(std::shared_ptr<const mj::Config> config, 
         std::shared_ptr<socket::Socket> socket);
  virtual ~Client();

  bool open(const char* host, int port);
  void close();
  bool isOpen() const;

  void operator()(std::shared_ptr<Client> self);

  Client& setName(const std::string& name);

  UMP_GETTER(Player, player_);
  size_t getSeat() const;

  std::shared_ptr<mj::Player> getPlayer(size_t seat) const {
    return super::getPlayer(seat);
  }

  std::shared_ptr<mj::Player> getPlayer(const char* seat) const {
    return super::getPlayer(seat);
  }

  size_t getRest() const override {
    return rest_;
  }

  UMP_GETTER(PonPattern, ponPattern_);
  UMP_GETTER(ChiPattern, chiPattern_);
  UMP_GETTER(KanPattern, kanPattern_);

  UMP_BIT_GETTER(GameEnd, flag_, FLAG_GAME_END);

  void replyCommand(Command reply, const Command& command);

  void dump() const;

 protected:
  virtual std::shared_ptr<mj::Player> createPlayer();

  virtual void onReceiveCommand(const Command& command);
  virtual void onReplyCommand(const Command& command);
  virtual void onGameEnd() {}

  UMP_GETTER(Socket, socket_);
  UMP_ACCESSOR(Hello, hello_);

  void beginKyoku();
  void onShowHai(const mj::Hai* hai) override;

 private:
  std::shared_ptr<Client> getThis() {
    return std::static_pointer_cast<Client>(super::shared_from_this());
  }

  void send(const Command& command);

  void recv(const Command& command);
  void execGameStart(const Command& command);
  void execKyokuStart(const Command& command);
  void execHaipai(const Command& command);
  void execPoint(const Command& command);
  void execTsumo(const Command& command);
  void execSutehai(const Command& command);
  void execOpen(const Command& command);
  void execSay(const Command& command);
  void execAgari(const Command& command);
  void execRyukyoku(const Command& command);

  void replySutehai(const Command& command);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
