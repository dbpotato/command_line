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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "CmdServer.h"
#include "CommandLine.h"

CmdServer::CmdServer(CommandLine* cmd_line)
    : _cmd_line(cmd_line)
    , _sock_listen(0)
    , _sock_read(0)
    , _started(false) {
}

CmdServer::~CmdServer() {
  _run_thread.Stop();
  _run_thread.Join();

  if(_sock_read)
    close(_sock_read);
  if(_sock_listen)
    close(_sock_listen);
}

bool CmdServer::Start(int port) {
  if(_started) {
    _cmd_line->Log("CmdLine: Server already started\n");
    return false;
  }

  sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  _sock_listen = socket(AF_INET, SOCK_STREAM, 0);
  if (_sock_listen < 0)
    return false;

  if (bind(_sock_listen, (sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    return false;

  _started = true;
  _run_thread.Run(&CmdServer::Run, this);

  return true;
}

bool CmdServer::IsSocketReady(int socket, timeval timeout) {
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(socket, &rfds);

  int result = select(socket +1, &rfds, NULL, NULL, &timeout);
  return result > 0;
}

void* CmdServer::Run(void* thread) {
  PosixThread* posix_thread = (PosixThread*)thread;
  CmdServer* server = (CmdServer*) posix_thread->GetObj();
  CommandLine* cmdl = server->_cmd_line;

  struct timeval accept_timeout, read_timeout;
  accept_timeout.tv_sec = 1;
  accept_timeout.tv_usec = 0;
  read_timeout.tv_sec = 1;
  read_timeout.tv_usec = 0;

  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  while(posix_thread->IsRunning()) {
    listen(server->_sock_listen, 0);

    while(!server->IsSocketReady(server->_sock_listen, accept_timeout))
      if(!posix_thread->IsRunning())
        return NULL;

    server->_sock_read = accept(server->_sock_listen,
                          (sockaddr*) &client_addr,
                          &client_addr_len);
    if (server->_sock_read >= 0 && posix_thread->IsRunning()) {
      cmdl->Log("CmdLine: Network client connected\n");
      int read_len = 0;
      char buffer[256];

      while(posix_thread->IsRunning()) {
        memset(buffer, 0, 256);

        while(!server->IsSocketReady(server->_sock_read, read_timeout))
          if(!posix_thread->IsRunning())
            return NULL;
        read_len = read(server->_sock_read, buffer, 255);
        if (read_len <= 0)
          break;
        cmdl->ProcessLine(std::string(buffer));
      }
      cmdl->Log("CmdLine: Network client disconnected\n");
    }
  }
}
