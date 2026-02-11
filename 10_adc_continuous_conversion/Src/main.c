/*
---------------------ANALOG-TO-DIGITAL CONVERTOR(ADC) THEORY----------------------------------------------------------------------------------------------------------------------
1. Most widely used devices among data acquisition.Digital computers use binary or discrete values but in physical world,
everything is analog or continuous.Temp.,pressure,humidity,velocity are eg.A physical quantity is converted to electrical signals using a device called transducer.
Transducer converts a physical quantity to either voltage or current.Transducer use to generate electrical output are called sensors. We need ADC to trslate analog
signal to digital numbers so that the micro-controller can read and process these numbers.

---------ADC Resolution-------------------
An ADC has n bit resolution where n can be 8,10,12,16,24. Higher resolution ADC provide a smaller step size where step size is the smallest change that can be detected by ADC.

----------------------------------TABLE----------------------------------------------
n-bit           |           Number of Steps            |        Step Size (Vref =5V)
8-bit           |                256                   |         5v/256 = 19.53mV
10-bit          |                1024                  |         5v/1024 = 4.88mV
12-bit          |                4096                  |         5v/4096 = 1.2mV
16-bit          |                65,536                |         5v/65556 = 0.076mV
--------------------------------------------------------------------------------------
8 bit ADC would have 256 steps.(how do we know this? we do 2^8 =256).To compute the step size we've to take
reference voltage into account.So the smallest change a 8 bit ADC can detect is 19.53mV.
Similarly 10 bit ADC can detect is 4.88mV and so on....  .Assuming the reference voltage is 5V.Vref is an input voltage use for the reference voltage,
the voltage connected to this pin in most micro-controller have the Vref and we can connect our own input voltage to or we can just use the same voltage
as the one used by the MCU.So the voltage connected to Vref pin along with ADC resolution chip,determine the step-size .
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
------------------------------ADC INDEPENDENT MODES--------------------------------------------------------------------------------------------------------------------------
1. Single-channel, single conversation mode
2. Multi-channel(scan), single conversation mode
3. Single-channel continuous conversation mode
4. Multi-channel continuous conversation mode
5. Injected continuous conversation mode

STM32F4 has a single ADC module.Other STM32 versions have two ADC modules.The two modules allows us to have the dual mode configurations, here we have a single ADC module,
we will only be using independent modes.We have a single ADC module, it doesn't mean we have a single channel. Module is different fromm channel, our single ADC module allows
us to have 16 channels,meaning we can connect 16 sensors to our micro-controller and sample them.

1.Single-channel, single conversion mode:
a) Simplest ADC mode
b) ADC performs a single conversion of a single channel x and stops after conversion is complete.

               FLOW DIAGRAM -        START-->CHx-->STOP
Eg.- Measurement of voltage level to determine if a system should be started or not

2. Multi-channel(scan), single conversation mode:
a) Used to convert multiple channels successively
b) Up to 16 different channels with different sampling times can be converted on the STM32F4

              FLOW DIAGRAM -      START-->CHx--CHn-->STOP
Eg.- Measurement of multiple sensors to determine whether a system should start or not.

3. Single-channel continuous conversation mode:
a) Used to convert a single channel continuously
b) Works in the background without intervention from the CPU

FLOW DIAGRAM -        START-->CHx---   (goes from CHx to start again)
                            ^-------|
Eg: Measurement of room temperature continuous to adjust air-conditioner

4. Multi-channel continuous conversation mode:
a) Used to convert a multiple channel continuously
b) Up to 16 different channels with different sampling times can be converted on the STM32F4

 FLOW DIAGRAM -        START-->CHx--CHn---   (goes from CHn to start again)
                            ^------------|
Eg: Continuously measuring multiple accelerometers to adjust joints of a robotic arm.

5. Injected continuous conversation mode:
a) Intended for use when conversion is triggered by an external event or by software.
b) The injected group has priority over the regular channel group.
c) Interrupts the conversion of the current channel in the regular channel group.

Eg:  For synchronizing the conversion of channels to an event.
Created adc.c and adc.h , follow for code explanation
_________________________________________THEORY COMPLETE__________________________________*/

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "adc.h"
#include "uart.h"

uint32_t sensor_value;

int main(void)
{
	uart2_tx_init();
	pa1_adc_init();//initializing the ADC
	start_conversion();//start conversion
	while(1){
		sensor_value= adc_read();
		printf("Sensor Value : %d\n\r",(int)sensor_value);
	}
}
/*HOW TO TEST
Locate pa1 in the nucleo board. go to nucleo user guide.In fig:18 It is A1 on the nucleo board.Have a jumper wire, connect one end to PA1 and other end to GND.
You expect to read zero.This is the first test to test.Second test is to connect one end to PA1, connect the other end to 3.3V and you expect to read 4950 or something,
about 4000 because it's 12 bit ADC.Another way of running the experiment is to have a potentiometerConnect middle leg to PA1, left or right leg to GND, and th other leg,
either the left or right to 3.3VAnd then as you turn the potentiometer , you expect to see the analog value change.*/

/*This code is for continuous conversion driver , we don't need to put start_conversion() inside the loop. we need to write only once in the main function .*/













