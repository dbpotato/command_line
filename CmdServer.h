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
#include "Client.h"
#include "Server.h"

class CommandLine;
class Conncection;

/**
 * Network server for receving commands from external clients
 */
class CmdServer : public std::enable_shared_from_this<CmdServer>
                , public ClientManager
                , public ServerManager {
public:
  /**
   * Constructor
   * \param cmd_line owner instance
   */
  CmdServer(std::weak_ptr<CommandLine> cmd_line);

  /**
   * Tries to start lisntening for connections
   * \param port listening port
   * \return true on success
   */
  bool Start(int port);

  /**
   * Pass string to connected clietns
   * \param str string to send
   */
  void PassLog(const std::string& str);

  /**
   * Implements ServerManager
   */
  void OnClientConnected(std::shared_ptr<Client> client) override;

  /**
   * Implements ClientManager
   */
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;

  /**
   * Implements ClientManager
   */
  void OnClientClosed(std::shared_ptr<Client> client) override;
  /**
   * Implements ClientManager
   */
  void OnMsgSent(std::shared_ptr<Client> client, std::shared_ptr<Message> msg, bool success) override {}

  /**
   * Passes recevied command to CommandLine
   * \param msg received network message
   */
  void PassCommand(std::shared_ptr<Message> msg);

private:
  std::weak_ptr<CommandLine> _cmd_line; ///< owner instance
  size_t _client_id_counter; ///< client id counter
  std::shared_ptr<Server> _server; ///< network server instance
  std::map<size_t, std::shared_ptr<Client> > _clients; ///< connected network clients
  std::shared_ptr<Connection> _connection; ///< network connection instance
};
