/*
Copyright (c) 2018 Adam Kaniewski

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

#pragma once

#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <string>
#include <stdarg.h>

class CmdLine {
private :
  void(*create)();
  void(*destroy)();
  bool(*add)(const std::string&, void(*)(const std::vector<std::string>&, void*), void*);
  void(*set_log_func)(void(*)(const char*));
  void(*log)(const char*);
  void(*run_local)();
  bool(*run_server)(int);
  bool _is_valid;
  void* _lib_handler;

protected:
  CmdLine()
      : create(nullptr)
      , destroy(nullptr)
      , add(nullptr)
      , set_log_func(nullptr)
      , log(nullptr)
      , run_local(nullptr)
      , run_server(nullptr)
      , _is_valid(false)
      , _lib_handler(nullptr) {

    if(!(_lib_handler = dlopen("libcmdline.so", RTLD_NOW))) {
      printf("CmdLine : Can't load libcmdline.so : %s\n", dlerror());
    }
    else {
      create = (void(*)()) dlsym(_lib_handler, "cmdl_create");
      destroy = (void(*)()) dlsym(_lib_handler, "cmdl_destroy");
      add = (bool(*)(const std::string&, void(*)(const std::vector<std::string>&, void*), void*)) dlsym(_lib_handler, "cmdl_add");
      set_log_func = (void(*)(void(*)(const char*))) dlsym(_lib_handler, "cmdl_set_log_func");
      log = (void(*)(const char*)) dlsym(_lib_handler, "cmdl_log");
      run_local = (void(*)()) dlsym(_lib_handler, "cmdl_run_local");
      run_server = (bool(*)(int)) dlsym(_lib_handler, "cmdl_run_server");
      if(create && destroy && add && set_log_func && log && run_local && run_server) {
        _is_valid = true;
        create();
      }
      else
        printf("CmdLine : Can't load some functions\n");
    }
  }
public:
  static CmdLine& Instance() {
    static CmdLine instance;
    return instance;
  }
  ~CmdLine() {
    if(destroy)
      destroy();
    if(_lib_handler)
      dlclose(_lib_handler);
  }

  bool Add(const std::string& name,
           void(*callback)(const std::vector<std::string>&, void*),
           void* obj = nullptr) {
    if(_is_valid)
      return add(name, callback, obj);
    return false;
  }

  bool RunLocal() {
    if(!_is_valid)
      return false;

    run_local();
    return true;
  }

  bool RunServer(int port) {
    if(_is_valid)
      return run_server(port);

    return false;
  }

  bool SetLogFunc(void(*callback)(const char*)) {
    if(!_is_valid)
      return false;
    set_log_func(callback);
    return true;
  }

  bool IsValid() {
    return _is_valid;
  }

  void Log(const char* format, ...) {
    if(!_is_valid)
      return;
    va_list args1;
    va_start(args1, format);
    va_list args2;
    va_copy(args2, args1);
    std::vector<char> buf(1+std::vsnprintf(nullptr, 0, format, args1));
    va_end(args1);
    std::vsnprintf(buf.data(), buf.size(), format, args2);
    va_end(args2);
    log(std::string(buf.begin(),buf.end()).c_str());
  }
};
