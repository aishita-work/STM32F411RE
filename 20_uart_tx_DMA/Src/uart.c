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

#define CR1_RXNEIE        (1U<<5)

#define DMA1EN                       (1U<<21)
#define CHSEL4                       (1U<<27)
#define DMA_MEM_INC                  (1U<<10)
#define DMA_DIR_MEM_TO_PERIPH        (1U<<6)
#define DMA_CR_TCIE                  (1U<<4)
#define DMA_CR_EN                    (1U<<0)
#define UART_CR3_DMAT                (1U<<7)

static void usrt_set_baudrate(USART_TypeDef *USARTx,uint32_t PeriphClk,uint32_t Baudrate);
static uint16_t compute_usrt_bd(uint32_t PeriphClk, uint32_t Baudrate);
void uart2_tx_init(void);
void uart2_write(int ch);

int __io_putchar(int ch){ //to use printf we need to implement to put character function,this takes ch character argument
	uart2_write(ch); //inside we call uart2_write function and pass the argument ch and return ch
	return ch;
}
/*Let's see which DMA stream and which DMA module our UART TX line is connected to.Open reference manual , go to the list of tables, over here, we have DMA1 request mapping and DMA2 request mapping.
 We click DMA 1 request mapping. In table, DMA1 request mapping (STM32F411xC/E).The columns indicate the DMA streams. The rows indicate the channel .here in stream 6, we have USART2_TX.
 Stream 6, we're going to enable DMA 1 and channel 4 of stream 6 uses this feature.*/

