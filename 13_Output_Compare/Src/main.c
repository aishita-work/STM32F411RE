/*In this code we develop the timer output compared driver and output compare allows us to toggle the GPIO directly when a particular period elapsed. In previous example we explicitly said that
  toggle the GPIOA output data register,toggle the LED bit there. But when we configure this to output compare mode, we wouldn't need to write this. We would be connecting PA5 directly to the timer
  such that the entire while loop(in code 12_Timers) is empty, yet we still have the LED to toggle. So this allows the timer to directly toggle a hardware pin when a particular period elapses.  */

/*Output compare requires us to know the GPIO pins connected to the timers.Go to datasheet document, go to alternate function mapping, it tells us the alternate function of all GPIO pins.
 For PA5 it is timer2 channel 1.If we set PA5 to alternate function mode one, AF01, it's access is timer2 channel 1. PA5 is connected to LED, we're going to use the timer to toggle PA5.
 Because the LED is connected , the LED is going to be going ON and OFF based on the timer period that we set or the frequency that we set for the timer.For eg. we can also use other pins if
 we ever want to toggle, let's say, PA10 to drive a DC motor or something else to drive in it output, then we know that it is timer1 channel3. Follow timer.c file for source code.*/

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "timer.h"



int main (void){
	tim2_pa5_output_compare();

}
/*Run code on STM32. We can see LED is toggling at one hertz rate.
 We can see that nothing in the main function is telling the LED to toggle. The function tim2_pa5_output_compare(), we're not even accessing the output data register
 of the LED but yet it is toggling. This is what we call output compared, toggling a pin periodically using the timer.*/













