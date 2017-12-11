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
#include "ump/mj/Agari.hpp"
#include "ump/mj/Game.hpp"
#include "ump/mj/Player.hpp"

namespace ump {
namespace mj {
const Agari::YakuTable Agari::YakuTable_[YAKU_MAX] = {
  { 1, 0, "リーチ" }, 
  { 2, 0, "ダブルリーチ" }, 
  { 1, 0, "一発" }, 
  { 1, 1, "嶺上開花" }, 
  { 1, 1, "槍槓" }, 
  { 1, 1, "海底摸月" }, 
  { 1, 1, "河底撈魚" }, 
  { 1, 1, "タンヤオ" }, 
  { 1, 0, "平和" }, 
  { 1, 0, "門前自摸" }, 
  { 1, 0, "一盃口" }, 
  { 2, 0, "七対子" }, 
  { 2, 1, "チャンタ" }, 
  { 3, 2, "純チャン" }, 
  { 2, 2, "混老頭" }, 
  { 2, 2, "対々和" }, 
  { 2, 2, "三暗刻" }, 
  { 2, 2, "三槓子" }, 
  { 3, 0, "二盃口" }, 
  { 2, 1, "三色同順" }, 
  { 2, 2, "三色同刻" }, 
  { 2, 1, "一気通貫" }, 
  { 1, 1, "役牌" }, 
  { 2, 2, "小三元" }, 
  { 3, 2, "混一色" }, 
  { 6, 5, "清一色" }, 
  { 0, 0, "役満" }, 
  { 1, 1, "国士無双" }, 
  { 1, 1, "大三元" }, 
  { 1, 1, "清老頭" }, 
  { 1, 1, "四暗刻" }, 
  { 1, 1, "四槓子" }, 
  { 1, 1, "小四喜" }, 
  { 2, 2, "大四喜" }, 
  { 1, 1, "字一色" }, 
  { 1, 1, "緑一色" }, 
  { 1, 0, "九蓮宝燈" }, 
  { 1, 0, "天和" }, 
  { 1, 0, "地和" }, 
  { 1, 0, "人和" }
};
static const int COLOR_MANZU = 1 << Hai::COLOR_MANZU;
static const int COLOR_PINZU = 1 << Hai::COLOR_PINZU;
static const int COLOR_SOUZU = 1 << Hai::COLOR_SOUZU;
static const int COLOR_ZIHAI = 1 << Hai::COLOR_ZIHAI;
static const int COLOR_ALL = COLOR_MANZU | COLOR_PINZU | COLOR_SOUZU;
/***********************************************************************//**
        @brief コンストラクタ
***************************************************************************/
Agari::Agari() {
  clear();
}
/***********************************************************************//**
        @brief 和了の内容を全てクリアする
***************************************************************************/
void Agari::clear() {
  mentsus_.clear();
  flag_.reset();
  clearYaku();
}
/***********************************************************************//**
	@brief 面子を追加する
	@param[in] mentsu 追加する面子
***************************************************************************/
void Agari::appendMentsu(const Mentsu& mentsu) {
  mentsus_.push_back(mentsu);
}
/***********************************************************************//**
	@brief 面子を数える
	@return 面子の数
***************************************************************************/
size_t Agari::countMentsu() const {
  return mentsus_.size();
}
/***********************************************************************//**
	@brief 面子を取得する
	@param[in] index インデックス
	@return 面子
***************************************************************************/
Mentsu& Agari::getMentsu(size_t index) {
  return mentsus_[index];
}
/***********************************************************************//**
	@brief 計算する
***************************************************************************/
void Agari::update(const Player& player) {
  clearYaku();
  if(player.isFirst()) {
    if(isRon()) {
      appendYaku(YAKUMAN_RENHO);
    }
    else if(player.isOya()) {
      appendYaku(YAKUMAN_TENHO);
    }
    else {
      appendYaku(YAKUMAN_CHIHO);
    }
  }
  if(mentsus_.empty()) {
    appendYaku(YAKUMAN_KOKUSHI);
  }
  else {
    if(player.isRichi()) {
      appendYaku(player.isDoubleRichi() ? YAKU_DOUBLE_RICHI : YAKU_RICHI);
      if(player.isIppatsu()) {
        appendYaku(YAKU_IPPATSU);
      }
    }
    if(player.isRinshan()) {
      appendYaku(YAKU_RINSHAN);
    }
    else if(player.isChankan()) {
      appendYaku(YAKU_CHANKAN);
    }
    else if(player.isHaitei()) {
      appendYaku(isRon() ? YAKU_HOUTEI : YAKU_HAITEI);
    }
    if(isMenzen() && !isRon()) {
      appendYaku(YAKU_TSUMO);
    }
    Info info;
    updateInfo(info, player);
    checkYakuhai(info, player);
    checkKind(info);
    checkKotsu(info);
    checkIpeikou(info);
    checkSanshoku(info);
    checkItsu(info);
    checkColor(info);
    checkSangen(info);
    checkSushiho(info);
    /* 緑一色*/
    if(info.kind & KIND_ALL_GREEN) {
      appendYaku(YAKUMAN_RYUISOU);
    }
    if(han_ > 0) {
      updateDora(player);
    }
  }
  updatePoint(player);
}
/***********************************************************************//**
        @brief 和了の総翻数を取得する
        @return	総翻数
***************************************************************************/
int Agari::getHan() const {
  if(isYakuman()) {
    return 0;
  }
  return han_ + dora_;
}
/***********************************************************************//**
	@brief 役の翻数を求める
	@param[in] yaku 調べる役
	@return 翻数
***************************************************************************/
int Agari::getHan(int yaku) const {
  if(!isInclude(yaku)) {
    return 0;
  }
  return yakuHan_[yaku];
}
/***********************************************************************//**
        @brief 役満数を取得する
	@return 役満数(ダブル=2, トリプル=3, ...)
***************************************************************************/
int Agari::getYakuman() const {
  if(!isYakuman()) {
    return 0;
  }
  return han_;
}
/***********************************************************************//**
	@brief 和了が存在するか調べる
	@return 和了が存在するとき真
***************************************************************************/
bool Agari::isExist() const {
  return han_ > 0;
}
/***********************************************************************//**
        @brief 役満か調べる
	@return	役満のとき真
***************************************************************************/
bool Agari::isYakuman() const {
  return flag_.test(FLAG_YAKUMAN);
}
/***********************************************************************//**
	@brief 役が含まれているか調べる
	@param[in] yaku 調べる役
	@return 役が含まれているとき真
***************************************************************************/
bool Agari::isInclude(int yaku) const {
  if(isYakuman() && yaku < YAKUMAN) {
    return false;
  }
  return (yakuHan_[yaku] > 0);
}
/***********************************************************************//**
	@brief ロン和了か調べる
	@return ロン和了のとき真
***************************************************************************/
bool Agari::isRon() const {
  for(auto& mentsu : mentsus_) {
    if(mentsu.isRon()) {
      return true;
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 役を追加する
        @param[in] 役ID
***************************************************************************/
void Agari::appendYaku(int yaku) {
  if(isYakuman() && yaku < YAKUMAN) {
    return;
  }
  const YakuTable& table = YakuTable_[yaku];
  int han = isMenzen() ? table.menzenHan : table.nakiHan;
  if(han > 0) {
    if(!isYakuman() && yaku > YAKUMAN) {
      clearYaku();
      flag_.set(FLAG_YAKUMAN);
    }
    yakuHan_[yaku] += han;
    han_ += han;
  }
}
/***********************************************************************//**
	@brief 比較する
	@param[in] other 比較対象
	@return 自身の方が低いとき真
***************************************************************************/
bool Agari::operator<(const Agari& rhs) const {
  if(getYakuman() < rhs.getYakuman()) {
    return true;
  }
  if(getYakuman() > rhs.getYakuman()) {
    return false;
  }
  if(getHan() < rhs.getHan()) {
    return true;
  }
  if(getHan() > rhs.getHan()) {
    return false;
  }
  return getFu() < rhs.getFu();
}
/***********************************************************************//**
	@brief 和了を文字列に変換する
        @return 和了を表す文字列
***************************************************************************/
std::string Agari::toString() const {
  std::ostringstream str;
  for(int i = 0; i < YAKU_MAX; i++) {
    auto han = getHan(i);
    if(han > 0) {
      str << GetYakuName(i) << " " << han << " ";
    }
  }
  if(isYakuman()) {
    str << getText() << " " << getPoint().toString() << "点";
  }
  else {
    for(auto& mentsu : mentsus_) {
      str << mentsu.toString() << "[" << mentsu.getFu() << "]";
    }
    str << " " << 
      getFu() << "符" << 
      getHan() << "翻" << 
      getText() << 
      getPoint().toString() << "点";
  }
  return str.str();
}
/***********************************************************************//**
	@brief 役を表す文字列を取得する
	@param[in] yaku 役
	@return 役を表す文字列
***************************************************************************/
const char* Agari::GetYakuName(int yaku) {
  if(yaku >= 0 && yaku < YAKU_MAX) {
    return YakuTable_[yaku].name;
  }
  return 0;
}
/***********************************************************************//**
        @brief 役をクリアする
***************************************************************************/
void Agari::clearYaku() {
  for(int i = 0; i < YAKU_MAX; i++) {
    yakuHan_[i] = 0;
  }
  dora_ = 0;
  han_ = 0;
  fu_ = 0;
  srcFu_ = 0;
  point_ = 0;
}
/***********************************************************************//**
	@brief 面前か調べる
	@return 面前のとき真
***************************************************************************/
bool Agari::isMenzen() const {
  for(auto& mentsu : mentsus_) {
    if(!mentsu.isMenzen()) {
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief 種別を更新する
***************************************************************************/
void Agari::updateInfo(Info& info, const Player& player) {
  for(auto& mentsu : mentsus_) {
    auto hai = mentsu.at(0);
    int index = hai->getNumber() - 1;
    int color = (1 << hai->getColor());
    info.color |= color;
    if(mentsu.isShuntsu()) {
      info.shuntsuNum++;
      if(info.ipeikouColor[index] & color) {
        info.ipeikouNum++;
      }
      info.ipeikouColor[index] ^= color;
      info.shuntsuColor[index] |= color;
    }
    else if(mentsu.isKotsu() || mentsu.isKantsu()) {
      info.kotsuNum++;
      if(mentsu.isAnko()) {
        info.ankoNum++;
      }
      if(mentsu.isKantsu()) {
        info.kantsuNum++;
      }
      info.kotsuColor[index] |= color;
    }
    else {
      info.toitsuColor[index] |= color;
    }
    if(info.isPinfu && !mentsu.isPinfu()) {
      info.isPinfu = false;
    }
    updateKind(info, mentsu);
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Agari::updateKind(Info& info, const Mentsu& mentsu) const {
  int kind = 0;
  for(auto hai : mentsu) {
    if(!hai->isGreen()) {
      info.kind &= ~KIND_ALL_GREEN;
    }
    if(hai->isZihai()) {
      kind |= KIND_ZIHAI;
    }
    else if(hai->isYaochu()) {
      kind |= KIND_YAOCHU;
    }
    else {
      kind |= KIND_TANYAO;
    }
    info.count[hai->getNumber() - 1]++;
  }
  if((kind & (KIND_YAOCHU | KIND_ZIHAI)) == 0) {
    info.kind &= ~KIND_CHANTA;
  }
  info.kind |= kind;
}
/***********************************************************************//**
	@brief ドラを数える
	@param[in] player プレイヤー
***************************************************************************/
void Agari::updateDora(const Player& player) {
  dora_ = 0;
  auto game = player.getGame();
  for(auto dora : game->getDoras()) {
    dora_ += countHai(dora);
  }
  for(auto& mentsu : mentsus_) {
    for(auto hai : mentsu) {
      if(hai->isDora()) {
        dora_++;
      }
    }
  }
}
/***********************************************************************//**
	@brief 点数を更新する
***************************************************************************/
void Agari::updatePoint(const Player& player) {
  if(isYakuman()) {
    fu_ = 0;
    srcFu_ = 0;
    point_ = 32000 * getYakuman();
    text_.assign("役満");
  }
  else {
    updateFu(player);
    auto han = getHan();
    if(han >= 13) {
      point_ = 32000;
      text_.assign("数え役満");
    }
    else if(han >= 11) {
      point_ = 24000;
      text_.assign("三倍満");
    }
    else if(han >= 8) {
      point_ = 16000;
      text_.assign("倍満");
    }
    else if(han >= 6) {
      point_ = 12000;
      text_.assign("跳満");
    }
    else {
      point_ = fu_ * int(pow(2, 4 + han));
      if(point_ >= 8000) {
        point_ = 8000;
        text_.assign("満貫");
      }
    }
  }
}
/***********************************************************************//**
	@brief 符を更新する
***************************************************************************/
void Agari::updateFu(const Player& player) {
  if(isInclude(YAKU_CHITOI)) {
    fu_ = srcFu_ = 25;
  }
  else {
    if(isInclude(YAKU_PINFU)) {
      fu_ = srcFu_ = isRon() ? 30 : 20;
    }
    else {
      srcFu_ = (isMenzen() && isRon()) ? 30 : 20;
      if(!isRon()) {
        srcFu_ += 2;
      }
      auto yakuhais = GetYakuhais(player);
      for(auto& mentsu : mentsus_) {
        srcFu_ += mentsu.getFu();
        if(mentsu.isToitsu()) {
          auto hai = mentsu.at(0);
          for(auto iter : yakuhais) {
            if(hai->isSame(iter)) {
              srcFu_ += 2;
            }
          }
        }
      }
      if(srcFu_ <= 20) {
        fu_ = srcFu_ = 30;
      }
      else {
        fu_ = Ceil(srcFu_, 10);
      }
    }
  }
}
/***********************************************************************//**
	@brief 役牌を調べる
***************************************************************************/
void Agari::checkYakuhai(Info& info, const Player& player) {
  for(auto hai : GetYakuhais(player)) {
    if(info.kotsuColor[hai->getNumber() - 1] & COLOR_ZIHAI) {
      appendYaku(YAKU_YAKUHAI);
    }
    if(info.toitsuColor[hai->getNumber() - 1] & COLOR_ZIHAI) {
      info.isPinfu = false;
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Agari::checkKotsu(const Info& info) {
  if(info.kotsuNum == 0) {
    if(info.shuntsuNum == 0) {
      appendYaku(YAKU_CHITOI);
    }
    else if(info.isPinfu) {
      appendYaku(YAKU_PINFU);
    }
  }
  else {
    if(info.ankoNum >= 4) {
      appendYaku(YAKUMAN_SUANKO);
    }
    else if(info.ankoNum == 3) {
      appendYaku(YAKU_SANANKO);
    }
    if(info.kantsuNum >= 4) {
      appendYaku(YAKUMAN_SUKANTSU);
    }
    else if(info.kantsuNum == 3) {
      appendYaku(YAKU_SANKANTSU);
    }
    if(info.shuntsuNum == 0) {
      appendYaku(YAKU_TOITOI);
    }
  }
}
/***********************************************************************//**
	@brief 一盃口/二盃口
***************************************************************************/
void Agari::checkIpeikou(const Info& info) {
  if(info.ipeikouNum >= 2) {
    appendYaku(YAKU_RYANPEIKOU);
  }
  else if(info.ipeikouNum == 1) {
    appendYaku(YAKU_IPEIKOU);
  }
}
/***********************************************************************//**
	@brief 三色同順/三色同刻のチェック
***************************************************************************/
void Agari::checkSanshoku(const Info& info) {
  for(int i = 0; i < 9; i++) {
    if(info.shuntsuColor[i] == COLOR_ALL) {
      appendYaku(YAKU_SANSHOKU);
    }
    if((info.kotsuColor[i] & COLOR_ALL) == COLOR_ALL) {
      appendYaku(YAKU_SANSHOKUDOUKOU);
    }
  }
}
/***********************************************************************//**
	@brief 一気通貫
***************************************************************************/
void Agari::checkItsu(const Info& info) {
  if((info.shuntsuColor[0] &
      info.shuntsuColor[3] &
      info.shuntsuColor[6]) != 0) {
    appendYaku(YAKU_ITSU);
  }
}
/***********************************************************************//**
	@brief 染め手(混一色/清一色/字一色/九蓮宝燈)
***************************************************************************/
void Agari::checkColor(const Info& info) {
  switch(info.color) {
  case COLOR_ZIHAI:
    appendYaku(YAKUMAN_TSUISOU);
    break;
  case COLOR_MANZU:
  case COLOR_PINZU:
  case COLOR_SOUZU:
    appendYaku(YAKU_CHINITSU);
    if(info.count[0] >= 3 && info.count[8] >= 3) {
      bool isChuren = true;
      for(int i = 1; i < 8; i++) {
        if(info.count[i] == 0) {
          isChuren = false;
          break;
        }
      }
      if(isChuren) {
        appendYaku(YAKUMAN_CHUREN);
      }
    }
    break;
  case COLOR_MANZU | COLOR_ZIHAI:
  case COLOR_PINZU | COLOR_ZIHAI:
  case COLOR_SOUZU | COLOR_ZIHAI:
    appendYaku(YAKU_HONITSU);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 三元牌のチェック
***************************************************************************/
void Agari::checkSangen(const Info& info) {
  int num = 0;
  for(int i = 4; i < 7; i++) {
    if(info.kotsuColor[i] & COLOR_ZIHAI) {
      num += 3;
    }
    else if(info.toitsuColor[i] & COLOR_ZIHAI) {
      num += 2;
    }
  }
  if(num == 9) {
    appendYaku(YAKUMAN_DAISANGEN);
  }
  else if(num == 8) {
    appendYaku(YAKU_SHOSANGEN);
  }
}
/***********************************************************************//**
	@brief 四喜和のチェック
***************************************************************************/
void Agari::checkSushiho(const Info& info) {
  int num = 0;
  for(int i = 0; i < 4; i++) {
    if(info.kotsuColor[i] & COLOR_ZIHAI) {
      num += 3;
    }
    else if(info.toitsuColor[i] & COLOR_ZIHAI) {
      num += 2;
    }
  }
  if(num >= 12) {
    appendYaku(YAKUMAN_DAISUSHI);
  }
  else if(num == 11) {
    appendYaku(YAKUMAN_SHOSUSHI);
  }
}
/***********************************************************************//**
	@brief 清老頭/混老頭/純チャン/チャンタ/タンヤオ
***************************************************************************/
void Agari::checkKind(const Info& info) {
  switch(info.kind) {
  case KIND_CHANTA | KIND_YAOCHU:
    appendYaku(YAKUMAN_CHINROUTOU);
    break;
  case KIND_CHANTA | KIND_YAOCHU | KIND_ZIHAI:
    appendYaku(YAKU_HONROUTOU);
    break;
  case KIND_CHANTA | KIND_YAOCHU | KIND_TANYAO:
    appendYaku(YAKU_JUNCHAN);
    break;
  case KIND_CHANTA | KIND_YAOCHU | KIND_ZIHAI | KIND_TANYAO:
    appendYaku(YAKU_CHANTA);
    break;
  case KIND_TANYAO:
    appendYaku(YAKU_TANYAO);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 牌を数える
	@param[in] hai 数える牌
	@return 牌の数
***************************************************************************/
size_t Agari::countHai(const Hai* hai) const {
  size_t num = 0;
  for(auto& mentsu : mentsus_) {
    num += mentsu.countSame(hai);
  }
  return num;
}
/***********************************************************************//**
	@brief 役になる牌列を取得する
***************************************************************************/
HaiArray Agari::GetYakuhais(const Player& player) {
  static const HaiArray SANGENHAIS("5z6z7z");
  HaiArray hais(SANGENHAIS);
  if(player.getBakaze()) {
    hais.push_back(player.getBakaze());
  }
  if(player.getZikaze()) {
    hais.push_back(player.getZikaze());
  }
  return hais;
}
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Agari::Info::Info()
  : kind(KIND_CHANTA | KIND_ALL_GREEN), 
    color(0), 
    shuntsuNum(0), 
    kotsuNum(0), 
    ankoNum(0), 
    kantsuNum(0), 
    ipeikouNum(0), 
    isPinfu(true)
{
  for(int i = 0; i < 9; i++) {
    shuntsuColor[i] = 0;
    kotsuColor[i] = 0;
    toitsuColor[i] = 0;
    ipeikouColor[i] = 0;
    count[i] = 0;
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
