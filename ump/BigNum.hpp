/***********************************************************************//**
	@file
***************************************************************************/
#pragma once

namespace ump {
/***********************************************************************//**
	@brief 
***************************************************************************/
class BigNum {
 private:
  class Ctx;

 private:
  BIGNUM* bigNum_;

 public:
  BigNum();
  BigNum(int value);
  BigNum(const char* string);
  BigNum(const std::string& string);
  BigNum(const BigNum& src);
  ~BigNum();

  BigNum& operator=(const BigNum& src);

  BigNum operator+(const BigNum& rhs) const;
  BigNum operator-(const BigNum& rhs) const;
  BigNum operator*(const BigNum& rhs) const;
  BigNum operator/(const BigNum& rhs) const;
  BigNum operator%(const BigNum& rhs) const;

  BigNum operator-() const;

  BigNum& operator+=(const BigNum& rhs);
  BigNum& operator-=(const BigNum& rhs);
  BigNum& operator*=(const BigNum& rhs);
  BigNum& operator/=(const BigNum& rhs);
  BigNum& operator%=(const BigNum& rhs);

  int compare(const BigNum& rhs) const;
  bool operator<=(const BigNum& rhs) const;
  bool operator<(const BigNum& rhs) const;
  bool operator>=(const BigNum& rhs) const;
  bool operator>(const BigNum& rhs) const;
  bool operator==(const BigNum& rhs) const;
  bool operator!=(const BigNum& rhs) const;

  BigNum& neg();
  BigNum& abs();
  BigNum getAbs() const;

  std::string toString(bool showpos = false) const;
};
/***********************************************************************//**
	@brief 
***************************************************************************/
class BigNum::Ctx {
 private:
  BN_CTX* ctx_;

 public:
  Ctx();
  ~Ctx();

  BN_CTX* get() const {
    return ctx_;
  }
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
