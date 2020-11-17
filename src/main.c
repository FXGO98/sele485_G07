//Defining clock frequency as 16MHz
#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD_RATE 9600

//Led Pin
#define LED PB5

//Buttons Pins
#define B1 PC1
#define B2 PC2

//Dip Switches Pins
#define D3 PB3
#define D2 PB2
#define D1 PB1
#define D0 PB0

//Pin that enables Writing or Reading Data 
#define W_R_ENABLE PC0


void PCINT0_setup()
{
  //Choosing Pins to monitor change
  PCMSK0 |= ((1 << PCINT0)|(1 << PCINT1)|(1 << PCINT2)|(1 << PCINT3));

  //Enabling Pin Change Interrupt 0
  PCICR |= (1 << PCIE0);

}


int main(void)
{

  //Defining Pins as Outputs
  DDRB = (1 << LED);
  DDRC = (1 << W_R_ENABLE);

  //Defining Pins as Inputs
  DDRB &= ~((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3));
  DDRC &= ~((1 << B1)|(1 << B2));

  //Enabling Pull-Up Resistors
  PORTB = ((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3));
  PORTC = ((1 << B1)|(1 << B2));

  //Master or Slave configuration
  uint8_t node_mode; 


  if(PINB & ((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3)))
  {
    //Master Configuration
    node_mode = 1;
  }
  else
  {
    //Slave Configuration
    node_mode = 0;
  }
  


  while(1)
  {
    //Master Code
    if(node_mode==1)
    {

    }

    //Slave Code
    if(node_mode==0)
    {
      
    }
  }
}

ISR(PCINT0_vect)
{
  
}
