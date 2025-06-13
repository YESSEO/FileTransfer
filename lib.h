
void dump(unsigned char *data_buffer, const unsigned int length){
  unsigned char byte;
  unsigned int i,j;
  for(i=0; i < length;i++){
    byte = data_buffer[i];
    printf("%x02 ", data_buffer[i]);
    if ((i % 16 == 15) || (i == length -1)){ // Get length without \n ?, 15 for just printing 15 column
      for (j=0; j < 15 - (i%16); j++) // i will be the same if its less than 16, but will start from 0 once reaches 16
        printf(" "); 
      printf("| ");
      for(j=(i - (i % 16)); j <=i; j++){
        byte = data_buffer[j];
        if((byte > 31) && (byte < 127)) // insure its a printable char
          printf("%c", byte);
        else
          printf("."); 
      }
        printf("\n");
    } //End if
  } // End for
}



void fatal(char *message){
	char error[100];

	strcpy(error, "[!!] Fatal error ");
	strncat(error, message, 80);
	perror(message);
  exit(-1);
}
