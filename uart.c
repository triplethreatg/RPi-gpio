#include "RPI.h"

// GPIO 14 (TXD) on pin 8
#define TXD_PIN			8
// GPIO 15 (RXD) on pin 10
#define RXD_PIN			10

//struct bcm2835_peripheral gpio = {GPIO_BASE};
//struct bcm2835_peripheral auxp = {AUX_PERI_BASE};

void init_uart();

unsigned char rxd_data = 0;

int main()
{
	if(map_peripheral(&gpio) == -1)
	{
		printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
		return -1;
	}
	if(map_peripheral(&auxp) == -1)
	{
		printf("Failed to map the physical Auxillary Peripheral registers into the virtual memory space.\n");
		return -1;
	}

	init_uart();

	while(1)
	{
		printf("Recieve Data Test"\n);
		// Poll until recieve interrupt is generated
		while(!(AUX_MU_IIR_REG & AUX_MU_IIR_REG_RXD));
		printf("Generated a recieve interrupt\n");
		// Recieve data and store as rxd_data
		rxd_data = AUX_MU_IO_REG;
		printf("%s", rxd_data);
		// Clear recieve FIFO
		AUX_MU_IER_REG |= AUX_MU_IER_REG_ID;
	}

	return 0;

}

void init_uart()
{
	// Define (txd) pin 8 as output
	INP_GPIO(TXD_PIN);
	OUT_GPIO(TXD_PIN);

	// Define (rxd) pin 10 as input
	INP_GPIO(RXD_PIN);

	// Use alt function 5 on pin 8 and pin 10
	SET_GPIO_ALT(TXD_PIN, 5);
	SET_GPIO_ALT(RXD_PIN, 5);

	// Disable mini UART
	// AUX_ENABLES &= ~(AUX_ENABLES_MU);

	// Disable UART interrupts
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_TXD);
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_RXD);

	// UART RXD line high
	AUX_MU_MCR_REG &= ~(AUX_MU_MCR_REG_RTS);

	// Clear FIFO
	AUX_MU_IER_REG |= (0x03 * AUX_MU_IER_REG_ID);

	// Enable UART Transmissions
	AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_TXD;
        AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_RXD;

	// Enable UART
	AUX_ENABLES |= AUX_ENABLES_MU;
}

// Transmit a character string of ASCII hex numbers
void transmit(char * txd_data)
{
	for(int x = 0; x < 5; x++)
	{
		while(!(AUX_MU_LSR_REG & AUX_MU_LSR_REG_EMT));
		AUX_MU_IO_REG = txd_data[x];
	}
}
