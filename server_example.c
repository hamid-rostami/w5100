#include <stdio.h>
#include <string.h>

#include "w5100.h"

#define SERVER_PORT 1234

W51Iface iface;
IfaceConfig ifcfg;
Socket server_socket;

/* Hardware deendent SPI functions */
void w5_sel(void) { /* Select W5100 SS pin */ }

void w5_desel(void) { /* Deselect W5100 SS pin */ }

uint8_t w5_xchg(uint8_t tx) {
  /* Send tx and return received byte from SPI */
  return 0;
}

int main(void) {
  int ret;
  Socket *client;
  uint8_t buf[255];

  /* Init W5100 SPI functions */
  memset(&iface, 0, sizeof(iface));
  iface.sel = w5_sel;
  iface.desel = w5_desel;
  iface.xchg = w5_xchg;
  ret = w51_init(&iface);
  printf("W5100 Init: %d", ret);

  /* W5100 network configurations */
  memset(&ifcfg, 0, sizeof(ifcfg));
  /* IP */
  ifcfg.ip[0] = 192;
  ifcfg.ip[1] = 168;
  ifcfg.ip[2] = 1;
  ifcfg.ip[3] = 123;
  /* Subnet */
  ifcfg.sn[0] = 255;
  ifcfg.sn[1] = 255;
  ifcfg.sn[2] = 255;
  ifcfg.sn[3] = 0;
  /* Gateway */
  ifcfg.gw[0] = 192;
  ifcfg.gw[1] = 168;
  ifcfg.gw[2] = 1;
  ifcfg.gw[3] = 1;
  /* MAC */
  ifcfg.mac[0] = 0x28;
  ifcfg.mac[1] = 0x39;
  ifcfg.mac[2] = 0x26;
  ifcfg.mac[3] = 0x47;
  ifcfg.mac[4] = 0xf4;
  ifcfg.mac[5] = 0x01;
  /* Apply interface configs */
  w51_init_iface(&iface, &ifcfg);

  /* Initial the server socket */
  sock_init(&iface, &server_socket);
  /* Bind socket to specific port */
  ret = sock_bind(&server_socket, SERVER_PORT);
  printf("bind: %d", ret);

  if (ret != 0)
    return -1;

  /* Listen on server socket */
  ret = sock_listen(&server_socket);

  /* Main loop to accept new connections */
  while (1) {
    /* Accept new connection */
    client = sock_accept(&server_socket);
    if (client != NULL) {
      /* Here you can send/recieve to/from client */
      ret = sock_read(client, buf, sizeof(buf));
      if (ret < 0) {
        /* Read error */
        continue;
      }
      printf("%d bytes received\n", ret);

      ret = sock_write(client, (uint8_t *)"Hello world!", 12);
      if (ret < 0) {
        /* Write error */
        continue;
      }
      printf("%d bytes send\n", ret);
      sock_close(client);
    }
  }
}
