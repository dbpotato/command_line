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

#include <unistd.h>

// #1 you want to copy from here (no need to change anything there)

#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <string>

struct CmdLine {
  void(*create)(void*);
  void(*destroy)();
  void(*add)(const std::string&, void(*)(const std::vector<std::string>&, void*));
  void(*set_log_func)(void(*)(const char*));
  void(*run_local)();
  bool(*run_server)(int);
  bool _is_valid;
  void* _lib_handler;

  CmdLine()
    : create(NULL)
    , destroy(NULL)
    , add(NULL)
    , set_log_func(NULL)
    , run_local(NULL)
    , run_server(NULL)
    , _is_valid(false)
    , _lib_handler(NULL) {

    if(!(_lib_handler = dlopen("libcmdline.so", RTLD_NOW))) {
      printf("CmdLine : Can't load libcmdline.so %s\n", dlerror());
    }
    else {
      create = (void(*)(void*)) dlsym(_lib_handler, "cmdl_create");
      destroy = (void(*)()) dlsym(_lib_handler, "cmdl_destroy");
      add = (void(*)(const std::string&, void(*)(const std::vector<std::string>&, void*))) dlsym(_lib_handler, "cmdl_add");
      set_log_func = (void(*)(void(*)(const char*))) dlsym(_lib_handler, "cmdl_set_log_func");
      run_local = (void(*)()) dlsym(_lib_handler, "cmdl_run_local");
      run_server = (bool(*)(int)) dlsym(_lib_handler, "cmdl_run_server");
      if(create && destroy && add && set_log_func && run_local && run_server)
        _is_valid = true;
      else
        printf("CmdLine : Can't load some functions\n");
    }
  }

  ~CmdLine() {
    if(destroy)
      destroy();
    if(_lib_handler)
      dlclose(_lib_handler);
  }
};
CmdLine g_command_line;
//to here

bool g_run = true;

class AppObj {
public:
  static void print_msg(const char* msg){
    printf("Example integraton : %s", msg);
  }
  void SetRunFlag(bool enabled){
    g_run = enabled;
  }
  void DoNothing(){
    print_msg("Doing nothing\n");
  }
};

// #2 create your callback functions
void do_something(const std::vector<std::string>& args, void* obj) {
  AppObj* app_obj = (AppObj*)obj;
  app_obj->DoNothing();
  if(args.size()) {
    printf("cmd command with args : ");
    for(size_t i = 0; i < args.size(); ++i)
      printf("%s ", args.at(i).c_str());
    printf("\n");
  }
  else
    printf("cmd command \n");
}

void do_exit(const std::vector<std::string>& args, void* obj) {
  AppObj* app_obj = (AppObj*)obj;
  app_obj->SetRunFlag(false);
}


int main() {
  AppObj app_obj;
// #3 and copy from here (you would want to change few things)
  if(g_command_line._is_valid) { //check if library has loaded successfully
    g_command_line.create((void*)&app_obj); // object pointer will be passed as argument to every callback function
    g_command_line.set_log_func(AppObj::print_msg); //set custom log function (by default printf will be used)

    g_command_line.add("cmd", do_something); //Register a command
    g_command_line.add("q", do_exit); //Register another command

    g_command_line.run_local(); //Start listening to commands typed in local console
    g_command_line.run_server(5566); //Start listening to commands on network port
  }
//to here

  while(g_run)
    usleep(1000000);

  return 0;
}
