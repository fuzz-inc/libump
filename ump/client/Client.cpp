/*
Copyright 2017 fuzz, Inc. All rights reserved. 
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
#include "ump/Logger.hpp"
#include "ump/Version.hpp"
#include "ump/client/Client.hpp"
#include "ump/mj/Config.hpp"
#include "ump/mj/Player.hpp"
#include "ump/socket/Socket.hpp"

namespace ump {
namespace client {
/***********************************************************************//**
	@brief デフォルトコンストラクタ
	@param[in] socket ソケット
***************************************************************************/
Client::Client(std::shared_ptr<Socket> socket)
  : SocketThread(socket, this), 
    hello_(Command::TYPE_HELLO), 
    state_(STATE_NULL), 
    seat_(0), 
    rest_(0)
{
  hello_.setOption("ump", ump::Version::Get().toString());
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Client::~Client() {
}
/***********************************************************************//**
	@brief サーバに接続する
	@param[in] host ホスト名
	@param[in] port ポート番号
        @return 接続に成功したら真
***************************************************************************/
bool Client::open(const char* host, int port) {
  if(SocketThread::connect(host, port)) {
    start();
    return true;
  }
  return false;
}
/***********************************************************************//**
	@brief 接続を閉じる
***************************************************************************/
void Client::close() {
  stop();
  state_ = STATE_NULL;
}
/***********************************************************************//**
	@brief 表示名をセットする
	@param[in] name 表示名
***************************************************************************/
void Client::setName(const std::string& name) {
  hello_.setOption("name", name);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<mj::Player> Client::getPlayer() const {
  return getPlayer(getSeat());
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Client::sendHello() {
  return sendCommand(getHello());
}
/***********************************************************************//**
	コマンドをサーバに送る
	@param	command	送信するコマンド
***************************************************************************/
bool Client::sendCommand(const Command& command) {
  if(auto logger = getLogger()) {
    logger->log(Logger::LEVEL_DEBUG, command.toString(false));
  }
  return SocketThread::sendCommand(command);
}
/***********************************************************************//**
	@brief 返答を送る
	@param[in] reply 返答コマンド
	@param[in] command 受信コマンド
***************************************************************************/
bool Client::replyCommand(Command reply, const Command& command) {
  reply.setSerial(command.getSerial());
  onReplyCommand(reply);
  return sendCommand(reply);
}
/***********************************************************************//**
	@brief デバッグ表示
***************************************************************************/
void Client::dump() const {
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getPlayer(i);
    printf("%s %s %s\n", 
           player->getSeatString().c_str(), 
           player->getName().c_str(), 
           player->getPoint().toString().c_str());
    printf("%s\n", player->toString().c_str());
  }
}
/***********************************************************************//**
	@brief プレイヤーを生成する
***************************************************************************/
std::shared_ptr<mj::Player> Client::createPlayer() {
  return std::make_shared<mj::Player>();
}
/***********************************************************************//**
	@brief プレイヤーがセットされた
	@param[in] player プレイヤー
***************************************************************************/
void Client::onSetPlayer(std::shared_ptr<mj::Player> player) {
}
/***********************************************************************//**
	@brief コマンド受信
	@param[in] command 受信したコマンド
	@return 中断するとき偽
***************************************************************************/
void Client::onRecvCommand(const Command& command) {
  if(command.getSerial() != 0) {
    setReconnect(command.isReconnect());
  }
  switch(command.getType()) {
  case Command::TYPE_HELLO:
    replyCommand(hello_, command);
    break;
  case Command::TYPE_SEAT:
    clearPlayer();
    seat_ = Command::StringToSeat(command.getArg(0).c_str());
    if(command.hasOption(Command::OPTION_GAMEID)) {
      setId(command.getOption(Command::OPTION_GAMEID));
    }
    break;
  case Command::TYPE_PLAYER:
    execPlayer(command);
    break;
  case Command::TYPE_GAMESTART:
    execGameStart(command);
    break;
  case Command::TYPE_GAMEEND:
    execGameEnd(command);
    break;
  case Command::TYPE_KYOKUSTART:
    execKyokuStart(command);
    break;
  case Command::TYPE_POINT:
  case Command::TYPE_RICHI:
    execPoint(command);
    break;
  case Command::TYPE_HAIPAI:
    execHaipai(command);
    break;
  case Command::TYPE_DORA:
  case Command::TYPE_URADORA:
    appendDora(mj::Hai::Get(command.getArg(0)));
    break;
  case Command::TYPE_TSUMO:
    execTsumo(command);
    break;
  case Command::TYPE_OPEN:
    execOpen(command);
    break;
  case Command::TYPE_SUTEHAI:
    execSutehai(command);
    break;
  case Command::TYPE_SAY:
    execSay(command);
    break;
  case Command::TYPE_AGARI:
    execAgari(command);
    break;
  case Command::TYPE_RYUKYOKU:
    execRyukyoku(command);
    break;

  case Command::TYPE_READY_Q:
    break;
  case Command::TYPE_TENPAI_Q:
    break;
  case Command::TYPE_NAKI_Q:
    {
      auto player = getPlayer();
      auto hai = mj::Hai::Get(command.getArg(0));
      if(command.hasArg(Command::TYPE_PON)) {
        ponPattern_ = player->getPonPattern(hai);
      }
      if(command.hasArg(Command::TYPE_CHI)) {
        chiPattern_ = player->getChiPattern(hai);
      }
      if(command.hasArg(Command::TYPE_KAN)) {
        kanPattern_ = player->getKanPattern(hai);
      }
    }
    break;
  case Command::TYPE_SUTEHAI_Q:
    onRecvSutehaiQ(command);
    break;

  default:
    break;
  }
}
/***********************************************************************//**
	@brief コマンドを返信した
	@param[in] command 返信するコマンド
***************************************************************************/
void Client::onReplyCommand(const Command& command) {
}
/***********************************************************************//**
	@copydoc Game::onEndGame
***************************************************************************/
void Client::onEndGame(const mj::Players& players) {
  state_ = STATE_END;
  super::onEndGame(players);
}
/***********************************************************************//**
	@copydoc Game::beginKyoku
***************************************************************************/
void Client::beginKyoku() {
  assert(getConfig());
  super::beginKyoku();
  rest_ = 0;
  hideHaiNums_.clear();
  for(auto iter : getConfig()->getHaiNums()) {
    hideHaiNums_[iter.first->getNormal()] = iter.second;
  }
}
/***********************************************************************//**
	@copydoc Game::onShowHai
***************************************************************************/
void Client::onShowHai(const mj::Hai* hai) {
  super::onShowHai(hai);
  hideHaiNums_[hai->getNormal()]--;
}
/***********************************************************************//**
	@brief playerコマンドを実行する
	@param[in] command コマンド
***************************************************************************/
void Client::execPlayer(const Command& command) {
  auto seat = Command::StringToSeat(command.getArg(0).c_str());
  auto player = createPlayer();
  player->setName(command.getArg(1));
  setPlayer(seat, player);
  if(seat == getSeat()) {
    state_ = STATE_SEAT;
  }
  onSetPlayer(player);
}
/***********************************************************************//**
	@brief ゲーム(半荘)開始
	@param[in] command コマンド
***************************************************************************/
void Client::execGameStart(const Command& command) {
  state_ = STATE_GAME;
  onGameStart();
}
/***********************************************************************//**
	@brief ゲーム終了
	@param[in] command コマンド
***************************************************************************/
void Client::execGameEnd(const Command& command) {
  mj::Players players;
  for(size_t i = 0, n = command.countArg(); i < n; i += 2) {
    auto player = getPlayer(command.getArg(i).c_str());
    player->setPoint(BigNum(command.getArg(i + 1)));
    players.push_back(player);
  }
  onEndGame(players);
}
/***********************************************************************//**
	@brief 局開始
	@param[in] command コマンド
***************************************************************************/
void Client::execKyokuStart(const Command& command) {
  beginKyoku();
  setRound(atoi(command.getArg(0).c_str()));
  setOya(Command::StringToSeat(command.getArg(1).c_str()));
  setRenchan(atoi(command.getArg(2).c_str()));
  setKyotaku(atoi(command.getArg(3).c_str()));
  auto bakaze = mj::Hai::Get(command.getArg(4).c_str());
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getPlayer(i);
    player->reset();
    player->setBakaze(bakaze);
    player->setZikaze(mj::Hai::Get(command.getArg(5 + i).c_str()));
  }
}
/***********************************************************************//**
	@brief ポイント
	@param[in] command コマンド
***************************************************************************/
void Client::execPoint(const Command& command) {
  auto player = getPlayer(command.getArg(0).c_str());
  const char* point = command.getArg(1).c_str();
  switch(point[0]) {
  case '=':
    player->setPoint(BigNum(point + 1));
    break;
  case '+':
    player->addPoint(BigNum(point + 1));
    break;
  case '-':
    player->addPoint(BigNum(point));
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 配牌
***************************************************************************/
void Client::execHaipai(const Command& command) {
  if(command.countArg() > 1) {
    auto player = getPlayer(command.getArg(0).c_str());
    player->drawHaipai(mj::HaiArray(command.getArg(1).c_str()));
  }
}
/***********************************************************************//**
	自摸.
***************************************************************************/
void Client::execTsumo(const Command& command) {
  auto seat = Command::StringToSeat(command.getArg(0).c_str());
  setTurn(seat);
  auto player = getPlayer(seat);
  rest_ = std::atoi(command.getArg(1).c_str());
  player->tsumo((command.countArg() > 2)
                ? mj::Hai::Get(command.getArg(2)) : nullptr, 
                command.hasArg(Command::TYPE_RINSHAN));
}
/***********************************************************************//**
	捨て牌.
***************************************************************************/
void Client::execSutehai(const Command& command) {
  mj::Sutehai sutehai(mj::Hai::Get(command.getArg(1)));
  sutehai.setTsumogiri(command.hasArg(Command::TYPE_TSUMOGIRI));
  sutehai.setRichi(command.hasArg(Command::TYPE_RICHI));
  super::sutehai(*getPlayer(command.getArg(0).c_str()), sutehai);
}
/***********************************************************************//**
	手牌を晒す.
***************************************************************************/
void Client::execOpen(const Command& command) {
  auto player = getPlayer(command.getArg(0).c_str());
  player->open(command.getArg(1).c_str(), 
               (command.countArg() > 2)
               ? mj::Hai::Get(command.getArg(2)) : nullptr);
  setTurn(player->getSeat());
}
/***********************************************************************//**
	@brief 発声
***************************************************************************/
void Client::execSay(const Command& command) {
  auto seat = Command::StringToSeat(command.getArg(0).c_str());
  auto player = getPlayer(seat);
  if(command.hasArg(Command::TYPE_TENPAI)) {
    player->sayTenpai();
  }
}
/***********************************************************************//**
	@brief 和了
***************************************************************************/
void Client::execAgari(const Command& command) {
}
/***********************************************************************//**
	@brief 流局
***************************************************************************/
void Client::execRyukyoku(const Command& command) {
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Client::onRecvSutehaiQ(const Command& command) {
  auto player = getPlayer();
  {
    std::string hais;
    if(command.getOption(Command::ToString(Command::TYPE_RICHI), hais)) {
      richiPattern_.parse(mj::HaiArray(hais.c_str()), 1);
    }
    else {
      richiPattern_.clear();
    }
  }
  if(command.hasArg(Command::TYPE_ANKAN) ||
     command.hasArg(Command::TYPE_KAKAN)) {
    kanPattern_ = player->getKanPattern();
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
