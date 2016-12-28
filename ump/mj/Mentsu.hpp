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

#include "ump/mj/HaiArray.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 面子
***************************************************************************/
class Mentsu
  : public HaiArray
{
  typedef HaiArray super;

 private:
  enum Type {
    TYPE_NULL, 
    TYPE_TOITSU, 		/**< 対子 */
    TYPE_SHUNTSU, 		/**< 順子 */
    TYPE_KOTSU, 		/**< 刻子 */
    TYPE_KANTSU, 		/**< カンツ */
    TYPE_MAX
  };
  enum {
    FLAG_MENZEN, 		/**< 面前 */
    FLAG_RON, 			/**< ロン和了 */
    FLAG_MAX
  };

 private:
  Type type_;			/**< 種別 */
  int fu_;			/**< 基本符 */
  int machiFu_;			/**< 待ちの形に付く符 */
  std::bitset<FLAG_MAX> flag_;

 public:
  Mentsu(const HaiArray& hais, bool isMenzen = true);
  ~Mentsu() = default;

  int getFu() const;

  bool isMenzen() const;
  bool isRon() const;
  bool isToitsu() const;
  bool isShuntsu() const;
  bool isKotsu() const;
  bool isKantsu() const;
  bool isAnko() const;
  bool isPinfu() const;

  void clearAgariHai();
  void setAgariHai(const Hai* hai, bool isRon);

  bool canKakan(const Hai* hai) const;
  void kakan(const Hai* hai);

  std::string toString() const;

 private:
  void setType(Type type);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
