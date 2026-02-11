#include "stm32f4xx.h"
#include "adxl345.h"

/*Pinout of adxl345 and the microcontroller
 adxl345-------STM32(Microcontroller)
 SCL-----------PB8
 SDA-----------PB9
 VCC-----------5V
 GND-----------GND

 Download the datasheet for adxl345 from analog devices.it gives a short description.It says it's a small, thin, ultra-low-power, 3-axis accelerometer with high resolution. Go to Register map on page 23. Just like our microcontroller has memory locations, has registers, this one has its own
 registers and we access them using these addresses and we'll see how to go by it.Eventually, further we would configure the device to derive or to read these values, and the reason we have X0, X1, Y0, Y1, Z0, Z1 is we need to combine X0, X1 into one value to get the X value, and then Y0, Y1
 into one value to get a Y value, Z0, Z1 into one value, a single value to get a Z value. In the memory/register map, it says memory location 0x00 is the device ID. We're going to create a symbolic name, DEVID_R  in adxl345.h to hold this memory location together with some other memory locations.
 Now, we need to find a the slave address. Remember, our function is to take the slave address.Go back to adxl345 datasheet, go to the section I2C where it explains the topic. Here it talks about the address.It says with the ALT ADDRESS pin high, the 7-bit I2C address for the device is 0x1D,
 followed by read/write(R/W) bit. This translates to 0x3A for a write and 0x3B for a read. An alternate I2C address of 0x53 followed by R/W bit can be chosen by grounding the ALT ADDRESS pin. This will translate to 0xA6 for write and 0xA7 for read.By default in adxl345 module, this alternate
 address is activated. So our address pin is grounded as it says over here, so 0x53 is going to be the slave address/device address .So we created a symbolic name for this value 0x53 called DEVICE_ADDR. We need to access a number of registers one by one .We have Data Format Register,this one has
 the address 0x31. So we created a symbolic name, DATA_FORMAT_R.Next, Power Control. It's address is 0x2D,POWER_CTRL_R.Next we can go ahead to start initializing, the start address is the address of Data X0. So we're going to indicate this as the data start address. Created a symbolic name ,
 DATA_START_ADDR, 0x32.Next we're going to create a symbolic name for Data Format, 0x31,DATA_FORMAT_R. Next we'll create symbolic names for some constant values we need. We're going to set our device to operate in 4G. So over here, we'll create a symbolic name called FOUR_G and this is found by
 reading the data sheet.Next, we're going to create a value to reset the device RESET which is 0x00. Next we're going to set the measurement bit. So this value is 0x08. What we have done after FOUR_G is, we shall write this value into the data format register to configure the data format range to
 plus or minus 4G, and then we shall reset the power control register. Once we've reset it, we're going to set the measure and wake up at 8 Hz. This is a standard configuration that is used. If you want to get deeper into this, it's just like our registers in our microcontroller.You see the
 register with all the bits, with the bits explained in the datasheet of adxl345. So, device ID register, it's an 8-bit register. Let's see our Power Control Register, the one we said we're going to set its measurement bit, our symbolic name we've set it to 0x8.Here we can see the first bit is 0,
 second bit is 0, third bit is 0, and bit number 4 is 1So, that's why we're calling this the MEASURE_B. So, we are using the same methods we used to access the microcontroller register and give the various bit symbolic names. So, also in our data formats register, we can see the bits that
 correspond to 4G, bits 0 and 1 are known as the range bits. We use bits 0 and 1 to set the range. And if we set bit 0 over here to 1 and bit 1 over here to 0, then we are setting the range to plus or minus 4G. So, that is what we are doing.now.Now, we're going to write a function known as
  ADXL read values. So, we're going to have a function that reads the values.Read description of all the below functions carefully, the functions are created in serial manner, which step should come first and which next , like that.
 */

/*This is a single function that reads registers.Here we're going to pass the register we want to read.What we can do here is take our I2C_byteRead function and we remember that our I2C_byteRead function, the first argument is the slave address, and we created a symbolic name for this slave address
 called device address. The second argument for our byte read function is the memory address and here that is what going to be passed as argument ,the register address and the third argument is it's a variable to store what we read so we're going to create this variable called data, it's going to be
  a global variable that will store what we read. */
char data;
uint8_t data_rec[6];
void adxl_read_address(uint8_t reg){
	I2C1_byteRead(DEVICE_ADDR, reg, &data);
}

/*This is a function to write.Here, we call our burstWrite function because we have just a single write function unlike our read, we have byte read and burst read.We create a local buffer here called data. and we're going to take the value we want to write and place it into data[0] array and thenwe're
 going to call our burstWrite function.The first document is the slave address, second is the memory within the slave that we want to write, third argument is the number of bytes we want to write, and then the last argument is the buffer that contains the information we want to write to the slave.*/
void adxl_write(uint8_t reg, char value){
	char data[1];
	data[0]=value;
	I2C1_burstWrite(DEVICE_ADDR, reg,1, data);
}

/*This function will read multiple values.We use this function to read the data registers which are DATAX0 to DATAZ1, these are six, so we'll set the burst read length to six(refere axdl345 datasheet register map for more details).We create a variable to hold what we read, uint8_t data_rec[6]this is an
  array of size six.Once that is done,we call our first read function, we pass the device ID, the register, the length, and where we want to store what we read. Once we've done this, we can go ahead and initialize the device. */
void adxl_read_values(uint8_t reg){
	I2C1_burstRead(DEVICE_ADDR, reg,6,(char *)data_rec);
}
/*This function is to initialize the device.First enable I2CC module.Then we're going to read the device ID, and when we read the device ID  successfully, this will indicate that our setup is done properly, or this will indicate that we are good.Read the device ID, DEVID , this should return, 0xE5
 according to the datasheet, then we know that everything is looking good. Once we've done this, we're going to set the data format range to plus or minus 4G.Then we're going to reset all bits, and we're going to set the power control measure bit. */
void adxl_init (void){

	I2C1_init();  //Enable I2C
	adxl_read_address(DEVID_R); //read the device ID, DEVID , this should return, 0xE5 according to the datasheet
	adxl_write(DATA_FORMAT_R, FOUR_G); //set the data format range to +/- 4G
	adxl_write(POWER_CTL_R,RESET);  //reset all bits
	adxl_write(POWER_CTL_R,SET_MEASURE_B);  //configure power control measure bit

}







