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

#include "PosixThread.h"
#include "CmdServer.h"

#include <stdarg.h>
#include <map>
#include <vector>



typedef void(*log_func)(const char*);
typedef void(*callback_func)(const std::vector<std::string>&, void* obj);

/**
* Main class which defines whole api. Public methods are exposed in shared library.
* */
class CommandLine : public std::enable_shared_from_this<CommandLine>, public ThreadObject {

friend void CmdServer::PassCommand(std::shared_ptr<Message> msg);

private:
  std::shared_ptr<CmdServer> _cmd_server; ///< server instance
  std::shared_ptr<Connection> _connection; ///< connection interface used by CmdServer
  log_func _log; ///< custom log function
  std::map<std::string, std::pair<callback_func, void*> > _callbacks; ///< callback collection
  PosixThread _local_reader; ///< thread for reading input from local console

  /**
   * Try to exectue callback.
   * Prints warrning if none has been found under specified name.
   *
   * \param name the name under which callback should have been registered with AddCommand method;
   * \param args the argument list which will be passed to callback.
   * */
  void CallCommand(const std::string& name, const std::vector<std::string>& args);

  /**
   * Convert va_list to string
   * \param format printf compatibile format
   */
  std::string VsnprintfToStr(const char* format, va_list args) const;

  /**
   * Checks if there is data to read from stdin
   *
   * \return true if read can be made
   */
  bool CanReadFormInput();

  /**
   * Process single line with command name and arguments
   * \param line string which should contain name of a command and parameters sparated by space
   */
  void ProcessLine(const std::string& line);
  /**
   * Logs internal messages
   * \param format printf compatibile format
   */
  void Log(const char* format, ...) const;

public:
  CommandLine();
  ~CommandLine();
  /**
   * Register new command
   * \param name string which will trigger command
   * \param callback function wich will be triggered by command
   * \param obj pointer which will be passed to callback function
   */
  bool AddCommand(const std::string& name, callback_func callback, void* obj = nullptr);
  /**
   *  Sets custom log function. By default all messages are printed to std::cout
   * \param log custom logging function
   */
  void SetLogger(log_func  log);
  /**
   * Start reading commands from local console
   */
  void RunLocal();
  /**
   * Run server for obtaining commands from network client
   * \param port port number on which server will be listening for connection
   * \return true if server started successfully, false otherwise
   */
  bool RunServer(int port);
  /**
   * Logs external messages
   * \param msg
   */
  void LogExt(const char* msg) const;

  /**
   * Implements ThreadObject
   */
  void OnThreadStarted(int thread_id) override;
};

