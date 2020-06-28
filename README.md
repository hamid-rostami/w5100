This library is hardware independent and can be use on any microcontroller
that can communiate via SPI port (No matter hardware or software SPI).

# Feautures
* No dynamic memory allocation
* Non-block design
* Highly portable
* Tried to be like POSIX API sockets

# How to use

## 1. Implement your own SPI functions
The user must implement own SPI functions based on hardware/microcontroller.
Four function like below needs to implement:

```c
void select(void) {
  /* Select the W5100 by CS pin */
}

void deselect(void) {
  /* Deselect the W5100 */
}

uint8_t spi_xchg(uint8_t tx) {
  /* Send tx byte and return received byte */
}

/* This is optional */
void reset(void) {
  /* Reset the W5100 chip by RESET pin */
}
```

## 2. Initialize the library
Declare a variable with type `W51Iface` and initialize it with
your SPI functions.

```c
W51Iface iface;

memset(&iface, 0, sizeof(iface));
iface.sel = select;
iface.desel = deselect;
iface.xchg = spi_xchg;
iface.rst = reset; /* Optional */
w51_init(&iface);
```

At this point, library can talk with chip.

## 3. Config interface addresses
Set your IP, Netmask, Gateway and MAC addresses.

```c
IfaceConfig ifcfg;

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
```

After this step you should be able to ping assocated ip address.
Now you can make a server by listening to specific port and
handle incoming connections (see section 4.1) or connect to
remote server (Not implemented yet)

## 4.1 Server mode
To run a server, first need to declare a `Socket`, bind it to
specific port and then listen to incoming connections.

```c
enum {
  SERVER_PORT = 1234
};

Socket server_socket;

/* Initial the server socket */
sock_init(&iface, &server_socket);

/* Bind socket to specific port */
sock_bind(&server_socket, SERVER_PORT);

/* Listen on server socket */
sock_listen(&server_socket);
```

Now it is time to get new connections, For this purpose use
`accept` function as below:

```c
int ret;
Socket *client;
uint8_t buf[255];
  
/* Main loop to accept new connections */
while(1) {
  /* Accept new connection */
  client = sock_accept(&server_socket);
  if (client != NULL) {
    /* Here you can send/recieve to/from client */
    ret = sock_read(client, buf, sizeof(buf));
    if (ret < 0 ) {
	/* Read error */
	continue;
    }
    printf("%d bytes received\n", ret);
    
    ret = sock_write(client, (uint8_t*)"Hello world!", 12);
    if (ret < 0) {
	/* Write error */
	continue;
    }
    printf("%d bytes send\n", ret);
    sock_close(client);
  }
}
```

For detailed example, see
[server_example.c](./server_example.c)
file

## 4.2 Client mode

Not implemented yet.