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

#include "CmdClient.h"
#include "Connection.h"
#include "MessageType.h"
#include "Message.h"
#include "Logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>

const size_t CHECK_INTERVAL_IN_SEC = 3;

CmdClient::CmdClient() {
  _connection = std::make_shared<Connection>();
  _connection->Init();
}


void CmdClient::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  _checker->Wake();
  switch(MessageType::TypeFromInt(msg->_type)) {
    case MessageType::YOU_SHOULD_KNOW_THAT:
      printf("%s", msg->ToString().c_str());
      break;
    default:
      break;
  }
}

void CmdClient::OnConnected(std::shared_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_client_mutex);
  log()->info("CmdClient: Connected. You can type your commands now");
  _client = client;
  _client->Start(shared_from_this());
}

void CmdClient::SendPing() {
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client)
    _client->Send(std::make_shared<Message>((uint8_t)MessageType::ARE_U_ALIVE));
}

void CmdClient::OnClientClosed(std::shared_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_client_mutex);
  log()->info("CmdClient: Connection closed, awaiting new connection");
  _client.reset();
  _checker->Reset();
}

void CmdClient::OnDisconnected() {
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client) {
    log()->info("CmdClient: Connection closed, awaiting new connection");
    _client.reset();
  }
}

void CmdClient::Init(int port, const char* host) {
  _checker = std::make_shared<ConnectionChecker>(shared_from_this()
                                                 ,_connection
                                                ,CHECK_INTERVAL_IN_SEC
                                                ,host
                                                ,port);
  _checker->Init();

  while (true) {
    std::string input_line;
    if(std::getline(std::cin, input_line)) {
      std::lock_guard<std::mutex> lock(_client_mutex);
      if(_client)
        _client->Send(std::make_shared<Message>((uint8_t)MessageType::COMMAND, input_line));
      else
        log()->warn("CmdClient: Not connected, message not send");
    }
  }
}

int main(int argc, char *argv[]) {
  log()->set_pattern("%v");

  std::shared_ptr<CmdClient> client = std::make_shared<CmdClient>();

  if (argc < 2 || argc > 3) {
    log()->error("CmdClient: Usage {} [hostname] port", argv[0]);
    exit(0);
  }

  if(argc== 3)
    client->Init(atoi(argv[2]), argv[1]);
  else
    client->Init(atoi(argv[1]));

  return 0;
}
