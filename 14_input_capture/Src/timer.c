#include "stm32f4xx.h"

#define TIM2EN     (1U<<0)
#define CR1_CEN    (1U<<0)
#define CCER_CC1E  (1U<<0)
#define GPIOAEN    (1U<<0)
#define GPIOAEN    (1U<<0)
#define AFR5_TIM    (1U<<20)

#define TIM3EN     (1U<<1)
#define AFR6_TIM   (1U<<25)
#define CCER_CC1S  (1U<<0)


void tim2_1hz_init(void){
	RCC->APB1ENR |=TIM2EN;
	TIM2->PSC = 1600-1;
	TIM2->ARR = 10000-1;
	TIM2->CNT = 0;
	TIM2->CR1 = CR1_CEN;
}
void tim2_pa5_output_compare(void){
	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER &=~(1U<<10);
	GPIOA->MODER |= (1U<<11);
    GPIOA->AFR[0] |=AFR5_TIM;
	RCC->APB1ENR |=TIM2EN;
	TIM2->PSC = 1600-1;
	TIM2->ARR = 10000-1;
	TIM2->CCMR1 = (1U<<4)| (1U<<5);
    TIM2->CCER |= CCER_CC1E;
	TIM2->CNT = 0;
	TIM2->CR1 = CR1_CEN;
}
/*Above two functions are taken from 13_Output_Compare project.Go to that project for explanation.*/

/*We're going to add another timer, we're still going to use output compared from PA5,it is still going to toggle our LED.
 But this time, the toggle rate that we generated at PA5, we want to capture it using another timer pin.So we will connect PA5
 to a separate time channel to be able to read the rate at which it is toggling.We'll capture PA5 using out timer 3.*/

/*Go to Block diagram of alternate function mapping in datasheet.We can see PA6 here when we set AF02,we have this act in as TIM3_CH1.
So we can configure PA6 to be acting as timer three channel one over here, such that we can connect the input whose toggle rate we want
to capture at PA6.*/

void tim3_pa6_input_capture(void){

	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set PA6 mode to alternate function - PA6 occupy but 12 and 13*/
	GPIOA->MODER &=~(1U<<12); //first bit 12 is set to zero
	GPIOA->MODER |= (1U<<13); //second bit 13 is set to one

	/*Set PA6 alternate function type to TIM3_CH1(AF02) - go to reference manual, GPIOx_AFRL.Because we are using PA6 , we are interested in AFRL6.
	 For AF2 we have to set the second bit to 1.In AFRL6[3:0] the second bit is 25, so we have to set bit 25 to one, we've created a symbolic name for this. */
	GPIOA->AFR[0]=AFR6_TIM;

	/*Enable clock access tim3 - Timer3 is located in the APB1 register just like timer2,Timer3 is bit number one.*/
	RCC->APB1ENR |= TIM3EN;

	/*Set prescaler value to slow down the clock by dividing means setting the prescaler value*/
	TIM3->PSC = 16000-1; // this is basically dividing our clock --> 16 000 000/16 000

	/*Set CH1 to capture at every edge(raising/falling) to input capture - Go to capture/compare mode register in our reference manual go to TIMx_CCMR1(for TIM1). We're interested in bit 0 here.
	 We need to be careful because same register is for output compare & input capture.So when we scroll down to the documentation , it tells us of output compare hence OC.
	 Then we get to a section known as input capture mode, meaning the same bit but this is the meaning of the bits when we work in an input capture mode.So we scroll down to bit 0.
	 Bit 0 here is called a capture/compare selection.Go to CC1S it says , if we set bit to 00 it's configured as output, when we set this to 01 channel is configured as inputs.
	 we're going to use that one zero and the input has various modes,TI1,TI2, for now we're not going to use any mode , we're going to configure it as input over here by setting bit zero to 1.*/
	TIM3->CCMR1 = CCER_CC1S;

	/*Set CH1 to capture at rising edge - Go to TIMx_CCER register(for TIM1).Go to CC1E.For channel configured as input bit. We set bit zero to one to enable capture and when we leave the other bits the same,
	  we would end up capturing at raising edge, if we wanted to capture at a falling edge, we would have to configure other bits as well, but default setting is raising.*/
	TIM3->CCER = CCER_CC1E; //it is the same bit here, we used this for output compare.

	/*Enable TIM3 - Similar to what we did for timer2,CR1_CEN for timer2 is bit zero, for timer3 is bit zero as well.*/
	TIM3->CR1 = CR1_CEN;

}



