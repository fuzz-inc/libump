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

#include "ump/mj/Hai.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 捨て牌
***************************************************************************/
class Sutehai {
 private:
  enum {
    FLAG_TSUMOGIRI, 
    FLAG_RICHI, 
    FLAG_NAKI, 
    FLAG_MAX
  };

 private:
  const Hai* hai_;
  std::bitset<FLAG_MAX> flag_;

 public:
  Sutehai() : hai_(nullptr) {}
  Sutehai(const Hai* hai) : hai_(hai) {}
  Sutehai(const Sutehai& src) = default;
  ~Sutehai() = default;

  UMP_GETTER(Hai, hai_);
  void setHai(const Hai* hai);

  UMP_BIT_ACCESSOR(Tsumogiri, flag_, FLAG_TSUMOGIRI);
  UMP_BIT_ACCESSOR(Richi, flag_, FLAG_RICHI);
  UMP_BIT_ACCESSOR(Naki, flag_, FLAG_NAKI);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
