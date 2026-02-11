#include "uart.h"

#include "stm32f4xx.h"


#define GPIOAEN            (1U<<0)
#define USART2EN           (1U<<17)

#define SYS_FREQ           16000000
#define APB1_CLK           SYS_FREQ
#define UART_BAUDRATE      115200

#define CR1_TE            (1U<<3)
#define CR1_UE            (1U<<13)
#define SR_TXE            (1U<<7)


static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate);
static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate);

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

static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate){
	USARTx->BRR = compute_usrt_bd(PeriphClk,Baudrate);
}

static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate){
	return ((PeriphClk + (Baudrate/2U))/Baudrate);

}

void uart2_write(int ch){

	while(!(USART2->SR & SR_TXE)){};

	USART2->DR = (ch & 0xFF);
}


