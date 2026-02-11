#include "stm32f4xx.h"

#define     GPIOBEN                  (1U<<1)  // GPIOB is in the AHB1 bus, therefore AHB1ENR bit number one
#define     I2C1EN                   (1U<<21) //I2C is in APB1 bus go to reference manual, go to RCC_APB1ENR,bit 21 is the bit for I2C module 1, so we set bit 21 to one
#define     I2C_100KHZ                  80     // 0B 0101 0000 = decimal = 80
#define     SD_MODE_MAX_RISE_TIME       17
#define     CR1_PE                   (1U<<0)

#define     SR2_BUSY                 (1U<<1)
#define     CR1_START                (1U<<8)
#define     SR1_SB                   (1U<<0)
#define     SR1_ADDR                 (1U<<1)
#define     SR1_TXE					 (1U<<7)
#define     CR1_ACK					 (1U<<10)
#define     CR1_STOP                 (1U<<9)
#define     SR1_RXNE                 (1U<<6)
#define     SR1_BTF                  (1U<<2)

/*Go to our datasheet document to see the I2C modules we have on our device. Go to block diagram to see any I2C modules.We have I2C1, I2C2, I2C3 and then they are all connected to the APB1 bus.So then we know we have to enable the clock
 through the APB1 bus. We're going to nominate I2C module 1 for this driver.So once that is done, we're going to configure the SCL and SDA lines. To locate the SCL and SDA lines for I2C1, we need to go to the table that talks about the
 alternate function of the pins.In the data sheet document,go to table 9, Alternate function mapping.Go to port B, PB6 over here is I2C1_SCL line.PB7 is I2C1_SDA line. PB9 also is I2C1_SDA line.We're going to use PB9 for our I2C1_SDA
 line because it is easily identifiable on the nuclear board.So PB9 here is going to be the SDA line. It's of alternate function type 1.And PB6 here is the SCL line, also of alternate function type 4. On the nuclear board  D15
 and D14 are called the SDA and SCL line, these pins are PB8 for SCL, PB9 for SDA.
 PB8--------SCL
 PB9--------SDA

 */

