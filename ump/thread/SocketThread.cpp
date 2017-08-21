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
#include "ump/thread/Thread.hpp"

namespace ump {
namespace thread {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] socket ソケット
***************************************************************************/
SocketThread::SocketThread(std::shared_ptr<socket::Socket> socket, 
                           const std::string& threadName)
  : socket_(socket), 
    threadName_(threadName)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
SocketThread::~SocketThread() {
  stopThread();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::operator()(std::shared_ptr<void> self) {
  Thread::SetThreadName(threadName_.c_str());
  do {
    if(!onUpdateThread()) {
      break;
    }
  } while(thread_->sleep(100));
  if(!thread_->isStop()) {
    thread_->detach();
  }
  onEndThread();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::startThread(std::shared_ptr<void> self) {
  stopThread();
  thread_.reset(new Thread());
  thread_->start(new std::thread(std::ref(*this), self));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void SocketThread::stopThread() {
  if(thread_) {
    if(socket_) {
      socket_->close();
    }
    thread_->stop();
    thread_.reset();
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool SocketThread::onUpdateThread() {
  Command command;
  if(socket_ && socket_->recvCommand(command)) {
    onRecvCommand(command);
  }
  return true;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
