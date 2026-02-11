/*______________________________________________________________________DIRECT MEMORY ACCESS - DMA_____________________________________________________________________________________________

DMA - INTRODUCTION
1) The DMA allows data transfers to take place in the background without the intervention of the Cortex-M processor.
2) During this operation, the main processor can execute other tasks, and it is only interrupted when a whole data block is available for processing.
3) Large amounts of data can be transferred with no major impact on the system performance.

DMA - TRANSFER PROPERTIES
1) DMA stream and channel
2) Stream priority
3) Source and destination addresses
4) Transfer mode
5) The transfer size
6) Source/destination address to increment or non-incrementing
7) Source and destination data width
8) Transfer type
9) FIFO mode (First In - First Out)
10) Source/destination burst size
11) Double-buffer mode
12) Flow control

DMA - PORTS
1) The DMA has two ports, one peripheral port and one memory port.

DMA -TRANSFER PROPERTIES BRIEF DESCRIPTION
1) DMA - Streams/Channels for F2/F4/F7

a) There are two DMA modules in the ARM STM32 F2, F4, and F7.
b) Each module implements eight different streams. We have REQ_STREAM0 to all the way to REQ_STREAM7.We have two of these controllers. If you are using the ARM STM32 F4, F7, or F2
  then you would have two of DMA modules.Each module implements eight different streams.
c) Each stream is dedicated to managing memory access requests from one or more peripherals.
d) Each stream has up to eight selectable channels(requests) in total. So, we have eight streams here, stream 0 to stream 7. And there is a multiplexer connected to stream 0, another multiplexer connected to stream 1.
  And this multiplexer allows us to have eight different channels.  we have over here, REQ_STRx_CH0, channel 0 to REQ_STRx_CH7. And this is entering a single stream. So, stream 0 has eight channels. Stream 1 has its own
  eight channels, stream 2, eight channels, and so on and so forth.
e) Only one channel can be active at the same time in a stream. So, when we say we're using stream 0, we cannot say we want to use stream 0 channel 1 and channel 3 at the same time. A single channel has to be active in a single stream.
f) DMA - Request Mapping (F4 DMA 1)
  How does all of this apply to our peripherals? Refer to table STM32F427/437 and STM32F429/429 DMA1 request mapping in the reference manual.
  So for instance, if we want our SPI Module 3 to receive data in DMA mode, what we would have to do is configure DMA Stream 0 Channel 0. As you can in the table, SPI3_RX says Stream 0 Channel 0. So we would have to configure DMA Stream 0 Channel 0,
  it is connected to the SPI internally. If we are using, the I2C Module, I2C Module 1, and we want to receive data from I2C Module 1 in DMA mode, I2C1_RX, what we have to configure is DMA Stream 0 Channel 1. In the same way, if we want a timer to
  trigger something or to do something after its timeout, and we want this event to occur in DMA mode, we want to use the DMA to accomplish this. Rather than having it in its normal mode, we would have to configure DMA Stream 0 Channel 2 for TIM4_CH1.
  All of our peripherals that have DMA capability, when you visit this table, you would see which DMA stream and which channel needs to be configured.This table exists in our reference module. And all of that brings us to what we just said over here.
  The DMA has eight streams. Each stream has eight different channels. And that is what we see over here. Stream 0 all the way to Stream 7. Channel 0 all the way to Channel 7.

2) DMA - Stream Priority
a) The DMA has an Arbiter for handling the priority between the DMA streams.
b) The priority between the DMA streams can be configured by software.
c) There are four priority levels.
d) If two or more DMA streams have the same software priority level, then the hardware priority is used and in the hardware priority, Stream 0 has priority over Stream 1. Stream 1 has priority over Stream 2. Stream 2 has priority over Stream 3 etc.
 So the stream with a smaller number has the highest priority.

 3) DMA - Source and Destination addresses
 a) A DMA transfer is defined by a source address and a destination address.
 b) Both the source and the destination should be in the AHB or the APB memory range.We have our memory port and then our peripheral port, and they are connected to the AHB matrix over here

4) DMA - Transfer Modes
DMA is capable of performing three different transfer modes:
a) We can transfer from peripheral to memory
b) from memory to peripheral or
c) from memory to memory. NOTE- memory to memory transfer is done only with DMA2 module.It's only DMA2 module that allows us to do memory to memory transfer.

5) DMA - Transfer Size
a) The transfer size defines the volume of data to be transferred from source to destination.
b) We only define this when the DMA is the flow controller. And we shall see this when we are configuring our DMA.

6) DMA - Incrementing Addresses
a) DMA can automatically increment source and destination addresses after each transfer. the source or the destination address automatically. If you are collecting data from a peripheral and storing it in an array where the array is the destination,
 you would want to increment the array address. You want to move, if you have already placed your piece of data in index zero, you'd want to automatically store the next piece of data at index one. So you would want the DMA to do this increment for you.
 And the DMA allows us to have this option whether both or either of the source or destination addresses need to be incremented. We would have the opportunity to make that selection during the configuration.

7) DMA -Source and Destination Data Width
a) Byte - 8 bits
b) Half-word - 16 bits
c) Word - 32 bits

8) DMA -Transfer Types
a) Circular mode: Circular mode is for handling circular buffers in continuous data flow.If you have an ADC and you are collecting the sensor values using DMA, and you want to continuously collect the sensor values, it would make sense to configure
  the DMA in circular mode. In the DMA circular mode, the DMA stream number of data register, which is DMA_SxNDTR, this register is then reloaded automatically with the previously programmed value. This is how we get the circular mode to work.
b) Normal mode: In normal mode, this register reaches zero and the stream is disabled.Once the DMA_SxNDTR register reaches zero, the stream is disabled.

9) DMA -FIFO Mode in F2/F4/F7
a) Each stream has an independent 4-word(4*32 bits) FIFO.4-word means is four times 32-bit as word means 32-bit.
b) The FIFO is used to store the data coming from the source before transmitting to the destination for a short period of time.
c) If the DMA FIFO is enabled, data packing/unpacking and/or burst mode can be used. It basically allows us a number of options. We can decide to have data packing and unpacking, or we can decide to use the DMA in burst mode.
d) The configured DMA FIFO threshold defines the DMA memory port request time. So we can configure a threshold for the FIFO such that we say if the FIFO is half full, tell us. If it is quarter full, tell us. If it is three-fourth full, tell us.
e) Benefits of FIFO mode - 1) reduces the amount of time we access the SRAM, and this gives more time for other masters to access the Bus matrix without additional concurrency.
                           2) The FIFO allows software to do burst transactions, and this would optimize the transfer bandwidth.
                           3) The FIFO allows packing and unpacking data to adapt source and destination data width with no extra DMA cost or no extra DMA access.

________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________*/
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include <uart.h>