void I2C1_init(void){   // It takes no argument

	/* enable clock access to GPIOB */
	RCC->AHB1ENR |=GPIOBEN;

	/* set PB8 and PB9 mode to alternate function */
	GPIOB->MODER &=~(1U<<16); //PB8 bit in the mode register is bit 16 and bit 17 and for alternate function mode the first bit has to be set to zero and the second bit has to be set to one
	GPIOB->MODER |=(1U<<17);

	GPIOB->MODER &=~(1U<<18); //PB9 bit in the mode register is bit 18 and bit 19, bit 18 is the first bit, it has to be set to zero and bit 19 is set to one
	GPIOB->MODER |=(1U<<19);

	/* set PB8 and PB9 output type to open drain - go to reference manual, go to GPIOx_OTYPER, it's the output type register. It's a 32-bit register. Upper 16 bits are reserved, lower 16 bits are used.It's telling us if
	 we want the output type, we can have two values, either push-pull or open drain. So if we want a particular one to be set to open drain, we need to set it to 1. So we're looking for pin 8 and pin 9. Therefore, we work with OT8
	 and OT9. We have to set these bits to 1 */
	GPIOB->OTYPER |=(1U<<8);
	GPIOB->OTYPER |=(1U<<9);

	/* Enable Pullup for PB8 and PB9 -go to reference manual, go to GPIOx_PUPDR,it's a 32-bit register, and all 32 bits are used.We can see there is separation after every two bits, meaning each pin requires two bits to configure its
	  pull-up, pull-down functionality. So because we are dealing with pins 8 and 9, we'll be interested in PUPDR8 and PUPDR9.PUPDR8 here occupies bits 17 and 16, according to truth table, it says if we want no pull-up or pull-down,
	  we set both bits to 0. If we want pull-up, we set the first bit to 1 and the second bit to 0. If we want pull-down, we set the first bit to 0 and the second bit to 1.Here we want pull-up because we want pull-up,
	  the first bit has to be set to 1 and the second bit to 0. So for PB8, the first bit is bit 16, the second bit is bit 17.And for PB9, the first bit is 18, the second bit is 19.     */
	GPIOB->PUPDR |=(1U<<16); // bit 16 set to 1 , bit 17 set to 0
	GPIOB->PUPDR &=~(1U<<17);

	GPIOB->PUPDR |=(1U<<18); // bit 18 set to 1 , bit 19 set to 0
	GPIOB->PUPDR &=~(1U<<19);

	/*Set PB8 and PB9 alternate function type to I2C(AF4)- PB8 over here is I2C SCL line. PB9 is I2C SDA line. They are of type AF4. I2C is AF4. We saw this in our alternate function table. Let's go back to the datasheet. There's the
	 datasheet over here. I'll do Ctrl-F, and then we see alternate function mapping. So we simply search alternate function over here. And then there's the table. When we scroll down to the pin 8 and pin 9 part of the table, assuming
	 a bit, PB8 over here is I2C CL line. PB9 is I2C SDA line. They are of type AF04. So we need to set this right. To do this, we need to go to our alternate function high register. Let's open the reference manual. I'll do Ctrl-F,
	 alternate. We see alternate function AFR. So we're looking for alternate function register high because we're dealing with pin 8 and 9. So these bits here, bit 0 to bit 3, are for pin 8. Bit 4 to bit 7 are for pin 9 because this
	 is AFRH9. So pin 9, AFRH8 for pin 8. So we said our I2C configuration is AF4. So the first bit has to be 0. The second bit has to be 0. Bit number 3, the third bit has to be 1, and then the fourth bit has to be 0. So we're going
	 to do that. So over here, after we set the pull-up and pull-down register, I'm going to say set alternate function, set PB8 and PB9 alternate function type to AF4. So we start with PB8. PB8, we said the first bit has to be 0, the
	 second bit 0, the third bit 1, and then the fourth bit, which is bit 3, has to be 0. Now we'll do the same for PB9. So I'll copy this. Then over here, PB9 starts from bit 4 to 7. The first bit has to be 0, the second bit has to be
	 0, the third bit has to be 1, and the final bit has to be 0.*/
	GPIOB->AFR[1] &=~(1U<<0);
	GPIOB->AFR[1] &=~(1U<<1);
	GPIOB->AFR[1] |=(1U<<2);
	GPIOB->AFR[1] &=~(1U<<3);

	GPIOB->AFR[1] &=~(1U<<4);
	GPIOB->AFR[1] &=~(1U<<5);
	GPIOB->AFR[1] |=(1U<<6);
	GPIOB->AFR[1] &=~(1U<<7);


	/* enabling clock access to I2C1 - to enable the I2C clock, we simply access the APB1ENR and then we enable the I2C1 bit, which is bit 21. I2C1EN is the symbolic name. */
	RCC->APB1ENR |= I2C1EN;

	/*Enter reset mode - In order to configure the I2C we need to enter reset mode.Go to reference manual, go to I2C_CR1. Bit number 15 here is called the software reset, when it's 0, peripheral not under reset and
	  when it's 1, peripheral under reset. So we need to set bit 15 to 1.  */
	I2C1->CR1 |= (1U<<15);

	/*Come out of reset mode - It's just a requirement to enter reset and come out of reset. So to come out of reset, we set this same bit to 0. */
	I2C1->CR1 &=~(1U<<15);

	/*Set Peripheral clock frequency -We need to configure the peripheral clock. So our default clock frequency is 16 MHz and this is the same frequency for the bus that the I2C1 is connected to, which is APB1.So we're going to set this.
	  To do this, we need to enter the I2C control register 2 and over here, we want to set the bus frequency and set it at the range here called Freq. Let's see what it means.It's bit 0 to bit 5. We need to set the binary
	  value that corresponds to the particular clock frequency. So if our frequency is 1 Hz, we would have just put 1 at bit number 0 . If our frequency is 2 Hz,
	then we would put 1 at bit number 1 and then all other bits would be 0 because that's the binary value or the binary number for 2. So because our clock is 16 MHz, so all of it is in MHz. If our clock is
	1 MHz, we simply write 1 to bit 0, binary 1, which is setting bit 0 to 1 and all other bits to 0. If our clock is 2 MHz, we'll write 1 at bit position 1, which is binary 2 then all other bits will be set
	to 0.Our clock is 16 MHz.So what we're going to do is write 1 at bit position 4, such that we'll end up with 001000. and we know when we have 1 at the 5th position we get 2 raised to the power 4.
	If we want to convert it to decimal 2 raised to the power 4, which is 16. So we'll get 16 here. */
	I2C1->CR2 = (1U<<4);  //16MHz


	/*Set I2C to standard mode , 100kHz clock - To do this, we're going to set this inside the Clock Control Register (CCR) register. Go to I2C_CCR.  So over here, bits 0 through 11. Here, we're going to set
	 it to 80. This will give us I2C standard mode, which is 100 kilohertz. Create a symbolic name ,I2C_100KHZ. We're simply going to pass the decimal number 80 into our CCR register. Then, we're going to
	have another symbolic name known as the standard mode maximum rise time,SD_MODE_MAX_RISE_TIME, this value is 17. One way of computing these values is having Latin Cube MX computer for it you. There are various
	tools for computing the rise time and the value to input into the I2C registers. Various automatic tools for doing that. So, we're going to set this in the CCR register. And then, the rise time,
	we'll set it to 17. So, we'll use our standard mode value, I2C 100 kilohertz. We will place that into our CCR register.And then, there's another register known as TRISE. So, inside this register, we'll set
	the rise time. Go to I2C_TRISE, we're going to set the rise time from bit 1 to 5. We created a symbolic name for the maximum rise time for standard mode, which is 17. We're going to pass that into the TRISE register  */
	I2C1->CCR = I2C_100KHZ; //Set I2C to standard mode, 100khz clock
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;  //Set rise time
	I2C1->CR1 |= CR1_PE;  //Enable I2C1 module  - Go to the I2C control register, PE here is bit 0 is peripheral enable

}

