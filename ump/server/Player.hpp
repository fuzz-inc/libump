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
#include "ump/Logger.hpp"
#include "ump/mj/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief プレイヤー
***************************************************************************/
class Player
  : public mj::Player, 
    public std::enable_shared_from_this<Player>
{
  typedef mj::Player super;

 private:
  class Thread;

 private:
  std::shared_ptr<socket::Socket> socket_;
  std::unique_ptr<std::thread> thread_;
  Receiver* receiver_;
  unsigned int serial_;
  Command command_;
  Command reply_;

 public:
  Player(const std::shared_ptr<socket::Socket>& socket);
  ~Player();

  std::shared_ptr<Game> getGame() const;

  void start();
  void stop();

  bool isOpen() const;

  void send(const Command& command, Receiver* receiver = nullptr);

  UMP_GETTER(Command, command_);
  UMP_GETTER(Reply, reply_);

  bool canRichi() const;

  void operator()(std::shared_ptr<Player> self);

 protected:
  mj::Sutehai* sutehai(const mj::Sutehai& sutehai) override;

 private:
  void log(Logger::Level level, const std::string& message) const;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
