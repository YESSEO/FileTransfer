#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lib.h"

#define PORT 7088

unsigned int copy_file(unsigned char *, unsigned int, char *);
void recv_file(int);

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

    recv_file(new_sockfd);
  }
}

ssize_t recv_all(int sockfd, void *buffer, size_t length){
  size_t total = 0;
  char *ptr = buffer;

  while(total < length){
    ssize_t bytes = recv(sockfd, ptr + total, length - total, 0);
    if(bytes <= 0)
      fatal("in recv");
    total += bytes;

  }
  return total;
}

void recv_file(int sockfd){
  uint32_t name_len_net;
  if(recv_all(sockfd, &name_len_net, sizeof(name_len_net)) <= 0)
    fatal("recv name_len");

  name_len_net = ntohl(name_len_net);
  printf("[DEBUG] Filename is %d bytes\n", name_len_net);

  // Get file name
  char *filename;
  filename = (char *) malloc(name_len_net +1);
  recv_all(sockfd, filename, name_len_net);
  filename[name_len_net] = '\0';
  printf("[DEBUG] Filename '%s'\n", filename);

 // Get  file size
 uint32_t file_size;
 if(recv_all(sockfd, &file_size, sizeof(file_size)) <=0){
  fatal("recv file size");
 }
 uint32_t size = ntohl(file_size);
 printf("[DEBUG] file size : %d\n", size);

 char *buffer = malloc(size);
 if(!buffer) fatal("allocating  file size");
 
 recv_all(sockfd, buffer, size);


 // Write file
 FILE *fd;
 fd = fopen(filename, "wb");
 if(!fd) fatal("openning file");
 fwrite(buffer, size, 1, fd);
 fclose(fd);

 free(buffer);
 }