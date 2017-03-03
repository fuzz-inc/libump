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
#include "ump/Version.hpp"
#include "ump/client/Client.hpp"
#include "ump/mj/Config.hpp"
#include "ump/mj/Player.hpp"
#include "ump/socket/Socket.hpp"

namespace ump {
namespace client {
/***********************************************************************//**
	@brief デフォルトコンストラクタ
	@param[in] config 設定
	@param[in] socket ソケット
***************************************************************************/
Client::Client(std::shared_ptr<const mj::Config> config, 
               std::shared_ptr<socket::Socket> socket)
  : super(config), 
    socket_(socket), 
    hello_(Command::TYPE_HELLO), 
    rest_(0)
{
  hello_.setOption("ump", ump::Version::Get().toString());
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Client::~Client() {
  close();
}
/***********************************************************************//**
	@brief サーバに接続する
	@param[in] host ホスト名
	@param[in] port ポート番号
        @return 接続に成功したら真
***************************************************************************/
bool Client::open(const char* host, int port) {
  close();
  if(socket_->connect(host, port)) {
    flag_.reset();
    thread_.reset(new std::thread(std::ref(*this), getThis()));
    return true;
  }
  return false;
}
/***********************************************************************//**
	@brief 接続を閉じる
***************************************************************************/
void Client::close() {
  socket_->close();
  while(thread_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
/***********************************************************************//**
	@brief 接続中か調べる
	@return	接続しているとき真
***************************************************************************/
bool Client::isOpen() const {
  return socket_->isOpen();
}
/***********************************************************************//**
	@brief スレッド処理
***************************************************************************/
void Client::operator()(std::shared_ptr<Client> self) {
  while(socket_->isOpen()) {
    Command command;
    if(socket_->recvCommand(command)) {
      if(command.isExist()) {
        onRecvCommand(command);
      }
    }
    else {
      socket_->close();
    }
  }
  thread_.release()->detach();
}
/***********************************************************************//**
	@brief 表示名をセットする
	@param[in] name 表示名
	@return 自分自身
***************************************************************************/
Client& Client::setName(const std::string& name) {
  hello_.setOption("name", name);
  return *this;
}
/***********************************************************************//**
	@brief 自身の席を取得する
	@return 自身の席
***************************************************************************/
size_t Client::getSeat() const {
  return getPlayer()->getSeat();
}
/***********************************************************************//**
	@brief 返答を送る
	@param[in] reply 返答コマンド
	@param[in] command 受信コマンド
***************************************************************************/
void Client::replyCommand(Command reply, const Command& command) {
  reply.setSerial(command.getSerial());
  onReplyCommand(reply);
  send(reply);
}
/***********************************************************************//**
	@brief デバッグ表示
***************************************************************************/
void Client::dump() const {
  for(size_t i = 0, n = countPlayer(); i < n; i++) {
    auto player = getPlayer(i);
    printf("%s %s %d\n", 
           player->getSeatString().c_str(), 
           player->getName().c_str(), 
           player->getPoint());
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
	@brief コマンド受信
	@param[in] command 受信したコマンド
	@return 中断するとき偽
***************************************************************************/
bool Client::onRecvCommand(const Command& command) {
  switch(command.getType()) {
  case Command::TYPE_HELLO:
    replyCommand(hello_, command);
    break;
  case Command::TYPE_GAMESTART:
    execGameStart(command);
    onGameStart();
    break;
  case Command::TYPE_GAMEEND:
    flag_.set(FLAG_GAME_END);
    socket_->close();
    onGameEnd();
    break;
  case Command::TYPE_KYOKUSTART:
    execKyokuStart(command);
    break;
  case Command::TYPE_POINT:
    execPoint(command);
    break;
  case Command::TYPE_HAIPAI:
    execHaipai(command);
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
    {
      auto player = getPlayer();
      if(command.hasArg(Command::TYPE_ANKAN) ||
         command.hasArg(Command::TYPE_KAKAN)) {
        kanPattern_ = player->getKanPattern();
      }
    }
    break;

  default:
    break;
  }
  return true;
}
/***********************************************************************//**
	@brief コマンドを返信した
	@param[in] command 返信するコマンド
***************************************************************************/
void Client::onReplyCommand(const Command& command) {
}
/***********************************************************************//**
	@copydoc Game::beginKyoku
***************************************************************************/
void Client::beginKyoku() {
  super::beginKyoku();
  rest_ = 0;
  hideHaiNums_.clear();
  for(auto iter : getConfig().getHaiNums()) {
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
	コマンドをサーバに送る
	@param	command	送信するコマンド
***************************************************************************/
void Client::send(const Command& command) {
  socket_->sendCommand(command);
}
/***********************************************************************//**
	@brief ゲーム(半荘)開始
	@param[in] command コマンド
***************************************************************************/
void Client::execGameStart(const Command& command) {
  if(command.hasOption("id")) {
    setId(command.getOption("id"));
  }
  clearPlayer();
  auto seat = Command::StringToSeat(command.getArg(0).c_str());
  for(size_t i = 1, n = command.countArg(); i < n; i++) {
    auto player = createPlayer();
    player->
      setSeat(i - 1).
      setName(command.getArg(i));
    appendPlayer(player);
  }
  player_ = getPlayer(seat);
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
    player->setPoint(atoi(point + 1));
    break;
  case '+':
  case '-':
    player->addPoint(atoi(point));
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 配牌
***************************************************************************/
void Client::execHaipai(const Command& command) {
  auto player = getPlayer(command.getArg(0).c_str());
  if(command.countArg() > 1) {
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
                ? mj::Hai::Get(command.getArg(2)) : 0);
}
/***********************************************************************//**
	捨て牌.
***************************************************************************/
void Client::execSutehai(const Command& command) {
  super::sutehai(*getPlayer(command.getArg(0).c_str()), 
                 mj::Sutehai(mj::Hai::Get(command.getArg(1))).
                 setTsumogiri(command.hasArg(Command::TYPE_TSUMOGIRI)).
                 setRichi(command.hasArg(Command::TYPE_RICHI)));
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
  if(command.hasArg(Command::TYPE_KAN) || 
     command.hasArg(Command::TYPE_PON) ||
     command.hasArg(Command::TYPE_CHI)) {
    getLastSutehai()->setNaki();
  }
  else if(command.hasArg(Command::TYPE_RON)) {
    getLastSutehai()->setRon();
  }
  else if(command.hasArg(Command::TYPE_RICHI)) {
    //player->setSayRichi(true);
  }
  else if(command.hasArg(Command::TYPE_TENPAI)) {
    player->sayTenpai();
  }
  else if(command.hasArg(Command::TYPE_NOTEN)) {
    //player->setClose();
  }
}
/***********************************************************************//**
	@brief 和了
***************************************************************************/
void Client::execAgari(const Command& command) {
  //Player* player = getPlayer(command.getArg(0).c_str());
  //player->setOpen();
}
/***********************************************************************//**
	流局
***************************************************************************/
void Client::execRyukyoku(const Command& command) {
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
