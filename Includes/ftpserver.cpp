#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>
#include "outputLine.h"
#include "ftpserver.h"

#ifdef NXDK
#include <lwip/opt.h>
#include <lwip/arch.h>
#include <lwip/netdb.h>
#include <lwip/errno.h>
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
#include <windows.h> // Used for file I/O
#include <nxdk/mount.h>
#include <xboxkrnl/xboxkrnl.h>
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

bool nxIsDriveMounted(char) { return true; }
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

#ifdef NXDK
const char* gai_strerror(int errc) {
  switch (errc) {
  case 200:
    return "EAI_NONAME";
  case 201:
    return "EAI_SERVICE";
  case 202:
    return "EAI_FAIL";
  case 203:
    return "EAI_MEMORY";
  case 204:
    return "EAI_FAMILY";
  case 210:
    return "HOST_NOT_FOUND";
  case 211:
    return "NO_DATA";
  case 212:
    return "NO_RECOVERY";
  case 213:
    return "TRY_AGAIN";
  default:
    return "ERR_OK";
  }
}
#endif

std::string sock_strerror(int errc) {
  switch (errc) {
  case EACCES:
    return "EACCES";
  case EAFNOSUPPORT:
    return "EAFNOSUPPORT";
  case EINVAL:
    return "EINVAL";
  case ENFILE:
    return "ENFILE";
  case EMFILE:
    return "EMFILE";
  case ENOBUFS:
    return "ENOBUFS";
  case ENOMEM:
    return "ENOMEM";
  case EPROTONOSUPPORT:
    return "EPROTONOSUPPORT";
  default:
    return "ERR_OK";
  }
}

std::string unixToDosPath(std::string const& path) {
  std::string ret;
  if (path[0] == '/' && path[1] == '/') {
    ret = path.substr(1, std::string::npos);
  } else {
    ret = path;
  }
  ret = ret.substr(1,1) + ":" + path.substr(2, std::string::npos);
  std::replace(ret.begin(), ret.end(), '/', '\\');
  return ret;
}

const std::string drives = "CDEFGXYZ";

void sendFolderContents(int fd, std::string &path) {
  if (path[0] == '/' && path[1] == '/') {
    path = path.substr(1, std::string::npos);
  }
  if (!path.compare("/")) {
    for (size_t i = 0; i < drives.length(); ++i) {
      if (nxIsDriveMounted(drives[i])) {
        sendStdString(fd, "drwxr-xr-x 1 XBOX XBOX 0 May 11 10:41 " +
                      drives.substr(i,1) + "\r\n");
      }
    }
    return;
  }
#ifdef NXDK
  WIN32_FIND_DATAA fData;
  std::string searchmask = unixToDosPath(path + "*");
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    return;
  }
  do {
    std::string retstr = "";
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      retstr = "d";
    } else {
      retstr = "-";
    }
    retstr += "rwxr-xr-x 1 XBOX XBOX " + std::to_string(fData.nFileSizeLow) +
      " May 11 14:40 " + fData.cFileName + "\r\n";
    sendStdString(fd, retstr);
  } while (FindNextFile(fHandle, &fData) != 0);
  FindClose(fHandle);
#else
  for (int q = 0; q < 10; ++q) {
    std::string retstr = "drwxr-xr-x 1 XBOX XBOX " + std::to_string(q) +
      " May 11 10:41 " + std::to_string(q) + "\r\n";
    sendStdString(fd, retstr);
  }
  std::string retstr = "-rwxr-xr-x 1 XBOX XBOX 1024 May 11 10:41 X\r\n";
  sendStdString(fd, retstr);
#endif
}

