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

#include <vector>
#include <string>
#include "CommandLine.h" 

CommandLine* g_cmdLine = NULL;

extern "C" void cmdl_create(void* funcObj) {
  if(g_cmdLine)
    delete g_cmdLine;

  g_cmdLine = new CommandLine(funcObj);
}

extern "C" void cmdl_destroy() {
  if(g_cmdLine) {
    delete g_cmdLine;
    g_cmdLine = NULL;
  }
}

extern "C" void cmdl_add(std::string& name, callback_func callback) {
  if(g_cmdLine)
    g_cmdLine->AddCommand(name, callback);
}

extern "C" void cmdl_set_log_func(void(*log_func)(const char*)) {
  if(g_cmdLine)
    g_cmdLine->SetLogger(log_func);
}

extern "C" void cmdl_run_local() {
  if(g_cmdLine)
    g_cmdLine->RunLocal();
}

extern "C" bool cmdl_run_server(int port) {
  if(g_cmdLine)
    return g_cmdLine->RunServer(port);
  return false;
}
