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
#include "ump/Command.hpp"
#include "ump/socket/Socket.hpp"
#include "ump/thread/SocketThread.hpp"

namespace ump {
namespace thread {
static const int DELTA_TIME = 100;
static const int TIMEOUT = 1 * 60 * 1000;
/***********************************************************************//**
	@brief 
***************************************************************************/
SocketThread::SocketThread(std::shared_ptr<Socket> socket, 
                           Listener* listener)
  : socket_(socket), 
    listener_(listener), 
    timeout_(0)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
SocketThread::~SocketThread() {
  stop();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::connect(const char* host, int port) {
  if(socket_) {
    return socket_->connect(host, port);
  }
  return false;
}
/***********************************************************************//**
	@brief 接続している？
	@return 接続しているとき真
***************************************************************************/
bool SocketThread::isOpen() const {
  return socket_ && socket_->isOpen();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::closeSocket() {
  flag_.set(FLAG_CLOSE);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::sendCommand(const Command& command) {
  if(isOpen()) {
    sendCommands_([&command](std::queue<Command>& queue) {
        queue.emplace(command);
      });
    return true;
  }
  return false;
}
/***********************************************************************//**
	@brief スレッドを開始する
***************************************************************************/
void SocketThread::start() {
  assert(isOpen());
  super::start();
}
/***********************************************************************//**
	@brief スレッドを停止する
***************************************************************************/
void SocketThread::stop() {
  if(socket_) {
    socket_->close();
  }
  super::stop();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::resetListener(Listener* listener) {
  listener_ = listener;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Socket& SocketThread::getSocket() const {
  assert(socket_);
  return *socket_;
}
/***********************************************************************//**
	@brief スレッド処理
***************************************************************************/
void SocketThread::operator()() {
  super::operator()();
  while(isOpen()) {
    onThread();
  }
  detach();
  listener_->onDisconnectSocket();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::onThread() {
  if(!flushCommand()) {
    return;
  }
  if(flag_.test(FLAG_CLOSE)) {
    socket_->close();
  }
  else if(poll(Socket::POLL_RECV)) {
    Command command;
    if(socket_->recvCommand(command)) {
      listener_->onRecvCommand(command);
    }
  }
  else if(isTimeout()) {
    Command command(Command::TYPE_ACK);
    sendCommand(command);
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::flushCommand() {
  Command command;
  while(fetchCommand(command)) {
    if(poll(Socket::POLL_SEND)) {
      if(socket_->sendCommand(command)) {
        dequeueCommand();
      }
      else {
        return false;
      }
    }
    else {
      if(flag_.test(FLAG_CLOSE) || isTimeout()) {
        socket_->close();
      }
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::fetchCommand(Command& command) {
  bool flag = false;
  sendCommands_([&](std::queue<Command>& queue) {
      if(!queue.empty()) {
        command = queue.front();
        flag = true;
      }
    });
  return flag;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::dequeueCommand() {
  sendCommands_([&](std::queue<Command>& queue) {
      queue.pop();
    });
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::poll(int flag) {
  if(socket_->poll(flag, DELTA_TIME)) {
    timeout_ = 0;
    return true;
  }
  timeout_ += DELTA_TIME;
  return false;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::isTimeout() const {
  return timeout_ > TIMEOUT;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
