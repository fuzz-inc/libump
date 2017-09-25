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
	$Revision$
	$Author$
	$Date::                           $
***************************************************************************/
#include "ump/mj/HaiArray.hpp"

namespace ump {
namespace mj {
const HaiArray HaiArray::YAOCHU("1m9m1p9p1s9s1z2z3z4z5z6z7z");
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] str 文字列
***************************************************************************/
HaiArray::HaiArray(const char* str) {
  for(size_t i = 0, n = strlen(str); i < n; i += 2) {
    append(Hai::Get(&str[i]));
  }
}
/***********************************************************************//**
	@brief コピーコンストラクタ
	@param[in] src コピー元
***************************************************************************/
HaiArray::HaiArray(const HaiArray& src)
  : super(src)
{
}
/***********************************************************************//**
	@brief 牌を追加する
	@param[in] hai 追加する牌
***************************************************************************/
void HaiArray::append(const Hai* hai) {
  assert(hai);
  push_back(hai);
}
/***********************************************************************//**
	@brief 配列を連結する.
	@param[in] src 連結する配列
***************************************************************************/
void HaiArray::append(const HaiArray& src) {
  insert(end(), src.begin(), src.end());
}
/***********************************************************************//**
	@brief 完全にユニークな牌を数える
	@return	完全にユニークな牌の数
***************************************************************************/
size_t HaiArray::countUniqueEqual() const {
  HaiArray uniq = *this;
  uniq.uniqueEqual();
  return uniq.size();
}
/***********************************************************************//**
	@brief 牌が含まれているか調べる
	@param[in] hai 調べる牌
	@return 含まれているとき真
***************************************************************************/
bool HaiArray::isInclude(const Hai* hai) const {
  return (countSame(hai) > 0);
}
/***********************************************************************//**
	@brief 完全に同じ牌が含まれているか調べる
	@param[in] hai 調べる牌
	@return 完全に同じ牌が含まれているとき真
***************************************************************************/
bool HaiArray::isIncludeEqual(const Hai* hai) const {
  return (countEqual(hai) > 0);
}
/***********************************************************************//**
	@brief 伏せ牌が含まれているか調べる
	@return 伏せ牌が含まれるとき真
***************************************************************************/
bool HaiArray::isIncludeUnknown() const {
  return isIncludeEqual(Hai::GetUnknown());
}
/***********************************************************************//**
	@brief ヤオチュウ牌が含まれているか調べる
	@return ヤオチュウ牌が含まれているとき真
***************************************************************************/
bool HaiArray::isIncludeYaochu() const {
  for(auto iter : *this) {
    if(iter->isYaochu()) {
      return true;
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 理牌する
	@return 自分自身
***************************************************************************/
HaiArray& HaiArray::sort() {
  std::sort(begin(), end(), 
            [](const Hai* lhs, const Hai* rhs) {
              return *lhs < *rhs;
            });
  return *this;
}
/***********************************************************************//**
	@brief 洗牌する
	@param[in] random 乱数生成器
***************************************************************************/
void HaiArray::shuffle(std::default_random_engine& random) {
  std::shuffle(begin(), end(), random);
}
/***********************************************************************//**
	@brief 文字列から牌列を追加する
	@param[in] pt 文字列
	@return 文字列の終点
***************************************************************************/
const char* HaiArray::parseString(const char* pt) {
  while(auto hai = Hai::Get(pt)) {
    append(hai);
    pt += Hai::STRING_SIZE;
  }
  return pt;
}
/***********************************************************************//**
	@brief 文字列に変換する
	@return	変換した文字列
***************************************************************************/
std::string HaiArray::toString() const {
  std::string str;
  for(auto iter : *this) {
    str.append(iter->toString());
  }
  return str;
}
/***********************************************************************//**
	@breif 重複を取り除く
	@return	重複を取り除いた自分自身
***************************************************************************/
HaiArray& HaiArray::unique() {
  sort();
  const Hai* hai = nullptr;
  for(auto iter = begin(); iter != end();) {
    if(hai && (*iter)->isSame(hai)) {
      iter = erase(iter);
    }
    else {
      hai = *iter++;
    }
  }
  return *this;
}
/***********************************************************************//**
	@brief 重複を取り除いた配列を返す
	@return ユニークな配列
***************************************************************************/
HaiArray HaiArray::getUnique() const {
  HaiArray uniq(*this);
  uniq.unique();
  return uniq;
}
/***********************************************************************//**
	@breif 完全な重複を取り除く.
	@return 完全な重複を取り除いた自分自身
***************************************************************************/
HaiArray& HaiArray::uniqueEqual() {
  sort();
  const Hai* hai = nullptr;
  for(auto iter = begin(); iter != end();) {
    if(hai && (*iter)->isEqual(hai)) {
      iter = erase(iter);
    }
    else {
      hai = *iter++;
    }
  }
  return *this;
}
/***********************************************************************//**
	@brief 牌を削除する.
	@param[in] hai 削除する牌
	@return 削除した牌(またはnullptr)
***************************************************************************/
const Hai* HaiArray::remove(const Hai* hai, bool isEqual) {
  auto found = std::find_if(begin(), end(), 
                            [&](const Hai* iter) {
                              return iter->isA(hai, isEqual);
                            });
  if(found == end()) {
    return nullptr;
  }
  hai = *found;
  erase(found);
  return hai;
}
/***********************************************************************//**
	@brief 複数の牌を削除する
	@param[in] hais 削除する牌の配列
	@param[in] isEqual 完全同一のみのときは真
***************************************************************************/
void HaiArray::remove(const HaiArray& hais, bool isEqual) {
  for(auto hai : hais) {
    if(!remove(hai, isEqual)) {
      assert(0);
    }
  }
}
/***********************************************************************//**
	@brief 牌を数える
	@param[in] hai 数える牌
	@param[in] isEqual 完全同一のみのときは真
	@return 牌の数
***************************************************************************/
size_t HaiArray::count(const Hai* hai, bool isEqual) const {
  size_t num = 0;
  if(hai) {
    for(auto iter : *this) {
      if(iter->isA(hai, isEqual)) {
        num++;
      }
    }
  }
  return num;
}
/***********************************************************************//**
	@brief 牌列が一致するか調べる
	@param[in] isEqual 完全同一のみのときは真
	@return 牌列が一致するとき真
***************************************************************************/
bool HaiArray::isA(const HaiArray& hais, bool isEqual) const {
  auto clone(*this);
  for(auto hai : hais) {
    if(!clone.remove(hai, isEqual)) {
      return false;
    }
  }
  return clone.empty();
}
/***********************************************************************//**
	@brief 牌列が含まれているか調べる
	@param[in] isEqual
	@return 牌列が含まれているとき真
***************************************************************************/
bool HaiArray::hasA(const HaiArray& hais, bool isEqual) const {
  auto clone(*this);
  for(auto hai : hais) {
    if(!clone.remove(hai, isEqual)) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
