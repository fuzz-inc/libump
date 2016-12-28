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
#include "ump/server/Job.hpp"
#include "ump/server/JobGame.hpp"
#include "ump/server/Player.hpp"
#include "ump/server/Server.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] config 設定
	@param[in] server サーバー
***************************************************************************/
Game::Game(std::shared_ptr<const Config> config, Server& server)
  : super(config), 
    server_(server), 
    yama_(*config)
{
  setId(createId());
  if(!getConfig().getLogPrefix().empty()) {
    openLogFile(getLogPath(getId()));
  }
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Game::~Game() {
  assert(!thread_);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
const Config& Game::getConfig() const {
  return static_cast<const Config&>(super::getConfig());
}
/***********************************************************************//**
	@brief ゲームを開始する
***************************************************************************/
void Game::start() {
  shufflePlayer();
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getPlayer(i);
    player->
      setSeat(i).
      setPoint(getConfig().getPoint());
  }
  beginJob(new JobGame(*this));
  assert(!thread_.get());
  thread_.reset(new std::thread(std::ref(*this)));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Game::stop() {
  flag_.set(FLAG_STOP);
  if(thread_) {
    thread_->join();
    thread_.reset();
  }
}
/***********************************************************************//**
	@brief ジョブを開始する
	@param[in] 開始するジョブ
***************************************************************************/
void Game::beginJob(Job* job) {
  jobs_.push(job);
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
  yama_.shuffle(getRandom(), 
                14 + 30 * (4 - std::min<size_t>(countPlayer(), 4)));
  const mj::Hai* kaze = mj::Hai::Get("1z");
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getTurnPlayer();
    player->setBakaze(getBakaze());
    player->setZikaze(kaze);
    kaze = kaze->rotate(1);
    nextTurn();
  }
}
/***********************************************************************//**
	@brief ドラを追加する
***************************************************************************/
void Game::appendDora() {
  auto dora = yama_.dora();
  super::appendDora(dora);
  Command command(Command::TYPE_DORA);
  command.append(dora->toString());
  sendAll(command);
}
/***********************************************************************//**
	@brief 裏ドラをめくる
***************************************************************************/
void Game::uraDora() {
  for(size_t i = 0, n = getDora().size(); i < n; i++) {
    appendDora();
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
	@brief ツモ
	@return ツモった牌
***************************************************************************/
const mj::Hai* Game::tsumo() {
  const mj::Hai* hai = yama_.tsumo();
  getTurnPlayer()->tsumo(hai);
  return hai;
}
/***********************************************************************//**
	@brief 牌山の残り枚数を取得する
	@return 牌山の残り枚数
***************************************************************************/
size_t Game::getRest() const {
  return yama_.getRest();
}
/***********************************************************************//**
	@brief 全員にコマンドを送る
	@param[in] command コマンド
***************************************************************************/
void Game::sendAll(const Command& command) const {
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    getPlayer(i)->send(command);
  }
}
/***********************************************************************//**
        @brief 実行
***************************************************************************/
void Game::operator()() {
  auto& deltaTime = getConfig().getDeltaTime();
  while(!isStop()) {
    updateJob(deltaTime);
    std::this_thread::sleep_for(deltaTime);
  }
  getServer().onEndGame(this);
}
/***********************************************************************//**
	@brief ジョブを実行する
***************************************************************************/
void Game::updateJob(const std::chrono::milliseconds& deltaTime) {
  if(!jobs_.empty()) {
    auto job = jobs_.top();
    auto nextJob = job->update(deltaTime);
    if(nextJob != job) {
      job->onEnd();
      jobs_.pop();
      if(nextJob) {
        beginJob(nextJob);
      }
    }
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
	@brief ゲームIDを生成する
	@return ゲームID
***************************************************************************/
std::string Game::createId() const {
  uint64_t id;
  auto now = time(nullptr);
  struct tm* tm = localtime(&now);
  id = (1900 + tm->tm_year) * 10000000000UL + 
    (tm->tm_mon + 1) * 100000000UL + 
    tm->tm_mday * 1000000UL + 
    tm->tm_hour * 10000UL + 
    tm->tm_min * 100UL + 
    tm->tm_sec;
  std::string idStr;
  while(true) {
    std::stringstream buff;
    buff << id;
    idStr = buff.str();
    auto path = getLogPath(idStr);
    struct stat status;
    if(stat(path.c_str(), &status) != 0) {
      break;
    }
    id++;
  }
  return idStr;
}
/***********************************************************************//**
	@brief ログ出力パスを取得する
	@param[in] id ゲームID
	@return ログ出力パス
***************************************************************************/
std::string Game::getLogPath(const std::string& id) const {
  return getConfig().getLogPrefix() + id + ".log";
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
