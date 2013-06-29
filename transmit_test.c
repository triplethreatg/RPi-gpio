#include "RPI.h"

// GPIO 14 (TXD) on pin 8
#define TXD_PIN			8
// GPIO 15 (RXD) on pin 10
#define RXD_PIN			10

//struct bcm2835_peripheral gpio = {GPIO_BASE};
//struct bcm2835_peripheral auxp = {AUX_PERI_BASE};

void init_uart();

unsigned char rxd_data = 0;
unsigned int x; 

int main(int argc, char * argv[])
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
		printf("Transmit Data Test\n");
		if(argc > 1)
			transmit(argv[1]);
		else {
			printf("Error: wrong amount of arguments\n");
			return 1;
		}
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
	AUX_ENABLES |= (AUX_ENABLES_MU);

	// DLAB
	AUX_MU_LCR_REG &= ~(AUX_MU_LCR_REG_LAB);

	// TX LOW
	AUX_MU_LCR_REG |= AUX_MU_LCR_REG_BK;

	// Disable UART interrupts
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_TXD);
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_RXD);

	// UART RXD line high
	//AUX_MU_MCR_REG &= ~(AUX_MU_MCR_REG_RTS);

	// Clear FIFO
	AUX_MU_IER_REG |= (0x03 * AUX_MU_IER_REG_ID);

	// Ignore CTS
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_TXCTS);

	// Enable UART Transmissions
	AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_TXD;
        //AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_RXD;

	// Enable UART
	//AUX_ENABLES |= AUX_ENABLES_MU;
}

// Transmit a character string of ASCII hex numbers
void transmit(char * txd_data)
{
	printf("Enter into transmit method\n");
	for(x = 0; x < 7; x++)
	{
		while(!(AUX_MU_LSR_REG & AUX_MU_LSR_REG_EMT));
		printf("Passed transmit interrupt");
		AUX_MU_IO_REG = txd_data[x];
	}
}
