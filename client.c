#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "layer.h"

#define UDP 1
#define TCP 2

void udpsend(unsigned char *data,int len);
void tcpsend(unsigned char *data,int len);
void dosend(unsigned char *data,int len,int head_type);

void udpsend(unsigned char *data,int len){
  UDPHeader head;
  int packet_len;
  unsigned char *buf;

  head.type=1;
  head.len=len;
  packet_len=sizeof(head)+len;

  printf("\n--- layer 2 ---\n");
  printf("type    =%d\n",head.type);
  printf("len     =%d\n",head.len);

  buf=(unsigned char *)malloc(packet_len);
  memcpy(buf,&head,sizeof(head));
  memcpy(&buf[sizeof(head)],data,len);

  dosend(buf,packet_len,UDP_TYPE);
  free(buf);
}

void tcpsend(unsigned char *data,int len){
  TCPHeader head;
  int packet_len;
  unsigned char *buf;
  unsigned char digest[16];
  MD_CTX context;
  int i;
  
  MDInit(&context);
  MDUpdate(&context,data,len);
  MDFinal(digest,&context);

  head.type=2;
  head.len=len;
  printf("\n--- layer 2 ---\n");
  printf("type    =%d\n",head.type);
  printf("len     =%d\n",head.len);
  
  for(i=0;i<16;i++){
    printf("%02x ",digest[i]);
  }
  printf("\n");

  memcpy(head.digest,digest,16);
  packet_len=sizeof(head)+len;

  buf=(unsigned char *)malloc(packet_len);
  memcpy(buf,&head,sizeof(head));
  memcpy(&buf[sizeof(head)],data,len);

  dosend(buf,packet_len,TCP_TYPE);
  free(buf);
}

void dosend(unsigned char *data,int len,int head_type){
  int sockfd;
  struct sockaddr_in address;
  int result;
  Header head;
  unsigned char *buf;
  int packet_len;
  int size;
  head.type=head_type;
  head.version=10;
  head.protocol=3;
  char temp[4];

  sockfd=socket(AF_INET,SOCK_STREAM,0);
  address.sin_family=AF_INET;
  address.sin_addr.s_addr=inet_addr("127.0.0.1");
  address.sin_port=htons(8000);

  result=connect(sockfd,(struct sockaddr *)&address,sizeof(address));
  if(result==-1){
    perror("connect error");
    exit(1);
  }

  printf("--- layer 1 ---\n");
  printf("type    =%d\n",head.type);
  printf("version =%d\n",head.version);
  printf("protocol=%d\n",head.protocol);

  packet_len=sizeof(head)+len;

  buf=(unsigned char *)malloc(packet_len);
  memcpy(buf,&head,sizeof(head));
  memcpy(&buf[sizeof(head)],data,len);

  write(sockfd,buf,packet_len);
  size=read(sockfd,temp,sizeof(temp));
  free(buf);
  
  close(sockfd);
  exit(0);
}

int main(int argc, char *argv[]){
  FILE *fp;
  int i;
  int size;
  char *fname="layer.h";
  int protocol_type;
  unsigned char data[1024];

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
    fprintf(stderr,"do not support the protocol\n");
    break;
  }

  return 0;
}
