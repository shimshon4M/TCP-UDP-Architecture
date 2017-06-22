#ifndef _LAYER_H_INCLUDED_
#define _LAYER_H_INCLUDED_

#include "global.h"
#include "md5.h"

#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final

typedef enum{
  UDP_TYPE=1,
  TCP_TYPE
}HeaderType;

typedef struct{
  int type;
  int version;
  int protocol;
}Header;

typedef struct{
  int type;
  int len;
}UDPHeader;

typedef struct{
  int type;
  int le;
  char digest[16];
}TCPHeader;

#endif
