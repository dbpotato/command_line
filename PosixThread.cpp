/*
Copyright (c) 2017 Adam Kaniewski

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <cstdio>
#include "PosixThread.h"

PosixThread::PosixThread()
    : _thread_func(NULL)
    , _thread_obj(NULL)
    , _exists(false)
    , _run_thread(false) {
  pthread_mutex_init(&_run_flag_mutex, NULL);
}

PosixThread::~PosixThread() {
  pthread_mutex_destroy(&_run_flag_mutex);
}

void PosixThread::Run(void*(*thread_func)(void*), void* obj) {
  if(!_exists) {
    _exists = true;
    _thread_func = thread_func;
    _thread_obj = obj;
    _run_thread = true;
    pthread_create(&_thread, NULL, thread_func, this);
  }
}

void* PosixThread::GetObj() {
  return _thread_obj;
}

void PosixThread::Stop() {
  pthread_mutex_lock(&_run_flag_mutex);
  _run_thread = false;
  pthread_mutex_unlock(&_run_flag_mutex);
}

bool PosixThread::IsRunning() {
  bool running = false;
  pthread_mutex_lock(&_run_flag_mutex);
  running = _run_thread;
  pthread_mutex_unlock(&_run_flag_mutex);
  return running;
}

void PosixThread::Join() {
  if(_exists)
    pthread_join(_thread, NULL);
}
