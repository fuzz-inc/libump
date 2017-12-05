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
  Condition stop_;

 public:
  Thread() = default;
  virtual ~Thread();

  void start();
  void start(std::thread* thread);
  bool isStart() const;

  void stop();
  bool isStop() const;

  void detach();

  bool sleep(int ms);
  void sleep();

  virtual void operator()();

  static void SetThreadName(const char* name);
  static std::string GetDemangleName(const char* name);

 protected:
  void setThreadName();
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
