#include "w5100.h"
#include <string.h>

static uint8_t read_byte(W51Iface *iface, uint16_t addr) {
  uint8_t haddr;
  uint8_t laddr;
  uint8_t out;

  haddr = (uint8_t)(addr >> 8);
  laddr = (uint8_t)(addr & 0xFF);

  iface->sel();
  iface->xchg(W5100_READ_OPCODE);
  iface->xchg(haddr);
  iface->xchg(laddr);
  out = iface->xchg(0x00);
  iface->desel();
  return out;
}

static uint16_t read_word(W51Iface *iface, uint16_t addr) {
  uint16_t h;
  uint16_t l;

  h = read_byte(iface, addr++);
  l = read_byte(iface, addr);
  return (h << 8) + l;
}

static void read_block(W51Iface *iface, uint16_t start_addr, uint16_t count,
                       uint8_t *data) {
  for (uint16_t addr = start_addr; addr < (start_addr + count); addr++) {
    *data = read_byte(iface, addr);
    data++;
  }
}

static void write_byte(W51Iface *iface, uint16_t addr, uint8_t data) {
  uint8_t haddr;
  uint8_t laddr;

  haddr = (uint8_t)(addr >> 8);
  laddr = (uint8_t)(addr & 0xFF);

  iface->sel();
  iface->xchg(W5100_WRITE_OPCODE);
  iface->xchg(haddr);
  iface->xchg(laddr);
  iface->xchg(data);
  iface->desel();
}

static void write_word(W51Iface *iface, uint16_t addr, uint16_t val) {
  uint8_t d;

  d = (uint8_t)(val >> 8);
  write_byte(iface, addr++, d);

  d = (uint8_t)(val & 0x00FF);
  write_byte(iface, addr, d);
}

static void write_block(W51Iface *iface, uint16_t start_addr, uint16_t count,
                        uint8_t *data) {
  for (uint16_t addr = start_addr; addr < (start_addr + count); addr++) {
    write_byte(iface, addr, *data);
    data++;
  }
}

/* Read from 8-bit socket register */
static uint8_t read_sreg_byte(Socket *s, uint16_t offset) {
  uint16_t addr;

  addr = W5100_SKT_BASE(s->_fd) + offset;
  return read_byte(s->_if, addr);
}

/* Read from 16-bit socket register */
static uint16_t read_sreg_word(Socket *s, uint16_t offset) {
  uint16_t addr;

  addr = W5100_SKT_BASE(s->_fd) + offset;
  return read_word(s->_if, addr);
}

/* Write to 8-bit socket register */
static void write_sreg_byte(Socket *s, uint16_t offset, uint8_t val) {
  uint16_t addr;

  addr = W5100_SKT_BASE(s->_fd) + offset;
  write_byte(s->_if, addr, val);
}

/* Write to 16-bit socket register */
static void write_sreg_word(Socket *s, uint16_t offset, uint16_t val) {
  uint16_t addr;

  addr = W5100_SKT_BASE(s->_fd) + offset;
  write_word(s->_if, addr, val);
}

static void reset(W51Iface *iface) {
  /* Will reset via RESET pin */
  if (iface->rst)
    iface->rst();
  /* Perform soft reset */
  write_byte(iface, W5100_MR, W5100_MR_SOFTRST);
}

int w51_init(W51Iface *iface) {
  Socket *s;

  if (iface->sel == NULL || iface->desel == NULL || iface->xchg == NULL)
    return -1;

  reset(iface);

  /* Close all socktes */
  for (int i = 0; i < W5100_NSOCK; i++) {
    s = &iface->sockets[i];
    s->_fd = i;
    s->_if = iface;
  }
  return 0;
}

int w51_init_iface(W51Iface *iface, IfaceConfig *cfg) {
  write_block(iface, W5100_GAR, 4, cfg->gw);
  write_block(iface, W5100_SUBR, 4, cfg->sn);
  write_block(iface, W5100_SHAR, 6, cfg->mac);
  write_block(iface, W5100_SIPR, 4, cfg->ip);

  return 0;
}

SockStatus sock_status(Socket *s) { return read_sreg_byte(s, W5100_SR_OFFSET); }

int sock_close(Socket *s) {
  write_sreg_byte(s, W5100_CR_OFFSET, W5100_SKT_CR_CLOSE);

  if (sock_status(s) == SockStatus_CLOSED)
    return 0;

  return -1;
}

bool sock_is_connected(Socket *s) {
  if (sock_status(s) == SockStatus_ESTABLISHED) {
    return true;
  }

  return false;
}

static Socket *get_free_socket(W51Iface *iface) {
  Socket *s = NULL;
  for (int i = 0; i < W5100_NSOCK; i++) {
    /* Get a socket from pool */
    s = &iface->sockets[i];
    /* Check status */
    switch (sock_status(s)) {
    case SockStatus_CLOSEWAIT:
      /* Clsoe the socket */
      if (sock_close(s) != 0)
        return NULL;
    /* fallthrough */

    case SockStatus_CLOSED:
      return s;

    default:
      break;
    }
  }

  return NULL;
}