/*This is a function that would allow our I2C to read a byte of data from the slave device. So this function is going to have three arguments. The first argument will be the address of the slave. The second argument will be the memory
 address within the slave that we want to read from. And the third argument will be a pointer of a variable where we want to store the data we read. We're going to have the slave address, memory address, and then data.So first thing
 we have to do is we would have to make sure that our device is not busy, the I2C is not busy, so we have to wait for it not to be busy. To do this, we have to check the busy flag in the I2C status register.find I2C_SR2, which is the
 status register. Bit number 1 in I2C_SR2 status register 2 will tell us if the device is busy or not. Created a symbolic name for this, SR2_BUSY. Once we've waited for it, once it's on busy, then we can go on to start the process.
 The first thing we need to do is generate a start condition. To generate a start condition, we need to set the start bit in the control register (CR1) to high. Go to I2C_CR1, bit number eight here is called the start bit. So we need
 to set bit number eight to one. Created a symbolic name, CR1_START.  So once the start condition is generated, we have to make sure or we have to wait until the start flag is set. So whenever we want to wait for a flag to be set, we
 need to go to one of our status registers.Go to I2C_SR1. The start flag is called the SB, the start bit,bit number zero.So when this is zero, no start bit. When this is one, there is a start bit. Created a symbolic name,SR_SB.
 Now we can transmit the slave address plus write.So we transmit the address, let's say we want to write to the slave. So to do that, we're going to transmit this into the I2C data register and then we're going to shift to
 position one,go to I2C_DR for this.Next, we need to clear the ADDR flag. When it is set, we're going to come out of the loop. When we come out of it, we want to clear it. We want to clear the ADDR flag. We can clear it by reading
 the SR2 register and store its content in temp. That's why we created temp. Then we will send the memory address, for this we will send it to our data register. Once that is sent, we have to wait until the data register is
 empty. To wait until the data register is empty, we need to check the TXE flag in the status register 1. So TXE is bit number 7.So once this is empty, we're going to generate another start condition. So because we've generated a
 first start condition, we call this a restart condition.Once we've generated a restart, we would have to wait again until the start flag is set. Whenever we generate a start condition, we wait until it is set.So once this is set,
 then we can transmit the slave address plus read, because we want to read something.First, all this time we've been transmitting the slave address and then memory within the slave that we want to read from. And to do that, we
 have to write to the slave. So once all of this is set, we would transmit.Now we will read and transmit the slave address.Once this is done, we have to wait until the address flag is set.So just like we did before, we wait until
 the address flag is set. Once the address flag is set, we're going to disable the Acknowledge.In I2C_CR1, there is a flag for ACK, bit 10.Then we're going to clear the address flag again by reading status register 2 and storing it
 in tmp.Since we've just disabled the ACK, we've got to generate our stop condition. In I2C_CR1, there is a bit for stop,it's bit 9.Then we will wait until the RXNE flag is set,RXNE is in status register 1.In I2C_SR1 we want to wait
 for the receive buffer not empty flag. RXNE is bit 6. So receive buffer not empty, meaning there is something that we've received.So then we're going to take that something and store it in our variable which is called data which is
 our pointer variable (*data).  So we read the data register and store it. */
