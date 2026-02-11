/*In this code we're going to see how to write the timer input capture driver.Input capture will allow us to
 be able to capture the frequency of inputs using our timer.*/

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "timer.h"


int timestamp = 0;
int main (void){
	tim2_pa5_output_compare();
	tim3_pa6_input_capture();

	/*we would have to wait until the edge is captured, to do this, we have to check a bit in the timer status register bit,
	 number one, to be specific.Go to timer status register(TIMx_SR).Bit 1 is known as CC1F, known as capture/compare interrupt flag.
	 (Creating a symbolic name for this in the timer.h file)*/
     while(1){

    	 /*wait until the edges captured*/
    	 while(!(TIM3->SR &SR_CC1IF)){} //While it returns zero, meaning its not set, we want to wait here

    	 /*read the captured counter value*/
    	 timestamp= TIM3->CCR1;
     }
}

/*remember PA5 is configured in output compare mode such that the pin is toggled every second. So this is generating a sort of square wave that goes, every second it goes up
and then another second it comes down.This is going to be out source or signal source and we're going to capture this at PA6.
 Set up- Connect a jumper wire from PA5 to PA6. After uploading the code to stm32 board , add timestamp global variable to live expressions, press enter and press to run.
 We can see the led toggle and capture value.*/







