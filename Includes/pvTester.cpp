#include "pvTester.h"

#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <string.h>

int pvTester(void*) {
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr; 
  
  unsigned char buf[1025];
  
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
    size_t n, bytes_recvd = 0;
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
    printf("New connection\n");
    while ((n = read(connfd, buf, sizeof(buf)-1)) > 0) {
      for (size_t i = 0; i < n - 1; ++i) {
        if (((buf[i] != (buf[i+1] - 1)) &&
             buf[i] != 255) ||
            (buf[i] == 255 && buf[i+1] != 0)) {
          printf("Bad value %d, index %d, other value %d\n", buf[i], i, buf[i+1]);
        }
      }
      bytes_recvd += n;
    }
    printf("Recv'd %d MiB\n", bytes_recvd/(1024*1024));
    close(connfd);
    Sleep(1);
  }
}
