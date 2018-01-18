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
namespace mj {
/***********************************************************************//**
	@brief 設定
***************************************************************************/
class Config {
 private:
  std::map<const Hai*, size_t> haiNums_;
  size_t playerNum_;
  size_t haipaiNum_;
  size_t wanpaiNum_;
  int richiPoint_;

 public:
  Config();
  Config(const Config& src) = default;
  ~Config() = default;

  void setHaiNum(const Hai* hai, size_t num);
  UMP_GETTER(HaiNums, haiNums_);
  size_t getHaiNum(const Hai* hai) const;
  void setDoraHaiNum(const Hai* hai, size_t num);
  void setDoraHaiNum(size_t num, int number = 5);
  size_t getHaiNum() const;

  UMP_ACCESSOR(PlayerNum, playerNum_);
  UMP_ACCESSOR(HaipaiNum, haipaiNum_);
  UMP_ACCESSOR(WanpaiNum, wanpaiNum_);
  UMP_ACCESSOR(RichiPoint, richiPoint_);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
