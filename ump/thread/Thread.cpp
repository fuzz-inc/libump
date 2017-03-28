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
bool Thread::sleep(int ms) {
  return cond_.wait(ms);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
