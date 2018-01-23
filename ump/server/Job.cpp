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
#include "ump/server/Job.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Job::~Job() {
}
/***********************************************************************//**
	@brief 更新
***************************************************************************/
Job* Job::update(const std::chrono::milliseconds& deltaTime) {
  time_ += deltaTime;
  return onUpdate();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Job::sleep(float sec) const {
  auto msec = int(getConfig()->getSpeed() * sec * 1000);
  getGame().sleep(msec);
}
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Job::Job(Game& game)
  : game_(game), 
    player_(game.getTurnPlayer()), 
    time_(0)
{}
/***********************************************************************//**
	@copydoc Game::getConfig
***************************************************************************/
std::shared_ptr<const Config> Job::getConfig() const {
  return getGame().getConfig();
}
/***********************************************************************//**
	@brief 待ち時間を過ぎているか調べる
	@param[in] time 待ち時間
	@return 待ち時間を過ぎているとき真
***************************************************************************/
bool Job::isOverTime(const std::chrono::milliseconds& time) const {
  if(getConfig()->isWait()) {
    return false;
  }
  return time.count() > 0 && getTime() > time;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
size_t Job::countPlayer() const {
  return getGame().countPlayer();
}
/***********************************************************************//**
	@brief プレイヤーを取得する
	@param[in] seat 席
	@return プレイヤー
***************************************************************************/
std::shared_ptr<Player> Job::getPlayer(size_t seat) const {
  return getGame().getPlayer(seat);
}
/***********************************************************************//**
	@brief 発声コマンドをすべてのプレイヤーに送る
	@param[in] command 発声コマンド
***************************************************************************/
void Job::sayAll(const Command& command) const {
  assert(command.getType() == Command::TYPE_SAY);
  getGame().sendAll(command);
  sleep(getConfig()->getSayWait());
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Job::log(Logger::Level level, const std::string& message) const {
  getGame().log(level, message);
}
/***********************************************************************//**
	@brief 終了フラグを立てる
***************************************************************************/
void Job::end() {
  flag_.set(FLAG_END);
}
/***********************************************************************//**
	@brief 終了フラグが立っているか調べる
	@return 終了フラグが立っているとき真
***************************************************************************/
bool Job::isEnd() const {
  return flag_.test(FLAG_END);
}
/***********************************************************************//**
	@brief 手牌を公開する
	@param[in] seat 公開する席
***************************************************************************/
void Job::openHand(size_t seat) {
  auto& game = getGame();
  auto command = game.createCommand(Command::TYPE_OPEN);
  command.append(Command::SeatToString(seat));
  command.append(getPlayer(seat)->getMenzen().toString());
  game.sendAll(command);
}
/***********************************************************************//**
	@brief 面子を晒す
	@param[in] player プレイヤー
	@param[in] type タイプ
	@param[in] hais 晒す手牌
	@param[in] hai 鳴く牌
***************************************************************************/
void Job::openMentsu(std::shared_ptr<Player> player, 
                     Command::Type type, 
                     const mj::HaiArray& hais, 
                     const mj::Hai* hai) {
  auto& game = getGame();
  {
    auto command = game.createCommand(Command::TYPE_SAY);
    command.append(player->getSeatString());
    switch(type) {
    case Command::TYPE_ANKAN:
    case Command::TYPE_KAKAN:
      command.append(Command::TYPE_KAN);
      break;
    default:
      command.append(type);
      break;
    }
    sayAll(command);
  }
  auto command = game.createCommand(Command::TYPE_OPEN);
  command.append(player->getSeatString());
  if(type == Command::TYPE_KAKAN) {
    command.append(std::string("[") + hais.toString() + "]");
    command.append(hai->toString());
    player->kakan(hais, hai);
  }
  else {
    if(hai) {
      command.append(std::string("<") + hais.toString() + ">");
      command.append(hai->toString());
    }
    else {
      command.append(std::string("(") + hais.toString() + ")");
    }
    player->openMentsu(hais, hai);
  }
  for(auto player : getGame().getPlayers()) {
    player->onOpen();
  }
  game.sendAll(command);
}
/***********************************************************************//**
	@brief 点数を加減算する
	@param[in] seat 席
	@param[in] value 加減算する値
	@param[in] type コマンド種別
***************************************************************************/
void Job::addPoint(size_t seat, const BigNum& value, Command::Type type) {
  assert(value != 0);
  auto& game = getGame();
  auto command = game.createCommand(type);
  command.append(Command::SeatToString(seat));
  command.append(value.toString(true));
  game.sendAll(command);
  getPlayer(seat)->addPoint(value);
}    
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
