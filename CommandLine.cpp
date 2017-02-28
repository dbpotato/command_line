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

#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>
#include <iostream>
#include <sstream>

#include "CommandLine.h"
#include "CmdServer.h"


bool CommandLine::CanReadFormInput() {
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);

  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  return select(STDIN_FILENO +1, &rfds, NULL, NULL, &timeout) > 0;
}

void* CommandLine::Reader(void* thread) {
  PosixThread* posix_thread = (PosixThread*)thread;
  CommandLine* cmd = (CommandLine*) posix_thread->GetObj();
  std::string input_line;

  while(posix_thread->IsRunning())
    if(cmd->CanReadFormInput())
      if(std::getline(std::cin, input_line))
        cmd->ProcessLine(input_line);
}

void CommandLine::ProcessLine(const std::string& line) {
  std::string command;
  std::vector<std::string> args;
  std::stringstream stream;
  stream.str(line);
  std::string item;
  for(int el_no = 0; std::getline(stream, item, ' '); el_no++) {
    if(!el_no)
      command = item;
    else if(!item.empty())
      args.push_back(item);
  }

  CallCommand(command, args);
}

void CommandLine::Log(const char* format, ...) const {
  size_t size = 1024;
  char stackbuf[size];
  std::vector<char> dynamicbuf;
  char *buf = &stackbuf[0];
  va_list ap, ap_copy;
  std::string formated;

  va_start (ap, format);

  while (true) {
    va_copy(ap_copy, ap);
    int needed = vsnprintf (buf, size, format, ap);
    va_end(ap_copy);

    if (needed <= (int)size && needed >= 0) {
      formated = std::string (buf, (size_t) needed);
      break;
    }

    size = (needed > 0) ? (needed+1) : (size*2);
    dynamicbuf.resize (size);
    buf = &dynamicbuf[0];
  }

  va_end (ap);

  if(_log)
    _log(formated.c_str());
  else
    printf("%s",formated.c_str());
}

CommandLine::CommandLine(void* funcObj)
    : _funcObj(funcObj)
    , _cmd_server(NULL)
    , _log(NULL) {
  pthread_mutex_init(&_mutex, NULL);
}

CommandLine::~CommandLine() {
  Log("CmdLine: Is shutting down...\n");
  _local_reader.Stop();
  _local_reader.Join();

  if(_cmd_server)
    delete _cmd_server;
}

void CommandLine::RunLocal() {
  _local_reader.Run(&CommandLine::Reader, this);
  Log("CmdLine: Started locally\n");
}

bool CommandLine::RunServer(int port) {
  if(!_cmd_server) {
    _cmd_server = new CmdServer(this);
    if(_cmd_server->Start(port)) {
      Log("CmdLine: Server started at port %d\n", port);
      return true;
    }
    else {
      Log("CmdLine: Server failed to start at port %d\n", port);
      delete _cmd_server;
      _cmd_server = NULL;
      return false;
    }
  }
  else
    Log("CmdLine: Server already started\n", port);
  return false;
}

void CommandLine::CallCommand(const std::string& name, const std::vector<std::string>& args) {
  callback_func callback = NULL;

  std::map<std::string, callback_func>::iterator it;
  it = _callbacks.find(name);

  if(it != _callbacks.end())
    callback = it->second;

  if(callback)
    callback(args, _funcObj);
  else
    Log("CmdLine: Unknown Command : %s\n", name.c_str());
}

void CommandLine::AddCommand(const std::string& name, callback_func callback) {

  std::map<std::string, callback_func>::iterator it;
  it = _callbacks.find(name);

  if(it != _callbacks.end())
    it->second = callback;
  else
    _callbacks.insert(std::make_pair(name, callback));
}

void CommandLine::SetLogger(log_func log) {
  _log = log;
}
