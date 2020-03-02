#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>
#include "outputLine.h"
#include "ftpServer.h"

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

const std::string drives = "CDEFGXYZ";

const std::string types[] = {
  "ASCII",
  "EBCDIC",
  "IMAGE",
  "LOCAL"
};

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
  "226 Data transfer finished successfully. Data connection closed.\r\n",
  "504 Command parameter not implemented.\r\n"
};

void ftpConnection::sendStdString(std::string const& s, int flags = 0) {
  sendStdString(_fd, s, flags);
}

void ftpConnection::sendStdString(int fd, std::string const& s, int flags = 0) {
  send(fd, s.c_str(), s.length(), flags);
}

ftpConnection::ftpConnection(int fd, ftpServer* s) :
  _fd(fd), server(s) {
  pwd = "/";
  logged_in = false;
  sendStdString(replies[0]);
};

void ftpConnection::doYourThing(void) {
  // handle data from a client
  int nbytes;
  if ((nbytes = recv(_fd, buf, sizeof buf, 0)) <= 0) {
    // got error or connection closed by client
    if (nbytes == 0) {
      // connection closed
    } else {
      outputLine("Error: recv\n");
    }
    outputLine("Closing %d!\n", _fd);
    close(_fd); // bye!
    server->forgetMe(_fd);
  } else {
    // We received a command!
    /* Do what the command asks of you: */
    /**
     *   ABOR - abort a file transfer
     * / CWD  - change working directory (Lacks sanity check)
     * X CDUP - Move up to parent directory
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
    size_t cmdDataSep = recvdata.find(' ', 0);
    std::string cmd = recvdata.substr(0, cmdDataSep);
    std::string arg = recvdata.substr(cmdDataSep+1, recvdata.find('\r') - (cmdDataSep + 1));

    if (!cmd.compare("ABOR")) {
      // cmdAbor();
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("CWD")) {
      cmdCwd(arg);
    } else if (!cmd.compare("CDUP")) {
      cmdCdup();
    } else if (!cmd.compare("DELE")) {
      // cmdDele(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("LIST")) {
      cmdList(arg);
    } else if (!cmd.compare("MDTM")) {
      // cmdMdtm(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("MKD")) {
      // cmdMkd(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("NLST")) {
      // cmdNlst(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("PASS")) {
      cmdPass(arg);
    } else if (!cmd.compare("PASV")) {
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("PORT")) {
      cmdPort(arg);
    } else if (!cmd.compare("EPRT")) {
      cmdEprt(arg);
    } else if (!cmd.compare("PWD")) {
      cmdPwd();
    } else if (!cmd.compare("QUIT")) {
      // cmdQuit();
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("RETR")) {
      cmdRetr(arg);
    } else if (!cmd.compare("RMD")) {
      // cmdRmd(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("RNFR")) {
      // cmdRnfr(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("RNTO")) {
      // cmdRnto(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("SITE")) {
      // cmdSite(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("SIZE")) {
      // cmdSize(arg);
      cmdUnimplemented(cmd);
    } else if (!cmd.compare("STOR")) {
      cmdStor(arg);
    } else if (!cmd.compare("SYST")) {
      cmdSyst();
    } else if (!cmd.compare("TYPE")) {
      cmdType(arg);
    } else if (!cmd.compare("USER")) {
      cmdUser(arg);
    } else {
      cmdUnimplemented(cmd);
    }
  }
}

void ftpConnection::cmdUser(std::string const& arg) {
  if(!arg.compare(username)) {
    sendStdString(replies[1]);
  }
}

void ftpConnection::cmdPass(std::string const& arg) {
  if (!arg.compare(passwd)) {
    sendStdString(replies[2]);
  }
}

void ftpConnection::cmdPwd(void) {
  sprintf(buf, replies[3].c_str(), pwd.c_str());
  sendStdString(buf);
}

void ftpConnection::cmdType(std::string const& arg) {
  if (arg[0] == 'I') {
    sprintf(buf, replies[5].c_str(), "IMAGE");
    sendStdString(buf);
  } else {
    sendStdString(replies[11]);
  }
}

void ftpConnection::cmdCwd(std::string const& arg) {
  if (arg[0] == '.' && arg[1] == '.') {
    pwd= pwd.substr(0, pwd.rfind('/', pwd.rfind('/')-1)+1);
  } else if (arg[0] == '/') {
    pwd = arg + "/";
  } else {
    pwd = pwd + arg + "/";
  }
  sprintf(buf, replies[6].c_str(), pwd.c_str());
  sendStdString(buf);
}

void ftpConnection::cmdCdup(void) {
  cmdCwd("..");
}

void ftpConnection::cmdSyst(void) {
  sendStdString(replies[7]);
}

void ftpConnection::cmdPort(std::string const& arg) {
  int valueSep = arg.find(',', 0);
  valueSep = arg.find(',', valueSep+1);
  valueSep = arg.find(',', valueSep+1);
  valueSep = arg.find(',', valueSep+1);
  std::string address = arg.substr(0, valueSep);
  std::replace(address.begin(), address.end(), ',', '.');

  int cmdDataSep = valueSep+1;
  valueSep = arg.find(',', cmdDataSep);
  std::string p1 = arg.substr(cmdDataSep, valueSep-cmdDataSep);
  cmdDataSep = valueSep+1;
  std::string p2 = arg.substr(cmdDataSep, std::string::npos);

  std::string port = std::to_string(stoi(p1)*256 + stoi(p2));
  outputLine((address + " " + port + " " + std::to_string(_fd) + "\n").c_str());
  dataFd = server->openConnection(address, port);
  if (dataFd != -1) {
    sendStdString(replies[8]);
  } else {
    sendStdString("425 Socket creation failed.");
  }
}

void ftpConnection::cmdList(std::string const& arg) {
  if (dataFd != -1) {
    sendStdString(replies[9]);
    sendFolderContents(dataFd, pwd);
    close(dataFd);
    sendStdString(replies[10]);
  }
}

void ftpConnection::cmdNlst(std::string const& arg) {
  sendStdString(replies[9]);
  sendFolderContents(_fd, pwd);
  sendStdString(replies[10]);
}

void ftpConnection::cmdEprt(std::string const& arg) {
  int family = std::stoi(arg.substr(1,1));
  if (family != 1 && family != 2) {
    sendStdString("502 Unknown address family; use (1,2)\r\n");
    return;
  }
  char delimiter = arg[0];
  int portDelimiter = arg.find(delimiter, 3);
  std::string address = arg.substr(3, arg.find(delimiter, portDelimiter)-3);
  ++portDelimiter;
  std::string port = arg.substr(portDelimiter,
                                arg.find(delimiter, portDelimiter) -
                                portDelimiter);
  dataFd = server->openConnection(address, port);
  if (dataFd != -1) {
    sendStdString(replies[8]);
  } else {
    sendStdString("425 Socket creation failed.");
  }
}

void ftpConnection::cmdRetr(std::string const& arg) {
  if (dataFd != -1) {
    std::string fileName = arg;
    if (arg[0] != '/') {
      fileName = pwd + arg;
    }
    outputLine("Trying to send file %s!\n", fileName.c_str());
    sendStdString("150 Sending file " + arg + "\r\n");
    sendFile(fileName);
    close(dataFd);
    sendStdString(replies[10]);
  }
}

void ftpConnection::cmdStor(std::string const& arg) {
  if (dataFd != -1) {
    std::string fileName = arg;
    if (arg[0] != '/') {
      fileName = pwd + arg;
    }
    outputLine("Trying to receive file %s!\n", fileName.c_str());
    sendStdString("150 Receiving file " + arg + "\r\n");
    recvFile(fileName);
    close(dataFd);
    sendStdString(replies[10]);
  }
}

void ftpConnection::cmdUnimplemented(std::string const& arg) {
  sprintf(buf, replies[4].c_str(), arg.c_str());
  sendStdString(buf);
}

std::string ftpConnection::unixToDosPath(std::string const& path) {
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


void ftpConnection::sendFolderContents(int fd, std::string &path) {
  if (path[0] == '/' && path[1] == '/') {
    path = path.substr(1, std::string::npos);
  }
  if (!path.compare("/")) {
    for (size_t i = 0; i < drives.length(); ++i) {
      if (nxIsDriveMounted(drives[i])) {
        sendStdString(fd, "drwxr-xr-x 1 XBOX XBOX 0 2020-03-02 10:41 " +
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
    TIME_FIELDS fTime;
    RtlTimeToTimeFields((PLARGE_INTEGER)&fData.ftLastWriteTime, &fTime);
    retstr += "rwxr-xr-x 1 XBOX XBOX " +
      std::to_string(fData.nFileSizeLow) + " " +
      std::to_string(fTime.Year) + "-" + std::to_string(fTime.Month) + "-" + std::to_string(fTime.Day) + " " +
      std::to_string(fTime.Hour) + ":" + std::to_string(fTime.Minute) + " " +
      fData.cFileName + "\r\n";
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

bool ftpConnection::sendFile(std::string const& fileName) {
#ifdef NXDK
  std::string filePath = unixToDosPath(fileName);
  HANDLE fHandle = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  outputLine(("\r\n" + filePath + "\r\n").c_str());
  if (fHandle == INVALID_HANDLE_VALUE) {
    outputLine("File opening failed. LOL. \n");
    return false;
  }
  int bytesToRead = 1048573;
  unsigned long bytesRead = 0;
  char* buff = static_cast<char*>(malloc(1048576));

  while (ReadFile(fHandle, buf, bytesToRead, &bytesRead, NULL) && (bytesRead > 0)) {
    send(dataFd, buf, bytesRead, 0);
  }
  CloseHandle(fHandle);
  free(buff);
  return true;
#else
  char buff[1024]; // Buffer of garbage
  int bytesRead = 1024;
  send(dataFd, buf, bytesRead, 0);
  return true;
#endif
}

bool ftpConnection::recvFile(std::string const& fileName) {
  std::string filePath = unixToDosPath(fileName);
#ifdef NXDK
  HANDLE fHandle = CreateFile(filePath.c_str(), GENERIC_WRITE,
                              0, NULL, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
  if (fHandle == INVALID_HANDLE_VALUE) {
    outputLine("File creation failed. LOL. \n");
    return false;
  }
#endif
  outputLine(("\r\n" + filePath + "\r\n").c_str());
  int bytesToWrite = (64 * 1024) - 3;
  unsigned long bytesWritten;
  unsigned long bytesRead;
  char* buff = static_cast<char*>(malloc(FTP_BUFFER_SIZE));

  while ((bytesRead = recv(dataFd, buf, bytesToWrite, 0))) {
#ifdef NXDK
    WriteFile(fHandle, buff, bytesRead, &bytesWritten, NULL);
#else
    outputLine(buf);
#endif
  }
#ifdef NXDK
  CloseHandle(fHandle);
#endif
  free(buff);
  return true;
}
