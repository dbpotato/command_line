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

#ifndef CMD_CLIENT_H
#define CMD_CLIENT_H

/**
 * Basic network client for sending messages to command line's server
 */

class CmdClient {
private:
  int _socket; ///< server connection socket
public:
  CmdClient();
  ~CmdClient();
  /**
   * Try to exectue callback. Prints warrning if none has been found under specified name
   *
   * \param port server's listening port
   * \param host server's location
   * */
  bool Connect(int port, const char* host = "localhost");
  /**
   * Endles loop for receving commands from standard input and sending them to server
   */
  void Run();
};

#endif //CMD_CLIENT_H
