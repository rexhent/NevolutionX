#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include "outputLine.h"
#include "ftpserver.h"

#ifdef NXDK
#include <lwip/opt.h>
#include <lwip/arch.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/debug.h>
#include <lwip/dhcp.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <pktdrv.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

const std::string username = "xbox";
const std::string passwd = "xbox";

const std::string replies[] = {
  "220 Please enter your login name now.\r\n",
  "331 Password required for " + username + ".\r\n",
  "230 User " + username + " logged in, proceed.\r\n",
  "257 \"%s\" is current directory\r\n",
  "502 %s not implemented.\r\n",
  "200 Type set to %s\r\n",
  "250 \"%s\" is current directory.\r\n",
  "215 UNIX type: L8\r\n",
  "200 Port command ok.\r\n",
  "150 Opening ASCII data connection for ls\r\n",
  "226 Data transfer finished successfully. Data connection closed.\r\n"
};

const std::string types[] = {
  "ASCII",
  "EBCDIC",
  "IMAGE",
  "LOCAL"
};

// get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sendStdString(int fd, std::string s, int flags = 0)
{
  send(fd, s.c_str(), s.length(), flags);
}

int ftpServer(void*)
{
  fd_set master;
  fd_set read_fds;
  int fdmax;

  std::map<int, std::string> PWDs;
  std::map<int, addrinfo*> TXs;
  std::map<int, int> TXFDs;

  int listener;
  int newfd;
  struct sockaddr_storage raddr; // Client address
  socklen_t addrlen;

  char buf[1024];
  int nbytes;

  int yes = 1;
  int i;

  struct addrinfo hints, *ai, *p;

  FD_ZERO(&master);    // clear the master and temp sets
  FD_ZERO(&read_fds);

  // get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
#ifdef NXDK
  if ((i = getaddrinfo(NULL, "21", &hints, &ai)) != 0) {
#else
  if ((i = getaddrinfo(NULL, "1089", &hints, &ai)) != 0) {
#endif
    outputLine("Error: selectserver\n");
    return 5;
  }

  /* Get network settings */
  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }
    break;
  }

  freeaddrinfo(ai);

  if (p == NULL) {
    outputLine("Error: selectServer: failed to bind\n");
    return 1;
  }

  /* Start listening */
  if (listen(listener, 10) == -1) {
    outputLine("Error: Listen\n");
    return 2;
  }
  FD_SET(listener, &master);
  fdmax = listener;

  /* `select` a file descriptor */
  for (;;) {
    read_fds = master;
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
      outputLine("Error: Select\n");
      return 3;
    }
    for(i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, &read_fds)) { // we got one!!
        if (i == listener) {
          // handle new connections
          addrlen = sizeof raddr;
          newfd = accept(listener,
                         (struct sockaddr *)&raddr,
                         &addrlen);

          if (newfd == -1) {
            outputLine("Error: accept\n");
          } else {
            FD_SET(newfd, &master); // add to master set
            if (newfd > fdmax) {    // keep track of the max
              fdmax = newfd;
            }
            send(newfd, replies[0].c_str(), replies[0].length(), 0);
            PWDs[newfd] = "/";
          }
        } else {
          // handle data from a client
          if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
              // connection closed
              outputLine("selectserver: socket %d hung up\n", i);
            } else {
              outputLine("Error: recv\n");
            }
            close(i); // bye!
            PWDs.erase(i);
            TXs.erase(i);
            TXFDs.erase(i);
            FD_CLR(i, &master); // remove from master set
          } else {
            // We received a command!
            /* Do what the command asks of you: */
            /**
             *   ABOR - abort a file transfer
             * / CWD  - change working directory (Lacks sanity check)
             *   DELE - delete a remote file
             * / LIST - list remote files
             *   MDTM - return the modification time of a file
             *   MKD  - make a remote directory
             *   NLST - name list of remote directory
             * X PASS - send password
             *   PASV - enter passive mode
             * X PORT - open a data port
             * X PWD  - print working directory
             *   QUIT - terminate the connection
             *   RETR - retrieve a remote file
             *   RMD  - remove a remote directory
             *   RNFR - rename from
             *   RNTO - rename to
             *   SITE - site-specific commands
             *   SIZE - return the size of a file
             *   STOR - store a file on the remote host
             * X SYST - Identify yourself
             * / TYPE - set transfer type (Only accepts IMAGE)
             * X USER - send username
             **/
            std::string recvdata(buf);
            recvdata = recvdata.substr(0, recvdata.find('\r'));
            size_t cmdDataSep = recvdata.find(' ', 0);
            std::string cmd = recvdata.substr(0, cmdDataSep);
            if (!cmd.compare("USER")) {
              if (!recvdata.substr(5, recvdata.find('\r')-5).compare(username)) {
                sendStdString(i, replies[1]);
              }
            } else if (!cmd.compare("PASS")) {
              if (!recvdata.substr(5,4).compare(passwd)) {
                sendStdString(i, replies[2]);
              }
            } else if (!cmd.compare("PWD")) {
              sprintf(buf, replies[3].c_str(), PWDs[i].c_str());
              send(i, buf, strlen(buf), 0);
              // sendStdString(i, 
            } else if (!cmd.compare("TYPE")) {
              if(recvdata[5] == 'I') {
                sprintf(buf, replies[5].c_str(), "IMAGE");
                send(i, buf, strlen(buf), 0);
              }
            } else if (!cmd.compare("CWD")) {
              if (recvdata[4] == '.' && recvdata[5] == '.') {
                PWDs[i] = PWDs[i].substr(0, PWDs[i].rfind('/', PWDs[i].rfind('/')-1)+1);
              } else if (recvdata[4] == '/') {
                PWDs[i] = recvdata.substr(cmdDataSep + 1) + "/";
              } else {
                PWDs[i] = PWDs[i] + recvdata.substr(4, recvdata.find('\r')-4) + "/";
              }
              sprintf(buf, replies[6].c_str(), PWDs[i].c_str());
              sendStdString(i, buf);
            } else if (!cmd.compare("SYST")) {
              sendStdString(i, replies[7]); 
            } else if (!cmd.compare("PORT")) {
              int valueSep = recvdata.find(',', cmdDataSep);
              ++cmdDataSep;
              std::string a1 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              cmdDataSep = valueSep+1;
              valueSep = recvdata.find(',', cmdDataSep);
              std::string a2 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              cmdDataSep = valueSep+1;
              valueSep = recvdata.find(',', cmdDataSep);
              std::string a3 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              cmdDataSep = valueSep+1;
              valueSep = recvdata.find(',', cmdDataSep);
              std::string a4 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              cmdDataSep = valueSep+1;
              valueSep = recvdata.find(',', cmdDataSep);
              std::string p1 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              cmdDataSep = valueSep+1;
              valueSep = recvdata.find(',', cmdDataSep);
              std::string p2 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              std::string address = a1 + "." + a2 + "." + a3 + "." + a4;
              std::string port = std::to_string(stoi(p1)*256 + stoi(p2));
              if (getaddrinfo(address.c_str(), port.c_str(), &hints, &TXs[i]) == 0)
              {
                if ((TXFDs[i] = socket(TXs[i]->ai_family, TXs[i]->ai_socktype, TXs[i]->ai_protocol))
                    != -1)
                {
                  if (connect(TXFDs[i], TXs[i]->ai_addr, TXs[i]->ai_addrlen) == 0)
                  {
                    sendStdString(i, replies[8]);
                  } else {
                    outputLine("Connecting socket %d failed!\n", TXFDs[i]);
                  }
                } else {
                  outputLine("Socket creation failed!\n");
                }
              } else {
                outputLine("Getting address info failed!\n");
              }
            } else if (!cmd.compare("LIST")) {
              if (TXFDs[i] != -1) {
                sendStdString(i, replies[9]);
                /*****************************************************
                 * List and send files (FIXME!)
                 * Due to a lack of winapi understanding, this remains
                 * unimplemented. No idea when I'll find time to learn
                 * this odd API.
                 *****************************************************/
                for (int q = 0; q < 1; ++q)
                {
                  sendStdString(TXFDs[i], "drwxr-xr-x 1 XBOX XBOX 0 May 11 10:41 Z\r\n");
                  sendStdString(TXFDs[i], "-rw-r--r-- 1 XBOX XBOX 77 Oct 16 22:06 JP\r\n");
                }
                close(TXFDs[i]);
                freeaddrinfo(TXs[i]);
                sendStdString(i, replies[10]);
              }
            } else if (!cmd.compare("EPRT")) {
              int family = std::stoi(recvdata.substr(6,1));
              int portDelimiter = recvdata.find('|',8);
              std::string address = recvdata.substr(8,recvdata.find('|', portDelimiter)-8);
              ++portDelimiter;
              std::string port = recvdata.substr(portDelimiter,
                                                 recvdata.find('|', portDelimiter) -
                                                 portDelimiter);
              std::cout << family << " " << address << " " << port << std::endl;
              if (getaddrinfo(address.c_str(), port.c_str(), &hints, &TXs[i]) == 0)
              {
                if ((TXFDs[i] = socket(TXs[i]->ai_family, TXs[i]->ai_socktype, TXs[i]->ai_protocol))
                    != -1)
                {
                  if (connect(TXFDs[i], TXs[i]->ai_addr, TXs[i]->ai_addrlen) == 0)
                  {
                    sendStdString(i, replies[8]);
                  } else {
                    outputLine("Connecting socket %d failed!\n", TXFDs[i]);
                  }
                } else {
                  outputLine("Socket creation failed!\n");
                }
              } else {
                outputLine("Getting address info failed!\n");
              }
            } else if (!cmd.compare("PWD\r")) {
            } else {
              sprintf(buf, replies[4].c_str(), cmd.c_str());
              send(i, buf, strlen(buf), 0);
            }
          }
        }
      }
    }
  }
}
