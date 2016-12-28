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
#pragma once

namespace ump {
namespace socket {
/***********************************************************************//**
	@brief ソケット基底クラス
***************************************************************************/
class Socket {
 public:
  static const int EOL_SIZE = 2;
  static const char EOL[EOL_SIZE + 1];

 private:
  mutable std::mutex mutex_;
  std::string error_;

 public:
  virtual ~Socket() = default;

  bool connect(const std::string& host, int port);
  bool listen(int port);
  std::shared_ptr<Socket> accept(int timeout);

  bool send(const char* buff, size_t size);
  bool recv(char* buff, size_t size);

  virtual bool isOpen() const = 0;
  void close();

  bool recvLine(std::string& msg);

  const std::string& getError() const {
    return error_;
  }

 protected:
  Socket() = default;

  std::mutex& getMutex() const {
    return mutex_;
  }

  virtual bool onConnect(const std::string& host, int port) = 0;
  virtual bool onListen(int port) = 0;
  virtual std::shared_ptr<Socket> onAccept(int timeout) = 0;

  virtual bool onSend(const char* buff, size_t size) = 0;
  virtual bool onRecv(char* buff, size_t size) = 0;

  virtual void onClose() = 0;

  void setError(const std::string& error) {
    error_ = error;
  }
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
