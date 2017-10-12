﻿/*
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
/***********************************************************************//**
	@brief 
***************************************************************************/
SocketThread::SocketThread(std::shared_ptr<Socket> socket)
  : socket_(socket)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
SocketThread::~SocketThread() {
  stop();
}
/***********************************************************************//**
	@brief ソケットを再設定する
	@param[in] socket ソケット
***************************************************************************/
std::shared_ptr<Socket>
SocketThread::resetSocket(std::shared_ptr<Socket> socket) {
  decltype(socket_) oldSocket;
  socket_.swap(oldSocket);
  stop();
  socket_ = socket;
  start();
  return oldSocket;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Socket& SocketThread::getSocket() const {
  assert(socket_);
  return *socket_;
}
/***********************************************************************//**
	@brief 接続している？
	@return 接続しているとき真
***************************************************************************/
bool SocketThread::isConnect() const {
  return socket_ && socket_->isOpen();
}
/***********************************************************************//**
	@brief スレッドを開始する
***************************************************************************/
void SocketThread::start() {
  assert(isConnect());
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
	@brief スレッド処理
***************************************************************************/
void SocketThread::operator()() {
  super::operator()();
  while(isConnect()) {
    onThread();
  }
  detach();
  onDisconnectSocket();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::onThread() {
  Command command;
  if(socket_ && socket_->recvCommand(command)) {
    onRecvCommand(command);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
