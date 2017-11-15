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
#include "ump/mj/Mentsu.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 和了形
***************************************************************************/
class Agari {
 public:
  enum {
    YAKU_RICHI, 		/**< リーチ */
    YAKU_DOUBLE_RICHI, 		/**< ダブルリーチ */
    YAKU_IPPATSU, 		/**< 一発 */
    YAKU_RINSHAN, 		/**< 嶺上開花 */
    YAKU_HAITEI, 		/**< 海底摸月 */
    YAKU_HOUTEI, 		/**< 河底撈魚 */
    YAKU_TANYAO, 		/**< タンヤオ */
    YAKU_PINFU, 		/**< 平和 */
    YAKU_TSUMO, 		/**< 門前自摸 */
    YAKU_IPEIKOU, 		/**< 一盃口 */
    YAKU_CHITOI, 		/**< 七対子 */
    YAKU_CHANTA, 		/**< チャンタ */
    YAKU_JUNCHAN, 		/**< 純チャン */
    YAKU_HONROUTOU, 		/**< 混老頭 */
    YAKU_TOITOI, 		/**< 対々和 */
    YAKU_SANANKO, 		/**< 三暗刻 */
    YAKU_SANKANTSU, 		/**< 三槓子 */
    YAKU_RYANPEIKOU, 		/**< 二盃口 */
    YAKU_SANSHOKU, 		/**< 三色同順 */
    YAKU_SANSHOKUDOUKOU, 	/**< 三色同刻 */
    YAKU_ITSU, 			/**< 一気通貫 */
    YAKU_YAKUHAI, 		/**< 役牌 */
    YAKU_SHOSANGEN, 		/**< 小三元 */
    YAKU_HONITSU, 		/**< 混一色 */
    YAKU_CHINITSU, 		/**< 清一色 */
    YAKUMAN, 			/**< 役満 */
    YAKUMAN_KOKUSHI, 		/**< 国士無双 */
    YAKUMAN_DAISANGEN, 		/**< 大三元 */
    YAKUMAN_CHINROUTOU, 	/**< 清老頭 */
    YAKUMAN_SUANKO, 		/**< 四暗刻 */
    YAKUMAN_SUKANTSU, 		/**< 四槓子 */
    YAKUMAN_SHOSUSHI, 		/**< 小四喜 */
    YAKUMAN_DAISUSHI, 		/**< 大四喜 */
    YAKUMAN_TSUISOU, 		/**< 字一色 */
    YAKUMAN_RYUISOU, 		/**< 緑一色 */
    YAKUMAN_CHUREN, 		/**< 九蓮宝燈 */
    YAKUMAN_TENHO, 		/**< 天和 */
    YAKUMAN_CHIHO, 		/**< 地和 */
    YAKUMAN_RENHO, 		/**< 人和 */
    YAKU_MAX
  };

 private:
  enum {
    FLAG_YAKUMAN, 
    FLAG_MAX
  };

  enum {
    KIND_TANYAO		= 1 << 0, 
    KIND_YAOCHU		= 1 << 1, 
    KIND_ZIHAI		= 1 << 2, 
    KIND_CHANTA		= 1 << 3, 
    KIND_ALL_GREEN	= 1 << 4, 
  };

  struct YakuTable {
    int menzenHan;
    int nakiHan;
    const char* name;
  };

  struct Info {
    int kind;
    int color;
    int shuntsuNum;
    int kotsuNum;
    int ankoNum;
    int kantsuNum;
    int shuntsuColor[9];
    int kotsuColor[9];
    int toitsuColor[9];
    int ipeikouColor[9];
    int ipeikouNum;
    int count[9];
    bool isPinfu;

    Info();
  };

 private:
  std::bitset<FLAG_MAX> flag_;
  std::vector<Mentsu> mentsus_;	/**< 構成面子 */
  int yakuHan_[YAKU_MAX];	/**< 役ごとの翻数 */
  int han_;			/**< 総翻数(役満数) */
  int fu_;			/**< 符(切り上げ後) */
  int srcFu_;			/**< 符(切り上げ前) */
  int dora_;			/**< ドラの数 */
  BigNum point_;		/**< 点数 */
  std::string text_;

  static const YakuTable YakuTable_[YAKU_MAX];

 public:
  Agari();
  Agari(const Agari& src) = default;
  ~Agari() = default;

  void clear();

  void appendMentsu(const Mentsu& mentsu);
  size_t countMentsu() const;
  Mentsu& getMentsu(size_t index);

  void update(const Player& player);

  int getHan() const;
  int getHan(int yaku) const;
  UMP_GETTER(Fu, fu_);
  UMP_GETTER(SrcFu, srcFu_);
  int getYakuman() const;
  UMP_GETTER(Dora, dora_);
  UMP_GETTER(Point, point_);
  UMP_GETTER(Text, text_);

  bool isExist() const;
  bool isYakuman() const;
  bool isInclude(int yaku) const;
  bool isRon() const;

  void appendYaku(int yaku);

  bool operator<(const Agari& rhs) const;

  std::string toString() const;

  static const char* GetYakuName(int yaku);

  /**
   * @brief 数値を切り上げる
   * @param[in] value 値
   * @param[in] base 基数
   * @return 切り上げた値
   */
  template <class T>
  static T Ceil(const T& value, int base) {
    return (value + base - 1) / base * base;
  }

 private:
  void clearYaku();

  bool isMenzen() const;

  void updateInfo(Info& info, const Player& player);
  void updateKind(Info& info, const Mentsu& mentsu) const;
  void updateDora(const Player& player);
  void updatePoint(const Player& player);
  void updateFu(const Player& player);

  void checkYakuhai(Info& info, const Player& player);
  void checkKotsu(const Info& info);
  void checkIpeikou(const Info& info);
  void checkSanshoku(const Info& info);
  void checkItsu(const Info& info);
  void checkColor(const Info& info);
  void checkSangen(const Info& info);
  void checkSushiho(const Info& info);
  void checkKind(const Info& info);

  size_t countHai(const Hai* hai) const;

  static HaiArray GetYakuhais(const Player& player);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
