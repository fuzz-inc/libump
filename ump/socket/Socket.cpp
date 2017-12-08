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
#include "ump/socket/Socket.hpp"

namespace ump {
namespace socket {
const char Socket::EOL[Socket::EOL_SIZE + 1] = { 0x0d, 0x0a, 0 };
/***********************************************************************//**
	@brief 接続する
	@param[in] host ホスト名
	@param[in] port ポート番号
	@return 成功したとき真
***************************************************************************/
bool Socket::connect(const std::string& host, int port) {
  close();
  std::lock_guard<std::mutex> lock(getMutex());
  return onConnect(host, port);
}
/***********************************************************************//**
	@brief サーバーとしてポートを開く
	@param[in] port ポート番号
	@return 成功したとき真
***************************************************************************/
bool Socket::listen(int port) {
  close();
  std::lock_guard<std::mutex> lock(getMutex());
  return onListen(port);
}
/***********************************************************************//**
	@brief サーバに接続してきたクライアントとのソケットを返す
	@param[in] timeout タイムアウト時間(ミリ秒)
	@return	接続されたソケット(もしくはnullptr)
***************************************************************************/
std::shared_ptr<Socket> Socket::accept(int timeout) {
  if(isOpen()) {
    std::lock_guard<std::mutex> lock(getMutex());
    return onAccept(timeout);
  }
  return nullptr;
}
/***********************************************************************//**
	@brief 閉じる
***************************************************************************/
void Socket::close() {
  if(isOpen()) {
    //std::lock_guard<std::mutex> lock(getMutex());
    onClose();
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Socket::poll(int flag, int timeout) {
  return true;
}
/***********************************************************************//**
	@brief コマンドを送信する
	@param[in] command 送信するコマンド
***************************************************************************/
bool Socket::sendCommand(const Command& command) {
  std::lock_guard<std::mutex> lock(getMutex());
  auto message = command.toString(true) + EOL;
  if(send(message.data(), message.size())) {
    if(auto data = command.getData()) {
      return send(data->data(), data->size());
    }
    return true;
  }
  return false;
}
/***********************************************************************//**
	@brief コマンドを受信する
***************************************************************************/
bool Socket::recvCommand(Command& command) {
  std::string message;
  if(!recvLine(message)) {
    return false;
  }
  if(command.parse(message.c_str())) {
    auto dataSize = command.getDataSize();
    if(dataSize > 0) {
      auto data = std::make_shared<std::vector<char>>(dataSize);
      if(!recv(data->data(), dataSize)) {
        return false;
      }
      command.setData(data);
    }
  }
  return true;
}
/***********************************************************************//**
	@brief データを送信する
	@param[in] buff 送信するデータ
	@param[in] size 送信するサイズ
	@return 成功したとき真
***************************************************************************/
bool Socket::send(const char* buff, size_t size) {
  if(isOpen()) {
    return onSend(buff, size);
  }
  return false;
}
/***********************************************************************//**
	@brief データを受信する
	@param[out] buff 受信バッファ
	@param[in] size 受信バッファのサイズ
	@return 成功したとき真
***************************************************************************/
bool Socket::recv(char* buff, size_t size) {
  if(isOpen()) {
    return onRecv(buff, size);
  }
  return false;
}
/***********************************************************************//**
	@brief 1行読み込み
	@param[out] msg 受信した文字列
***************************************************************************/
bool Socket::recvLine(std::string& msg) {
  char buff[EOL_SIZE];
  if(!recv(buff, EOL_SIZE)) {
    return false;
  }
  while(strncmp(buff, EOL, EOL_SIZE) != 0) {
    msg.append(1, buff[0]);
    memmove(buff, &buff[1], EOL_SIZE - 1);
    if(!recv(&buff[EOL_SIZE - 1], 1)) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
