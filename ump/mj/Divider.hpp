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

#include "ump/mj/Mentsu.hpp"
#include "ump/mj/Tatsu.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 要素に分解する
***************************************************************************/
class Divider {
 private:
  enum {
    FLAG_VETO, 
    FLAG_MAX
  };

 private:
  std::bitset<FLAG_MAX> flag_;
  HaiArray hais_;
  HaiArray head_;
  std::vector<Mentsu> mentsus_;
  std::vector<Tatsu> tatsus_;

 public:
  Divider();
  virtual ~Divider();

  void set(const HaiArray& hais);

  void eachHead();
  void eachMentsu(size_t index = 0);
  void eachTatsu(size_t index = 0);

  bool isAll() const;
  bool hasHead() const;

  size_t countHai() const;
  const Hai* getHai(size_t index) const;

  const HaiArray& getHead() const;

  UMP_GETTER(Mentsus, mentsus_);
  UMP_GETTER(Tatsus, tatsus_);

  size_t countHai(const Hai* hai) const;

  std::string toString() const;
  void dump() const;

 protected:
  UMP_GETTER(Hais, hais_);

  void veto();

  virtual void onHead();
  virtual void onMentsu();
  virtual void onTatsu();

 private:
  bool isVeto() const;
  void pushMentsu(const Hai* a, const Hai* b, const Hai* c, size_t index);
  void pushTatsu(const Hai* a, const Hai* b, size_t index);
  void pop(const HaiArray& elem);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
