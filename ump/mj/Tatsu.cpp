/*
Copyright 2018 fuzz, Inc. All rights reserved. 
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
#include "ump/mj/Tatsu.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] hais 牌列
***************************************************************************/
Tatsu::Tatsu(const HaiArray& hais)
  : super(hais)
{
  assert(hais.size() == 2);
  sort();
  if(at(0)->isSame(at(1))) {
    type_ = TOITSU;
  }
  else {
    assert(!at(0)->isZihai());
    if(at(0)->shift(1)->isSame(at(1))) {
      if(at(0)->getNumber() == 1 || at(0)->getNumber() == 8) {
        type_ = PENCHAN;
      }
      else {
        type_ = RYANMEN;
      }
    }
    else if(at(0)->shift(2)->isSame(at(1))) {
      type_ = KANCHAN;
    }
    else {
      assert(0);
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
