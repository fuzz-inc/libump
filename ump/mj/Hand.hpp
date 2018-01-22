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
#include "ump/mj/Kawa.hpp"
#include "ump/mj/Mentsu.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 手牌
***************************************************************************/
class Hand {
 private:
  HaiArray menzen_;
  std::vector<std::shared_ptr<Mentsu>> mentsus_;
  Kawa kawa_;

 public:
  Hand() = default;
  virtual ~Hand() = default;

  virtual void drawHaipai(const HaiArray& haipai);
  UMP_GETTER(Menzen, menzen_);
  Hand& sort();

  virtual void reset();

  void open(const char* str, const Hai* sutehai = 0);

  UMP_GETTER(Kawa, kawa_);
  Sutehai* sutehai(const Sutehai& sutehai);

  bool isMenzen() const;
  bool hasUnknown() const;
  bool isAllUnknown() const;

  std::string toString() const;

  void openMentsu(const HaiArray& hais, const Hai* hai);
  void kakan(const HaiArray& hais, const Hai* hai);
  size_t countMentsu() const;
  const Mentsu& getMentsu(size_t index) const;
  std::shared_ptr<Mentsu> findMentsu(const HaiArray& hais) const;

  Pattern getPonPattern(const Hai* hai) const;
  Pattern getChiPattern(const Hai* hai) const;
  Pattern getKanPattern() const;
  Pattern getKanPattern(const Hai* hai) const;

  virtual bool canTenpai(const Shanten& shanten) const;

 protected:
  UMP_GETTER(Mentsus, mentsus_);

  /**
   * 手牌を開いたときの処理
   * @param[in] menzen 開いた手牌
   */
  virtual void onOpen(const HaiArray& menzen) {}

  /**
   * 牌を晒したときの処理
   * @param[in] mentsu 晒したメンツ
   */
  virtual void onOpenMentsu(std::shared_ptr<const Mentsu> mentsu) {}

  /**
   * 加槓したときの処理
   * @param[in] mentsu メンツ
   * @param[in] hai 加槓した牌
   */
  virtual void onKakan(std::shared_ptr<const Mentsu> mentsu, 
                       const Hai* hai) {}

  /**
   * 牌が見えたときの処理
   * @param[in] hai 見えた牌
   */
  virtual void onShowHai(const Hai* hai) {}

  void append(const Hai* hai);

 private:
  void drop(const Hai* hai);
  void drop(const HaiArray& hais);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