int sock_init(W51Iface *iface, Socket *s) {
  if (iface == NULL)
    return -1;

  memset(s, 0, sizeof(Socket));
  s->_if = iface;
  s->_fd = -1;

  return 0;
}

int sock_bind(Socket *s, int port) {
  Socket *new;

  /* Validate input */
  if (s->_if == NULL || port < 0)
    return -1;

  /* Get new free socket */
  if ((new = get_free_socket(s->_if)) == NULL)
    return -1;

  /* Initialize the socket */
  s->_sport = port;
  s->_fd = new->_fd;

  /* Set socket port */
  write_sreg_word(s, W5100_PORT_OFFSET, port);

  /* Set protocol to TCP */
  write_sreg_byte(s, W5100_MR_OFFSET, (uint8_t)SockProto_TCP);

  /* Open the socket */
  write_sreg_byte(s, W5100_CR_OFFSET, W5100_SKT_CR_OPEN);

  /* Check status, should be INIT */
  if (sock_status(s) != SockStatus_INIT)
    return -2;

  return 0;
}

int sock_listen(Socket *s) {
  if (s->_if == NULL || s->_fd == -1 || sock_status(s) != SockStatus_INIT)
    return -1;

  write_sreg_byte(s, W5100_CR_OFFSET, W5100_SKT_CR_LISTEN);

  if (sock_status(s) != SockStatus_LISTEN)
    return -1;

  return 0;
}

Socket *sock_accept(Socket *p) {
  Socket *out;

  /* Check the passive socket, rebind if necessary */
  if (p->_fd == -1) {
    if (sock_bind(p, p->_sport) != 0) {
      /* Bind error */
      return NULL;
    } else {
      if (sock_listen(p) != 0) {
        /* Listen error */
        return NULL;
      }
    }
  }

  /* Check new connection */
  if (sock_status(p) != SockStatus_ESTABLISHED)
    return NULL;

  out = &p->_if->sockets[p->_fd];

  /* Clear passive socket descriptor to listen on new socket at next call */
  p->_fd = -1;

  return out;
}

int sock_read(Socket *s, uint8_t *buf, size_t count) {
  uint16_t sz;
  uint16_t rx_rd;
  uint16_t offset;
  uint16_t saddr;
  uint16_t i;
  uint16_t base;
  uint16_t mask = 0x7FF;

  if (sock_status(s) != SockStatus_ESTABLISHED)
    return -1;

  base = s->_fd * 0x800 + 0x6000;
  sz = read_sreg_word(s, W5100_RX_RSR_OFFSET);
  if (sz == 0)
    return 0;
  if (sz > count)
    return -ENOMEM;

  rx_rd = read_sreg_word(s, W5100_RX_RD_OFFSET);
  offset = rx_rd & mask;
  saddr = base + offset;

  if ((offset + sz) > 0x800) {
    i = 0x800 - offset;
    read_block(s->_if, saddr, i, buf);
    buf += i;
    i = sz - i;
    read_block(s->_if, base, i, buf);
  } else {
    // Normal
    read_block(s->_if, saddr, sz, buf);
  }

  rx_rd += sz;
  write_sreg_word(s, W5100_RX_RD_OFFSET, rx_rd);

  write_sreg_byte(s, W5100_CR_OFFSET, W5100_SKT_CR_RECV);
  return sz;
}

int sock_write(Socket *s, uint8_t *buf, size_t count) {
  uint16_t free;
  uint16_t tx_wr;
  uint16_t offset;
  uint16_t saddr;
  uint16_t i;
  uint16_t base;
  uint16_t mask = 0x7FF;

  if (sock_status(s) != SockStatus_ESTABLISHED)
    return -1;

  /* Get free memory size */
  free = read_sreg_word(s, W5100_TX_FSR_OFFSET);

  /* Return if no free memory available */
  if (free == 0)
    return -ENOMEM;

  /* Decrease count to free memory size if needed */
  if (count > free)
    count = free;

  tx_wr = read_sreg_word(s, W5100_TX_WR_OFFSET);
  base = s->_fd * 0x800 + 0x4000;
  offset = tx_wr & mask;
  saddr = base + offset;

  if ((offset + count) > 0x800) {
    i = 0x800 - offset;
    write_block(s->_if, saddr, i, buf);
    buf += i;
    i = count - i;
    write_block(s->_if, base, i, buf);
  } else {
    write_block(s->_if, saddr, count, buf);
  }

  tx_wr += count;
  write_sreg_word(s, W5100_TX_WR_OFFSET, tx_wr);

  write_sreg_byte(s, W5100_CR_OFFSET, W5100_SKT_CR_SEND);

  return count;
}
