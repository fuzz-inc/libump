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
	@brief 
***************************************************************************/
void Thread::start(std::thread* thread) {
  stop();
  thread_.reset(thread);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Thread::stop() {
  if(thread_) {
    cond_.notify();
    thread_->join();
    thread_.reset();
    cond_.reset();
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Thread::isStop() const {
  return cond_.isNotify();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Thread::sleep(int ms) {
  return cond_.wait(ms);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Thread::sleep() {
  cond_.wait();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Thread::detach() {
  if(thread_) {
    thread_.release()->detach();
  }
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
