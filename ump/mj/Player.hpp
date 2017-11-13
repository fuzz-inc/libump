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

#include "ump/BigNum.hpp"
#include "ump/mj/Agari.hpp"
#include "ump/mj/Hand.hpp"
#include "ump/mj/Shanten.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief プレイヤー基底クラス
***************************************************************************/
class Player
  : public Hand
{
  typedef Hand super;

 private:
  enum {
    FLAG_FIRST, 
    FLAG_RICHI, 
    FLAG_DOUBLE_RICHI, 
    FLAG_IPPATSU, 
    FLAG_SAY_TENPAI, 
    FLAG_AGARI, 
    FLAG_RINSHAN, 
    FLAG_MAX
  };

 private:
  std::weak_ptr<Game> game_;
  std::bitset<FLAG_MAX> flag_;
  std::string name_;
  BigNum point_;
  size_t seat_;
  const Hai* bakaze_;
  const Hai* zikaze_;
  const Hai* tsumoHai_;
  Sutehai* sutehai_;
  Shanten shanten_;
  Agari agari_;

 public:
  Player();
  ~Player() override = default;

  void reset() override;

  virtual void setGame(std::shared_ptr<Game> game);
  std::shared_ptr<Game> getGame() const;

  void resetFirst();
  bool isFirst() const;

  UMP_BIT_ACCESSOR(Richi, flag_, FLAG_RICHI);
  UMP_BIT_GETTER(DoubleRichi, flag_, FLAG_DOUBLE_RICHI);
  UMP_BIT_GETTER(Ippatsu, flag_, FLAG_IPPATSU);

  void sayTenpai();
  bool isSayTenpai() const;

  void agari();
  bool isAgari() const;

  UMP_BIT_ACCESSOR(Rinshan, flag_, FLAG_RINSHAN);

  void onOpen();

  UMP_ACCESSOR(Name, name_);

  void setPoint(const BigNum& value);
  void addPoint(const BigNum& value);
  UMP_GETTER(Point, point_);

  UMP_ACCESSOR(Seat, seat_);
  std::string getSeatString() const;
  bool isOya() const;

  UMP_ACCESSOR(Bakaze, bakaze_);
  UMP_ACCESSOR(Zikaze, zikaze_);

  virtual void tsumo(const Hai* hai, bool rinshan);
  const Hai* getTsumoHai() const;

  bool updateAgari(const Hai* ron = nullptr);
  Agari& getAgari();
  bool canAgari() const;
  bool canRon(const Hai* hai);

  bool canRichi() const;

  bool canKan(const Hai* hai) const;
  bool canKan(const HaiArray& hais, const Hai* hai) const;

  bool canAnkan() const;
  bool canAnkan(const HaiArray& hais) const;

  bool canKakan() const;
  std::shared_ptr<Mentsu> findKakanMentsu(const Hai* hai) const;

  bool canPon(const Hai* hai) const;
  bool canPon(const HaiArray& hais, const Hai* hai) const;

  bool canChi(const Hai* hai) const;
  bool canChi(const HaiArray& hais, const Hai* hai) const;

  virtual Sutehai* sutehai(const Sutehai& sutehai);
  void discardAnyHai(Sutehai& sutehai) const;

  int getShanten() const;
  bool isTenpai() const;
  const HaiArray& getRichiableHai() const;

  bool isHaitei() const;

  virtual void drawHaipai(const HaiArray& haipai, 
                          const Command& command);

 protected:
  /**
   * 点数がセットされたときの処理
   * @param[in] value 点数
   */
  virtual void onSetPoint(const BigNum& value) {}

  /**
   * 点数が加減算されたときの処理
   * @param[in] value 加算値
   */
  virtual void onAddPoint(const BigNum& value) {}

  /**
   * 捨て牌時の処理
   * @param[in] sutehai 捨て牌
   */
  virtual void onSutehai(const Sutehai* sutehai) {}

  void onShowHai(const Hai* hai) override;

 private:
  void updateShanten();
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
