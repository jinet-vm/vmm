#ifndef IO_H
#define IO_H

unsigned char inb(unsigned short int port);
void outb(unsigned short int port, unsigned char value);
void io_wait();

#endif