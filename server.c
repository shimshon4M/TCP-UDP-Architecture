#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "layer.h"

void show(Header *head){
  printf("type    =%d\n",head->type);
  printf("version =%d\n",head->version);
  printf("protocol=%d\n",head->protocol);
}
void show_data(unsigned char *buf,int len){
  int i;
  printf("--- layer 3 ---");
  for(i=0;i<len;i++){
    if(i%16==0)
      printf("\n");
    printf("%02x ",buf[i]);
  }
  printf("\n");
}

void udpreceive(unsigned char *data){
  UDPHeader *head;
  head=(UDPHeader *)data;
  printf("--- layer 2 ---\n");
  printf("type    =%d\n",head->type);
  printf("len     =%d\n",head->len);
  
  show_data((unsigned char *)&head[1],head->len);
}

void tcpreceive(unsigned char *data){
  TCPHeader *head;
  int i;
  int fakeFlg=0;
  unsigned char digest[16];
  MD_CTX context;

  printf("--- layer 2 ---\n");
  head=(TCPHeader *)data;
  printf("type    =%d\n",head->type);
  printf("len     =%d\n",head->len);
  for(i=0;i<16;i++){
    printf("%02x ",head->digest[i]&0xff);
  }
  printf("\n");

  MDInit(&context);
  MDUpdate(&context,(unsigned char *)&head[1],head->len);
  MDFinal(digest,&context);
  for(i=0;i<16;i++){
    printf("%02x ",digest[i]&0xff);
  }
  printf("\n");

  for(i=0;i<16;i++){
    if((head->digest[i]&0xff)!=(digest[i]&0xff)){
      fakeFlg=1;
      break;
    }
  }

  if(fakeFlg){
    fprintf(stderr,"packet is not correct\n");
    return ;
  }else{
    printf("packet is protected\n");
  }

  show_data((unsigned char *)&head[1],head->len);
}

void receive(unsigned char *data){
  Header *head;
  head=(Header *)data;
  
  printf("--- layer 1 ---\n");
  printf("type    =%d\n",head->type);
  printf("version =%d\n",head->version);
  printf("protocol=%d\n",head->protocol);

  if(head->type==UDP_TYPE){
    udpreceive((unsigned char *)&head[1]);
  }else if(head->type==TCP_TYPE){
    tcpreceive((unsigned char *)&head[1]);
  }
}

int main(void){
  int server_sockfd,client_sockfd;
  int server_len;
  socklen_t client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  Header *head;
  unsigned char buf[1024];

  server_sockfd=socket(AF_INET,SOCK_STREAM,0);
  server_address.sin_family=AF_INET;
  server_address.sin_addr.s_addr=INADDR_ANY;
  server_address.sin_port=htons(8000);

  server_len=sizeof(server_address);
  bind(server_sockfd,(struct sockaddr *)&server_address,server_len);
  
  listen(server_sockfd,5);

  while(1){
    printf("server waiting\n");
    client_sockfd=accept(server_sockfd,(struct sockaddr *)&client_address,&client_len);
    read(client_sockfd,buf,sizeof(buf));
    write(client_sockfd,"4",2);
    close(client_sockfd);
    receive(buf);
  }
}
