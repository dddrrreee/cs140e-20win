#ifndef __SEND_RECV_H__
#define __SEND_RECV_H__

// send a byte to the pi.  panic's if error.
void put_byte(int fd, uint8_t b);

// wait for a byte from the pi.  panic's if error.
uint8_t get_byte(int fd);

// get a uint32 from the pi.
uint32_t get_uint32(int fd);

// put a uint32 to the pi
void put_uint32(int fd, uint32_t u);

#endif
