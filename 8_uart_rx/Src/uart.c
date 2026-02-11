#include "uart.h"

#include "stm32f4xx.h"


#define GPIOAEN            (1U<<0)
#define USART2EN           (1U<<17)

#define SYS_FREQ           16000000
#define APB1_CLK           SYS_FREQ
#define UART_BAUDRATE      115200

#define CR1_TE            (1U<<3)
#define CR1_RE            (1U<<2)
#define CR1_UE            (1U<<13)
#define SR_TXE            (1U<<7)
#define SR_RXNE           (1U<<5)

static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate);
static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate);
void uart2_tx_init(void);
void uart2_write(int ch);

int __io_putchar(int ch){ //to use printf we need to implement to put character function,this takes ch character argument
	uart2_write(ch); //inside we call uart2_write function and pass the argument ch and return ch
	return ch;
}

void uart2_tx_init(void){

	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER &=~(1U<<4);
    GPIOA->MODER |=(1U<<5);
    GPIOA->AFR[0] |=(1U<<8);
	GPIOA->AFR[0] |=(1U<<9);
    GPIOA->AFR[0] |=(1U<<10);
    GPIOA->AFR[0] &=~(1U<<11);

    RCC->APB1ENR |= USART2EN;

    usrt_set_baudrate(USART2,APB1_CLK, UART_BAUDRATE);

    USART2->CR1 = CR1_TE;

    USART2->CR1 |= CR1_UE;
}
void uart2_rxtx_init(void){

	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER &=~(1U<<4);
    GPIOA->MODER |=(1U<<5);
    GPIOA->AFR[0] |=(1U<<8);
	GPIOA->AFR[0] |=(1U<<9);
    GPIOA->AFR[0] |=(1U<<10);
    GPIOA->AFR[0] &=~(1U<<11);

    /*set PA3 mode to alternate function mode for RX to work(PA2 was used for TX purpose)
     PA3 is MODER3, therefore we set alternate function for MODER3 i.e. bit 6 and 7*/
    GPIOA->MODER &=~(1U<<6);
    GPIOA->MODER |= (1U<<7);
    /*set PA3 alternate function type to UART_RX (AF07), so we are looking for AFRL3
     In AFRL register for pin 3 , we deal with bits 12, 13, 14, 15*/
    GPIOA->AFR[0] |=(1U<<12);
    GPIOA->AFR[0] |=(1U<<13);
    GPIOA->AFR[0] |=(1U<<14);
    GPIOA->AFR[0] &=~(1U<<15);


    RCC->APB1ENR |= USART2EN;

    usrt_set_baudrate(USART2,APB1_CLK, UART_BAUDRATE);

/*we need to add enable receiver over here also for RX to work.In USART_CR1 , bit no.
 *  2 is RE, means receiver. Created symbolic name above.*/
    USART2->CR1 = CR1_TE | CR1_RE; // using OR operator to include RE


    USART2->CR1 |= CR1_UE;
}

static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate){
	USARTx->BRR = compute_usrt_bd(PeriphClk,Baudrate);
}

static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate){
	return ((PeriphClk + (Baudrate/2U))/Baudrate);

}

/*A function to allow us to be able to read data from the UART*/
char uart2_read(void){ //char is used because this function returns what it reads and takes no argument.

/*make sure the receive data register is NOT empty.This is known as the RXNE flag. USART_SR(status register) RXNE flag is bit 5, it tells us to read data register.
we have to make sure this register is NOT empty. If this is empty there is nothing to read.Created a symbolic name for this RXNE.*/
	while(!(USART2->SR & SR_RXNE)){}

	return USART2->DR; //To read data we return content of the data register.
}
void uart2_write(int ch){

	while(!(USART2->SR & SR_TXE)){}

	USART2->DR = (ch & 0xFF);
}


