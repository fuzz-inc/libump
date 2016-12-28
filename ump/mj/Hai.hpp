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
	@brief 牌
***************************************************************************/
class Hai {
 public:
  /** 種別 */
  enum {
    COLOR_MANZU, 	/**< マンズ */
    COLOR_PINZU, 	/**< ピンズ */
    COLOR_SOUZU, 	/**< ソウズ */
    COLOR_ZIHAI, 	/**< 字牌 */
    COLOR_MAX, 
    COLOR_UNKNOWN, 	/**< 伏せ牌 */
  };

  static const size_t STRING_SIZE = 2;

 private:
  class Table;
  friend class Table;

  enum {
    FLAG_DORA, 		/**< 赤ドラ */
    FLAG_MAX
  };

 private:
  int color_;                   /**< 種別 */
  int number_;                  /**< 数(1~9) */
  std::bitset<FLAG_MAX> flag_;  /**< フラグ */
  char string_[STRING_SIZE + 1];

  static const char* COLORS;
  static Hai Unknown_;

 public:
  ~Hai() = default;

  int getColor() const;
  int getNumber() const;
  const Hai* getNormal() const;
  const Hai* getDora() const;

  bool operator<(const Hai& rhs) const;
  bool isA(const Hai* hai, bool isEqual) const;
  bool isSame(const Hai* hai) const;
  bool isEqual(const Hai* hai) const;

  bool isDora() const;
  bool isZihai() const;
  bool isYaochu() const;
  bool isSangenpai() const;
  bool isGreen() const;
  bool isUnknown() const;
  
  const char* toString() const;

  const Hai* rotate(int add) const;
  const Hai* shift(int add) const;

  static const Hai* Get(const char* str);
  static const Hai* Get(const std::string& str);
  static const Hai* Get(int color, int number, bool isDora = false);
  static const Hai* GetUnknown();

 private:
  Hai();
  void set(int color, int number, bool isDora);
  static const Table& GetTable();
};
/***********************************************************************//**
	@brief 
***************************************************************************/
class Hai::Table {
 private:
  Hai instances_[Hai::COLOR_MAX][9][2];

 public:
  Table();
  ~Table() = default;

  const Hai* get(int color, int number, bool isDora) const;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
