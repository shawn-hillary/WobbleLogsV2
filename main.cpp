/*
******************************************************************************
File:     main.cpp
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-11-06

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "stm32f0xx.h"
#include "lcd_stm32f0.h"
#include "lcd_stm32f0.c"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
void I2C_StartWrite(int NBytes){
	 I2C1->CR2 &= ~I2C_CR2_RD_WRN;			//Set read or Write
	 I2C1->CR2 |= (NBytes<<16);				//Set Number of Bytes to send
	 I2C1->CR2 |= I2C_CR2_START;			//Establish Start Condition
	 while (I2C1->CR2 & I2C_CR2_START);		//Check that the start condition is met.
}

void I2C_StartRead(int NBytes){
	I2C1->CR2 |= I2C_CR2_RD_WRN;			//Set read or Write
	I2C1->CR2 |= (NBytes<<16);				//Set Number of Bytes to send
	I2C1->CR2 |= I2C_CR2_START;				//Establish Start Condition
	while (I2C1->CR2 & I2C_CR2_START);		//Check that the start condition is met.
}

void acceleroWake(){
	lcd_putstring("Waking");
	I2C1->CR2 |= (0x1D<<1);					//Set slave address
	I2C_StartWrite(1);
	I2C1->TXDR = 0x0B;						//Write a byte to the transmit data register
	lcd_command(LINE_TWO);
	lcd_putstring("up ");
	while(!(I2C1->ISR & I2C_ISR_TXE));		//Confirm that TXdata is empty i.e. Sent
	lcd_putstring("up ");
	I2C_StartWrite(1);
	I2C1->TXDR = 0x01;						//Write a byte to the transmit data register
	lcd_putstring("up ");
	while(!(I2C1->ISR & I2C_ISR_TXE));		//Confirm that TXdata is empty i.e. Sent
	lcd_putstring("up");
	I2C1->CR2 |= I2C_CR2_STOP;				//Establish stop condition
	//while (I2C1->CR2 & I2C_CR2_STOP);			//Confirm stop condition
}

int8_t readReg(char regAddress){
	I2C_StartWrite(1);
	I2C1->TXDR = regAddress;						//Write a byte to the transmit data register
	while(!(I2C1->ISR & I2C_ISR_TXE));				//Confirm that TXdata is empty i.e. Sent
	I2C_StartRead(1);
	lcd_putchar(0x52);
	I2C1->CR2 |= I2C_CR2_STOP;						//Establish stop condition
	// while (I2C1->CR2 & I2C_CR2_STOP);			//Confirm stop condition
	// while(~(I2C1->ISR & I2C_ISR_RXNE));			//Check RXdata not-empty
	int8_t regData = I2C1->RXDR;				// REad RXdata
	return regData;
	}
/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  int i = 0;
  	++i;
  /* TODO - Add your application code here */
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= 	~(GPIO_MODER_MODER6_0|
  		  	  		GPIO_MODER_MODER7_0);
  GPIOB->MODER |= 	(GPIO_MODER_MODER6_1|
		  	  		GPIO_MODER_MODER7_1);
  GPIOB->OTYPER |= 	~(GPIO_OTYPER_OT_6|
		  	  	   	  GPIO_OTYPER_OT_7);
  GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR6_0|
  	  	  	  	  	 GPIO_OSPEEDR_OSPEEDR7_0);
  GPIOB->PUPDR &= 	~(GPIO_PUPDR_PUPDR6|
		  	  	  	  GPIO_PUPDR_PUPDR7);
  GPIOB->AFR[0] = 0b00010001000000000000000000000000;

  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  I2C1->TIMINGR = 0x0010020A;
  I2C1->CR1 |= I2C_CR1_PE;					//Enable I2C
  init_LCD();

  I2C1->CR2 |= (0x1D<<1);					//Set slave address
 // acceleroWake();
//  delay(50000);
  lcd_command(CLEAR);

  /* Infinite loop */
  while (1)
  {
	  lcd_putdata(readReg(0x05));
  }
}
