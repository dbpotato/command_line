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

#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <pthread.h>
#include <string>
#include <unistd.h>
#include <map>
#include <vector>

#include "PosixThread.h"

class CmdServer;
typedef void(*log_func)(const char*);
typedef void(*callback_func)(const std::vector<std::string>&, void* obj);

/**
* Base class which defines api. Public methods are exposed in shared library.
* */
class CommandLine {
friend class CmdServer;

private:
  void* _funcObj; ///< pointer which will be passed to all callback functions.
  CmdServer* _cmd_server; ///< server instance
  log_func _log; ///< custom log function
  std::map<std::string, callback_func> _callbacks; ///< callback collection
  pthread_mutex_t _mutex; ///< mutex for accessing callback collection
  /**
   * Try to exectue callback. Prints warrning if none has been found under specified name.
   *
   * \param name the name under which callback should have been registered with AddCommand method;
   * \param args the argument list which will be passed to callback.
   * */
  void CallCommand(const std::string& name, const std::vector<std::string>& args);

protected:
  PosixThread _local_reader; ///< thread for reading input from local console

  /**
   * Checks if there is data to read from stdin
   *
   * \return returns true if read can be made
   */
  bool CanReadFormInput();
  /**
   * Used by thread for reading input from local console
   *
   * \param thread PosixThread object
   * \return returns NULL at exit
   */
  static void* Reader(void* thread);
  /**
   * Process single line with command name and arguments
   * \param line string which should contain name of a command and parameters
   */
  void ProcessLine(const std::string& line);
  /**
   * Logs internal messages
   * \param format printf compatibile format
   */
  void Log(const char* format, ...) const;

public:
  /**
   * Class constructor
   *
   * \param funcObj pointer which will be passed as argument to each callback function
   */
  CommandLine(void* funcObj);
  /**
   * Register new command
   * \param name string which will trigger command
   * \param callback function wich will be triggered by command
   */
  void AddCommand(const std::string& name, callback_func callback);
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
   * \return true if server started running, false otherwise
   */
  bool RunServer(int port);
  ~CommandLine();
};

#endif //COMMAND_LINE_H
