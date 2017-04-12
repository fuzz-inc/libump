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
/***********************************************************************//**
	コマンド
***************************************************************************/
class Command {
 public:
  typedef std::vector<std::string> Args;

 public:
  enum Type {
    TYPE_NULL, 

    TYPE_HELLO, 
    TYPE_ERROR, 
    TYPE_SEAT, 
    TYPE_PLAYER, 
    TYPE_GAMESTART, 
    TYPE_GAMEEND, 
    TYPE_KYOKUSTART, 
    TYPE_KYOKUEND, 
    TYPE_READY_Q, 
    TYPE_POINT, 
    TYPE_DICE, 
    TYPE_HAIPAI, 
    TYPE_OPEN, 
    TYPE_DORA, 
    TYPE_URADORA, 
    TYPE_TSUMO, 
    TYPE_SUTEHAI_Q, 
    TYPE_SUTEHAI, 
    TYPE_NAKI_Q, 
    TYPE_SAY, 
    TYPE_AGARI, 
    TYPE_RYUKYOKU, 
    TYPE_TENPAI_Q, 

    TYPE_TSUMOGIRI, 
    TYPE_RICHI, 
    TYPE_ANKAN, 
    TYPE_KAKAN, 
    TYPE_YES, 
    TYPE_NO, 

    TYPE_CHI, 
    TYPE_PON, 
    TYPE_KAN, 
    TYPE_RON, 

    TYPE_TENPAI, 
    TYPE_NOTEN, 

    TYPE_EX, 

    TYPE_MAX
  };

  static const std::string OPTION_GAMEID;

 private:
  static const std::string OPTION_DATASIZE;

 private:
  Type type_;
  unsigned int serial_;
  std::vector<std::string> args_;
  std::map<std::string, std::string> options_;
  std::shared_ptr<const std::vector<char>> data_;

  static const char* TYPE_TABLE[];

 public:
  Command(Type command = TYPE_NULL, unsigned int serial = 0);
  Command(const Command& src) = default;

  UMP_ACCESSOR(Type, type_);
  UMP_ACCESSOR(Serial, serial_);

  void clear();
  bool isExist() const;

  size_t countArg() const;
  const std::string& getArg(size_t index) const;
  const Args& getArgs() const;
  bool hasArg() const;
  bool hasArg(const std::string& value) const;
  bool hasArg(Type type) const;

  Command& append(const char* value);
  Command& append(const std::string& value);

  template <class T>
  Command& append(const T& value) {
    return append(ToString(value));
  }

  Command& setOption(const std::string& name, const std::string& value);
  const std::string& getOption(const std::string& name) const;
  bool hasOption(const std::string& name) const;

  Command& setData(std::shared_ptr<const std::vector<char>> data);
  UMP_GETTER(Data, data_);
  size_t getDataSize() const;

  bool parse(const char* command);

  std::string toString(bool isEncode) const;

  static Type GetType(const std::string& string);

  static std::string SeatToString(size_t seat);
  static size_t StringToSeat(const char* string);

  static std::string Encode(const char* src);
  static std::string Decode(const char* src, size_t len = 0);

  template <class T>
  static std::string ToString(const T& value) {
    std::ostringstream buff;
    buff << value;
    return buff.str();
  }

  static std::string ToString(Type type);

  template <class T>
  static T FromString(const std::string& value) {
    T buff;
    std::istringstream(value) >> buff;
    return buff;
  }

 private:
  void parseElem(const char* pt, size_t len = 0);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
