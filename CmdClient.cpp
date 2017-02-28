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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "CmdClient.h"

CmdClient::CmdClient()
  : _socket(0) {
}

CmdClient::~CmdClient() {
  if(_socket)
    close(_socket);
}

bool CmdClient::Connect(int port, const char* host) {
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket < 0)
    return false;

  sockaddr_in serv_addr;
  hostent* server ;

  server = gethostbyname(host);
  if (!server)
    return false;

  memset(&serv_addr, 0, sizeof(server));
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr,
    server->h_addr,
    server->h_length);
  serv_addr.sin_port = htons(port);

  if (connect(_socket, (sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    return false;

  return true;
}

void CmdClient::Run() {
  int len = 0;
  std::string input_line;
  printf("Enter messages: \n");

  while (true) {
    if(std::getline(std::cin, input_line)) {
      len = write(_socket, input_line.c_str(), input_line.length());
      if (len <= 0)
        break;
    }
  }
  printf("Server disconnected: \n");
}


int main(int argc, char *argv[]) {
  CmdClient client;
  int port = 0;
  bool connected = false;

  if (argc < 2 || argc > 3) {
    fprintf(stderr,"usage %s [hostname] port\n", argv[0]);
    exit(0);
  }

  if(argc== 3)
    connected = client.Connect(atoi(argv[2]), argv[1]);
  else
    connected = client.Connect(atoi(argv[1]));

  if(connected)
    client.Run();
  else
    fprintf(stderr,"Failed to connect\n");

  return 0;
}
