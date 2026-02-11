/*___________________________________INTERRUPT IN ARM PROCESSOR THEORY___________________________________________________

Interrupts vs. Polling - A single microprocessor can save several modules by way of interrupt or polling.

1) Interrupt - When module needs service, it notifies the CPU by sending an interrupt signal.When the CPU receives the signal
the CPU interrupts whatever it is doing and services the module.The module could be ADC, GPIO, UART etc.
Code snippet for interrupt-> int main()
                             {
                               while(1){
                                 }
                             }
                             OnSwitch_ISR{
                              getData()
                             }

In interrupt mode, the function is here ,nothing is being checked. We have another block of code known as the interrupt service routine
written as ISR and then we just put what we want to execute over here.

2) Polling -The CPU continuously monitors the status of a given module, when a particular status condition is met the CPU then
services the module.
Code snippet for polling->  int main()
                            {
                              while(1){

                              if(switch = on){
                              getData(); }
                              }
                             }
In polling method, we often have our function and then we check in a condition, for instance if switch equals on, then gets data.

Interrupt Service Routine - The function that gets executed when an interrupt occurs is called the Interrupt Service Routine(ISR) or the Interrupt Handler.

_________________________________________________Nest Vector Interrupt Controller(NVIC)_________________________________________________________________

1) NVIC is a dedicated hardware inside the Cortex - Micro-controller. It is responsible for handling interrupts.
2) Interrupts from the processor are known as exceptions.
3) Interrupts from outside the processor core are known as Hardware Exceptions or Interrupt Requests(IR).

4) The vector table contains the addresses of the Interrupt Handlers and Exception Handlers that would execute an interrupt occurs.

5)In the example code for interrupt, one wonders, how does the code know this function exists and issued execute it?We are not calling the function OnSwitch_ISR with while,
 how do I know I have to come here and execute this function.So what the reason and knows this is something known as vector table, the vector table contains the addresses
 of all the interrupt handlers and the exception handlers such that when an interrupt is triggered, the processor goes to the vector table and finds the address of the ISR,
 then executes that.
_______________________________________________External Interrupt(EXTI) lines_____________________________________________________________________________________

1)GPIO pins are connected to EXTI lines.
2)It is possible to enable interrupt for any GPIO pin.
3)Multiple pins share the same EXTI line.

Pin 0 of every Port is connected EXTI0_IRQ
Pin 1 of every Port is connected EXTI1_IRQ
Pin 2 of every Port is connected EXTI2_IRQ
Pin 3 of every Port is connected EXTI3_IRQ

This means we cannot have PB0 and PA0 as interrupt pins at the same time since they are connected to the same multiplexer i.e. EXTI0.
Same for PC4 and PB4 at the same time, etc.
Pins 10 to 15 share the same IRQ inside the NVIC and therefore are serviced by the same interrupt Service Routine(ISR).

Our PA0 and PB0 all of them are routed to EXTI0.And inside the NVIC, they are EXTI0 again. However when we check PA10 all the way to PA15,
meaning including the PA11,12,13 to 15 they are all connected to a single IRQ line in the NVIC.What this means is our application code must be able to find which
pin from 10 to 15 generated the interrupt that we are using these numbers.

_____________________________________________________Interrupts - States ______________________________________________________________________________________________
1) Disabled - This is the default state.
2) Enabled  - Interrupt is enabled.
3) Pending  - Waiting to be serviced,meaning the interrupt has been generated and is waiting for the CPU to service.
4) Active   - Being Served.
_____________________________________________________States - Pending vs Active________________________________________________________________________________________

												   |          |          |          |       |
											       |          |          |          |       |
											       |          |  TIMER   |     TIMER        |
												   |          F          P          |       |
												   |          |          |          |       |
												   P          |          |          |       |
												   |         ADC         |          |       |
												   F          |          |          |       |
												   0          1    2     3    4     5       6
															(Time in seconds)
TIMER from 1 to 3
TIMER from 3 to 6
ADC from 0 to 3
TIMER and ADC - Active State
TIMER from 1 to 3 sec is in Pending State
P - Pending State Cleared
F - Interrupt Fired
Let's assume - ADC Interrupt has a higher priority than TIMER interrupt

1) ADC interrupt fires at t=0. This is indicated by F.
2) Since there is no other interrupt, the pending state is cleared and the interrupt becomes active. This is indicated by P.
3) At time t=1 TIMER interrupt fires. This is indicated by F.
4) Since it has a lower priority than the ADC interrupt it remains in the pending state.
5) At time t=3 ADC interrupt completes its execution.
6) Since there is no other interrupt with a higher priority, the pending state of the TIMER interrupt is cleared and the
   interrupt becomes active. This is indicated by P.

____________________________________________________________Interrupt : Priorities _________________________________________________
1) Priorities allow us to set which interrupt should execute first.
2) They also allow us to set which interrupt can interrupt which.
3) The vector table tells us all the interrupts we have, the  numbers and then the priorities.
4) Some interrupt priority are defined by ARM, these cannot be changed Eg:
a) RESET     : Priority of -3 , highest priority in the system.
b) NMI       : Priority of -2
c) HardFault : Priority of -1
   Lower Number = Higher Priority

__________________________________________________________Interrupts : Priorities in M3/M4/M7______________________________________________
1) Priority of each interrupt is defined using one of the Interrupt Priority Registers(IPR).
2) Each Interrupt Request(IRQ) uses 8-bits inside a single IPR register.
3) Therefore one IPR register allows us to configure the priorities of four different Interrupt Requests.
4) Example: IPR0 holds the priority of 0, 1, 2, 3.
5) There are 60 Interrupt Priority Registers : IPR0 - IPR59.
6) There are 60x4=240 Interrupt Requests (IRQ).

7) 8-bits to configure the priority of an IRQ implies there are 2^8 = 255 priority levels.
8) STM32 micro-controllers use only the 4 upper bits to configure the priority of each , this implies that in
   STM32 MCUs there are 2^4 = 16 priority levels.
9) IPRn = (4n+3), (4n+2), (4n+1) and (4n), this is the equation to find IPR register number that a particular  belongs to
   we can use this equation over here.
10)The 16 priority levels: 0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
                           0x80,0x90,0xA0,0xB0,0xC0,0xD0,0xE0,0xF0
11)Highest Priority = 0x00=0
12)Lowest Priority  = 0xF0=16

13)To find the IPR number we divide the  number by 4, the remainder will determine which byte it is in the IPR register.
14)Because only the highest 4 bits are used for priority, the priority number needs to be multiplied by 16 or left shift 4 bits.
15)To calculate , the NVIC_IPRx are defined as an array of 8-bit registers IP[x] in the core_cm3.h, core_cm7.h files.
   Such that the priority of x is controlled by IP[x].
   Eg: Setting TIM2 interrupt priority to 3
               TIM2 interrupt is  28 (we can find this in the stm32f411xe.h)
               NVIC->IP[28] = 3<<4; (3 because we want to set it to 3, because we are using the highest four bits, we do the shift 4 bits)
               (use the above format in bare metal programming)
               NVIC SetPriority(TIM2_n,3);
               (when using LL or HAL library)
16)The interrupt priority Registers (IPR) can also be divided into sub-priorities.
17)In this configuration there are series of bits defining preemption priority and a series of bits defining the sub-priority.
18)The sub-priority will determine which  will be executed first in the case of multiple pending s.

_____________________________________________________________________________________________________________________________________________________________*/


