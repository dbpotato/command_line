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

#ifndef COMMAND_SERVER_H
#define COMMAND_SERVER_H

#include "PosixThread.h"

class CommandLine;
struct timeval;

/**
 * Simple server that accept single client connection
 */
class CmdServer {
public:
  /**
   * class constructor
   * \param cmd_line owner
   */
  CmdServer(CommandLine* cmd_line);
  ~CmdServer();
  /**
   * Tries to start listening on specified port
   * \param port port number
   * \return true if server succesfully started listening for connection
   */
  bool Start(int port);
protected:
  CommandLine* _cmd_line; ///<parent object
  /**
   * Checks if there is data to read from socket
   *
   * \param socket handler to check
   * \param timeout max waiting time
   * \return returns true if read can be made
   */
  bool IsSocketReady(int socket, timeval timeout);
private:
  PosixThread _run_thread;  ///< thread for listening for connection
  /**
   * Awaits for connection and recevies commands from client
   * \param  thread PosixThread object
   * \return returns NULL at exit
   */
  static void* Run(void* thread);
  int _sock_listen; ///<socket handle for client connection
  int _sock_read; ///<socket handle for receiving data
  bool _started; ///<prevents from straring server for a second time
};

#endif //COMMAND_SERVER_H
