#include "RPI.h"

// GPIO 14 (TXD) on pin 8
#define TXD_PIN			14
// GPIO 15 (RXD) on pin 10
#define RXD_PIN			15

extern struct bcm2835_peripheral gpio;
extern struct bcm2835_peripheral auxp;

void init_uart();

unsigned char rxd_data = 0;
unsigned int x;
unsigned int status;

int main(int argc, char * argv[])
{

        printf("GPIO register address: %x\n", gpio.addr_p);
        printf("AUX register address: %x\n\n\n", auxp.addr_p);

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

	// Debug print register addresses
	printf("BCM Peri Base address: %x\n",BCM2708_PERI_BASE);
	printf("GPIO register address: %x\n", gpio.addr);
	printf("AUX register address: %x\n", auxp.addr);
	printf("AUX_ENABLE register address: %x\n", &AUX_ENABLES);

	// Print Aux Enable Register
	status = AUX_ENABLES & (3 << 0);
	printf("AUXILLARY ENABLED BITS: %i\n", status);

        // Check to see if AUX is enabled
        status = AUX_ENABLES & (AUX_ENABLES_MU);
        if(status)
                printf("The AUX MU is enabled\n");
        else
                printf("The AUX MU is not enabled\n");

        // Check to see if transmit interrupts will be generated
        status = AUX_MU_IER_REG & AUX_MU_IER_REG_TXD;
        if(status)
                printf("The interrupt line is asserted whenever the transmit FIFO is empty\n");
        else
                printf("No transmit interrupts are generated\n");

	// Check FIFO enable bits
	status = AUX_MU_IIR_REG & AUX_MU_IIR_REG_FIFO;
	printf("FIFO holds: %i\n", status);

	// Any AUX Interrupt test
	status = AUX_IRQ & ~(AUX_IRQ_MU);
	if(status)
		printf("The MINI UART has an interrupt\n");
	else
		printf("No MINI UART interrupt\n");

	// Check if interrupt is pending
	status = AUX_MU_IIR_REG & ~(1);
	if(!status)
	{
		printf("\n\nWe found an interrupt\n");
		printf("The interrupt id is: %i\n", (AUX_MU_IIR_REG & AUX_MU_IIR_REG_ID));
	}
	else
		printf("Still no interrupt\n");

        // Bits stored in the Transmitter FIFO
        status = AUX_MU_STAT_REG & (7 << 24);
        printf("The MINI UART Transmitter FIFO holds %i bits\n", status);

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

//	INP_GPIO(32);

	// Define (rxd) pin 10 as input
//	INP_GPIO(RXD_PIN);

	// Use alt function 5 on pin 8 and pin 10
	SET_GPIO_ALT(TXD_PIN, 5);
//	SET_GPIO_ALT(32, 5);
//	SET_GPIO_ALT(RXD_PIN, 5);

	// Enable mini UART
	AUX_ENABLES |= AUX_ENABLES_MU;
	//AUX_ENABLES |= 7;
	//AUX_ENABLES = 0;

	// DLAB
	AUX_MU_LCR_REG &= ~(AUX_MU_LCR_REG_LAB);

	// Try to reset enable transmit interrupt
	AUX_MU_IER_REG &= ~(1);

	// Reset Interrupt Pending bit
	AUX_MU_IIR_REG |= 1;

	// FIFO reset
	AUX_MU_IIR_REG |= AUX_MU_IIR_REG_FIFO;

	// TX LOW
//	AUX_MU_LCR_REG |= AUX_MU_LCR_REG_BK;

	// TX High
	AUX_MU_LCR_REG &= ~(AUX_MU_LCR_REG_BK);

	// UART WORKS IN 8-bit mode
	AUX_MU_LCR_REG |= AUX_MU_LCR_REG_DSZ;

	// No new symbols sent or recieved by transmitter or reciever
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_TXD);
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_RXD);

	// UART RXD line high
	//AUX_MU_MCR_REG &= ~(AUX_MU_MCR_REG_RTS);

	// Clear FIFO
	//AUX_MU_IER_REG |= (AUX_MU_IIR_REG_ID);

	// FIFO reset
	AUX_MU_IIR_REG &= ~(AUX_MU_IIR_REG_ID);

	// CTS LEVEL HIGH
	AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_CTS;

	// Ignore CTS
	AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_TXCTS);

	// Enable UART Transmissions
	//AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_TXD;
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
		//AUX_MU_CNTL_REG &= ~(AUX_MU_CNTL_REG_TXD);
		while(!(AUX_MU_LSR_REG & AUX_MU_LSR_REG_EMT));
		printf("Passed transmit interrupt");
		AUX_MU_IO_REG = txd_data[x];
		AUX_MU_CNTL_REG |= AUX_MU_CNTL_REG_TXD;
	}
}
