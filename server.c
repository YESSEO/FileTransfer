#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lib.h"

#define PORT 7088

unsigned int copy_file(unsigned char *, unsigned int, char *);

int main(void){

  int debug=1;

  int sockfd, new_sockfd, sock_size;
  struct sockaddr_in server_addr, client_addr;
  socklen_t recv_size;
  char buffer[1024];
  char banner[] = "FileTransfer v0.1\n";



  /* Create socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1)
    fatal("creating socket");


  /* Allow socket for re use*/
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
    fatal("in setsockopt()");
  
  /* Prepare socket struct*/
  memset(&server_addr.sin_zero, 0, 8); // Keeping the last 8byes 0.
  server_addr.sin_addr.s_addr = 0; // Any interface
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  /* Binding */
  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1 )
    fatal("while binding");

  /* Listenning */
  if (listen(sockfd, 5) == -1)
    fatal("while listenning");
  
  printf("Listenning on %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));


  /* accepting*/
  while(1){
    sock_size = sizeof(struct sockaddr_in);
    new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sock_size);
    printf("Accepting connection from %s:%d\n",
      inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    /*Sending Banner*/
    send(new_sockfd, banner, strlen(banner),0 );

    // send options

    recv_size = recv(new_sockfd, &buffer, sizeof(buffer), 0);
    
    while(recv_size > 0){
      printf("RCV: %d bytes\n", recv_size);
      if (debug == 1)
        dump(buffer, recv_size);
      /* 
        file copy here 
      */
      recv_size = recv(new_sockfd, &buffer, sizeof(buffer), 0);
    }
  }
}


unsigned int copy_file(unsigned char *buffer, unsigned int size, char *filename){
  size_t buffer_size = strlen(buffer);

  FILE *file = fopen(filename, "a");
  if(file == NULL)
    fatal("Creating file");

  size_t bytes_written = fwrite(buffer, 1, buffer_size, file);
  if(bytes_written != buffer_size)
    fatal("Error writing file");
  
    if(fclose(file))
      fatal("In closing file");

    return 0;
}
