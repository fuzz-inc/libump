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
#pragma once

#include "ump/socket/Socket.hpp"

namespace ump {
namespace socket {
/***********************************************************************//**
	@brief TCPソケット
***************************************************************************/
class TcpSocket
  : public Socket
{
  typedef Socket super;

 public:
  static const int INVALID_FD = -1;

 private:
  int fd_;
  int port_;

 public:
  TcpSocket();
  TcpSocket(int fd);
  ~TcpSocket() override;

  UMP_GETTER(Port, port_);

  bool isOpen() const override;

 protected:
  bool onConnect(const std::string& host, int port) override;
  bool onListen(int port) override;
  std::shared_ptr<Socket> onAccept(int timeout) override;

  bool poll(int flag, int timeout) override;

  bool onSend(const char* buff, size_t size) override;
  bool onRecv(char* buff, size_t size) override;

  void onClose() override;

  virtual std::shared_ptr<TcpSocket> createSocket(int fd) const;

 private:
  void open();
  void setError();
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
