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
#include <iomanip>
#include "ump/Logger.hpp"

namespace ump {
/***********************************************************************//**
	@brief デフォルトコンストラクタ
***************************************************************************/
Logger::Logger()
  : level_(LEVEL_DEBUG)
{
}
/***********************************************************************//**
	@brief 出力先をファイルに設定する
	@param[in] path ファイルパス
***************************************************************************/
void Logger::openLogFile(const std::string& path) {
  output_.reset(new std::ofstream(path.c_str()));
  path_ = path;
}
/***********************************************************************//**
	@brief メッセージを出力する
	@param[in] level レベル
	@param[in] message メッセージ
***************************************************************************/
void Logger::log(Level level, const std::string& message) {
  static const char* LEVELS[] = {
    "DEBUG", 
    "INFO", 
    "UMP", 
    "WARN", 
    "ERROR", 
    "FATAL"
  };
  if(level >= level_) {
    auto time = GetTime("%F %T");
    {
      std::lock_guard<std::mutex> lock(mutex_);
      getOutput() << time
                  << " " << LEVELS[level] << " | "
                  << message << std::endl;
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::string Logger::Format(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char buff[256];
  vsnprintf(buff, sizeof(buff), format, args);
  va_end(args);
  return std::string(buff);
}
/***********************************************************************//**
	@brief 現在時刻の文字列を求める
	@param[in] format 書式
	@return 現在時刻の文字列
***************************************************************************/
std::string Logger::GetTime(const char* format) {
  auto now = std::time(nullptr);
  auto time = *std::localtime(&now);
  char buff[256];
  std::strftime(buff, sizeof(buff), format, &time);
  return buff;
}
/***********************************************************************//**
  @brief ストリームをフラッシュする
***************************************************************************/
void Logger::flush() {
  getOutput().flush();
}
/***********************************************************************//**
  @brief 入力ストリームを取得する
  @return 入力ストリーム
***************************************************************************/
std::unique_ptr<std::istream> Logger::getInput() const {
  return std::unique_ptr<std::istream>(new std::ifstream(path_.c_str()));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::ostream& Logger::getOutput() const {
  return output_ ? *output_ : std::cerr;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
