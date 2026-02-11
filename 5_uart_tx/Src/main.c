/* UART - SERIAL VS PARALLEL
 * parallel - 8 bit data is transferred at the same time
 * serial - 8 bit data is transferred one bit at a time.
 * --------------------------------------------------------------------------------------------------------------------
 * UART INTRODUCTION : Serial data communication uses two methods-
 * 1. Synchronous - Clock is transmitted with the data.
 * 2. Asynchronous - No clock is transmitted. Transmitter and receiver agree on the clock speed
 * for the data transmission(Baudrate).
 * Our module(STM Documentation) is actually UART-Universal Asynchronous Receiver/Transmitter.(When we want to use Asynchronous Mode we use UART.)
 *                                       OR USART-Universal Synchronous Asynchronous Receiver/Transmitter.(We can use it either in its synchronous mode or in its asynchronous mode).
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * UART - TRANSMISSION MODE
 * Duplex- Data can be transmitted and received.
 * Simplex - Data can be transmitted only or received only i.e. one direction only.
 * Half Duplex- Data can be transmitted in only one way at a time.
 * Full Duplex- Data can be transmitted in both ways at a time.
 * ------------------------------------------------------------------------------------------------------------
 * UART - PROTOCOL SUMMARY
 * In asynchronous transmission, each byte(character) is packed between start and stop bits..
 * --> Start bit is always 1 bit, the value of the start bit is always 0.
 * --> Stop bit can be 1 or 2 bits, the value of the stop bit is always 1.
 * Eg. Transmitting ASCII 'A' = 0100 001
 *           (STOP BIT 11)-->(11 0100 0001 0)<--(START BIT 0)
                             (^^^^^^^^^^^^^^) one frame
   ------------------------------------------------------------------------------------------------
   UART - CONFIGURATION PARAMETERS
   Baudrate- Connection speed expressed in bits per second.
   Stop Bit - number of stop bits transmitted, can be one or two.
   Parity- indicated the parity mode, whether odd or even. used for error checking.
   Mode- Specific whether RX or TX mode is enabled or disables.
   Word Length - Specifies the number of data bits transmitted or received. Can be 8-bits or 9-bits.
   Hardware Flow Control- Specifies whether Hardware Flow Control is enabled or disabled.
                             */
//____________________________________________THEORY LECTURE NOTES COMPLETE________________________________________________
#include "stdint.h" //because we are using uint32_t
#include "stm32f4xx.h"
/*We will be using USART2. It is connected to APB1. So we have to check the APB1 enable register to see where in the register the USART2 bit is.
 *In reference manual we see , RCC_APB1ENR has USART2 at pin 17. USART has a TX and RX line,these are GPIO pins.We have to find which GPIO pins
 *USART2 uses. We go to data sheet and search for alternate function mapping. We see that PA2 is the TX line of USART2 and PA3 is RX line of USART2.
 *If we want to use TX , we enable PA2 and then we need to configure it to be an alternate function mode, And the type of alternate function is AF07or usart2.
 *
 *When we check the column it's under AF07. */


#define GPIOAEN     (1U<<0)
#define USART2EN    (1U<<17)

#define SYS_FREQ     16000000 //default periph clock is 16MHz for STM32.
#define APB1_CLK      SYS_FREQ
#define UART_BAUDRATE  115200

#define CR1_TE          (1U<<3) //1 at bit three to enable transmitter.
#define CR1_UE          (1U<<13) //In control register 1 , in USART_CR1,bit no. 13 is UE, means UART enable .

#define SR_TXE            (1U<<7)

static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate);//function defination
static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate);//function definition
void uart2_tx_init(void); //function definition
void uart2_write(int ch);


int main(void){
	uart2_tx_init();//initialize uart


	while(1){
		uart2_write('Y');//write the character
/*to test this, we need a program on out PC, that's capable of reading the serial data and receiving it and display it.
 we are seeing this data on realterm application. Go to port and select correct baudrate and then clear and then select open
 You will see Y being printed.In the port tab the rxd button blinks which means we are recieving the data*/
	}

}

