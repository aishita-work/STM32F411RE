#include "SPI.h"


#define  SPI1EN       (1U<<21)
#define  GPIOAEN      (1U<<0)
#define  SR_TXE	  	  (1U<<1)
#define  SR_BSY       (1U<<7)
#define  SR_RXNE      (1U<<0)
/*We're going to have a function to enable the SPI. Let's go to the block diagram to see how many SPI modules we have and then we'll nominate one of them to be used.Go to the datasheetsdocument , go to block diagram, we've got SPI1, SPI2, SPI3,SPI1, SPI5.So we have about 5 SPI modules.
 So let's nominate SPI1. We're going to use SPI1 for this driver.SPI1 is connected to the APB2 bus. Now, we need to find out which bit in the APB2 enable register is for SPI1.Go to the reference module document  and search APB2ENR, bits number 12 here is for SPI1. So we're going to create
 a symbolic name to hold this, SPI1EN, bits 12.Now, the SPI requires pins, because we need the MISO pin, the MOSI pin, the slave select pin(SS), the clock pin(SCK). So we have to find out the GPIO pins that are connected, to find this out, we need to check the alternate function mapping,
 which can be found in the datasheet document.Search alternate function mapping here.Over here, look for SPI1. We find SPI1_MOSI is PA7, SPI1_MISO is PA6, SPI1_SCK is PA5 and for our slave select line, we can use any pin at all, we're going to use PA9 for that.Let's see the pinout.

adxl345---------------------STM32(microcontroller)'
SCL--------------------------PA5(CLK)
SDA--------------------------PA7(MOSI)
SDO--------------------------PA6(MISO)
CS---------------------------PA9(SS)
VCC---------------------------5V
GND--------------------------GND
 Furthar,we realize that the SPI pins are part of GPIO Port A then we need to enable clock access to GPIO A as well. So we need to create a simple nickname, GPIOAEN. We know that it's in the AHPB1 enable register pin series for GPIO Port A(AHB1ENR). So we're going to start off by enabling the clock
 to GPIO A, then we want to configure PA5, PA6, PA7 to alternate function mode. So to do this, we need to go to the moder register. So PA5 would occupy  bits 10 and 11 in the moder register. And to set the particular pin to alternate function mode, the first bit has to be 0, the second bit
 has to be 1. PA6 would occupy bits 12 and 13,The first bit has to be 0, the second bit has to be 1. PA7 would occupy bits 14 and 15, The first bit has to be 0, the second bit has to be 1.PA9, the slave select pin, we're going to set this as an output pin. Because this pin we need to be
 setting it high and low to select the slave device. So PA9 is going to occupy bits 18 and 19 in the moder register and for output , first bit has to be 1 , the second bit has to be 0.Next we need to set PA5, PA6, and PA7 alternate function type. We're going to set the alternate function type to SPI1.
 In the The alternate function type here is AF05. This is the SPI1 alternate function type. We need to set our three pins to AF5 (0101). So to do that, PA5, PA6, PA7, they are all in the alternate function load register.We need to go to that register and configure this. go to the reference module and search
 GPIOx_AFRL.So PA5 will start from AFRL5 . So this will occupy bits 20 to 23. We want to set this to AF5. The first bit has to be 1. The second bit 0. The third 1. The fourth 0. So this will be 0101. Next, PA6 is AFRL6. PA6 will start from bit 24 to 27,0101.PA7 will start from bits 28 to 31, 0101.*/
void spi_gpio_init(void){
	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA

	/*Set PA5,PA6,PA7 to alternate function*/
	GPIOA->MODER &=~(1U<<10);  //PA5
	GPIOA->MODER |=(1U<<11); //PA5

	GPIOA->MODER &=~(1U<<12);  //PA6
	GPIOA->MODER |=(1U<<13); //PA6

	GPIOA->MODER &=~(1U<<14);  //PA7
	GPIOA->MODER |=(1U<<15); //PA7

	GPIOA->MODER |=(1U<<18);  //set PA9 as output pin
	GPIOA->MODER &=~(1U<<19); //set PA9 as output pin

	/*set PA5,PA6,PA7 alternate function type to SPI1*/
	/*PA5*/
	GPIOA->AFR[0] |=(1U<<20);
	GPIOA->AFR[0] &=~(1U<<21);
	GPIOA->AFR[0] |=(1U<<22);
	GPIOA->AFR[0] &=~(1U<<23);

	/*PA6*/
	GPIOA->AFR[0] |=(1U<<24);
	GPIOA->AFR[0] &=~(1U<<25);
	GPIOA->AFR[0] |=(1U<<26);
	GPIOA->AFR[0] &=~(1U<<27);

	/*PA7*/
	GPIOA->AFR[0] |=(1U<<28);
	GPIOA->AFR[0] &=~(1U<<29);
	GPIOA->AFR[0] |=(1U<<30);
	GPIOA->AFR[0] &=~(1U<<31);
}

