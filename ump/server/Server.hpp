﻿/*
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

#include "ump/thread/SocketThread.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief サーバー
***************************************************************************/
class Server
  : public SocketThread, 
    public SocketThread::Listener, 
    public std::enable_shared_from_this<Server>
{
 private:
  int port_;
  std::shared_ptr<const Config> config_;
  int timeout_;
  std::vector<std::shared_ptr<Game>> games_;
  std::shared_ptr<Game> game_;
  int gameId_ = 0;

 public:
  Server(const std::shared_ptr<socket::Socket>& socket, 
         int port = DEFAULT_PORT);
  ~Server();

  UMP_ACCESSOR(Config, config_);
  UMP_ACCESSOR(Timeout, timeout_);

  void start();
  void stop();

  virtual void onEndGame(std::shared_ptr<Game> game);

  void recvCommand(std::shared_ptr<Player> player, 
                   const Command& command);

  virtual void onDisconnectPlayer(std::shared_ptr<Player> player) {}

 protected:
  void onThread() override;

  virtual std::shared_ptr<Player>
  createPlayer(std::shared_ptr<socket::Socket> socket);

  UMP_GETTER(Games, games_);
  virtual std::shared_ptr<Game> createGame();

  void startGame(std::shared_ptr<Game> game);
  virtual void onBeginGame(std::shared_ptr<Game> game) {}

  virtual void onConnectPlayer(std::shared_ptr<Player> player);
  virtual void onRecvCommand(std::shared_ptr<Player> player, 
                             const Command& command);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
