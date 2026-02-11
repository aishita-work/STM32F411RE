
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "adc.h"
#include "uart.h"


static void adc_callback(void);
uint32_t sensor_value;

int main(void)
{
	uart2_tx_init();
	pa1_adc_interrupt_init();//initializing the ADC
	start_conversion();//start conversion
	while(1){
		sensor_value= adc_read();
		printf("Sensor Value : %d\n\r",(int)sensor_value);
	}
}

static void adc_callback(void)
{
	/*In an interrupt we need not wait for the SR flag. We simply need to check when it occurs, when the interrupt occurs. We just want to make sure it's the end of conversion interrupt.
	 That's why we are performing this check. So over here we want to return the content of the ADC data register and we can print that once we've returned it*/
	sensor_value= ADC1->DR;
    printf("Sensor Value : %d\n\r",(int)sensor_value);
}

/*We need to implement the interrupt request handler. When we check the vector table, we realize the name for the ADC interrupt request handler is ADC_IRQhandler and this is supposed to be a void void function.
 So we write this accordingly[void ADC_IRQHandler(void)], open and close.*/
void ADC_IRQHandler(void)
{
/*First thing we want to do is, we have to check if the end of conversion flag is raised. We check for EOC in status register(SR),
 and then once that happens, we would clear it.So let's do the check first.We say if we want to read the ADC status register.
So we say ADC1 over here, status register, and then what we're looking for is the EOC in the status register. We created a symbolic name for this (SR_EOC).I'll also put this in our .h file so that we can have it in main.c.
So we want to read this bit. Perform an AND operation. I'm going to say if this is not equal to zero meaning it is set,if it is set then we can go ahead and service it but to do that we first clear it.So we'll clear it.
When an interrupt occurred, you have to clear before the next one. To clear it, we simply disable it.  */
	if ((ADC1->SR & SR_EOC) !=0 )   /*Check for EOC in SR*/
		{
			 ADC1->SR &=~SR_EOC;  /*Clear EOC*/

			 //Do something
			 adc_callback();  //call our ADC callback
		}
}

/*Build the project.Do Debug as.We want to observe the sensor value. Select number 17 and we say OK.Click Switch .So we're going to get our global variable sensor value. Write sensor_value to live expression.
  Hit Enter. Press play button in the navigation bar. And because there is nothing connected to the ADC pin, we'll record in our random values and realterm applications show you this as well.
  To test it out. Find the location of PA1.Connect a jumper wire to PA1 and then ground. You can see the value is 0. And when we record, when we check our watch window or live expressions window it's also 0.
  So this is our ADC interrupt driver completed. */











