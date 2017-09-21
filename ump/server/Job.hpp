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

namespace ump {
namespace server {
/***********************************************************************//**
	@brief 
***************************************************************************/
class Job {
 private:
  enum {
    FLAG_END, 
    FLAG_MAX
  };

 private:
  Game& game_;
  std::shared_ptr<Player> player_;
  std::bitset<FLAG_MAX> flag_;
  std::chrono::milliseconds time_;

 public:
  virtual ~Job();

  Job* update(const std::chrono::milliseconds& deltaTime);

  virtual void onBegin() {}
  virtual void onEnd() {}

  void sleep(float sec) const;

 protected:
  Job(Game& game);

  Game& getGame() const {
    return game_;
  }

  std::shared_ptr<const Config> getConfig() const;

  UMP_GETTER(Time, time_);
  bool isOverTime(const std::chrono::milliseconds& time) const;

  size_t countPlayer() const;
  UMP_GETTER(Player, player_);
  std::shared_ptr<Player> getPlayer(size_t seat) const;

  void sendAll(const Command& command) const;
  void sendAllLog(const Command& command) const;
  void sayAll(const Command& command) const;
  void log(Logger::Level level, const std::string& message) const;

  void end();
  bool isEnd() const;

  void openHand(size_t sheet);

  void openMentsu(std::shared_ptr<Player> player, 
                  Command::Type type, 
                  const mj::HaiArray& hais, 
                  const mj::Hai* hai = nullptr);

  void addPoint(size_t sheet, const BigNum& value, 
                Command::Type = Command::TYPE_POINT);

  virtual Job* onUpdate() = 0;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
