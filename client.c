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
char *save_file(const char *);
long get_file_size(const char *);


void help(char *prog_name){
  printf("<Usage>: %s <File path to send> \n", prog_name);
  exit(0);
}


int main(int argc, char *argv[]){

  /* cheking args */
  if(argc < 2)
    help(argv[0]);

  /*
  File struct 
  */

  struct _file{
    FILE *name;
    long size;
    char *mim_type;
    char content[];
  };

  char *filename;
  filename = argv[1];

  printf("[DEBUG] filename: '%s'. \n", filename);
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


  /* Attempting to read files */
  /* Save file struct */

  // old func
  // char *saved_file_ptr = save_file(filename);

}

void _send(int sockfd, char *message, unsigned int size){
  send(sockfd, message, size, 0);
}


char *save_file(const char *filename){

  FILE *file;
  file = fopen(filename, "rb");

  if(file == NULL)
    fatal("openning file");


  fseek(file, 0, SEEK_END);
  long filesize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = (char *) malloc(filesize + 1);

  if(buffer == NULL)
    fatal("Allocating memory for reading file");

  size_t bytes_read = fread(buffer, 1, filesize, file);
  if(bytes_read != filesize){
    free(buffer);
    fclose(file);
    fatal("Reading file");
  }

  buffer[filesize] = '\0';
  fclose(file);

  return buffer;

}