bool sendFile(int fd, std::string const& pwd, std::string const& fileName) {
#ifdef NXDK
  std::string filePath = unixToDosPath(pwd + fileName);
  outputLine(fileName.c_str());
  HANDLE fHandle = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  outputLine(("\r\n" + filePath + "\r\n").c_str());
  if (fHandle == INVALID_HANDLE_VALUE) {
    outputLine("File opening failed. LOL. \n");
    return false;
  }
  int bytesToRead = 1048573;
  unsigned long bytesRead = 0;
  char* buf = static_cast<char*>(malloc(1048576));

  while (ReadFile(fHandle, buf, bytesToRead, &bytesRead, NULL) && (bytesRead > 0)) {
    send(fd, buf, bytesRead, 0);
  }
  CloseHandle(fHandle);
  free(buf);
  return true;
#else
  char buf[1024]; // Buffer of garbage
  int bytesRead = 1024;
  send(fd, buf, bytesRead, 0);
  return true;
#endif
}

bool recvFile(int fd, std::string const& pwd, std::string const& fileName) {
#ifdef NXDK
  std::string filePath = unixToDosPath(pwd + fileName);
  outputLine(fileName.c_str());
  HANDLE fHandle = CreateFile(filePath.c_str(), GENERIC_WRITE,
                              0, NULL, CREATE_NEW,
                              FILE_ATTRIBUTE_NORMAL, NULL);
  outputLine(("\r\n" + filePath + "\r\n").c_str());
  if (fHandle == INVALID_HANDLE_VALUE) {
    outputLine("File creation failed. LOL. \n");
    return false;
  }
  int bytesToWrite = (64 * 1024) - 3;
  unsigned long bytesWritten;
  unsigned long bytesRead;
  char* buf = static_cast<char*>(malloc(64 * 1024));

  while ((bytesRead = recv(fd, buf, bytesToWrite, 0))) {
    WriteFile(fHandle, &buf, bytesRead, &bytesWritten, NULL);
  }
  CloseHandle(fHandle);
  free(buf);
  return true;
#else
  char buf[1024]; // Buffer of garbage
  int bytesRead = 1024;
  while(recv(fd, buf, &bytesRead, 0)) {
  }
  return true;
#endif
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
  int rv;

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
            outputLine("Error: accept: %s\n", sock_strerror(errno).c_str());
          } else {
            FD_SET(newfd, &master); // add to master set
            if (newfd > fdmax) {    // keep track of the max
              fdmax = newfd;
            }
            sendStdString(newfd, replies[0]);
            PWDs[newfd] = "/";
          }
        } else {
          // handle data from a client
          if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
              // connection closed
            } else {
              outputLine("Error: recv\n");
            }
            outputLine("Closing %d!\n", i);
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
             * X LIST - list remote files
             *   MDTM - return the modification time of a file
             *   MKD  - make a remote directory
             *   NLST - name list of remote directory
             * X PASS - send password
             *   PASV - enter passive mode
             * X PORT - open a data port
             * X EPRT - Extended data port (IPv6)
             * X PWD  - print working directory
             *   QUIT - terminate the connection
             * X RETR - retrieve a remote file
             *   RMD  - remove a remote directory
             *   RNFR - rename from
             *   RNTO - rename to
             *   SITE - site-specific commands
             *   SIZE - return the size of a file
             * X STOR - store a file on the remote host
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
              if (recvdata[5] == 'I') {
                sprintf(buf, replies[5].c_str(), "IMAGE");
                send(i, buf, strlen(buf), 0);
              } else if (recvdata[5] == 'A') {
                sprintf(buf, replies[5].c_str(), "ASCII");
                send(i, buf, strlen(buf), 0);
              } else {
                sendStdString(i, "504 Command parameter not implemented.\r\n");
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
              valueSep = recvdata.find(',', valueSep+1);
              valueSep = recvdata.find(',', valueSep+1);
              valueSep = recvdata.find(',', valueSep+1);
              std::string address = recvdata.substr(cmdDataSep+1, valueSep - (cmdDataSep+1));
              std::replace(address.begin(), address.end(), ',', '.');

              cmdDataSep = valueSep+1;
              valueSep = recvdata.find(',', cmdDataSep);
              std::string p1 = recvdata.substr(cmdDataSep, valueSep-cmdDataSep);
              cmdDataSep = valueSep+1;
              std::string p2 = recvdata.substr(cmdDataSep, std::string::npos);

              std::string port = std::to_string(stoi(p1)*256 + stoi(p2));
              outputLine((address + " " + port + " " + std::to_string(i) + "\n").c_str());
              if ((rv = getaddrinfo(address.c_str(), port.c_str(), &hints, &TXs[i])) == 0)
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
                  outputLine("Socket creation failed! %s\n", sock_strerror(errno).c_str());
                  sendStdString(i, "425 Can't open data connection\r\n");
                  freeaddrinfo(TXs[i]);
                }
              } else {
                outputLine("Getting address info failed: %s\n", gai_strerror(rv));
                sendStdString(i, "425 Getting address info failed.\r\n");
              }
            } else if (!cmd.compare("LIST")) {
              if (TXFDs[i] != -1) {
                sendStdString(i, replies[9]);
                /*****************************************************
                 * List and send files
                 *****************************************************/
                sendFolderContents(TXFDs[i], PWDs[i]);
                close(TXFDs[i]);
                freeaddrinfo(TXs[i]);
                sendStdString(i, replies[10]);
              }
            } else if (!cmd.compare("EPRT")) {
              int family = std::stoi(recvdata.substr(6,1));
              if (family != 1 && family != 2) {
                sendStdString(i, "502 Unknown address family; use (1,2)\r\n");
                continue;
              }
              char delimiter = recvdata[5];
              int portDelimiter = recvdata.find(delimiter,8);
              std::string address = recvdata.substr(8,recvdata.find(delimiter, portDelimiter)-8);
              ++portDelimiter;
              std::string port = recvdata.substr(portDelimiter,
                                                 recvdata.find(delimiter, portDelimiter) -
                                                 portDelimiter);
              if ((rv = getaddrinfo(address.c_str(), port.c_str(), &hints, &TXs[i])) == 0)
              {
                if ((TXFDs[i] = socket(TXs[i]->ai_family, TXs[i]->ai_socktype, TXs[i]->ai_protocol))
                    != -1)
                {
                  if (connect(TXFDs[i], TXs[i]->ai_addr, TXs[i]->ai_addrlen) == 0)
                  {
                    sendStdString(i, replies[8]);
                  } else {
                    outputLine("Connecting socket %d failed; %s\n", TXFDs[i], sock_strerror(errno).c_str());
                  }
                } else {
                  outputLine("Socket creation failed; %s\n", sock_strerror(errno).c_str());
                  sendStdString(i, "425 Can't open data connection\r\n");
                  freeaddrinfo(TXs[i]);
                }
              } else {
                outputLine("Getting address info failed; %s\n", gai_strerror(rv));
                sendStdString(i, "425 Getting address info failed.\r\n");
              }
            } else if (!cmd.compare("RETR")) {
              if (TXFDs[i] != -1) {
                std::string fileName = recvdata.substr(cmdDataSep + 1, std::string::npos);
                outputLine("Trying to send file %s!\n", fileName.c_str());
                sendStdString(i, "150 Sending file " + fileName + "\r\n");
                sendFile(TXFDs[i], PWDs[i], fileName);
                close(TXFDs[i]);
                freeaddrinfo(TXs[i]);
                sendStdString(i, replies[10]);
              }
            } else if (!cmd.compare("STOR")) {
              if (TXFDs[i] != -1) {
                std::string fileName = recvdata.substr(cmdDataSep +1, std::string::npos);
                outputLine("Trying to receive file %s!\n", fileName.c_str());
                sendStdString(i, "150 Receiving file " + fileName + "\r\n");
                recvFile(TXFDs[i], PWDs[i], fileName);
                close(TXFDs[i]);
                freeaddrinfo(TXs[i]);
                sendStdString(i, replies[10]);
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
