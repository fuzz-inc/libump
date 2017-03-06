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
#include "ump/Command.hpp"

namespace ump {
const char* Command::TYPE_TABLE[] = {
  "NULL", 

  "hello", 
  "error", 
  "seat", 
  "player", 
  "gamestart", 
  "gameend", 
  "kyokustart", 
  "kyokuend", 
  "ready?", 
  "point", 
  "dice", 
  "haipai", 
  "open", 
  "dora", 
  "tsumo", 
  "sutehai?", 
  "sutehai", 
  "naki?", 
  "say", 
  "agari", 
  "ryukyoku", 
  "tenpai?", 

  "tsumogiri", 
  "richi", 
  "ankan", 
  "kakan", 
  "yes", 
  "no", 

  "chi", 
  "pon", 
  "kan", 
  "ron", 

  "tenpai", 
  "noten", 

  "_ex_", 

  nullptr
};

const std::string Command::OPTION_DATASIZE("datasize");
/***********************************************************************//**
	@brief コンストラクタ
        @param[in] type 種別
        @param[in] serial シリアル番号
***************************************************************************/
Command::Command(Type type, unsigned int serial)
  : type_(type), 
    serial_(serial)
{}
/***********************************************************************//**
	@brief クリア
***************************************************************************/
void Command::clear() {
  setType(TYPE_NULL);
  setSerial(0);
  args_.clear();
  options_.clear();
  data_.reset();
}
/***********************************************************************//**
	@brief コマンドが存在するか調べる
	@return コマンドが存在するとき真
***************************************************************************/
bool Command::isExist() const {
  return getType() != TYPE_NULL;
}
/***********************************************************************//**
	@brief 引数の数を数える
	@return 引数の数
***************************************************************************/
size_t Command::countArg() const {
  return args_.size();
}
/***********************************************************************//**
	@brief 引数を参照する
	@param[in] index インデックス
	@return 引数(範囲外のときは空文字列)
***************************************************************************/
const std::string& Command::getArg(size_t index) const {
  static const std::string EMPTY;
  return (index < args_.size()) ? args_.at(index) : EMPTY;
}
/***********************************************************************//**
	@brief 引数の配列を取得する
	@return 引数の配列
***************************************************************************/
const Command::Args& Command::getArgs() const {
  return args_;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Command::hasArg(const std::string& value) const {
  return std::find(args_.begin(), args_.end(), value) != args_.end();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Command::hasArg(Type type) const {
  return hasArg(ToString(type));
}
/***********************************************************************//**
	@brief 引数を追加する
	@param[in] value 追加する引数
	@return 自分自身
***************************************************************************/
Command& Command::append(const char* value) {
  return append(std::string(value));
}
/***********************************************************************//**
	@brief 引数を追加する
	@param[in] value 追加する引数
	@return 自分自身
***************************************************************************/
Command& Command::append(const std::string& value) {
  assert(!value.empty());
  args_.push_back(value);
  return *this;
}
/***********************************************************************//**
	@brief オプションをセットする
	@param[in] name オプション名
	@param[in] value 値
	@return 自分自身
***************************************************************************/
Command& Command::setOption(const std::string& name, 
                            const std::string& value) {
  options_[name] = value;
  return *this;
}
/***********************************************************************//**
	@brief オプションの値を取得する
	@param[in] name オプション名
	@return オプションの値
***************************************************************************/
const std::string& Command::getOption(const std::string& name) const {
  static const std::string EMPTY;
  return hasOption(name) ? options_.at(name) : EMPTY;
}
/***********************************************************************//**
	@brief オプションが存在するか調べる
	@param[in] name オプション名
	@return 存在するとき真
***************************************************************************/
bool Command::hasOption(const std::string& name) const {
  return options_.find(name) != options_.end();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Command& Command::setData(std::shared_ptr<const std::vector<char>> data) {
  data_ = data;
  setOption(OPTION_DATASIZE, std::to_string(data->size()));
  return *this;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
size_t Command::getDataSize() const {
  return hasOption(OPTION_DATASIZE)
    ? std::stoul(getOption(OPTION_DATASIZE))
    : 0;
}
/***********************************************************************//**
	@brief 文字列を解析する
	@param[in] command コマンド文字列
***************************************************************************/
bool Command::parse(const char* command) {
  clear();
  const char* pt = command;
  while(pt) {
    if(auto sep = strchr(pt, ' ')) {
      parseElem(pt, sep - pt);
      pt = sep + 1;
    }
    else {
      parseElem(pt);
      pt = 0;
    }
  }
  if(args_.size() < 2) {
    return false;
  }
  serial_ = atoi(args_.front().c_str());
  args_.erase(args_.begin());
  int i = 0;
  while(TYPE_TABLE[i]) {
    if(args_.front() == TYPE_TABLE[i]) {
      setType(Type(i));
      args_.erase(args_.begin());
      return true;
    }
    i++;
  }
  return false;
}
/***********************************************************************//**
	@brief 文字列に変換する
	@param[in] isEncode エンコードするとき真
	@return コマンドを表す文字列
***************************************************************************/
std::string Command::toString(bool isEncode) const {
  std::ostringstream buff;
  buff << serial_;
  buff << " " << TYPE_TABLE[getType()];
  for(auto& option : options_) {
    buff << " " << option.first << "="
         << (isEncode ? Encode(option.second.c_str()) : option.second);
  }
  for(auto& arg : args_) {
    buff << " " << (isEncode ? Encode(arg.c_str()) : arg);
  }
  return buff.str();
}
/***********************************************************************//**
	@brief 文字列をタイプに変換する
	@param[in] string 文字列
	@return タイプ
***************************************************************************/
Command::Type Command::GetType(const std::string& string) {
  for(size_t i = 0; TYPE_TABLE[i]; i++) {
    if(string == TYPE_TABLE[i]) {
      return Type(i);
    }
  }
  return TYPE_NULL;
}
/***********************************************************************//**
	@brief 席を文字列に変換する
	@param[in] seat 席
	@return 席を表す文字列
***************************************************************************/
std::string Command::SeatToString(size_t seat) {
  std::string string;
  string.append(1, 'A' + seat % ('Z' - 'A' + 1));
  string.append(1, '0' + seat / ('Z' - 'A' + 1));
  return string;
}
/***********************************************************************//**
        @brief 文字列を席に変換する
	@param[in] string 文字列
	@return 席。文字列が不正なときはsize_tの最大値
***************************************************************************/
size_t Command::StringToSeat(const char* string) {
  if(string[0] < 'A' || string[0] > 'Z' ||
     string[1] < '0' || string[2] > '9' ||
     string[2] != '\0') {
    return std::numeric_limits<size_t>::max();
  }
  return (string[0] - 'A') + (string[1] - '0') * ('Z' - 'A' + 1);
}
/***********************************************************************//**
	@brief ASCII以外の文字をエンコードする
	@param[in] src 入力文字列
	@return エンコードされた文字列
***************************************************************************/
std::string Command::Encode(const char* src) {
  std::string des;
  char buff[8];
  unsigned char c;
  while((c = *src++) != '\0') {
    if(c != '%' && c > 0x20 && c < 0x7f) {
      des.append(1, c);
    }
    else {
      sprintf(buff, "%%%02X", c);
      des.append(buff);
    }
  }
  return des;
}
/***********************************************************************//**
	@brief エンコードされた文字列をデコードする
	@param[in] src 文字列のポインタ
	@param[in] len 文字列の長さ
	@return デコードされた文字列
***************************************************************************/
std::string Command::Decode(const char* src, size_t len) {
  std::string des;
  const char* pt = src;
  unsigned char c;
  while((c = *pt++) != '\0') {
    if(c == '%') {
      int code;
      sscanf(pt, "%02X", &code);
      c = static_cast<unsigned char>(code);
      pt += 2;
    }
    des.append(1, c);
    if(len > 0 && pt >= src + len) {
      break;
    }
  }
  return des;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::string Command::ToString(Type type) {
  return std::string(TYPE_TABLE[type]);
}
/***********************************************************************//**
	@brief 要素をパースする
	@param[in] pt 文字列の先頭
	@param[in] len 文字列の長さ
***************************************************************************/
void Command::parseElem(const char* pt, size_t len) {
  if(len == 0) {
    len = strlen(pt);
  }
  auto sep = static_cast<const char*>(memchr(pt, '=', len));
  if(sep && sep != pt) {
    size_t nameLen = sep - pt;
    setOption(std::string(pt, nameLen), 
              Decode(sep + 1, len - nameLen - 1));
  }
  else {
    append(Decode(pt, len));
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
