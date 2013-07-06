#ifndef _RPI_H
#define _RPI_H

#include <stdio.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#include <fcntl.h>


#define BCM2708_PERI_BASE		0x20000000			// physical address value  which the peripheral register starts
#define GPIO_BASE			(BCM2708_PERI_BASE + 0x200000)	// for GPIO controller
#define AUX_PERI_BASE			(BCM2708_PERI_BASE + 0x205000)	// for Auxilary periferals ie UART

#define BLOCK_SIZE			(4*1024)
#define PAGE_SIZE			(4*1024)

// GPIO setup macros. Always use INP_GPIO(x) to force bits to 0, before using OUT_GPIO(x)
// "*gpio.addr + ((g))/10" is the register address that contains GPFSEL bits of the pin "g"
#define INP_GPIO(g)			*(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)			*(gpio.addr + ((g/10))) |= (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a)		*(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET 			*(gpio.addr + 7)	// sets bits which are 1 ignores bits which are 0
#define GPIO_CLR			*(gpio.addr + 10)	// clears bits which are 1 ignores bits which are 0

#define GPIO_READ(g)			*(gpio.addr + 13) &= (1<<(g))

// Auxillary Peripheral macros
#define AUX_IRQ				*(auxp.addr + 0x00)	// auxillary interrupt status
#define AUX_ENABLES			*(auxp.addr + 0x04)	// auxillary enables
#define AUX_MU_IO_REG			*(auxp.addr + 0x40)	// mini UART I/O data
#define AUX_MU_IER_REG			*(auxp.addr + 0x44)	// mini UART interrupt enable
#define AUX_MU_IIR_REG			*(auxp.addr + 0x48)	// mini UART interrupt identify
#define AUX_MU_LCR_REG			*(auxp.addr + 0x4C)	// mini UART line control
#define AUX_MU_MCR_REG			*(auxp.addr + 0x50)	// mini UART modem control
#define AUX_MU_LSR_REG			*(auxp.addr + 0x54)	// mini UART line status
#define AUX_MU_MSR_REG			*(auxp.addr + 0x58)	// mini UART modem status
#define AUX_MU_SCRATCH			*(auxp.addr + 0x5C)	// mini UART scratch
#define AUX_MU_CNTL_REG			*(auxp.addr + 0x60)	// mini UART extra control
#define AUX_MU_STAT_REG			*(auxp.addr + 0x64)	// mini UART extra status
#define AUX_MU_BAUD_REG			*(auxp.addr + 0x68)	// mini UART baudrate

#define AUX_IRQ_MU			(1 << 0)		// if set MU has interrupt pending
#define AUX_ENABLES_MU			(1 << 0)		// if set MU enabled; if clear MU is disabled
#define AUX_MU_IER_REG_RXD		(1 << 0)		// if set recieve interrupt asserted when FIFO holds 1 byte; if clear no recieve interrupt generated
#define AUX_MU_IER_REG_TXD		(1 << 1)		// if set transmit interrupt asserted when FIFO is empty; if clear no interrupt generated
#define AUX_MU_IIR_REG_FIFO		(3 << 6)		// FIFO enables; always 11
#define AUX_MU_IIR_REG_ID		(3 << 1)		// 2 bit interrupt R/W (pg. 13)
#define AUX_MU_IIR_REG_IP		(1 << 0)		// clear when interrupt is pending
#define AUX_MU_LCR_REG_DS		(1 << 0)		// clear UART 7-bit mode; set UART 8-bit mode
#define AUX_MU_LCR_REG_BK		(1 << 7)		// DLAB access; set Mini UART register access the Baudrate register; must be cleared for operation
#define AUX_MU_LCR_REG_LAB		(1 << 6)		// Pull TX line low
#define AUX_MU_LCR_REG_DSZ		(1 << 0)		// DATA size clear UART 7-bit mode; set UART 8-bit mode
#define AUX_MU_MCR_REG_RTS		(1 << 1)		// clear UART1_RTS1 line high; set UART1_RTS line low
#define AUX_MU_LSR_REG_EMT		(1 << 5)		// set if transmit FIFO can accept one byte
#define AUX_MU_LSR_REG_DR		(1 << 0)		// set if recieve FIFO holds symbol
#define AUX_MU_CNTL_REG_CTS		(1 << 7)		// auto flow control; set level low; clear level high
#define AUX_MU_CNTL_REG_TXCTS		(1 << 3)		// set, stop when CTS deasserted; clear, ignore CTS
#define AUX_MU_CNTL_REG_TXD		(1 << 1)		// set MU transmit enabled; clear MU transmitter disabled
#define AUX_MU_CNTL_REG_RXD		(1 << 0)		// set MU reciever enabled; clear MU reciever disabled


// IO Access
// Will contain the information about the location of the registers
struct bcm2835_peripheral {
	unsigned long addr_p;
	int mem_fd;
	void *map;
	volatile unsigned int *addr;
};

int map_peripheral(struct bcm2835_peripheral *p);
void unmap_peripheral(struct bcm2835_peripheral *p);

//struct bcm2835_peripheral gpio = {GPIO_BASE};
//struct bcm2835_peripheral auxp = {AUX_PERI_BASE};

//extern struct bcm2835_peripheral gpio;
//extern struct bcm2835_peripheral auxp;

#endif
