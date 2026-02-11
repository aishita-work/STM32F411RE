/*
We want an interrupt to occur when we press our push button. The pushbutton is connected to PC13. So  we have go and configure PC13.Input pin 13 to generate an interrupt
whether a rising edge is detected or a falling edge. Because PC13 is where the pushbutton is connected and when we press the pushbutton and release it,
the changing signal would create the interrupt for us.
 */

#include "exti.h"
#define GPIOCEN       (1U<<2)
#define SYSCFGEN      (1U<<14)
void pc13_exti_init(void) //the function takes no argument
{
    /*Disable Global Interrupt -> It's often recommended that before we configure interrupt, we disable global interrupt.It wouldn't affect the code.This one is optional, we can skip it
     To disable global interrupt, we use an intrinsic function, a function that comes with our cortex m.h files. */
	__disable_irq();

	/*Enable clock access for GPIOC-> GPIOC is connected to the AHP1 bus, we need to go through the AHP1 Enabled Register and we know bit 2 in the AHP1 Enabled Register is for GPIOC,
	 GPIOC is connected to the AHP1 bus, we need to go through the AHP1 Enabled Register and bit 2 in the AHP1 Enabled Register is for GPIOC,, we saw this in our Input Driver lesson.*/
	RCC->AHB1ENR |= GPIOCEN;

	/*Set PC13 as input-> Because we're dealing with PC13 then, we're looking for MODER13. This occupies bits 26 and 27, and the true table says to set as input both of the bits have to be 0.*/
	GPIOC->MODER &=~(1U<<26);
	GPIOC->MODER &=~(1U<<27);

	/*Enable clock access to SYSCFGEN -> EXTI is part of the SYSCFGEN(System configuration controller clock enable) module, so we need to enable clock access for that module as well.
	The SYSCFGEN is connected to the APB2 bus.Go to APB2 Enabled Register in reference manual, bit number 14 here is called SYSCFGEN. */
	RCC->APB2ENR |= SYSCFGEN;

	/*Select PORTC to be connected to ->To select port C, Go to our SYSCFG EXTI configuration register.Go to the reference module again,search SYSCFG_EXTICR.We have a number of configuration registers over here. We have SYSCFG_EXTICR1, CR2, CR3, CR4. Let's start with number 1.
	 When we observe, we realize there is a separation after every 4 bits, such that the first 4 bits are called EXTI0, the next 4 bits are called EXTI1, EXTI2, EXTI3. In the truth table, it tells us how to connect the various ports to the EXTI line.For eg, it says if you want any of the exti-lines to be connected to port A PA, you have to pass 0000 in all 4 bits.
	 So if we want EXTI line 2 to be connected to port A, then we have to set bit 8 to 0, bit 9 to 0, bit 10 to 0, and bit 11 to 0. If we want exti-line 1 to be connected to port C, we have to set bit 4 to 0, bit 5 to 1, bit 6 to 0, and bit 7 to 0.If this register didn't have everything set to 0, we would need to clear the bits we want to use before we configure them.
	 But because it's all 0000 that is why we're not clearing any of the bits, because they are all 0 by default.We need to set 1 at the positions we want to set in order to get the perfect arrangement for the port.We are interested in , we have to access the configuration register 4. We want to connect  to PC13 to port C.The first bit has to be 0, the second bit has to be 1.
	 0010 : PC[x] pin. Here, bit 5 is the second bit. We set bit 5 to 1, then all other bits have to be set to 0.We have a single member in the structure for the register, this member is an array such that if we want exti-configuration-register 1, we use index 0.Here, we will use index 3 for configuration register 4.  */
	SYSCFG->EXTICR[3] |= (1U<<5);

	/*Unmask EXTI13 ->Unmask means uncover. They are covered by default. To do this, we need to go to the interrupt mask register(IMR). Go to reference manual and search EXTI_IMR. Here it says if it is 0, interrupt request from line X is masked. If it is 1, interrupt request is unmasked. We are interested in line 13, we need to set bit 13 to 1 in this register. */
	EXTI->IMR |= (1U<<13);

	/*Select falling edge trigger -> We have to decide whether we want the interrupt to be triggered by rising edge or falling edge, here we're going to use falling edge.Go to reference manual and search EXTI_FTSR. It tells us if it is zero, Falling Trigger is disabled. If it is one, Falling Trigger is enabled. So we are looking for line 13. We want to enable Falling Trigger for line 13.*/
	EXTI->FTSR |= (1U<<13);

	/*Enable EXTI line in the nested vector interrupt controller(NVIC) -> To do this, we're going to use a function that comes with our core CM4.h file.The interrupt number for our line 13 is actually number 40, but there is a symbolic name for this. We can  say EXTI15_10_IRQn for number. If we highlight this, we see this is number 40. We can inspect it,if we double-click(NVIC_EnableIRQ),
	  right-click, go to Open Declaration. We can see, this is in our core_cm4.h file. By doing this, we've enabled interrupt for this. We can check this definition as well. Double-click(EXTI15_10_IRQn), right-click, and open Declaration, this here is telling us all the interrupt numbers.*/
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/*Enable global interrupt -> Because we disabled global interrupt above, we have to enable global interrupt after we are complete . Otherwise, no interrupt will work in our system*/
    __enable_irq();
}