void I2C1_byteRead(char saddr, char maddr, char* data){

	volatile int tmp; //local variable tmp , to use it to read the register just to clear it
    /*wait until bus not busy - to check if the device is busy or to wait. What we want to do actually is wait for it to be free. So we'll wait until it is not busy.That is why we do not have the exclamation sign here.*/
	while(I2C1->SR2 & (SR2_BUSY)){}

	I2C1->CR1 |=CR1_START;  //generate start

	/*we'll wait for SB to be set.We'll wait until the start bit is set.Look carefully to see the distinction here.We are saying while false because we have the exclamation sign here. So while the start bit is not set, we will get
	 stuck here. When it's set, we'll come out of this loop. */
	while(!(I2C1->SR1 & (SR1_SB))){}

	I2C1->DR = saddr<<1;  //Transmit slave address + write

	/*Wait unitl address flag is set -  addr is the address flag in Status Register, I2C_SR1.*/
	while(!(I2C1->SR1 & (SR1_ADDR))){}

	tmp= I2C1->SR2;     /*clear addr flag*/
	I2C1->DR = maddr;  /*send memory address*/
	while(!(I2C1->SR1 & (SR1_TXE))){}  /*wait until the transmitter is empty*/

	I2C1->CR1 |=CR1_START;  //generate restart
	while(!(I2C1->SR1 & SR1_SB)){} //wait until start flag is set
	I2C1->DR = saddr<<1 |1 ;  //Transmit slave address + read
	while(!(I2C1->SR1 & (SR1_ADDR))){}  //Wait unitl addr flag is set
	I2C1->CR1 &= ~CR1_ACK;  //disable acknowledge
	tmp= I2C1->SR2;     /*clear addr flag*/
	I2C1->CR1 |=CR1_STOP;  //generate stop after data recieved
	while(!(I2C1->SR1 & (SR1_RXNE))){}  //Wait unitl RXNE flag is set
	*data++= I2C1->DR;  //read data from DR
}

/*This function is to develop the I2C burst read function,this function would allow us to read multiple bytes.So, the function is going to have four arguments.The first argument is going to be the address of the slave. The second
 argument will be the memory, the starting memory location or the memory location rather of where you want to read the data and then following that is the number of bytes you want to read and then a pointer to the storage where
 you want to store what you read. So, we're going to have the tmp variable again because we need to clear the address flag by reading SR2. Like we did earlier, we have to wait until the bus is not busy. So, we start by waiting,
 and once we are given the go ahead, once it is not busy, we would generate a start condition. Just like we did before, set the start bit to 1 in the control register 1. Next, we wait for the start bit to be set.we read the status
 register 1 to make sure the start bit is set. We have to wait until it is set. Like we did before, we use a while loop again to wait until it is set.Once it is set, we're going to transmit the slave address plus write.
 This is similar to what we did in our byteread function.We transmit a slave address plus write, and then we wait until the address flag is set and then once the address flag is set, we're going to clear the address flag by reading
 SR2. We're going to read it and store it in our tmp variable that we created.so once that is done, we're going to, we're going to make sure the transmitter is empty.Wait until it is empty and as soon as it is empty, we're going to
 send the memory address we want to access so we send the memory address by writing it into the data register, and then we wait again for transmitter to be empty. It is advisable to always wait for transmitter to be empty before
 you transmit.So we wait for transmitter to be empty again.Once that is done, we're going to send a restart condition which is just a start condition, but because we've sent the first one, we call this the restart. So we do the same
 thing by setting the start bit in control register 1 to 1. So when we do that, we have to wait for the start bit to be set by inspecting the status register 1 start bit. So once that is done, we're going to transmit the slave address
 plus read. So just like we did earlier, transmit slave address with read and we wait until the address flag is set.Then we clear the address flag, we clear by reading SR2. Once we've cleared the address flag, we're going to enable
 acknowledge by setting the ACK bit to 1. Now we will say while the number of byte (n) passed an argument, while this is equal to 0, we want to do the number of things. (furthar comments are added above while loop for more clarity
 with the code itself.) */