#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "exti.h"
#include "uart.h"

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5

static void exti_callback(void);


int main (void){
	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0


	pc13_exti_init();
	uart2_tx_init();


	while(1){


	}
}

static void exti_callback(void){
	printf("BTN Pressed......\n\r");
	GPIOA->ODR ^= LED_PIN;
}


/*The interrupt handler for this particular interrupt is known as EXTI15_10_IRQHandler. The reason name has to be exactly this is because this is the name
 defined in the vector table.To find the vector table we need to come to our project tree and then come to the startup folder and open the startup.s file.
 When we scroll down we see portion called g_pfnVectors, below are all the names of the various interrupts we can use. The name of this interrupt says EXTI15_10,
 this means this interrupt request handler is used for multiple interrupts. the comment here in the vector table for the function is telling us that this function
 is for EXTI interrupts for the numbers from interrupt line 10 to 15. If we have interrupt line 10, 11, 12, 13, 14, 15, all enabled, and the interrupt request,
 the same function is the one that is going to be executed .To do this, we have to find a way to distinguish which of them, which interrupt within this range
 has been triggered. We know we are interested in line 13. We will create a symbolic name for line 13 in exti.h named LINE13 . */
void EXTI15_10_IRQHandler(void){

	/*Search EXTI_PR in reference manual.It says if this is zero, no trigger request occurred. If it is one selected, trigger request occurred.
	 We're going to read 13 to see if it is one, and that's how we'll know the trigger has just occurred. When an interrupt occurs, we're going to enter this interrupt request handler,
	 which is the above function, and in here we're going to check if it is from line 13. To do this we will write the below if condition. */

	/*To read a bit we perform an AND operation using the bit as one operand, and the register we want to read from as the other operand.So we check if this is not equal to zero,
	 meaning if it is one, or we can simply say if it is true. If this is not equal to zero, this means the interrupt has occurred, and we want to clear the PR. So we clear the PR flag and then we do something.*/
	if((EXTI->PR & LINE13)!=0)
	{
		/*clear the PR flag */
		EXTI->PR |= LINE13;

	   /*Do something .....*/
		exti_callback();
	}
}

/* After download successfully, open Realterm to see what we have. Select the port range. Select the port. Click Open. Click Open again. Push the button. As we can see, BTN pressed... is getting printed
   when we press the button. When the button is pushed, an interrupt occurs. The interrupt handler is executed. Inside the interrupt handler, we call a callback function. This is where our user code is.
   We want to print BTN Pressed and toggle the LED*/