//enabling LED
#define GPIOAEN     (1U<<0)
#define GPIOA_5     (1U<<5)
#define LED_PIN     GPIOA_5

static void dma_callback(void);
int main(void)
{
	char message[31] = "Hello from STM32 DMA transfer\n\r"; //this is our source buffer, we will be transferring this data , this string using DMA
	RCC->AHB1ENR |=GPIOAEN; //enable clock access to GPIOA

	//set PA5 as output pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

    uart2_tx_init();

    /*source buffer here is going to be an array. Our destination is DMA data register, Our destination is the USART2 data register.The length of the source is 31. */
    dma1_stream6_init((uint32_t) message, (uint32_t)&USART2->DR, 31 );  //void dma1_stream6_init(uint32_t src, uint32_dst, uint32_t len);

    while(1){}
}
// In our callback function, we want to read the USAT data register.
static void dma_callback(void)
{
	GPIOA->ODR |= LED_PIN;/*Turn ON the LED when the transfer is complete*/
}

/*each DMA stream has its own interrupt request handler because we are using stream 6 and it's part of DMA module 1, we say DMA1_Stream6_IRQHandle. In here there are number
 of interrupt requests, we have the transfer complete interrupt, half complete interrupt, direct mode error interrupt, error interrupt, etc, but we enabled just a single interrupt,
 we enabled the transfer complete interrupt, so we've got to check, to be specific.We can just say,when any interrupt occurs, run whatever we put here, we want to make this properly
 by checking if it is indeed the transfer complete interrupt that has occurred, if that's the case, then we want to run a particular piece of code.To do this, we need to check our DMA1 high status register and check for transfer complete bit.
 Search for DMA_HISR in reference manual.It's called high because it's for stream 4 and above. The low is for starting from stream 0 to 3.We are interested in transfer complete interrupt for stream 6, bit 21 is TCIF6, created a symbolic name HISR_TCIF6.
 Because we want to access this in our main function, we have put put this one in our uart.h file.Then we want to clear the flag. Remember, we clear the interrupt flag and then we can do something .To clear the flag, we need to go to a separate register.
 This is known as the HIFC register.In DMA_HIFCR , bit 21 is clear transfer complete interrupt flag, writing 1 to this bit clears the corresponding TCIFx flag in the DMA_HISR register */
void DMA1_Stream6_IRQHandler(void)
{
	/*check for transfer complete - we do this by using if statement. Remember when we check, we use if,when we want to wait we use while.Here we want to read, so to read a bit, we perform an AND operation. */
	if(DMA1->HISR & HISR_TCIF6)
	{
		DMA1->HIFCR |=HIFCR_CTCIF6; /*clear flag*/
	}
	/*Do something.*/
	dma_callback();
}
/*Go to project folder, right click and select run as. Go to realTerm and select correct baudrate(115200) and port.
  you will see "Hello from STM32 DMA transfer" and the LED will glow only once because we have not set a loop */








