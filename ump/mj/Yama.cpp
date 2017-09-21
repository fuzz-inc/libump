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
#include "ump/mj/Config.hpp"
#include "ump/mj/Yama.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] config 設定
***************************************************************************/
Yama::Yama(std::shared_ptr<const Config> config)
  : config_(config)
{
  for(auto iter : config->getHaiNums()) {
    for(size_t i = 0, n = iter.second; i < n; i++) {
      append(iter.first);
    }
  }
}
/***********************************************************************//**
	@brief 洗牌
	@param[in] random 乱数生成器
***************************************************************************/
void Yama::shuffle(std::default_random_engine& random) {
  super::shuffle(random);
  pt_ = 0;
  restNum_ = size() - config_->getWanpaiNum();
}
/***********************************************************************//**
	@brief 配牌を取る
	@param[in] num 枚数
	@return 配牌
***************************************************************************/
HaiArray Yama::getHaipai(size_t num) {
  HaiArray haipai;
  for(size_t i = 0; i < num; i++) {
    haipai.append(tsumo());
  }
  return haipai;
}
/***********************************************************************//**
	@brief ツモる
	@return ツモった牌
***************************************************************************/
const Hai* Yama::tsumo() {
  assert(restNum_ > 0);
  restNum_--;
  return draw();
}
/***********************************************************************//**
	@brief リンシャンツモ
	@return ツモった牌
***************************************************************************/
const Hai* Yama::rinshan() {
  return tsumo();
}
/***********************************************************************//**
	@brief ドラ表示牌をめくる
	@return ドラ表示牌
***************************************************************************/
const Hai* Yama::dora() {
  return draw();
}
/***********************************************************************//**
	@brief 残り枚数を返す
	@return 残り枚数
***************************************************************************/
size_t Yama::getRest() const {
  return restNum_;
}
/***********************************************************************//**
	@brief 牌を引く
***************************************************************************/
const Hai* Yama::draw() {
  return at(pt_++);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