void I2C1_burstRead(char saddr, char maddr,int n, char* data){

	volatile int tmp;

	while(I2C1->SR2 & (SR2_BUSY)){}  //wait until bus not busy
	I2C1->CR1 |=CR1_START;  //generate start
	while(!(I2C1->SR1 & (SR1_SB))){}  //wait until start flag is set
	I2C1->DR = saddr<<1;  //Transmit slave address + write
	while(!(I2C1->SR1 & (SR1_ADDR))){} //Wait unitl address flag is set
	tmp= I2C1->SR2;     //clear addr flag
	while(!(I2C1->SR1 & (SR1_TXE))){}  //wait until the transmitter is empty
	I2C1->DR = maddr;  //send memory address
	while(!(I2C1->SR1 & (SR1_TXE))){}  //wait until the transmitter is empty
	I2C1->CR1 |=CR1_START;  //generate restart.
	while(!(I2C1->SR1 & SR1_SB)){} //wait until start flag is set
	I2C1->DR = saddr<<1 |1 ;  //Transmit slave address + read
	while(!(I2C1->SR1 & (SR1_ADDR))){}  //Wait unitl addr flag is set
	tmp= I2C1->SR2;     /*clear addr flag*/
	I2C1->CR1 |= CR1_ACK;  //enable acknowledge


	/* while n is greater than 0, we just put U here to indicate it's unsigned.We check if it's one byte left or if n is 1 or if the argument passed is 1 or if the n value here equals 1, the reason the n value be equal to 1
	 is we'll be decrementing the n value we would disable the ACK Knowledge and once we've done that, we'll generate a stop condition. Once we've generated a stop condition, we wait for the RXNE flag to be set. In other words,
	 we wait until the receive buffer is not empty and then we read the data from the data register.This is what we would execute when n equals 1.But if n is not equal to 1, we're going to implement an else condition here. All
	 we want to do is wait for the data to arrive by inspecting the RXNE. Once that is done, we're going to store it in our variable data. Then we're going to decrement n by saying n--. */
	while(n> 0U){
		if(n == 1U) // if one byte
		{
			I2C1->CR1 &=~CR1_ACK; //disable acknowledge
			I2C1->CR1 |= CR1_STOP; //generate stop
			while(!(I2C1->SR1 & SR1_RXNE)){}  //wait for RXNE flag to set
			*data++ = I2C1->DR;  //read data from DR
			break;
		}
		else
		{
			while(!(I2C1->SR1 & SR1_RXNE)){}	//wait for RXNE flag to set
			*data++ = I2C1->DR;  //read data from DR
			n--;
		}
	}
}
/*We shall implement the I2C burst write function.The function prototype is the first argument is slave address (saddr), second argument is memory address(maddr) then number of bytes (n) and then a pointer variable called data.This
 time the last argument is going to hold the data we want to write to the I2C slave device. In our read functions, this argument was an empty variable that stores what we want to read from the I2C.So we start off like we did before,
 we're going to create our tmp variable here, because we would have to clear the address flag. So we'll create a tmp over here. Once that is done, we're going to wait until the bus is not busy like we did earlier. Wait for the bus
 not to be busy and then we're going to generate a start condition. We always start with a start condition. Once that is done, we wait for the start flag to be set in the status register 1. Once the start flag is set, we can go ahead
 and transmit the slave address plus write.Then,we'll wait until the address flag is set. And then we clear the address flag by reading status register 2. Then we wait until the data register is empty by reading the TXE flag in the
 status register 1. Then we send the memory address, because we want to read multiple bytes, we're going to use a for loop.This loop is going to transmit all the bytes. It's going to write all the bytes to the I2C.Once the loop is
 complete, we would have to wait until the BTF flag is set.Find  BTF flag in I2C_SR1. So this is called a byte transfer finished, it's bit number 2, set it to 1.Once that is done, we're going to generate a stop condition.

 */
void I2C1_burstWrite(char saddr, char maddr,int n, char* data){

	volatile int tmp;

	while(I2C1->SR2 & (SR2_BUSY)){}  //wait until bus not busy
	I2C1->CR1 |=CR1_START;  //generate start
	while(!(I2C1->SR1 & (SR1_SB))){}  //wait until start flag is set
	I2C1->DR = saddr<<1;  //Transmit slave address + write
	while(!(I2C1->SR1 & (SR1_ADDR))){} //Wait unitl addr flag is set
	tmp= I2C1->SR2;     //clear addr flag
	while(!(I2C1->SR1 & (SR1_TXE))){}  //wait until the transmitter is empty
	I2C1->DR = maddr;  //send memory address

	for(int i=0; i <n; i++){
		while(!(I2C1->SR1 & (SR1_TXE))){}  //wait until data register is empty,once it is empty, we transmit the next piece of data and then we increment
		I2C1->DR = *data++;  //transmit memory address, here data is going to be an array that has what we want to write
	}
	while(!(I2C1->SR1 & (SR1_BTF))){}  //wait until transfer finished
	I2C1->CR1 |= CR1_STOP; //generate stop

}







