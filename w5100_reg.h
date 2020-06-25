#ifndef _W5100_REG_H
#define _W5100_REG_H

#define W5100_WRITE_OPCODE 0xF0
#define W5100_READ_OPCODE 0x0F

/* Wiznet W5100 register addresses */
#define W5100_MR 0x0000
#define W5100_GAR 0x0001
#define W5100_SUBR 0x0005
#define W5100_SHAR 0x0009
#define W5100_SIPR 0x000F
#define W5100_IR 0x0015
#define W5100_IMR 0x0016
#define W5100_RTR 0x0017
#define W5100_RCR 0x0019
#define W5100_RMSR 0x001A
#define W5100_TMSR 0x001B
#define W5100_PATR 0x001C
#define W5100_PTIMER 0x0028
#define W5100_PMAGIC 0x0029
#define W5100_UIPR 0x002A
#define W5100_UPORT 0x002E

#define W5100_SKT_REG_BASE 0x0400
#define W5100_SKT_OFFSET 0x0100
#define W5100_SKT_BASE(n) (W5100_SKT_REG_BASE + (n * W5100_SKT_OFFSET))

#define W5100_NSOCK 4

/* TX and RX buffer base addresses */
#define W5100_TXBUFADDR 0x4000
#define W5100_RXBUFADDR 0x6000

/* Socket's base register addresses  */
#define W5100_MR_OFFSET 0x0000
#define W5100_CR_OFFSET 0x0001
#define W5100_IR_OFFSET 0x0002
#define W5100_SR_OFFSET 0x0003
#define W5100_PORT_OFFSET 0x0004
#define W5100_DHAR_OFFSET 0x0006
#define W5100_DIPR_OFFSET 0x000C
#define W5100_DPORT_OFFSET 0x0010
#define W5100_MSS_OFFSET 0x0012
#define W5100_PROTO_OFFSET 0x0014
#define W5100_TOS_OFFSET 0x0015
#define W5100_TTL_OFFSET 0x0016
#define W5100_TX_FSR_OFFSET 0x0020
#define W5100_TX_RR_OFFSET 0x0022
#define W5100_TX_WR_OFFSET 0x0024
#define W5100_RX_RSR_OFFSET 0x0026
#define W5100_RX_RD_OFFSET 0x0028

/* Mode register values  */
#define W5100_MR_SOFTRST (1 << 7)
#define W5100_MR_PINGBLK (1 << 4)
#define W5100_MR_PPPOE (1 << 3)
#define W5100_MR_AUTOINC (1 << 1)
#define W5100_MR_INDINT (1 << 0)

/* Intrrtupt register values */
#define W5100_IR_CONFLICT (1 << 7)
#define W5100_IR_UNREACH (1 << 6)
#define W5100_IR_PPPOE (1 << 5)
#define W5100_IR_S3_INT (1 << 3)
#define W5100_IR_S2_INT (1 << 3)
#define W5100_IR_S1_INT (1 << 3)
#define W5100_IR_S0_INT (1 << 3)

/* Interrupt mask register values */
#define W5100_IMR_CONFLICT (1 << 7)
#define W5100_IMR_UNREACH (1 << 6)
#define W5100_IMR_PPPOE (1 << 5)
#define W5100_IMR_S3_INT (1 << 3)
#define W5100_IMR_S2_INT (1 << 3)
#define W5100_IMR_S1_INT (1 << 3)
#define W5100_IMR_S0_INT (1 << 3)

/* Socket protocl bits in Mode register */
#define W5100_SKT_P3 (1 << 3)
#define W5100_SKT_P2 (1 << 2)
#define W5100_SKT_P1 (1 << 1)
#define W5100_SKT_P0 (1 << 0)

/* Socket Status register values */
#define W5100_SKT_SR_CLOSED 0x00
#define W5100_SKT_SR_INIT 0x13
#define W5100_SKT_SR_LISTEN 0x14
#define W5100_SKT_SR_SYNSENT 0x15
#define W5100_SKT_SR_SYNRECV 0x16
#define W5100_SKT_SR_ESTABLISHED 0x17
#define W5100_SKT_SR_FIN_WAIT 0x18
#define W5100_SKT_SR_CLOSING 0x1A
#define W5100_SKT_SR_TIME_WAIT 0x1B
#define W5100_SKT_SR_CLOSE_WAIT 0x1C
#define W5100_SKT_SR_LAST_ACK 0x1D
#define W5100_SKT_SR_UDP 0x22
#define W5100_SKT_SR_IPRAW 0x32
#define W5100_SKT_SR_MACRAW 0x42
#define W5100_SKT_SR_PPPOE 0x5F

/* Socket command values for Command register */
#define W5100_SKT_CR_OPEN 0x01
#define W5100_SKT_CR_LISTEN 0x02
#define W5100_SKT_CR_CONNECT 0x04
#define W5100_SKT_CR_DISCON 0x08
#define W5100_SKT_CR_CLOSE 0x10
#define W5100_SKT_CR_SEND 0x20
#define W5100_SKT_CR_SEND_MAC 0x21
#define W5100_SKT_CR_SEND_KEEP 0x22
#define W5100_SKT_CR_RECV 0x40

#define W5100_SKT_MR_CLOSE 0x00
#define W5100_SKT_MR_TCP 0x01
#define W5100_SKT_MR_UDP 0x02
#define W5100_SKT_MR_IPRAW 0x03
#define W5100_SKT_MR_MACRAW 0x04
#define W5100_SKT_MR_PPPOE 0x05
#define W5100_SKT_MR_ND 0x20
#define W5100_SKT_MR_MULTI 0x80

#endif
