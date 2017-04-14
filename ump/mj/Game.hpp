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
#include "ump/mj/Hai.hpp"
#include "ump/mj/HaiArray.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
        @brief ゲーム(半荘)基底クラス
***************************************************************************/
class Game
  : public std::enable_shared_from_this<Game>
{
 private:
  enum {
    FLAG_RENCHAN, 
    FLAG_MAX
  };

 private:
  std::shared_ptr<const Config> config_;
  std::unique_ptr<std::default_random_engine> random_;
  std::string id_;
  std::vector<std::shared_ptr<Player>> players_;
  size_t oya_;
  size_t round_;
  int renchan_;
  BigNum kyotaku_;
  HaiArray dora_;
  size_t turn_;
  Sutehai* lastSutehai_;
  std::bitset<FLAG_MAX> flag_;

 public:
  Game(std::shared_ptr<const Config> config);
  virtual ~Game();

  const Config& getConfig() const {
    return *config_;
  }

  /**
   * 乱数生成器を取得する
   * @return 乱数生成器
   */
  std::default_random_engine& getRandom() const {
    return *random_;
  }

  UMP_ACCESSOR(Id, id_);
  UMP_GETTER(Players, players_);
  void setPlayer(size_t seat, std::shared_ptr<Player> player);
  size_t countPlayer() const;
  std::shared_ptr<Player> getPlayer(size_t seat) const;
  std::shared_ptr<Player> getPlayer(const char* seat) const;

  UMP_GETTER(Oya, oya_);
  UMP_GETTER(Round, round_);
  UMP_GETTER(Renchan, renchan_);

  UMP_GETTER(Kyotaku, kyotaku_);
  Game& addKyotaku(const BigNum& value);
  BigNum resetKyotaku();

  Game& setTurn(size_t turn);
  UMP_GETTER(Turn, turn_);
  void resetTurn();
  size_t nextTurn();

  Sutehai* sutehai(Player& player, const Sutehai& sutehai);
  UMP_GETTER(LastSutehai, lastSutehai_);

  UMP_BIT_GETTER(Renchan, flag_, FLAG_RENCHAN);

  const Hai* getBakaze() const;

  UMP_GETTER(Dora, dora_);
  void appendDora(const Hai* hai);

  void beginKyoku();
  void endKyoku();

  bool isAgari() const;;
  bool isRyukyoku() const;

  virtual size_t getRest() const;

  bool canRichi() const;
  bool canKan() const;
  bool canPon() const;
  bool canChi() const;

  /**
   * 牌が見えたときの処理
   * @param[in] hai 見えた牌
   */
  virtual void onShowHai(const Hai* hai) {}

 protected:
  void clearPlayer();

  Game& setOya(size_t oya);
  UMP_SETTER(Round, round_);
  UMP_SETTER(Renchan, renchan_);
  UMP_SETTER(Kyotaku, kyotaku_);
};
/***********************************************************************//**
        $Id$
***************************************************************************/
}
}
