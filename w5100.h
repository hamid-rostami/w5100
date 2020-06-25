#ifndef _W5100_H_
#define _W5100_H_

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "w5100_reg.h"

typedef struct W51Iface W51Iface;

typedef struct {
  /* Interface instance */
  W51Iface *_if;
  /* Socket number on W5100 chip */
  int _fd;
  /* Source port */
  int _sport;
} Socket;

typedef struct W51Iface {
  /* SPI Functions */
  void (*sel)(void);
  void (*desel)(void);
  uint8_t (*xchg)(uint8_t tx);
  void (*rst)(void);

  Socket sockets[W5100_NSOCK];
} W51Iface;

typedef struct {
  uint8_t ip[4];
  uint8_t sn[4];
  uint8_t gw[4];
  uint8_t mac[6];
} IfaceConfig;

enum SockProto {
  SockProto_CLOSED = 0,
  SockProto_TCP,
  SockProto_UDP,
  SockProto_RAW
};

typedef enum {
  SockStatus_CLOSED = 0x00,
  SockStatus_INIT = 0x13,
  SockStatus_LISTEN = 0x14,
  SockStatus_ESTABLISHED = 0x17,
  SockStatus_CLOSEWAIT = 0x1C,
  SockStatus_UDP = 0x22,
  SockStatus_IPRAW = 0x32,
  SockStatus_MACRAW = 0x42,
  SockStatus_PPPOE = 0x5F,
  SockStatus_SYNSET = 0x15,
  SockStatus_SYNRECV = 0x16,
  SockStatus_FINWAIT = 0x18,
  SockStatus_CLOSING = 0x1A,
  SockStatus_TIMEWAIT = 0x1B,
  SockStatus_LASTACK = 0x1D
} SockStatus;

int w51_init(W51Iface *iface);
int w51_init_iface(W51Iface *iface, IfaceConfig *cfg);

int sock_init(W51Iface *iface, Socket *s);
int sock_bind(Socket *s, int port);
int sock_listen(Socket *s);
Socket* sock_accept(Socket *p);
int sock_close(Socket *s);
int sock_read(Socket *s, uint8_t *buf, size_t count);
int sock_write(Socket *s, uint8_t *buf, size_t count);

bool sock_is_connected(Socket *s);


#endif
