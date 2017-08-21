/***********************************************************************//**
	@file
***************************************************************************/
#pragma once

#include "ump/thread/Condition.hpp"

namespace ump {
namespace thread {
/***********************************************************************//**
	@brief スレッド
***************************************************************************/
class Thread {
 private:
  std::unique_ptr<std::thread> thread_;
  Condition cond_;

 public:
  Thread(std::thread* thread);
  ~Thread();

  bool sleep(int ms);
  void sleep();

  static void SetThreadName(const char* name);

 protected:
  Thread() = default;

  void startThread(std::thread* thread);
  void stopThread();
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
