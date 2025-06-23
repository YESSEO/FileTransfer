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


void _send(int, char *, unsigned int);
void save_file(int, const char *);
long get_file_size(const char *);


void help(char *prog_name){
  printf("<Usage>: %s <File path to send> \n", prog_name);
  exit(0);
}

ssize_t recv_all(int sockfd, void *buffer, size_t length) {
    size_t total = 0;
    char *ptr = buffer;

    while (total < length) {
        ssize_t bytes = recv(sockfd, ptr + total, length - total, 0);
        if (bytes <= 0) {
            return -1;
        }
        total += bytes;
    }

    return total;
}



int main(int argc, char *argv[]){

  /* cheking args */
  if(argc < 2)
    help(argv[0]);

  char *filename;
  filename = argv[1];
  printf("[DEBUG] - Full path: '%s'. \n", filename);
  int sockfd, new_sockfd, sock_size;
  struct sockaddr_in client_addr;
  socklen_t recv_size;
  char buffer[1024];


  /* Create socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1)
    fatal("creating socket");

  /* Defining struct */
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(PORT);
  client_addr.sin_addr.s_addr= inet_addr(SERVER_IP);

  /* Connecting */
  if(connect(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
    fatal("Connecting to server");

  /* Read */
  int bytes_recieved = recv(sockfd, buffer, sizeof(buffer), 0);
  if(bytes_recieved == -1){
    close(sockfd);
    fatal("While receiving bytes from server");
  } else if (bytes_recieved == 0) {
    printf("Server closed the connection.\n");
  } else {
    buffer[bytes_recieved] = '\0';
    printf("RCV: %d bytes.\n", bytes_recieved);
    dump(buffer, bytes_recieved);
  }

  save_file(sockfd, filename);

  EXIT_SUCCESS;
}


void send_all(int sockfd, const void *buffer, size_t length){
  size_t total = 0;
  const char *ptr = buffer;

  while(total < length){
      ssize_t sent = send(sockfd, ptr + total, length - total, 0);
      if(sent <= 0 ){
        fatal("in send");
      }
      total += sent;
  }
  
}

void save_file(int sockfd, const char *file_path){

  const char *filename = strrchr(file_path, '/');
  filename = filename ? filename + 1 : file_path;

  uint32_t file_name_len = htonl(strlen(filename));
  send_all(sockfd, &file_name_len, sizeof(file_name_len));  
  send_all(sockfd, filename, strlen(filename));  

  FILE *pf = fopen(file_path, "rb");
  if(!pf)
    fatal("opening file");

  
  fseek(pf, 0, SEEK_END);
  uint64_t size = ftell(pf);
  
  printf("[DEBUG] File size: %ld\n", size);
  rewind(pf);

  // sending file size
  uint32_t file_size;
  file_size = htonl(size);
  send_all(sockfd, &file_size, sizeof(file_size));  

 // Sending file
 char buffer[4094];
 size_t bytes;

 while((bytes = fread(buffer, 1, sizeof(buffer), pf)) >  0) {  
    send_all(sockfd, buffer, bytes);
 }

}
