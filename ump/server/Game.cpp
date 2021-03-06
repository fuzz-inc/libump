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
#include "ump/server/Config.hpp"
#include "ump/server/Game.hpp"
#include "ump/server/Job.hpp"
#include "ump/server/JobGame.hpp"
#include "ump/server/Player.hpp"
#include "ump/server/Server.hpp"
#include "ump/thread/Thread.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] config 設定
	@param[in] server サーバー
***************************************************************************/
Game::Game(std::shared_ptr<const Config> config, 
           std::shared_ptr<Server> server)
  : server_(server), 
    yama_(config), 
    kanNum_(0)
{
  setConfig(config);
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Game::~Game() {
  stop();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<const Config> Game::getConfig() const {
  return std::static_pointer_cast<const Config>(super::getConfig());
}
/***********************************************************************//**
	@brief プレイヤーを追加する
	@param[in] player プレイヤー
***************************************************************************/
void Game::appendPlayer(std::shared_ptr<Player> player) {
  auto seat = findSeat();
  super::setPlayer(seat, player);
  {
    auto command = createCommand(Command::TYPE_SEAT);
    command.append(Command::SeatToString(seat));
    command.setOption(Command::OPTION_GAMEID, getId());
    sendCommand(player, command);
  }
  /* 先に席についていた面子をplayerに送信する */
  for(auto& iter : getPlayers()) {
    if(iter && iter != player) {
      auto command = createCommand(Command::TYPE_PLAYER);
      command.append(Command::SeatToString(iter->getSeat()));
      command.append(iter->getName());
      sendCommand(player, command);
    }
  }
  /* playerの席を全員に送信する */
  {
    auto command = createCommand(Command::TYPE_PLAYER);
    command.append(Command::SeatToString(seat));
    command.append(player->getName());
    sendAll(command);
  }
}
/***********************************************************************//**
	@brief プレイヤーが揃っているか調べる
	@return プレイヤーが揃っているとき真
***************************************************************************/
bool Game::canStart() const {
  for(size_t i = 0, n = getConfig()->getPlayerNum(); i < n; i++) {
    if(!getPlayer(i)) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief ゲームが開始しているか調べる
	@return ゲームが開始しているとき真
***************************************************************************/
bool Game::isStart() const {
  return !jobs_.empty();
}
/***********************************************************************//**
	@brief ゲームを開始する
***************************************************************************/
void Game::start() {
  for(auto player : getPlayers()) {
    player->setPoint(getConfig()->getPoint());
  }
  beginJob(new JobGame(*this));
  Thread::start();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Game::stop() {
  if(isStart()) {
    stopAllJob();
    Thread::stop();
  }
  else {
    clearPlayer();
  }
}
/***********************************************************************//**
	@brief ジョブを開始する
	@param[in] 開始するジョブ
***************************************************************************/
void Game::beginJob(Job* job) {
  jobs_.emplace(job);
  job->onBegin();
}
/***********************************************************************//**
	@copydoc ump::Game::getPlayer
***************************************************************************/
std::shared_ptr<Player> Game::getPlayer(size_t index) const {
  return std::static_pointer_cast<Player>(super::getPlayer(index));
}
/***********************************************************************//**
	@brief 手番のプレイヤーを取得する
***************************************************************************/
std::shared_ptr<Player> Game::getTurnPlayer() const {
  return getPlayer(getTurn());
}
/***********************************************************************//**
	@copydoc ump::Game::beginKyoku
***************************************************************************/
void Game::beginKyoku() {
  super::beginKyoku();
  yama_.shuffle(getRandom());
  const mj::Hai* kaze = mj::Hai::Get("1z");
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getTurnPlayer();
    player->setBakaze(getBakaze());
    player->setZikaze(kaze);
    kaze = kaze->rotate(1);
    nextTurn();
  }
  kanNum_ = 0;
}
/***********************************************************************//**
	@brief ドラを追加する
***************************************************************************/
void Game::appendDora() {
  appendDora(Command::TYPE_DORA);
}
/***********************************************************************//**
	@brief 裏ドラをめくる
***************************************************************************/
void Game::uraDora() {
  for(size_t i = 0, n = getDoras().size(); i < n; i++) {
    appendDora(Command::TYPE_URADORA);
  }
}
/***********************************************************************//**
	@brief 配牌を引く
	@param[in] num 枚数
	@return 配牌
***************************************************************************/
mj::HaiArray Game::drawHaipai(size_t num) {
  return yama_.getHaipai(num);
}
/***********************************************************************//**
	@brief 牌山から1枚引く
	@return 引いた牌
***************************************************************************/
const mj::Hai* Game::drawHai() {
  return yama_.tsumo();
}
/***********************************************************************//**
	@brief 牌が捨てられたときの処理(鳴き処理後)
	@param[in] player 牌を捨てたプレイヤー
	@param[in] hai 捨てた牌
***************************************************************************/
void Game::onDiscarded(const Player& player, const mj::Hai* hai) {
  for(auto& iter : getPlayers()) {
    std::static_pointer_cast<Player>(iter)->onDiscarded(player, hai);
  }
}
/***********************************************************************//**
	@brief 牌を晒したときの処理
	@param[in] player 晒したプレイヤー
	@param[in] mentsu 晒した面子
***************************************************************************/
void Game::onOpenMentsu(std::shared_ptr<Player> player, 
                        std::shared_ptr<const mj::Mentsu> mentsu) {
  if(mentsu->isKantsu()) {
    kanNum_++;
  }
}
/***********************************************************************//**
	@brief 槓することができるか調べる
	@return 槓できるとき真
***************************************************************************/
bool Game::canKan() const {
  return getRest() >= 2 && kanNum_ < 4;
}
/***********************************************************************//**
	@brief コマンドを生成する
	@param[in] type タイプ
	@return 生成したコマンド
***************************************************************************/
Command Game::createCommand(Command::Type type) {
  return Command(type, ++serial_);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Game::setTurn(size_t seat) {
  super::setTurn(seat);
  auto player = getPlayer(seat);
  if(!player->isRichi()) {
    player->setFuriten(false);
  }
}
/***********************************************************************//**
	@brief 牌山の残り枚数を取得する
	@return 牌山の残り枚数
***************************************************************************/
size_t Game::getRest() const {
  return yama_.getRest();
}
/***********************************************************************//**
	@brief オーラス？
	@return オーラスのとき真
***************************************************************************/
bool Game::isLastKyoku() const {
  return getRound() + 1 >= getConfig()->getRoundMax() && 
    getOya() + 1 >= countPlayer();
}
/***********************************************************************//**
        @brief 実行
***************************************************************************/
void Game::operator()() {
  Thread::operator()();
  auto self = getThis();
  auto& deltaTime = getConfig()->getDeltaTime();
  do {
    if(!updateJob(std::chrono::milliseconds(deltaTime))) {
      break;
    }
  } while(sleep(deltaTime));
  clearPlayer();
  detach();
  getServer()->onEndGame(self);
}
/***********************************************************************//**
	@brief 全員にコマンドを送る
	@param[in] command コマンド
	@param[in] isLog ログに残すとき真
***************************************************************************/
void Game::sendAll(const Command& command, bool isLog) {
  if(isLog) {
    log(LEVEL_UMP, command.toString(false));
  }
  for(auto& player : getPlayers()) {
    if(player) {
      sendCommand(std::static_pointer_cast<Player>(player), command);
    }
  }
}
/***********************************************************************//**
	@brief コマンドを送る
	@param[in] player 送信先のプレイヤー
	@param[in] command 送信するコマンド
	@param[in] isLog ログに残すとき真
***************************************************************************/
bool Game::sendCommand(std::shared_ptr<Player> player, 
                       const Command& command, 
                       bool isLog) {
  if(isLog) {
    log(LEVEL_UMP, command.toString(false));
  }
  return player->sendCommand(command);
}
/***********************************************************************//**
	@brief コマンドを受信した
	@param[in] player 受信したプレイヤー
	@param[in] command 受信したコマンド
***************************************************************************/
void Game::onRecvCommand(std::shared_ptr<Player> player, 
                         const Command& command) {
}
/***********************************************************************//**
	@brief プレイヤーが切断した
	@param[in] player 切断したプレイヤー
***************************************************************************/
void Game::onDisconnectPlayer(std::shared_ptr<Player> player) {
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<Game> Game::getThis() {
  return std::static_pointer_cast<Game>(shared_from_this());
}
/***********************************************************************//**
	@brief 空いている席をランダムで取得する
***************************************************************************/
size_t Game::findSeat() const {
  std::vector<size_t> seats;
  for(size_t i = 0, n = getConfig()->getPlayerNum(); i < n; i++) {
    if(!getPlayer(i)) {
      seats.push_back(i);
    }
  }
  assert(!seats.empty());
  std::uniform_int_distribution<size_t> random(0, seats.size() - 1);
  return seats.at(random(getRandom()));
}
/***********************************************************************//**
	@brief ジョブを実行する
	@param[in] deltaTime デルタ時間
	@return ジョブが存在するとき真
***************************************************************************/
bool Game::updateJob(const std::chrono::milliseconds& deltaTime) {
  std::lock_guard<std::mutex> lock(mutex_);
  if(!jobs_.empty()) {
    auto& job = jobs_.top();
    auto nextJob = job->update(deltaTime);
    if(nextJob != job.get()) {
      job->onEnd();
      jobs_.pop();
      if(nextJob) {
        beginJob(nextJob);
      }
    }
    return true;
  }
  return false;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Game::stopAllJob() {
  std::lock_guard<std::mutex> lock(mutex_);
  while(!jobs_.empty()) {
    jobs_.pop();
  }
}
/***********************************************************************//**
	@brief デバッグ表示
***************************************************************************/
void Game::dump() const {
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    printf("[%s] %s\n", Command::SeatToString(i).c_str(), 
           getPlayer(i)->toString().c_str());
  }
}
/***********************************************************************//**
	@brief ドラを追加する
	@param[in] type コマンド
***************************************************************************/
void Game::appendDora(Command::Type type) {
  auto hai = yama_.dora();
  auto dora = getDora(hai);
  super::appendDora(dora);
  auto command = createCommand(type);
  command.append(dora->toString());
  command.append(hai->toString());
  sendAll(command);
}
/***********************************************************************//**
	@brief ドラを取得する
	@param[in] hai ドラ表示牌
	@return ドラ
***************************************************************************/
const mj::Hai* Game::getDora(const mj::Hai* hai) const {
  assert(getConfig());
  for(auto dora = hai->rotate(1);
      dora != hai;
      dora = dora->rotate(1)) {
    if(getConfig()->getHaiNum(dora) > 0 ||
       getConfig()->getHaiNum(dora->getDora()) > 0) {
      return dora;
    }
  }
  return nullptr;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
