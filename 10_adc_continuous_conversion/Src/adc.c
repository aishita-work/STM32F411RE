/* Go to datasheet document. Go to block diagramWe have ADC1 (only one in our microcontroller). It's got 16 channels and it is connected to APB2 bus.
 We have to go to APB2 bus to enable register.Go to reference manual, go to APB2ENR register and look for which bit gives us ADC. Bit 8 enables ADC.
 ADC is a module and it has channels, these channels, GPIO pins that needs to be configured in the analog mode. So we need to find which pin are connected to our ADC module,
 so that we know where is channel1 , channel2, so that we know where to connect our  analog sensor essentially.Go to ADC in datasheet. it says,one 12-bit analog-to-digital converter
is embedded and shares up to 16 external channels,performing conversions in the single-shot or scan mode. In scan mode, automatic conversion is performed on a selected group of analog inputs.
We can sample from internal temperature sensor inside the chip i.e. from ADC input channel 18.In table 8. STM32F411xC/xE pin definitions. In additional function column, there is ADC1_10 (ADC channel 10)
which is at PC0. ADC1_11 is PC2, ADC1_12 is PC3, ADC1_1 IS PA1, ADC1_0 is PA0, PA3 is ADC1_3. We will use ADC channel 1(ADC1_1) , PA1 as our analog input pin.
 */
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "adc.h"
#define ADC1EN         (1U<<8)   //symbolic name for ADC enable
#define GPIOAEN        (1U<<0)
#define ADC1EN         (1U<<8)

#define ADC_CH1        (1U<<0)  //0B 0000... 0001
#define ADC_SEQ_LEN_1   0x00

#define CR2_ADON        (1U<<0)
#define CR2_SWSTART     (1U<<30)

#define SR_EOC          (1U<<1)

#define CR2_CONT         (1U<<1) //for continuous conversion

void pa1_adc_init(void){

	/*1.********Configure ADC GPIO pin********/
	/*enable clock access to ADC pin's port i.e. PA1(GPIOA)*/
	RCC->AHB1ENR |= GPIOAEN;

	/*set the mode of PA1 to analog mode in mode register.*/
	GPIOA->MODER |=(1U<<2);//for PA1 we use MODER1. for analog mode both bit should be 11. bit 2 and 3 will be 1.
	GPIOA->MODER |=(1U<<3);

	/*2.*************Configure ADC module*****************/
	/*enable clock access to ADC */
	RCC->APB2ENR |= ADC1EN;//ADC is connected toAPB2 bus

	/*3.*************Configure ADC parameters************/
	/*3.1 conversion sequence start*/
/*Each SQ represents the sequence number. Think of it as the order, the order in which the channel is to be sampled.So the first channel you want to be sampled,
  you place it's number in the location SQ1,second channel you place, it's in the location of SQ2.
  Using SQR3 here because it includes SQ1.SQ1 is the position for the first sequence,the first channel sequence,
 SQ2 is the position for the second channel in the sequence.If we had two channels,we wanted the first one to start with its channel no.(SQ1).
 If we want the second one we use SQ2.
 For eg. if we have ADC configure with 3 channels
  ch2,ch3,ch5
  first =ch5, second = ch2, third= ch3 <---this is the order in which we want our ADC to sample channels
  we want first channel to be ch5, second to be ch2, third to be ch3.When we come to sequence register at a position of SQ1 , this means first sequence,
  we have to put five here.We have to put binary number five , and it would occupy these zero through four and then we want second channel to be ch2.
  SQ2 is the position for the second channel you want to be sampled. We have to put 2 in SQ2, from bit 5 to 9, we have to put the binary no. 2.For third channel ch3,
  we have to put three in SQ3.
  Here in our case because we are using ch1, and that's the only channel in the first channel, so we would put the position of SQ1, which is first channel or the first sequence.
  We put binary no. one here, which is one.*/
  ADC1->SQR3  =ADC_CH1;

	/*3.2 conversion sequence length.Length of the sequence means how many channels are you sampling.*/
  /*here we have ADC length 1 , we have got a single channel. We go to SQR1 we can see that to configure the regular channel sequence length, bit 20 to 23 are used.
   if we put 0000 it means length one.*/
  ADC1->SQR1 =ADC_SEQ_LEN_1;

//*3.3 enable ADC module*/
  ADC1->CR2 |=CR2_ADON;//control register two, bit number zero is called ADON, it is the bits to turn on the ADC or turn it off. Applying or operator because we want to update this single bit.using CR2 because it has ADON bit

}

void start_conversion(void){

	/*Enable Continuous conversion*/
	ADC1->CR2 |= CR2_CONT; //inside CR2, there is bit known CONT,this is the continuous conversion bit,if we set this to 1,means if we enable CONT.using OR operator because we only want to set this particular bit only.
	/*start ADC conversion*/
	//In CR2 SWSTART(bit 30) meaning Software Start,we want to start conversion with software trigger because we can trigger the conversion with a timer as well. We set this to 1 to start conversion.
	ADC1->CR2 |=CR2_SWSTART; // using OR operator because we don't want to disturb other bits.
}

/*Function to read the data that is converted from the ADC.
 * The function here that returns uint32_t. This is going to return the data that the ADC has converted. Function takes no argument.*/

uint32_t adc_read(void){
	//Wait for conversion to be complete.Check the flag.Go to status register & find the end of conversion flag.The flag will be raised when the conversion is complete.
	while(!(ADC1->SR & SR_EOC)){}  //AND operation with between SR_EOC and ADC1 status register. While this is false. we want to stuck here.When it is true, meaning the conversion is complete,
	                               // we will pass this line and the next line will be to return the data from the ADC data register.

	//read converted result
	return (ADC1->DR);
}






