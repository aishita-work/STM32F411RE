#include "stm32f4xx.h"

/*Go to the block diagram in datasheet. We can see we have TIM2,TIM3,TIM4,TIM5,TIM9,TIM1/PWM. We will use TIM2 for this project.*/
#define TIM2EN     (1U<<0) //TIM2 is connected APB1 bus, we need to enable it to clock through the APB1 enable register.

#define CR1_CEN    (1U<<0)

#define DIER_UIE   (1U<<0)

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

/*So over here, we simply need to enable timer interrupt and then enable it in the NVIC.*/
void tim2_1hz_interrupt_init(void){      //Task - We're going to write this driver to generate a timeout every second.Let TIM2, write a driver for timer2 that would generate a timeout each second.

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


	/*Here, we want to enable the number of interrupts. We're going to enable the update interrupt. Go to reference module.Search timx_dier register. We find this in DMA/interrupt enable register.
	 We are interested in the UIE, bit 0.Bit 0 is the update interrupt event. So, this means a timeout has occurred.Created a symbolic name DIER_UIE */
	TIM2->DIER |=DIER_UIE;/*Enable TIM interrupt*/

	/*Next, we're going to enable this in the NVIC. So, we'll use our NVIC enable IRQ function and we'll pass the interrupt number of timer2. This one here is 28. The symbolic name is defined in our STM32F411xE.h file. */
	NVIC_EnableIRQ(TIM2_IRQn);/*Enable TIM interrupt in NVIC*/

}

