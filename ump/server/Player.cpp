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
#include "ump/server/Config.hpp"
#include "ump/server/Game.hpp"
#include "ump/server/Player.hpp"
#include "ump/server/Server.hpp"
#include "ump/socket/Socket.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Player::Player(std::shared_ptr<Server> server, 
               std::shared_ptr<socket::Socket> socket)
  : server_(server), 
    socket_(socket), 
    serial_(0)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Player::~Player() {
  stop();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<Game> Player::getGame() const {
  return std::static_pointer_cast<Game>(super::getGame());
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Player::start() {
  thread_.reset(new std::thread(std::ref(*this), shared_from_this()));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Player::stop() {
  if(socket_) {
    socket_->close();
  }
  while(thread_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  socket_.reset();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Player::isOpen() const {
  return socket_ && socket_->isOpen();
}
/***********************************************************************//**
	@brief コマンドを送る
	@param[in] command 送信するコマンド
***************************************************************************/
void Player::send(const Command& command) {
  command_ = command;
  command_.setSerial(++serial_);
  reply_.clear();
  if(socket_->sendCommand(command_)) {
    log(Logger::LEVEL_DEBUG, std::string(" <- ") + command_.toString(false));
  }
  else {
    log(Logger::LEVEL_ERROR, std::string(" <- ") + command_.toString(false));
  }
}
/***********************************************************************//**
	@copydoc ump::mj::Hand::canRichi
***************************************************************************/
bool Player::canRichi() const {
  if(!super::canRichi()) {
    return false;
  }
  auto& config = getGame()->getConfig();
  return config.isHakoshita() ||
    getPoint() >= config.getRichiPoint();
}
/***********************************************************************//**
	@brief スレッド処理
***************************************************************************/
void Player::operator()(std::shared_ptr<Player> self) {
  while(isOpen()) {
    Command command;
    if(socket_->recvCommand(command)) {
      if(command.getSerial() == serial_) {
        log(Logger::LEVEL_DEBUG, 
            std::string(" -> ") + command.toString(true));
        reply_ = command;
      }
      if(auto game = getGame()) {
        game->onRecvCommand(shared_from_this(), command);
      }
      else {
        getServer()->onRecvCommand(shared_from_this(), command);
      }
    }
  }
  thread_.release()->detach();
}
/***********************************************************************//**
	@brief 牌を河に捨てる
	@param[in] sutehai 捨て牌
	@return 河の捨て牌
***************************************************************************/
mj::Sutehai* Player::sutehai(const mj::Sutehai& _sutehai) {
  auto sutehai(_sutehai);
  if(isRichi() || !sutehai.getHai()) {
    sutehai.
      setHai(getTsumoHai()).
      setTsumogiri(true);
  }
  else {
    auto num = getMenzen().countEqual(sutehai.getHai());
    if(num == 0) {
      sutehai.
        setHai(getTsumoHai()).
        setTsumogiri(true);
    }
    else if(sutehai.isTsumogiri()) {
      if(!sutehai.getHai()->isEqual(getTsumoHai())) {
        sutehai.setTsumogiri(false);
      }
    }
    else {
      if(sutehai.getHai()->isEqual(getTsumoHai()) && num == 1) {
        sutehai.setTsumogiri(true);
      }
    }
    if(sutehai.isRichi()) {
      if(!canRichi() || 
         !getRichiableHai().hasEqual(sutehai.getHai())) {
        sutehai.setRichi(false);
      }
    }
  }
  return super::sutehai(sutehai);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<Server> Player::getServer() const {
  return server_.lock();
}
/***********************************************************************//**
	@brief ログ出力
	@param[in] level レベル
	@param[in] message 出力メッセージ
***************************************************************************/
void Player::log(Logger::Level level, const std::string& message) const {
  if(auto logger = getGame()) {
    logger->log(level, getSeatString() + message);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
