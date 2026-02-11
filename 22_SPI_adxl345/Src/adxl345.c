#include "stm32f4xx.h"
#include "adxl345.h"

#define  MULTI_BYTE_EN       0X40
#define  READ_OPERATIONS     0x80

/*Pinout of adxl345 and the microcontroller
 SCL------PA5
 SDA------PA7
 SDO------PA6
 CS-------PA9
 VCC------5V
 GND------GND */

/*here, firstly we have created  symbolic name known as READ_OPERATIONS. The value for this is 0x80.Now, what we will bring our buffer over here of size two.Next, Enable multibyte. Then, set read operation.
 To do this, it's as simple as performing an OR operation. So whatever the value of the address is, we're going to perform an OR operation with the symbolic name ReadOperation. Then we're going to take what
 we get and then perform an OR operation with MultiLightEnable. So our final address will have the value of ReadOperation and MultiLightOperation added to it.Once this is done, we will pull the CS line load to enable the slave.
 Next , we're going to send the address of the memory. Location that we want to write.To send address, we call our transmit function of size one. We pass size as one.Over here, we need not use the buffer. We can simply send the
 address directly because we manipulated it outside of the buffer. So we can simply transmit to the address.To read, remember we want to read the data. we've got six data items that we want to read X0,X1,Y0,Y1,Z0,Z1.
 And we combine X0, X1 to get the X value like we did in the I2C lesson. So to do this, we're going to say we want to receive a piece of data. We're going to create a buffer to store the received data.We can let this be passed as
 argument to this function. We'll call this rxdata.To do this, we call our SPI receive function. Then, we want to store the received data into our rxdata in a size of six.Next, once we've done this, we're going to pull the CS
 line I to disable the slave device. */
void adxl_read(uint8_t address, uint8_t *rxdata){

	uint8_t data[2];
	address |= READ_OPERATIONS;//set read operation
	address |= MULTI_BYTE_EN;//enable multi byte
	cs_enable(); //pull cs line low to enable slave
	spi1_transmit(&address,1); //send address
	spi1_receive(rxdata,6);  //read 6 bytes
	cs_disable();//pull cs line high to disable slave

}


/*In write function ,we start off with a local buffer called data of size 2 and this time, using our slave device with SPI, we have to enable multibyte.To enable multibyte, we'll create a symbolic name, MULTI_BYTE_EN.
 and this is found in our ADXL345 documentation.Then, we want to place address and data into data buffer.So, the address will be passed as argument.Then, we perform an OR operator, with  multibyte enable.
 Then, we'll place that at index zero and index one, we place the value that is passed. So, we want to write  value to address. Once we've done this, we will pull the CS line to enable the slave.So we call our CS enable function.
 Remember, our cs_enable function sets PA9 to low.Once this is done, we're going to transmit the data. To transmit the data, we call our SPI1 transmit function. The first argument is the buffer that has the data. The second argument
 is the size of the data we are going to transmit. So I'll say over here, transmit data and address, like this.Next, we're going to pull the CS line high to enable the slave select. This is the sequence to write to the SPI slave. */
void adxl_write(uint8_t address, uint8_t value){

	uint8_t data[2];
	data[0] = address | MULTI_BYTE_EN;  //enable multi-byte , place address into buffer
	data[1] = value;//place data into buffer
	cs_enable(); //pull cs line low to enable slave
	spi1_transmit(data,2);//transmit data and address
	cs_disable();//pull cs line high to disable slave


}

void adxl_init (void){

	spi_gpio_init();  //Enable SPI gpio
	spi1_config();   //config SPI
	adxl_write(DATA_FORMAT_R, FOUR_G); //set the data format range to +/- 4G
	adxl_write(POWER_CTL_R,RESET);  //reset all bits
	adxl_write(POWER_CTL_R,SET_MEASURE_B);  //configure power control measure bit

}







