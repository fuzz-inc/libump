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
#include "ump/Version.hpp"

namespace ump {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] major メジャーバージョン番号
	@param[in] minor マイナーバージョン番号
***************************************************************************/
Version::Version(int major, int minor)
  : major_(major), 
    minor_(minor)
{}
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] str バージョンを表す文字列
***************************************************************************/
Version::Version(const char* str) {
  std::istringstream buff(str);
  char c;
  buff >> major_ >> c >> minor_;
  assert(c == '.');
}
/***********************************************************************//**
	@brief 文字列に変換する
***************************************************************************/
std::string Version::toString() const {
  std::ostringstream buff;
  buff << major_ << "." << minor_;
  return buff.str();
}
/***********************************************************************//**
	@brief 比較する
	@param[in] other 比較対象
	@return 自身の方が古いとき負、同じとき真、新しいとき正
***************************************************************************/
int Version::compare(const Version& other) const {
  return (major_ != other.major_)
    ? (major_ - other.major_)
    : (minor_ - other.minor_);
}
/***********************************************************************//**
	@brief メジャーバージョンを比較する
	@param[in] other 比較対象
	@return 自身の方が古いとき負、同じとき真、新しいとき正
***************************************************************************/
int Version::compareMajor(const Version& other) const {
  return (major_ - other.major_);
}
/***********************************************************************//**
	@brief ライブラリのバージョンを取得する
	@return ライブラリのバージョン
***************************************************************************/
const Version& Version::Get() {
  static const Version version(0, 2);
  return version;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
