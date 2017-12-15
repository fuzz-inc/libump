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
#include "ump/mj/Pattern.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 牌列を追加する
	@param[in] hais 追加する牌列
	@return 追加したときは真(重複していたときは偽)
***************************************************************************/
bool Pattern::append(const HaiArray& hais) {
  for(auto& iter : *this) {
    if(iter.isEqual(hais)) {
      return false;
    }
  }
  push_back(hais);
  return true;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Pattern::parse(const HaiArray& hais, size_t num) {
  if(num <= hais.size()) {
    parse(hais, num, HaiArray());
  }
}
/***********************************************************************//**
	@brief 含まれている牌を取得する
	@return 含まれている牌の配列
***************************************************************************/
HaiArray Pattern::getHais() const {
  HaiArray hais;
  for(auto& iter : *this) {
    for(auto hai : iter) {
      if(!hais.isIncludeEqual(hai)) {
        hais.append(hai);
      }
    }
  }
  return hais;
}
/***********************************************************************//**
	@brief 文字列表記を得る
	@return 文字列表記
***************************************************************************/
std::string Pattern::toString() const {
  std::ostringstream stream;
  for(size_t i = 0, n = size(); i < n; i++) {
    if(i > 0) {
      stream << "|";
    }
    stream << at(i).toString();
  }
  return stream.str();
}
/***********************************************************************//**
	@brief 組み合わせを求める
	@param[out] pattern 組み合わせ
	@param[in] num 組み合わせのサイズ
	@param[in] hais 引き継ぎ
***************************************************************************/
void Pattern::parse(const HaiArray& hais, 
                    size_t num, 
                    const HaiArray& prefix) {
  if(num == 0) {
    append(prefix);
  }
  else {
    for(size_t i = 0, n = hais.size(); i < n; i++) {
      HaiArray clone(hais);
      auto hai = clone.at(i);
      clone.erase(clone.begin() + i);
      parse(clone, num - 1, prefix + hai);
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
