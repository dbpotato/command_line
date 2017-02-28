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

#ifndef POSIX_THREAD_H
#define POSIX_THREAD_H

#include <pthread.h>

/**
 * pthread wrapper
 */
class PosixThread {
private:
  void*(*_thread_func)(void*); ///< function which will be called on thread strart
  void* _thread_obj; ///< object which can be stored with Run method
  pthread_t _thread; ///< pthread handler
  bool _run_thread; ///< run flag
  pthread_mutex_t _run_flag_mutex; ///< run flag mutex
  bool _exists; ///< indicateds if thread was created

public:
  PosixThread();
  ~PosixThread();
  /**
   * Starts detached thread by calling pthread_create with passed function and self pointer.
   * /param thread_func function which will be passed as param to pthread creation
   * /param obj object which will be available to thread_func trough GetObj method
   */
  void Run(void*(*thread_func)(void*), void* obj);
  /**
   * Returns pointer passed in Run method.
   */
  void* GetObj();
  /**
   * Sets stop flag. Thread safe.
   */
  void Stop();
  /**
   * Gets stop flag. Thread safe.
   */
  bool IsRunning();
  /**
   * Joins to current thread
   */
  void Join();
};

#endif //POSIX_THREAD_H
