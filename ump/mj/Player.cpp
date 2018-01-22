/*
Copyright 2017 fuzz, Inc. All rights reserved. 
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
#include "ump/mj/AgariParser.hpp"
#include "ump/mj/Game.hpp"
#include "ump/mj/Player.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Player::Player()
  : name_("noname"), 
    point_(0), 
    seat_(std::numeric_limits<size_t>::max()), 
    bakaze_(nullptr), 
    zikaze_(nullptr), 
    tsumoHai_(nullptr), 
    sutehai_(nullptr), 
    shanten_(*this)
{}
/***********************************************************************//**
	@copydoc Hand::reset
***************************************************************************/
void Player::reset() {
  super::reset();
  flag_.reset();
  flag_.set(FLAG_FIRST);
}
/***********************************************************************//**
	@brief ゲームをセットする
	@param[in] game ゲーム
***************************************************************************/
void Player::setGame(std::shared_ptr<Game> game) {
  assert(!game || !game_.lock());
  game_ = game;
}
/***********************************************************************//**
	@brief ゲームを取得する
	@return ゲーム
***************************************************************************/
std::shared_ptr<Game> Player::getGame() const {
  return game_.lock();
}
/***********************************************************************//**
	@brief 純粋な一巡目(天和/地和/人和可能)を無効にする
	@return 自分自身
***************************************************************************/
void Player::resetFirst() {
  flag_.reset(FLAG_FIRST);
}
/***********************************************************************//**
	@brief 純粋な一巡目(天和/地和/人和可能)か調べる
	@return 純粋な一巡目のとき真
***************************************************************************/
bool Player::isFirst() const {
  return flag_.test(FLAG_FIRST);
}
/***********************************************************************//**
	@brief テンパイ宣言
***************************************************************************/
void Player::sayTenpai() {
  flag_.set(FLAG_SAY_TENPAI);
}
/***********************************************************************//**
	@brief テンパイ宣言しているか調べる
	@return テンパイ宣言しているとき真
***************************************************************************/
bool Player::isSayTenpai() const {
  return flag_.test(FLAG_SAY_TENPAI);
}
/***********************************************************************//**
	@brief 和了った
***************************************************************************/
void Player::agari() {
  flag_.set(FLAG_AGARI);
}
/***********************************************************************//**
	@brief 和了ったか調べる
	@return 和了ったとき真
***************************************************************************/
bool Player::isAgari() const {
  return flag_.test(FLAG_AGARI);
}
/***********************************************************************//**
	@brief 鳴きがあった
***************************************************************************/
void Player::onOpen() {
  flag_.reset(FLAG_FIRST);
  flag_.reset(FLAG_IPPATSU);
}
/***********************************************************************//**
	@brief 点数をセットする
	@param[in] value 点数
	@return 自分自身
***************************************************************************/
void Player::setPoint(const BigNum& value) {
  point_ = value;
  onSetPoint(value);
}
/***********************************************************************//**
	@brief 点数を加減算する
	@param[in] value 加減算する値
	@return 自分自身
***************************************************************************/
void Player::addPoint(const BigNum& value) {
  point_ += value;
  onAddPoint(value);
}
/***********************************************************************//**
	@brief 席の文字列を取得する
***************************************************************************/
std::string Player::getSeatString() const {
  return Command::SeatToString(getSeat());
}
/***********************************************************************//**
	@brief 親か調べる
	@return 親のとき真
***************************************************************************/
bool Player::isOya() const {
  return getGame()->getOya() == getSeat();
}
/***********************************************************************//**
	@copydoc Hand::drawHaipai
***************************************************************************/
void Player::drawHaipai(const HaiArray& hais) {
  super::drawHaipai(hais);
  updateShanten();
}
/***********************************************************************//**
	@brief 牌を自摸る
	@param[in] hai ツモった牌
	@param[in] rinshan 嶺上牌のとき真
***************************************************************************/
void Player::tsumo(const Hai* hai, bool rinshan) {
  super::sort();
  tsumoHai_ = hai;
  append(hai);
  updateShanten();
  setRinshan(rinshan);
  updateAgari();
}
/***********************************************************************//**
	@brief ツモった牌を取得する
	@return ツモった牌
***************************************************************************/
const Hai* Player::getTsumoHai() const {
  return tsumoHai_;
}
/***********************************************************************//**
	@brief 和了を求める
        @param[in] ron ロン和了の牌(ツモ和了のときはnullptr)
***************************************************************************/
bool Player::updateAgari(const Hai* ron, bool chankan) {
  agari_.clear();
  if(hasUnknown()) {
    return false;
  }
  if(!isTenpai()) {
    return false;
  }
  flag_.set(FLAG_CHANKAN, chankan);
  AgariParser parser(*this);
  agari_ = parser.parse(ron);
  return agari_.isExist();
}
/***********************************************************************//**
	@brief 和了を取得する
	@return 和了
***************************************************************************/
Agari& Player::getAgari() {
  return agari_;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Player::canAgari() const {
  return agari_.isExist();
}
/***********************************************************************//**
	@brief ロンできるか調べる
	@param[in] hai 捨て牌
	@param[in] chankan 槍槓のとき真
	@return ロンできるとき真
***************************************************************************/
bool Player::canRon(const Hai* hai, bool chankan) {
  return updateAgari(hai, chankan);
}
/***********************************************************************//**
	@brief リーチをかけることができるか調べる
	@return リーチをかけることができるとき真
***************************************************************************/
bool Player::canRichi() const {
  return getGame()->canRichi() && 
    isMenzen() &&
    !isRichi() &&
    isTenpai();
}
/***********************************************************************//**
	@brief 大明槓できるか調べる
	@param[in] hai 捨て牌
	@return 大明槓できるとき真
***************************************************************************/
bool Player::canKan(const Hai* hai) const {
  return !isRichi() && getMenzen().countSame(hai) >= 3;
}
/***********************************************************************//**
	@brief 大明槓できるか調べる
	@param[in] hais 晒す牌
	@param[in] hai 捨て牌
	@return 大明槓できるとき真
***************************************************************************/
bool Player::canKan(const HaiArray& hais, const Hai* hai) const {
  return getMenzen().hasEqual(hais) && Mentsu(hais + hai).isKantsu();
}
/***********************************************************************//**
	@brief 暗槓できるか調べる
	@return 暗槓できるとき真
***************************************************************************/
bool Player::canAnkan() const {
  if(isRichi()) {
    if(auto hai = getTsumoHai()) {
      return shanten_.getKanables().hasSame(hai);
    }
  }
  else {
    auto& menzen = getMenzen();
    for(auto hai : menzen) {
      if(menzen.countSame(hai) >= 4) {
        return true;
      }
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 暗槓できるか調べる
	@param[in] hais 暗槓する牌列
	@return 暗槓できるとき真
***************************************************************************/
bool Player::canAnkan(const HaiArray& hais) const {
  assert(hais.size() == 4 && hais.getUnique().size() == 1);
  if(getMenzen().hasEqual(hais)) {
    if(isRichi()) {
      if(auto hai = getTsumoHai()) {
        return hais.at(0)->isSame(hai) && 
          shanten_.getKanables().hasSame(hai);
      }
    }
    else {
      return true;
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 加槓できるか調べる
	@return 加槓できるとき真
***************************************************************************/
bool Player::canKakan() const {
  for(auto hai : getMenzen()) {
    if(findKakanMentsu(hai)) {
      return true;
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 加槓できる面子を探す
	@param[in] hai 加槓する牌
	@return 加槓できる面子
***************************************************************************/
std::shared_ptr<Mentsu> Player::findKakanMentsu(const Hai* hai) const {
  if(getMenzen().hasEqual(hai)) {
    for(auto& mentsu : getMentsus()) {
      if(mentsu->isKotsu() && mentsu->at(0)->isSame(hai)) {
        return mentsu;
      }
    }
  }
  return nullptr;
}
/***********************************************************************//**
	@brief ポンできるか調べる
	@param[in] hai 捨て牌
	@return ポンできるとき真
***************************************************************************/
bool Player::canPon(const Hai* hai) const {
  return getGame()->canPon() &&
    !isRichi() && 
    getMenzen().countSame(hai) >= 2;
}
/***********************************************************************//**
	@brief ポンできるか調べる
	@param[in] hais 晒す牌
	@param[in] hai 捨て牌
	@return ポンできるとき真
***************************************************************************/
bool Player::canPon(const HaiArray& hais, const Hai* hai) const {
  return canPon(hai) && 
    getMenzen().hasEqual(hais) &&
    Mentsu(hais + hai).isKotsu();
}
/***********************************************************************//**
	@brief チーできるか調べる
	@param[in] hai 捨て牌
	@return チーできるとき真
***************************************************************************/
bool Player::canChi(const Hai* hai) const {
  auto& menzen = getMenzen();
  return getGame()->canChi() && 
    !isRichi() && 
    ((menzen.isInclude(hai->shift(-1)) && 
      menzen.isInclude(hai->shift(-2))) || 
     (menzen.isInclude(hai->shift(1)) && 
      menzen.isInclude(hai->shift(2))) || 
     (menzen.isInclude(hai->shift(-1)) && 
      menzen.isInclude(hai->shift(1))));
}
/***********************************************************************//**
	@brief チーできるか調べる
	@param[in] hais 晒す牌
	@param[in] hai 捨て牌
	@return チーできるとき真
***************************************************************************/
bool Player::canChi(const HaiArray& hais, const Hai* hai) const {
  return canChi(hai) && 
    getMenzen().hasEqual(hais) &&
    Mentsu(hais + hai).isShuntsu();
}
/***********************************************************************//**
	@brief 牌を河に捨てる
	@param[in] sutehai 捨て牌
	@return 河の捨て牌
***************************************************************************/
Sutehai* Player::sutehai(const Sutehai& sutehai) {
  assert(!sutehai.isNaki());
  sutehai_ = super::sutehai(sutehai);
  tsumoHai_ = nullptr;
  super::sort();
  updateShanten();
  if(sutehai.isRichi()) {
    flag_.set(FLAG_RICHI);
    flag_.set(FLAG_IPPATSU);
    if(isFirst()) {
      flag_.set(FLAG_DOUBLE_RICHI);
    }
  }
  else {
    flag_.reset(FLAG_IPPATSU);
  }
  flag_.reset(FLAG_FIRST);
  setRinshan(false);
  onSutehai(sutehai_);
  return sutehai_;
}
/***********************************************************************//**
	@brief 適当な牌を捨てる
	@param[out] sutehai 捨て牌
***************************************************************************/
void Player::discardAnyHai(Sutehai& sutehai) const {
  if(auto hai = getTsumoHai()) {
    sutehai.setHai(hai);
    sutehai.setTsumogiri(true);
  }
  else {
    hai = getMenzen().back();
    assert(hai);
    sutehai.setHai(hai);
    sutehai.setTsumogiri(false);
  }
}
/***********************************************************************//**
	@brief シャンテン数を取得する
	@return シャンテン数
***************************************************************************/
int Player::getShanten() const {
  return isRichi() ? 0 : shanten_.getShanten();
}
/***********************************************************************//**
	@brief テンパイ形か調べる
	@return	テンパイのとき真
***************************************************************************/
bool Player::isTenpai() const {
  return shanten_.isTenpai();
}
/***********************************************************************//**
	@brief リーチがかけられる牌を取得する
	@return	リーチがかけられる捨て牌の配列
***************************************************************************/
const HaiArray& Player::getRichiableHai() const {
  return shanten_.getRichi();
}
/***********************************************************************//**
	@brief ハイテイか調べる
	@return ハイテイのとき真
***************************************************************************/
bool Player::isHaitei() const {
  return getGame()->getRest() == 0;
}
/***********************************************************************//**
	@copydoc Hand::onOpenMentsu
***************************************************************************/
void Player::onOpenMentsu(std::shared_ptr<const Mentsu> mentsu) {
  super::onOpenMentsu(mentsu);
  if(auto hai = mentsu->getClaimHai()) {
    if(auto sutehai = getGame()->getLastSutehai()) {
      if(sutehai->getHai()->isEqual(hai)) {
        sutehai->setNaki();
      }
    }
  }
}
/***********************************************************************//**
	@copydoc Hand::onShowHai
***************************************************************************/
void Player::onShowHai(const Hai* hai) {
  getGame()->onShowHai(hai);
}
/***********************************************************************//**
	@brief シャンテン数を計算する
***************************************************************************/
void Player::updateShanten() {
  if(!hasUnknown() && !isRichi()) {
    shanten_.update();
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
