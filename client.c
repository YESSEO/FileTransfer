#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "lib.h"

#define PORT 7088
#define SERVER_IP "127.0.0.1"

int main(int argc, char* argv[]){

  int sockfd, new_sockfd, sock_size;
  struct sockaddr_in client_addr;
  socklen_t recv_size;
  char buffer[1024];

  /* Create socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1)
    fatal("creating socket");

  /* Defining struct */
  memset(&client_addr.sin_zero,0, 8);
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  client_addr.sin_port = htons(PORT);

  /* Connecting */
  if(connect(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1)
    fatal("connecting");


  /* Read */

  fdopen(sockfd, "r");

  printf("%s\n", buffer);

}