/*This function here is going to take a number of arguments.We going to pass the source, the destination, and the length of the data we want to transfer */
void dma1_stream6_init(uint32_t src,uint32_t dst,uint32_t len)
{
	/*enable clock access to DMA - If we go to our block diagram in the datasheet, we realize that the DMA module 1 is connected to the AHB1 bus.Go to reference manual and search AHB1ENR.
	 We find bit 21 is called DMA 1 enable. */
	RCC->AHB1ENR |=DMA1EN;

	/*disable DMA1 and stream 6- To do this, we have to go to the configuration register of the particular stream.Go to reference manual and go to DMA section.Scroll down to DMA registers.It's called
	DMA_SxCR,here S stands for stream and x stands for the stream number.Bit number 0 over here is the EN, meaning enable bit,we need to disable, essentially set to 0 in order to disable the particular DMA stream.
	Created a symbolic name. The name of the register is configuration register, but the name of the structure for the DMA stream 6 is written as DMA1_nameofthestream and then we access the configuration register. */
	DMA1_Stream6->CR &=~DMA_CR_EN; //DMA1_Stream 6, this structure is exist in our .h file.

	/*	Wait until DMA1 Stream6 is disabled*/
	while(DMA1_Stream6->CR &= DMA_CR_EN){}

	/*clear all the interrupted flags for stream6-go to the DMA1 HIFCR (High Interrupt Flag Clear Register) register. We're looking for stream 6. Let's read what the description says. It says when we see CTCIFx
	 this is stream x, clear transfer complete interrupt flag. So there are a number of interrupt flags for each stream. There is the transfer complete, there is the half transfer, meaning if the transfer is half
	 complete the interrupt can be generated.There is error interrupt. So we said we're going to clear all stream 6 interrupts. we want to clear all the interrupts for stream 6 we said.Bit 16, bit 18, bit 19, bit 20,
	 bit 21.We'll  clear all these bits.Writing 1 to these bits clears the corresponding flag in the register. */
	DMA1->HIFCR |=(1U<<16);
	DMA1->HIFCR |=(1U<<18);
	DMA1->HIFCR |=(1U<<19);
	DMA1->HIFCR |=(1U<<20);
	DMA1->HIFCR |=(1U<<21);

	/*set the destination buffer - go to the PAR register, and simply assign our destination to it.*/
	DMA1_Stream6->PAR = dst;

	/*set the source buffer - the source is the memory source. So we go to the M0AR register, and we can find this in DMA registers. This is where we put a peripheral address, PAR is the peripheral because we
	 want our destination to be the UART peripheral that's why we're setting our destination address at PAR, and then our source is going to be memory. */
	DMA1_Stream6->M0AR=src;

	/*set the length = NDTR register here holds the length of the transfer*/
	DMA1_Stream6->NDTR =len;

	/*select Stream6 CH4 -  go to the DMA Stream configuration Register the same register that we analyzed earlier,DMA_SxCR.The channel select bit is CHSEL its bits 25,26,27.I f we want to select Channel 4, these are the values.
	 The first bit has to be 0,second bit has to be 0, third bit, which is bit 27, has to be set to 1. We have created a symbolic name.*/
	DMA1_Stream6->CR = CHSEL4;  // not using the OR operator because we want to clear the register

	/*Enable memory to increment-we have the option to see whether the memory should increment or not or whether the peripheral should increment, or whether both of them should increment.and we want the memory to increment,
	 because the data is stored in the buffer. When you access the first index of the buffer, you want to increment to the next index.we're going to access the same register to enable memory increment.Go to the configuration register
	 DMA_SxCR, we have MINC which stands for memory increment.Bit 10 is memory increment mode. */
	DMA1_Stream6->CR |= DMA_MEM_INC; //this time we will use OR operator here because we don't want to clean CHSEL4,we want to add this other parameter to what is already set in our stream 6 configuration register.

	/*configure transfer direction - Whether the data is moving from memory to a peripheral, or from peripheral to a memory. In this case, the data will be moving from memory to a peripheral.And what this means is that the data
	 is stored in an array and we're going to take the content of this array and move it to the UART peripheral using DMA. So the direction here is memory to peripheral. Go to configuration register DMA_SxCR, bit 6,7, together we call it a
	 DIR, meaning direction.We want to set bit 6 to 1 and 7 to 0(which is by default ) for our Memory-to-peripheral.We have created a symbolic name.  */
	DMA1_Stream6->CR |= DMA_DIR_MEM_TO_PERIPH;

	/*enable DMA transfer complete interrupt - here, we're going to enable at least one interrupt. Remember, we cleared all of  interrupt flags.We're going to just make use of one.We're going to use the transfer complete interrupt,TCIE.
	which is bit 4. Whenever you see a bit name ending with IE, it means interrupt enable.Createdd a symbolic name to set bit 4 to 1.*/
	DMA1_Stream6->CR |= DMA_CR_TCIE;
	/*enable direct mode, disable the FIFO mode - go to the FIFO control register in DMA Register section.We're going to write zero to this entire register because when we write zero it says FEIE interrupt disabled.
	 We disable FTH and FS to zero.We say we are enabling direct mode. So when we set DMDIS, direct mode disable to zero we'll get direct mode enabled as well. We'l lwrite zero to this register.
	And in effect we'll be disabling the FIFO and enabling the direct mode. */
	DMA1_Stream6->FCR =0;

	/*enable DMA1 stream 6 - go to the DMA Configuration Register.Bit 0 here is EN. */
	DMA1_Stream6->CR |= DMA_CR_EN;

	/*enable UART2 transmitter DMA - We're going to enable UART Transmitter DMA over here. Go to UART module. This time UART Control Register 3 to connect it to the DMA. Search USART_CR3.Bit 7 here is DMAT, DMA Transmitter.*/
	USART2->CR3 |= UART_CR3_DMAT;

	/*DMA interrupt enable in NVIC - after the transfer is complete, we can decide to run a piece of code.We can run a piece of code in the middle of the transfer or when it's complete, we can say if the transfer
	 is half complete, run this piece of code. If it is one-third complete, run this other piece of code etc.We use NVIC interrupt IRQ and we use DMA1_Stream6_IRQn. That's the interrupt request number for DMA 1 stream 6. */
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
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
/*To enable interrupt, we need to go to the UART control register ,go to reference manual and  search USART_CR1.
 Bit number 5 is called RXNEIE interrupt enable(IE implies interrupt enable). If this is 0, interrupt is blocked,it is not enabled.
 If it is 1, it is enabled. We will create a symbolic name called CR1_RXNEIE*/
void uart2_rxtx_interrupt_init(void){

	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER &=~(1U<<4);
    GPIOA->MODER |=(1U<<5);
    //PA2 configured for tx
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

    /*Enable RXNE Interrupt: We enabled transmitter and receiver above. After that, we're going to enable receiver or RXNE interrupt*/
    USART2->CR1 |= CR1_RXNEIE;
    /*Enable UART2 interrupt in NVIC: After you've enabled the interrupt within its register, we've to enable the interrupt inside the NVIC. */
    NVIC_EnableIRQ(USART2_IRQn);

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

