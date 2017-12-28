/***********************************************************************//**
	@file
***************************************************************************/
#pragma once

namespace ump {
namespace thread {
/***********************************************************************//**
	@brief 簡易アトミック操作
***************************************************************************/
template <class T>
class Atomic {
 private:
  T value_;
  mutable std::mutex mutex_;

 public:
  template <class... Args>
  Atomic(const Args&&... args)
    : value_(args...)
  {}
  ~Atomic() = default;

  void set(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    value_ = value;
  }

  T get() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return value_;
  }

  void operator()(std::function<void(T& value)> func) {
    std::lock_guard<std::mutex> lock(mutex_);
    func(value_);
  }

  void operator()(std::function<void(const T& value)> func) const {
    std::lock_guard<std::mutex> lock(mutex_);
    func(value_);
  }

  template <class Result>
  Result eval(std::function<Result(T& value)> func) {
    std::lock_guard<std::mutex> lock(mutex_);
    return func(value_);
  }

  template <class Result>
  Result eval(std::function<Result(const T& value)> func) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return func(value_);
  }
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
