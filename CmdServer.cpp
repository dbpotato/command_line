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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include "CmdServer.h"
#include "CommandLine.h"
#include "MessageType.h"

CmdServer::CmdServer(std::weak_ptr<CommandLine> cmd_line)
    : _cmd_line(cmd_line) {
}

void CmdServer::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  switch(MessageType::TypeFromInt(msg->_type)) {
    case MessageType::COMMAND:
      PassCommand(msg);
      break;
    default:
      break;
  }
}

void CmdServer::PassLog(const std::string& str) {
  if(str.size()) {
    auto msg = std::make_shared<Message>((uint8_t)MessageType::YOU_SHOULD_KNOW_THAT, str);
    std::vector<std::shared_ptr<Client> > vec;
    GetClients(vec);
    for(auto client : vec)
      client->Send(msg);
   }
}

void CmdServer::PassCommand(std::shared_ptr<Message> msg) {
  if(auto cmdl = _cmd_line.lock())
    cmdl->ProcessLine(msg->ToString());
}

