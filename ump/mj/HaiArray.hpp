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

#include "ump/mj/Hai.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 牌の配列
***************************************************************************/
class HaiArray
  : public std::vector<const Hai*>
{
  typedef std::vector<const Hai*> super;

 public:
  static const HaiArray YAOCHU;

 public:
  HaiArray() = default;
  HaiArray(const char* str);
  HaiArray(const HaiArray& src);

  void append(const Hai* hai);
  void append(const HaiArray& other);

  template <class T>
  HaiArray operator+(const T& rhs) const {
    auto result(*this);
    result.append(rhs);
    return result;
  }

  const Hai* removeSame(const Hai* hai) {
    return remove(hai, false);
  }

  const Hai* removeEqual(const Hai* hai) {
    return remove(hai, true);
  }

  size_t countSame(const Hai* hai) const {
    return count(hai, false);
  }

  size_t countEqual(const Hai* hai) const {
    return count(hai, true);
  }

  size_t countUniqueEqual() const;
        
  bool isInclude(const Hai* hai) const;
  bool isIncludeEqual(const Hai* hai) const;
  bool isIncludeUnknown() const;
  bool isIncludeYaochu() const;

  bool isSame(const HaiArray& hais) const {
    return isA(hais, false);
  }

  bool isEqual(const HaiArray& hais) const {
    return isA(hais, true);
  }

  bool hasSame(const Hai* hai) const {
    return count(hai, false) > 0;
  }

  bool hasSame(const HaiArray& hais) const {
    return hasA(hais, false);
  }

  bool hasEqual(const Hai* hai) const {
    return count(hai, true) > 0;
  }

  bool hasEqual(const HaiArray& hais) const {
    return hasA(hais, true);
  }

  HaiArray& sort();
  void shuffle(std::default_random_engine& random);

  HaiArray& unique();
  HaiArray getUnique() const;
  HaiArray& uniqueEqual();

  HaiArray getNormal() const;

  const char* parseString(const char* pt);
  std::string toString() const;

  const Hai* remove(const Hai* hai, bool isEqual);
  void remove(const HaiArray& hais, bool isEqual);
  size_t count(const Hai* hai, bool isEqual) const;
  bool isA(const HaiArray& hais, bool isEqual) const;
  bool hasA(const HaiArray& hais, bool isEqual) const;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
