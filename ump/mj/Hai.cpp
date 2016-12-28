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
#include "ump/mj/Hai.hpp"

namespace ump {
namespace mj {
const char* Hai::COLORS = "mpsz";
Hai Hai::Unknown_;
/***********************************************************************//**
        @brief 牌の種別を取得する
	@return 牌の種別
***************************************************************************/
int Hai::getColor() const {
  return color_;
}
/***********************************************************************//**
        @brief 牌の数字を取得する
	@return 牌の数字
***************************************************************************/
int Hai::getNumber() const {
  return number_;
}
/***********************************************************************//**
	@brief デフォルトの牌(赤ドラではない)を返す
	@return デフォルトの牌
***************************************************************************/
const Hai* Hai::getNormal() const {
  return Get(getColor(), getNumber());
}
/***********************************************************************//**
	@brief 自身のドラ牌を取得する
	@return ドラ牌
***************************************************************************/
const Hai* Hai::getDora() const {
  return Get(getColor(), getNumber(), true);
}
/***********************************************************************//**
	@brief 赤ドラか調べる
	@return	赤ドラのとき真
***************************************************************************/
bool Hai::isDora() const {
  return flag_.test(FLAG_DORA);
}
/***********************************************************************//**
	@brief 字牌か調べる
	@return	字牌のとき真
***************************************************************************/
bool Hai::isZihai() const {
  return color_ == COLOR_ZIHAI;
}
/***********************************************************************//**
	@brief ヤオチュウ牌か調べる
	@return	ヤオチュウ牌のとき真
***************************************************************************/
bool Hai::isYaochu() const {
  return (isZihai() || number_ == 1 || number_ == 9);
}
/***********************************************************************//**
	@brief 三元牌か調べる
	@return	三元牌のとき真
***************************************************************************/
bool Hai::isSangenpai() const {
  return (isZihai() && number_ > 4);
}
/***********************************************************************//**
	@brief 緑一色に使える牌か調べる
	@return	緑一色に使える牌のとき真
***************************************************************************/
bool Hai::isGreen() const {
  return ((color_ == COLOR_ZIHAI && number_ == 6) || 
          (color_ == COLOR_SOUZU && (number_ == 2 ||
                                     number_ == 3 ||
                                     number_ == 4 ||
                                     number_ == 6 ||
                                     number_ == 8)));
}
/***********************************************************************//**
	@brief 伏せ牌か調べる
	@return 伏せ牌のとき真
***************************************************************************/
bool Hai::isUnknown() const {
  return color_ == COLOR_UNKNOWN;
}
/***********************************************************************//**
	@brief 文字列に変換する
	@return 変換した文字列
***************************************************************************/
const char* Hai::toString() const {
  return string_;
}
/***********************************************************************//**
	@brief 次の牌を返す
	@param[in] add いくつ進めるか
	@return 牌
***************************************************************************/
const Hai* Hai::rotate(int add) const {
  int num = getNumber() + add;
  switch(getColor()) {
  case COLOR_MANZU:
  case COLOR_PINZU:
  case COLOR_SOUZU:
    num = (num - 1) % 9 + 1;
    break;
  case COLOR_ZIHAI:
    if(getNumber() <= 4) {
      num = (num - 1) % 4 + 1;
    }
    else {
      num = (num - 5) % 3 + 5;
    }
    break;
  default:
    assert(0);
    break;
  }
  return Get(getColor(), num);
}
/***********************************************************************//**
	@brief 順子となる牌を返す
	@param[in] add 加算値
	@return 加算した牌(もしくはnullptr)
***************************************************************************/
const Hai* Hai::shift(int add) const {
  switch(getColor()) {
  case COLOR_MANZU:
  case COLOR_PINZU:
  case COLOR_SOUZU:
    {
      int num = getNumber() + add;
      if(num >= 1 && num <= 9) {
        return Get(getColor(), num);
      }
    }
    break;
  default:
    break;
  }
  return nullptr;
}
/***********************************************************************//**
	@brief 自身がrhsより小さいか調べる
	@return 自身がrhsより小さいとき真
***************************************************************************/
bool Hai::operator<(const Hai& rhs) const {
  if(getColor() != rhs.getColor()) {
    return getColor() < rhs.getColor();
  }
  if(getNumber() != rhs.getNumber()) {
    return getNumber() < rhs.getNumber();
  }
  if(!isDora() && rhs.isDora()) {
    return true;
  }
  return false;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Hai::isA(const Hai* hai, bool isEqual) const {
  return isEqual ? this->isEqual(hai) : isSame(hai);
}
/***********************************************************************//**
	@brief 同じ牌か調べる(赤ドラは考慮しない)
	@param[in] hai 比較する牌
	@return 同じ牌のとき真
***************************************************************************/
bool Hai::isSame(const Hai* hai) const {
  return hai && 
    getColor() == hai->getColor() && 
    getNumber() == hai->getNumber();
}
/***********************************************************************//**
	@brief 赤ドラも含めて完全に同じ牌か調べる
	@param[in] hai 比較する牌
	@return 同じ牌のとき真
***************************************************************************/
bool Hai::isEqual(const Hai* hai) const {
  return (this == hai);
}
/***********************************************************************//**
	@brief 牌のインスタンスを返す.
	@param[in] color 種別
	@param[in] number 数字(1-9)
	@param[in] isDora 赤ドラのとき真
	@return	牌のインスタンスのポインタ(エラーのときは0)
***************************************************************************/
const Hai* Hai::Get(int color, int number, bool isDora) {
  return GetTable().get(color, number, isDora);
}
/***********************************************************************//**
	@brief 文字列から牌のインスタンスを返す.
	@param[in] str 牌を示す文字列.
	@return 牌のインスタンスのポインタ(エラーのときは0)
***************************************************************************/
const Hai* Hai::Get(const char* str) {
  if(!str || strnlen(str, STRING_SIZE) < STRING_SIZE) {
    return nullptr;
  }
  if(str[0] == 'x' && str[1] == 'x') {
    return &Unknown_;
  }
  const char* index = strchr(COLORS, tolower(str[1]));
  if(!index) {
    return nullptr;
  }
  return Get(int(index - COLORS), str[0] - '0', isupper(str[1]));
}
/***********************************************************************//**
	@copydoc Hai::Get(const char* str)
***************************************************************************/
const Hai* Hai::Get(const std::string& str) {
  return Get(str.c_str());
}
/***********************************************************************//**
	@brief 伏せ牌を取得する
	@return 伏せ牌のインスタンスのポインタ
***************************************************************************/
const Hai* Hai::GetUnknown() {
  return &Unknown_;
}
/***********************************************************************//**
	@brief デフォルトコンストラクタ
***************************************************************************/
Hai::Hai()
  : color_(COLOR_UNKNOWN), 
    number_(0)
{
  strcpy(string_, "xx");
}
/***********************************************************************//**
	@brief セットする
	@param[in] color 種別
	@param[in] number 数字
	@param[in] isDora 赤ドラのとき真
***************************************************************************/
void Hai::set(int color, int number, bool isDora) {
  color_ = color;
  number_ = number;
  flag_.set(FLAG_DORA, (isDora ? 1 : 0));
  string_[0] = number + '0';
  string_[1] = isDora ? toupper(COLORS[color]) : COLORS[color];
  string_[2] = '\0';
}
/***********************************************************************//**
	@brief 
***************************************************************************/
const Hai::Table& Hai::GetTable() {
  static Table TABLE;
  return TABLE;
}
/***********************************************************************//**
	@brief デフォルトコンストラクタ
***************************************************************************/
Hai::Table::Table() {
  for(int color = 0; color < Hai::COLOR_MAX; color++) {
    for(int number = 0; number < 9; number++) {
      for(int isDora = 0; isDora < 2; isDora++) {
        Hai& hai = instances_[color][number][isDora];
        hai.set(color, number + 1, isDora);
      }
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
const Hai* Hai::Table::get(int color, int number, bool isDora) const {
  if(number >= 1 &&
     ((color >= COLOR_MANZU && color <= COLOR_SOUZU && number <= 9) ||
      (color == COLOR_ZIHAI && number <= 7))) {
    return &instances_[color][number - 1][isDora ? 1 : 0];
  }
  return nullptr;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
