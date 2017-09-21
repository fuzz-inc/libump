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
#include "ump/server/Config.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Config::Config()
  : point_(25000), 
    notenBappu_(3000), 
    roundMax_(1), 
    doraNum_(1), 
    uma_({ 10000, -20000, -40000, -50000 }), 
    deltaTime_(10), 
    speed_(1.0f), 
    diceWait_(1.0f), 
    haipaiWait_(0.1f), 
    sutehaiWait_(10), 
    nakiWait_(10), 
    tenpaiWait_(10), 
    readyWait_(10), 
    sayWait_(1), 
    agariWait_(1), 
    uraDoraWait_(1)
{
  setAgariyame(true);
  setCanChi(true);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Config& Config::setDoraNum(size_t num) {
  doraNum_ = num;
  setWanpaiNum((6 + num) * 2);
  return *this;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