//this function configures for uart2
void uart2_tx_init(void){
	/*--------configure the usart gpio pin---------------*/
	/*  1. enable clock access to gpioa */
	RCC->AHB1ENR |= GPIOAEN;

	/* 2. set PA2 mode to alternate function mode.*/
	GPIOA->MODER &=~(1U<<4);//MODER, we are dealing with pin2 so we will use MODER2. It occupies bit 4 & 5 . To set this to alternate function , we need to set first bit to 0 second bit to 1.bit 4 =0, bit 5=1.
    GPIOA->MODER |=(1U<<5);

	/* 3. set PA2 alternate function type to USART_TX(AF07)*/
    GPIOA->AFR[0] |=(1U<<8);//STM32 header file , the structure doesn't provide us with AFRL & AFRH. It gives AFR & AFR is an array of size to the first index is for all AFRL & second index is for AFRH. We will use index 0 for AFRL. For AFRH use index 1.
	GPIOA->AFR[0] |=(1U<<9);//We have GPIO alternate function register low. AFRL0. All pin 0 use AFRL0, All pin 1 use AFRL1 & goes on. For AF7 , set 0111. Set first bit to 1, second bit to 1, third bit to 1, fourth bit to 0.(bit 8 to 11).
    GPIOA->AFR[0] |=(1U<<10);
    GPIOA->AFR[0] &=~(1U<<11);

	/*  -----------configure usart module-------------------*/
	/*  1. enable clock access to uart2*/
    RCC->APB1ENR |= USART2EN;//usart is connected to APB1 bus , use APB1ENR register.
	/*  2. configure the usart baudrate */
    usrt_set_baudrate(USART2,APB1_CLK, UART_BAUDRATE);

	/*  3. configure the transfer direction */
    //not putting any operator here , want to clean everything in the UART.In the UART CR1 control register one , just set bit no. three to 1, so everything else in the register will become 0 & bit no. three will become 1.
    USART2->CR1 = CR1_TE;//go to control register 1 and inspect which bit is used to enable the transmitter of UART.In USART_CR1 register bit no. three is TE. set bit three to 1 to enable transmitter.

	/*  4. enable usart module. */
    USART2->CR1 |= CR1_UE; //In control register 1 , in USART_CR1,bit no. 13 is UE, means UART enable. we don't want to clean everything , just set bit 13 to 1 that's why using the operator.
}



/*this function to be able to work with any UART module. first argument allows any UART module, second argument is peripheral clock.
 * last argument is desired baudrate.*/
static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate){
	USARTx->BRR = compute_usrt_bd(PeriphClk,Baudrate);//access BRR(Baudrate register)
}


/* To configure baudrate, there is an equation that is derives empirically. We are writing a function for that equation here.this function takes
 * the bus frequency(Periph_Clk, default system clock for STM32 is 16MHz), the baudrate that we want as the argument & it's going to compute the value and this value will be stored into our UART baud rate register. */
static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate){
	return ((PeriphClk + (Baudrate/2U))/Baudrate);

}

void uart2_write(int ch){ //pass an argument the value is int ch

	//1.  make sure the transmit data register is empty before we put something into it
/*how do we wait for transmit data register to be empty? To check for this we have to check the UART status register. In USART_SR register, bit no. 7is TXE.
 * TXE is transmit data register empty.sysmblic name for this bit is SR_TXE.here we read the bit and to read a bit, take the register and perform AND(&) operation.
 * with the bit you are interested in. */
	while(!(USART2->SR & SR_TXE)){};//This return true if this bit(SR_TXE) is set inside this register(SR). if this bit(SR) is not set it return false. here using while we are saying that while this returns false by doing so , so we'll get stuck here until it becomes true and then we can write our value into the data register.

	USART2->DR = (ch & 0xFF);  // DR is for data register. Because we want to transmit 8 bit , we do 0xFF, we perform AND operation with this
	//2. write to transmit data register.
}




