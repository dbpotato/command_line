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

#include "Message.h"
#include "ServerImpl.h"

class CommandLine;
class Conncection;

/**
 * Network server for receving commands from external clients
 */
class CmdServer : public ServerImpl {
public:
  /**
   * Constructor
   * \param cmd_line owner instance
   */
  CmdServer(std::weak_ptr<CommandLine> cmd_line);

  /**
   * Pass string to connected clietns
   * \param str string to send
   */
  void PassLog(const std::string& str);

  /**
   * Overrides ServerImpl
   */
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;

  /**
   * Passes recevied command to CommandLine
   * \param msg received network message
   */
  void PassCommand(std::shared_ptr<Message> msg);

private:
  std::weak_ptr<CommandLine> _cmd_line; ///< owner instance
  std::shared_ptr<Connection> _connection; ///< network connection instance
};
