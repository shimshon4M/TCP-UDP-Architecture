#include <stdio.h>
#include <sys/type.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "layer.h"

#define UDP 1
#define TCP 2

void udpsend(unsigned char *data,int len);
void tcpsend(unsigned char *data,int len);
void dosend(unsigned char *data,int len);

void udpsend(unsigned char *data,int len){
}

void tcpsend(unsigned char *data,int len){
}

void dosend(unsigned char *data,int len){
}

int main(int argc, char *argv[]){
  FILE *fp;
  int i;
  int size;
  char *fname="layer.h";
  int protocol_type;
  unsigned char data[1024];//今回は最大1024とする

  protocol_type=atoi(argv[1]);
  fname=argv[2];

  fp=fopen(fname,"rb");
  if(fp==NULL){
    fprintf(stderr,"can not open file\n");
    return -1;
  }
  
  size=fread(data,sizeof(unsigned char),1024,fp);
  printf("size=%d\n",size);

  printf("--- layer 3 ---\n");
  for(i=0;i<size;i++){
    if(i%16==0)
      printf("\n");
    printf("%02x ",data[i]);
  }
  fclose(fp);

  switch(protocol_type){
  case UDP:
    udpsend(data,size);
    break;
  case TCP:
    tcpsend(data,size);
    break;
  default:
    fprintf(stderrr,"do not support the protocol\n");
    break;
  }

  return 0;
}
