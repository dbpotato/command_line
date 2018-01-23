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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>


#include "CmdClient.h"
#include "Connection.h"
#include "MessageType.h"
#include "Message.h"

CmdClient::CmdClient() {
  _connection = std::make_shared<Connection>();
  _connection->Init();
}


void CmdClient::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  switch(MessageType::TypeFromInt(msg->_type)) {
    case MessageType::YOU_SHOULD_KNOW_THAT:
      printf("%s", msg->ToString().c_str());
      break;
    default:
      break;
  }
}

void CmdClient::OnClientClosed(std::shared_ptr<Client> client) {
}

void CmdClient::OnMsgSent(std::shared_ptr<Client> client, std::shared_ptr<Message> msg, bool success) {
}

bool CmdClient::Connect(int port, const char* host) {
  _client = _connection->CreateClient(port, host);
  if(!_client)
    return false;

  _client->Start(0, shared_from_this());
  return true;
}

void CmdClient::Run() {
  int len = 0;
  std::string input_line;
  printf("Enter messages: \n");

  while (true) {
    if(std::getline(std::cin, input_line)) {
      _client->Send(std::make_shared<Message>((uint8_t)MessageType::COMMAND, input_line));
    }
  }
  printf("Server disconnected: \n");
}

int main(int argc, char *argv[]) {
  std::shared_ptr<CmdClient> client = std::make_shared<CmdClient>();
  bool connected = false;

  if (argc < 2 || argc > 3) {
    fprintf(stderr,"usage %s [hostname] port\n", argv[0]);
    exit(0);
  }

  if(argc== 3)
    connected = client->Connect(atoi(argv[2]), argv[1]);
  else
    connected = client->Connect(atoi(argv[1]));

  if(connected)
    client->Run();
  else
    fprintf(stderr,"Failed to connect\n");

  return 0;
}
