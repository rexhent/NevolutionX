#include "pvTester.h"

#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <string.h>

int pvTester(void*) {
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr; 
  
  char buf[1025];
  size_t ticks; 
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(buf, '0', sizeof(buf)); 
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(5000); 
  
  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
  
  listen(listenfd, 10); 
  
  while(1)
  {
    int n, bytes_recvd = 0;
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
    printf("New connection\n");
    while ((n = read(connfd, buf, sizeof(buf)-1)) > 0) {
      bytes_recvd += n;
    }
    printf("Recv'd %d MiB\n", bytes_recvd/(1024*1024));
    close(connfd);
    Sleep(1);
  }
}
