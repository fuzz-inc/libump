﻿/***********************************************************************//**
	@file
***************************************************************************/
#include "ump/thread/Thread.hpp"

namespace ump {
namespace thread {
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Thread::~Thread() {
  stop();
}
/***********************************************************************//**
	@brief スレッドを開始する
	@param[in] thread スレッド
***************************************************************************/
void Thread::start(std::thread* thread) {
  stop();
  thread_.reset(thread);
}
/***********************************************************************//**
	@brief スレッドを停止する
***************************************************************************/
void Thread::stop() {
  if(isStart()) {
    stop_.notify();
    thread_->join();
    stop_.reset();
    thread_.reset();
  }
}
/***********************************************************************//**
	@brief スレッドが開始しているか調べる
	@return スレッドが開始しているとき真
***************************************************************************/
bool Thread::isStart() const {
  return bool(thread_);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Thread::sleep(int ms) {
  return stop_.wait(ms);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Thread::sleep() {
  stop_.wait();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Thread::SetThreadName(const char* name) {
#if defined(UMP_PLATFORM_MAC)
  pthread_setname_np(name);
#endif
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
