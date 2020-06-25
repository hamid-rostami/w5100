#include "w5100.h"

Socket* get_free_socket(Socket* list, int n) {
  for (int i = 0; i < n; i++) {
    if (sock_is_closed(list[i]))
      return list[i];
  }
  return NULL;
}

int main(void) {
  W51Iface eth;
  IFConfig ifcfg;
  Socket server;
  Socket slist[4];
  Socket *conn;
  /* Set SPI functions for eth */
  w51_init(&eth);

  /* Initial ifcfg */
  w51_init_iface(&eth, &ifcfg);

  socket_bind(&server, 1234);
  socket_listen(&server);

  while(1) {
    if ((conn = get_free_socket(slist, 4)) == NULL)
      continue;

    if (accept(&server, conn) == 0) {
      /* Server the socket */
    }
  }
}
