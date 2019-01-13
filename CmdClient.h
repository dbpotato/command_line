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

#include "Client.h"
#include "ConnectionChecker.h"

class Conncection;

/**
 * Basic network client for sending messages to CmdServer
 * and receving response.
 */
class CmdClient : public std::enable_shared_from_this<CmdClient>
                , public ClientManager
                , public ConnectionKeeper{
public:
  CmdClient();

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
  void OnMsgSent(std::shared_ptr<Client> client, std::shared_ptr<Message> msg, bool success) override {;}

  /**
   * Implementes ConnectionKeeper
   */
  void OnConnected(std::shared_ptr<Client> client) override;

  /**
   * Implementes ConnectionKeeper
   */
  void OnDisconnected() override;

  /**
   * Implementes ConnectionKeeper
   */
  void SendPing() override;

  /**
   * Tries to connect to CmdServer.
   * Starts endless async loop for reading input from console
   * \param port server's listening port
   * \param host server's url
   */
  void Init(int port, const char* host = "localhost");

private:
  std::shared_ptr<Client> _client; ///< network client instance
  std::shared_ptr<Connection> _connection; ///< network connection instance
  std::shared_ptr<ConnectionChecker> _checker; ///< checker instance
  std::mutex _client_mutex; ///< mutex for client modifications
};
