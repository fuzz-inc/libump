/***********************************************************************//**
	@file
***************************************************************************/
#pragma once

namespace ump {
namespace thread {
/***********************************************************************//**
	@brief 
***************************************************************************/
class Condition {
 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  bool notify_;

 public:
  Condition();
  ~Condition() = default;

  bool isNotify() const {
    return notify_;
  }

  void notify();
  void wait();
  bool wait(int ms);
  void reset();
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