/*We are going to implement the SPI config function. This is the function that would configure the SPI module,it takes no argument. We start off by enabling clock access we're going to use SPI1_EN to enable clock access to the SPI1 module. We'll do that by setting the  bit 12 in the APB2 enable register to 1.
 Once that is done, we're going to set the clock of the SPI module. To do that, we need to analyze the SPI control register 1.Go to reference module and search SPI_CR1.So bits 3, 4, 5 here are known as the BR, we use this to configure the baud rate.So we simply want to put a value that we want to divide the
 SPI clock by. Remember, the default frequency of our microcontroller is 16 MHz without changing anything. The default frequency of the STM32F411 microcontroller is 16 megahertz. And this is the same frequency that goes through various buses without any pre-scaler. So then this means the frequency of the APB2
 bus, which the SPI is connected to, is also 16 megahertz. So what we're going to do is we're going to slow that down a bit. We're going to divide the clock by, say, 4. We're going to divide it by 4 over here.So I'm going to set PCLK, PCLK stands for peripheral clock. And the peripheral clock is basically the
 clock of the bus that the peripheral is connected to. In our case, it's the APB2 bus, which is 16 megahertz. We're going to set 16 megahertz divided by 4 here. And to do that, we have to set the first bit to 1, the second bit to 0, the third bit to 0, that is 001.Next we're going to set the clock polarity to
 1 and the clock phase to 1.To do that, we need to check the SPI Control Register 1. So there is the CPOL and CPHA. Okay.We can simply do that by setting bit 0 to 1 and bit 1 to 1 as well.Next, we're going to enable full duplex so let's check our Control Register 1 and find the bit that is responsible for that.
 Bit number 10,it's called the RXONLY bit. It says when this is 0, full duplex is enabled. When it is 1, it is in receive-only mode, we set it to 0.Next, we're going to set the MSB to be first.In the Control Register 1 bit number 7 is called the LSB first, if this is set to 0, MSB is first and if this is set to
 1, LSB is first. So we're going to set it to 0. So we simply disable bit number 7.Next, we're going to set the SPI to be in master mode. Because our microcontroller is the master and the Accelerometer device is the slave.Bit number two is called master in control register 1.It says if it is zero, this is slave
 configuration. If it is one, it is master configuration,so set it to one.Next we're going to set the data size, whether we want eight bit or 16 bit. Bit 11 here is called a DFF. This is the data frame.If we want eight bit mode, we've got to set this to zero. If we want 16 bit mode, we've got to set this to one.
 So we said we want eight bit mode, so we want to set this to zero. So we disable bit 11. Next we're going to configure our SPI to work in our software slave management, and this implies that we have to set the SS,SSI bit to one, and then SSM bit to one as well.We find these bits in the control register.
 SSM bit is bit number nine. SSI bit is number eight.SSM is the software slave management,when zero software slave management is disabled, when one software slave management is enabled and SSI here is internal slave select. This bit has an effect only when SSM bit is set. The value of this bit is forced onto the NSS,
 and the IO value of the NSS pin is ignored. So we're going to set SSM to one, and set internal slave select.Next we're going to enable the SPI module. See the SPI control register , here bit number six is SPE, meaning SPI enable, it says this disables the peripheral when it's zero. When it's one, the peripheral
is enabled. So we go to set bit number six to one to enable the SPI. */

void spi1_config(void){
	RCC->APB2ENR |= SPI1EN; //enable clock access to SPI1 module

	/*Set clock to fPCLK/4*/
	SPI1->CR1 |= (1U<<3);
	SPI1->CR1 &=~(1U<<4);
	SPI1->CR1 &=~(1U<<5);

	/*Set CPOL to 1 and CPHA to 1*/
	SPI1->CR1 |=(1U<<0);
	SPI1->CR1 |=(1U<<1);

	SPI1->CR1 &=~(1U<<10);  //Enable full duplex

	SPI1->CR1 &=~(1U<<7);  //Set MSB first

	SPI1->CR1 |=(1U<<2);  //set mode to MASTER

	SPI1->CR1 &=~(1U<<11);   //set 8 bit data mode

	SPI1->CR1 |=(1U<<8); //select software slave management by setting SSM=1 and SSI=1
	SPI1->CR1 |=(1U<<9);

	SPI1->CR1 |=(1U<<6);  //enable SPI module
}

