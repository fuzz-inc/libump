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

  BigNum operator+(const BigNum& rhs) const;
  BigNum operator-(const BigNum& rhs) const;
  BigNum operator*(const BigNum& rhs) const;
  BigNum operator/(const BigNum& rhs) const;
  BigNum operator%(const BigNum& rhs) const;

  BigNum& operator+=(const BigNum& rhs);
  BigNum& operator-=(const BigNum& rhs);
  BigNum& operator*=(const BigNum& rhs);
  BigNum& operator/=(const BigNum& rhs);
  BigNum& operator%=(const BigNum& rhs);

  std::string toString() const;
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
