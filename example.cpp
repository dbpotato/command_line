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

#include <unistd.h>
#include <atomic>
#include <sstream>


#include "CmdWrapper.h"

std::atomic<bool> g_run(true);


class AppObj {
public:
  static void print_msg(const char* msg){
    printf("Example integraton : %s", msg);
  }
  void DoNothing(){
    print_msg("Doing nothing\n");
  }
};

//create your callback functions
void do_something(const std::vector<std::string>& args, void* obj) {
  AppObj* app_obj = (AppObj*)obj;
  app_obj->DoNothing();
  CmdLine& command_line = CmdLine::Instance();
  std::stringstream msg;
  if(args.size()) {
    msg << "cmd command with args : ";
    for(size_t i = 0; i < args.size(); ++i)
      msg << args.at(i).c_str() << " ";
    msg << "\n";
    command_line.Log(msg.str().c_str());
  }
  else
    command_line.Log("cmd command \n");
}

void do_exit(const std::vector<std::string>& args, void* obj) {
  g_run = false;
}


int main() {
  AppObj app_obj;
  CmdLine& command_line = CmdLine::Instance();
  if(command_line.IsValid()) { //check if library has loaded successfully
    command_line.SetLogFunc(AppObj::print_msg); //set custom log function (by default printf will be used)

    command_line.Add("cmd", do_something, &app_obj); //Register a command
    command_line.Add("q", do_exit); //Register another command

    command_line.RunLocal(); //Start listening to commands typed in local console
    command_line.RunServer(5566); //Start listening to commands on network port
  }

  while(g_run)
    sleep(1);

  return 0;
}
