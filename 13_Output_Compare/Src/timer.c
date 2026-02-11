#include "stm32f4xx.h"

/*Go to the block diagram in datasheet. We can see we have TIM2,TIM3,TIM4,TIM5,TIM9,TIM1/PWM. We will use TIM2 for this project.*/
#define TIM2EN     (1U<<0) //TIM2 is connected APB1 bus, we need to enable it to clock through the APB1 enable register.
#define CR1_CEN    (1U<<0)
#define CCER_CC1E  (1U<<0)

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0



void tim2_1hz_init(void){      //Task - We're going to write this driver to generate a timeout every second.Let TIM2, write a driver for timer2 that would generate a timeout each second.

	/*Enable clock access tim2*/
	RCC->APB1ENR |=TIM2EN;

    /*Set prescaler value - To set the prescaler value,we put this inside the prescaler register.Default system clock is 16MHz and this is same clock for the APB1 bus,
    because we've not configured the clock tree yet.We want to reduce this clock to 1hz,we want the timer to end up with one hz ,essentially.*/
	TIM2->PSC = 1600-1;   //1600 because we count from 0 , we'll say -1 here. Think of this having a 16MHz divided by 1600-->16 000 000/1600 = 10 000

	/*Set auto-reload value*/
	TIM2->ARR =10000-1; //we come from zero, we say -1.Think of it as 10000/10000=1.Therefore we end up with 1hz

	/*Clear counter*/
	TIM2->CNT =0;
	/*Enable timer - We need to locate a particular bit inside a time control register.In reference manual go to TIMx_CR1. Bit 0 CEN enables the timer*/
	TIM2->CR1 = CR1_CEN;

}

void tim2_pa5_output_compare(void){

	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set PA5 mode to alternate function - PA5 occupies bit 10 and 11 in the mode register. To set alternate function mode,the first bit(bit 10) has to be set to 0 and second bit(bit 11) has to be set to 1.*/
	GPIOA->MODER &=~(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 0)
	GPIOA->MODER |= (1U<<11);  //set bit 11 to 1  NOTE: 11-10 -->1-0: Alternate Function Mode

	/*Set PA5 alternate function type to TIM2_CH1 - This is AF01( Go to datasheet , go to Table 9 - Alternate function mapping. For PA5 we have TIM2_CH1. */
    GPIOA->AFR[0] |= (1U<<20); //because this is pin five, we need to use the alternate function register low. Therefore we're looking for AFRL5(in GPIOx_AFRL Register).
                               //It occupies bit 20 to bit 23, AF01 means set one to the first bit,means bit 20 has to be 1. [0001:AF1].Also symbolic name can be created as AFR_TIM (1U<<20)


    /*Enable clock access tim2*/
	RCC->APB1ENR |=TIM2EN;

    /*Set prescaler value - To set the prescaler value,we put this inside the prescaler register.Default system clock is 16MHz and this is same clock for the APB1 bus,
    because we've not configured the clock tree yet.We want to reduce this clock to 1hz,we want the timer to end up with one hz ,essentially.*/
	TIM2->PSC = 1600-1;   //1600 because we count from 0 , we'll say -1 here. Think of this having a 16MHz divided by 1600-->16 000 000/1600 = 10 000

	/*Set auto-reload value*/
	TIM2->ARR =10000-1; //we come from zero, we say -1.Think of it as 10000/10000=1.Therefore we end up with 1hz

	/*Set output compare toggle mode - Go to reference manual, go to TIMx_CCMR(we will use CCMR1 here) register.We are interested in OC1M(Output Compare 1 Mode)
	 because we are using channel 1 of this timer.If we were to use channel2 , we got to use OC2M.If we set bit 6-5-4 to 0-0-1 = set channel to active level on match.
	 If we want active level on match, we set the first bit to 1 and the other two bits to 0. If we set it to 0-1-1 = toggle i.e. set first two bit(bit 4&5) to 1 and third to 0. */
	TIM2->CCMR1 = (1U<<4)| (1U<<5);  //bit 4&5 set to 1 .Also we can create symbol for these bits as OC_TOGGLE = (1U<<4) | (1U<<5)

	/*Enable timer2 channel1 in compare mode - Go to TIMx_CCER(capture/compare enable register).There is another mode known as input capture mode and we use these same registers to
	 configure the input capture mode as well.Bit 0 is capture/compare output enable,1 means capture enabled.we're using bit zero because we are using channel1. */
    TIM2->CCER |= CCER_CC1E;

	/*Clear counter*/
	TIM2->CNT =0;
	/*Enable timer - We need to locate a particular bit inside a time control register.In reference manual go to TIMx_CR1. Bit 0 CEN enables the timer*/
	TIM2->CR1 = CR1_CEN;

}
