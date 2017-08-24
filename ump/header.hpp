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
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#define UMP_PLATFORM "mac"
#define UMP_PLATFORM_MAC
#define UMP_PLATFORM_APPLE
#define UMP_PLATFORM_POSIX
#endif
/***********************************************************************//**
	@brief 
***************************************************************************/
#include <assert.h>
#include <cxxabi.h>
#include <math.h>
#include <netdb.h>
#include <poll.h>
#include <stdarg.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <random>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <vector>
/***********************************************************************//**
	@brief OpenSSL
***************************************************************************/
#include <openssl/bn.h>
/***********************************************************************//**
	@brief 
***************************************************************************/
namespace ump {
class BigNum;
class Command;
class Logger;
class Version;

namespace client {
class Bot;
class Client;
}

namespace mj {
class Agari;
class AgariParser;
class Config;
class Divider;
class Game;
class Hai;
class HaiArray;
class Hand;
class Kawa;
class Mentsu;
class Pattern;
class Player;
class Shanten;
class Sutehai;
class Yama;

typedef std::vector<std::shared_ptr<Player>> Players;
}

namespace server {
class Config;
class Game;
class Job;
class JobAgari;
class JobGame;
class JobHaipai;
class JobKyoku;
class JobNaki;
class JobReady;
class JobRyukyoku;
class JobTsumo;
class Player;
class Server;
}

namespace socket {
class Socket;
class SslSocket;
class TcpSocket;
}
using namespace socket;

namespace thread {
class Condition;
class SocketThread;
class Thread;
}
using namespace thread;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
#define UMP_SETTER(name, var)                                           \
  auto set##name(const decltype(var)& value) -> decltype(*this)& {      \
    var = value;                                                        \
    return *this;                                                       \
  }

#define UMP_GETTER(name, var)                   \
  const decltype(var)& get##name() const {      \
    return var;                                 \
  }

#define UMP_ACCESSOR(name, var)                 \
  UMP_SETTER(name, var);                        \
  UMP_GETTER(name, var);                        \
  decltype(var)& get##name() {                  \
    return var;                                 \
  }

#define UMP_BIT_SETTER(name, var, flag)                         \
  auto set##name(bool value = true) -> decltype(*this)& {       \
    var.set(flag, value);                                       \
    return *this;                                               \
  }

#define UMP_BIT_GETTER(name, var, flag)         \
  bool is##name() const {                       \
    return var.test(flag);                      \
  }

#define UMP_BIT_ACCESSOR(name, var, flag)       \
  UMP_BIT_SETTER(name, var, flag);              \
  UMP_BIT_GETTER(name, var, flag)

#define UMP_CAN_SETTER(name, var, flag)                         \
  auto setCan##name(bool value = true) -> decltype(*this)& {    \
    var.set(flag, value);                                       \
    return *this;                                               \
  }

#define UMP_CAN_GETTER(name, var, flag)         \
  bool can##name() const {                      \
    return var.test(flag);                      \
  }

#define UMP_CAN_ACCESSOR(name, var, flag)       \
  UMP_CAN_SETTER(name, var, flag);              \
  UMP_CAN_GETTER(name, var, flag)
/***********************************************************************//**
	$Id$
***************************************************************************/
