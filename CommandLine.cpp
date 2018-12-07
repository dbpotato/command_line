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

#include "CommandLine.h"
#include "CmdServer.h"

#include <cstdarg>
#include <iostream>
#include <sstream>
#include <unistd.h>

bool CommandLine::CanReadFormInput() {
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);

  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  return select(STDIN_FILENO +1, &rfds, nullptr, nullptr, &timeout) > 0;
}

void CommandLine::OnThreadStarted(int thread_id) {
  std::string input_line;
  while(_local_reader.ShouldRun())
    if(CanReadFormInput())
      if(std::getline(std::cin, input_line))
        ProcessLine(input_line);
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

std::string CommandLine::VsnprintfToStr(const char* format, va_list args) const {
  va_list args2;
  va_copy(args2, args);
  std::vector<char> buf(1+std::vsnprintf(nullptr, 0, format, args));
  std::vsnprintf(buf.data(), buf.size(), format, args2);
  va_end(args2);
  return std::string(buf.begin(),buf.end());
}

void CommandLine::Log(const char* format, ...) const {
  va_list args;
  va_start (args, format);
  LogExt(VsnprintfToStr(format, args).c_str());
  va_end (args);
}

void CommandLine::LogExt(const char* msg) const{
  if(_log)
    _log(msg);
  else
    printf("%s", msg);

  if(_cmd_server)
    _cmd_server->PassLog(msg);
}

CommandLine::CommandLine() : _log(nullptr) {
}

CommandLine::~CommandLine() {
  Log("CmdLine: Is shutting down...\n");
  _local_reader.Stop();
  _local_reader.Join();
}

void CommandLine::RunLocal() {
  _local_reader.Run(shared_from_this());
  Log("CmdLine: Started locally\n");
}

bool CommandLine::RunServer(int port) {
  if(!_cmd_server) {
    _connection = std::make_shared<Connection>();
    _connection->Init();

    _cmd_server = std::make_shared<CmdServer>(shared_from_this());
    if(_cmd_server->Init(port, _connection)) {
      Log("CmdLine: Server started at port %d\n", port);
      return true;
    }
    else {
      Log("CmdLine: Server failed to start at port %d\n", port);
      _cmd_server = nullptr;
      return false;
    }
  }
  else
    Log("CmdLine: Server already started\n", port);
  return false;
}

void CommandLine::CallCommand(const std::string& name, const std::vector<std::string>& args) {
  callback_func callback = nullptr;
  void* obj = nullptr;

  auto it = _callbacks.find(name);

  if(it ==_callbacks.end()) {
    Log("CmdLine: Unknown Command : %s\n", name.c_str());
    return;
  }

  callback = it->second.first;
  obj = it->second.second;

  if(callback)
    callback(args, obj);
}

bool CommandLine::AddCommand(const std::string& name, callback_func callback, void* obj) {

  auto it = _callbacks.find(name);

  if(it != _callbacks.end()) {
    Log("CmdLine: Command already added : %s\n", name.c_str());
    return false;
  }

  _callbacks.insert(std::make_pair(name, std::make_pair(callback, obj)));
  return true;
}

void CommandLine::SetLogger(log_func log) {
  _log = log;
}