/*This is the SPI transmit function. This function is going to take two arguments. It's going to take a pointer to the buffer that has the data we want to transmit and then the second argument is going to be the size of the data we want to transmit.We're going to start off by creating two local variables, we're going
 to have temp over here because we are required to read the data register to clear some bits,so we'll be reading into this, and then this will be this for a while,we shall have here, so because the data size, because it's going to be multiple data items, we need a loop to be able to transmit all of them. We say while
 i is less than the size passed as argument.,so, we want to wait until the TXE bit is set in the SPI. And what I mean by that is, we want to wait until the transmit buffer is empty. When the transmit buffer is empty, this indicates that there is space to add your new piece of data that you want to send. So TXE is a
 flag, and we find this in the status register ,SPI_SR. Bits number one is the TXE. Create a symbolic name, SR_TXE. We'll say while i is less than the size passed as argument. And what I mean by that is, we want to wait until TXE is set in the SPI. We know we wait to use the while loop.So, to read a bit, we perform
 the AND operation with a bit of interest. Okay, so we said if we read this bit and it's not set, meaning if it is false,we put exclamation sign,so if this is false, we want to get stuck here.Now if there is space, then we want to write the data to the data register.Will do that by saying SPI module one, data register
 equals data item index i and we're going to increment the index because this is going to be a pointer to a buffer.An array that has all of the data we want to transmit.Once we are done here, there's a few housekeeping we need to take care of. We would have to wait for the TXE to be set again to indicate we've transmitted
 what we want to transmit and there is nothing else to transmit. We also have to wait for the busy flag to be reset to indicate it's not busy, so wait for it.In Status register, bit number 7 is busy bit.So here, what we want to do is wait for the busy flag to reset. While the busy flag is 1, while it is 1, we want to get
 stuck here. When it becomes 0, then we want to move. So this is important. There is no exclamation sign here. We want to wait while it is high. When it goes low, then we come out of the loop. Next, we're going to clear the overflow flag by reading the data register and the status register. Go to page 599 of the reference
 manual or section 20.4.8 Error Flags, it tells us about the overrun flag(OVR). It says, this flag is set when data is received and the previous data have not yet been read.What we are interested in is how it is cleared,this is some strange way of clearing it. It says, clearing the OVR bit is done by a read operation on the
 SPI_DR register, followed by a read access to the SPI_SR register. So like we said, we agreed to read the data register and the status register, and that is how we clear the overrun flag.*/
void spi1_transmit(uint8_t *data, uint32_t size){

	uint32_t i=0;
	uint8_t temp;

	while(i<size){
		while(!(SPI1->SR & (SR_TXE))){}  //wait until TXE is set

		SPI1->DR = data[i]; //write the data to the data register
		i++;
	}
	while(!(SPI1->SR & (SR_TXE))){}  //wait until TXE is set
	while((SPI1->SR & (SR_BSY))){}  //wait for BUSY flag to reset

	/*clear OVR flag*/
	temp = SPI1->DR;
	temp = SPI1->SR;

}
/*This is SPI receive function. This function is going to take two arguments, a pointer to a buffer to store the received data and the size of what we want to receive. So we're going to start
 off by saying, while size exists, meaning while it is not equal to zero. The reason is going to, we can say this is we're going to decrement the size. So we start off by saying while size exists,
 we want to send the dummy data. We are advised to receive or first send dummy data. So, send dummy data and then to receive, we've got to wait for the RXNE. RXNE stands for the receive buffer.
 This implies there is something to be received. So we have to wait for the RXNE flag to be set. Go to reference manual to find the RXNE. Bit zero in the status register(SPI_SR) of the SPI is for
 RXNE. Create a symbolic name, SR_RXNE Bit .So we have to wait for this to be set. Once it is set, it implies,  there is new data in the receive buffer that we can consume.Now, read data from data
 register and we want to store the data here. This is the pointer to our storage buffer. We want to increment as well and then we want to decrement the size .*/
void spi1_receive(uint8_t *data, uint32_t size){
	while(size){
		SPI1->DR = 0;  //send dummy data
		while(!(SPI1->SR & (SR_RXNE))){} //wait for RXNE flag to be set
		*data++ = (SPI1->DR);  //read data from data register
		size--;
	}
}

/* We create two functions to select this slave and to deselect it. We call first one CS_nable, the CS line. This function will help us enable the CS line. To enable the CS line, what we want to do is
 set the pin low. We want to set it to zero. So then we do this by accessing GPIOA output data register, bit 9 because we're dealing with PA9. The slave select line is PA9.We set it to zero to enable it.
  Next is cs_disable, if we want to disable the CS line, we have to pull it high such that CS disable is rather setting bit 9 to one.
 */
void cs_enable(void){
	GPIOA->ODR &=~(1U<<9);
}
/*pull high to disable*/
void cs_disable(void){
	GPIOA->ODR |=(1U<<9);
}
