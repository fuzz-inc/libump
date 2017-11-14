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
#include "ump/thread/SocketThread.hpp"

namespace ump {
namespace client {
/***********************************************************************//**
	@brief UMPクライアント基底クラス
***************************************************************************/
class Client
  : public mj::Game, 
    public SocketThread
{
  typedef mj::Game super;

 public:
  enum State {
    STATE_NULL, 
    STATE_SEAT, 	/**< 席についた */
    STATE_GAME, 	/**< ゲーム中 */
    STATE_END		/**< ゲーム終了 */
  };

 private:
  enum Flag {
    FLAG_RECONNECT, 	/**< 再接続中 */
    FLAG_MAX
  };

 private:
  Command hello_;
  State state_;
  size_t seat_;
  size_t rest_;
  mj::Pattern ponPattern_;
  mj::Pattern chiPattern_;
  mj::Pattern kanPattern_;
  std::map<const mj::Hai*, size_t> hideHaiNums_;
  std::bitset<FLAG_MAX> flag_;
  std::shared_ptr<Logger> logger_;

 public:
  Client(std::shared_ptr<Socket> socket);
  virtual ~Client();

  bool open(const char* host, int port = DEFAULT_PORT);
  void close();
  bool isOpen() const;
  UMP_BIT_GETTER(Reconnect, flag_, FLAG_RECONNECT);

  UMP_ACCESSOR(Hello, hello_);

  UMP_GETTER(State, state_);

  void setName(const std::string& name);

  UMP_GETTER(Seat, seat_);
  std::shared_ptr<mj::Player> getPlayer() const;

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

  bool sendHello();
  bool sendCommand(const Command& command);
  bool replyCommand(Command reply, const Command& command);

  UMP_ACCESSOR(Logger, logger_);

  void dump() const;

 protected:
  virtual std::shared_ptr<mj::Player> createPlayer();
  virtual void onSetPlayer(std::shared_ptr<mj::Player> player);

  void onRecvCommand(const Command& command) override;
  virtual void onReplyCommand(const Command& command);

  virtual void onGameStart() {}
  void onEndGame(const mj::Players& players) override;

  void beginKyoku();
  void onShowHai(const mj::Hai* hai) override;

 private:
  std::shared_ptr<Client> getThis() {
    return std::static_pointer_cast<Client>(super::shared_from_this());
  }

  void execPlayer(const Command& command);
  void execGameStart(const Command& command);
  void execGameEnd(const Command& command);
  void execKyokuStart(const Command& command);
  void execHaipai(const Command& command);
  void execPoint(const Command& command);
  void execTsumo(const Command& command);
  void execSutehai(const Command& command);
  void execOpen(const Command& command);
  void execSay(const Command& command);
  void execAgari(const Command& command);
  void execRyukyoku(const Command& command);

  UMP_BIT_SETTER(Reconnect, flag_, FLAG_RECONNECT);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
