/*
    Desenvolvido por: João Conceição e Francisco Oliveira
*/

#include <Arduino.h>
#include <RS_485.h>

RS_485 rs;

unsigned char id = 0;
unsigned char b1 = 0, b2 = 0;

void setup() {

    DDRB = DDRB | 0x20; // Define o port P5 como output (LED)
    DDRC = DDRC & 0x06; // Define os ports PC1 e PC2 como inputs (Botões de controlo)
    DDRD = DDRD & 0xF0; // Define os ports PB0, PB1, PB2 e PB3 como inputs (DIP Switches)
    
    rs.setup();
    rs.setBaudRate(9600);
    id = PIND & 0x0F;
    rs.setId(id);

    b1 = ((PINC & (1<<PINC1) )>>PINC1);
    b2 = ((PINC & (1<<PINC1) )>>PINC2);
}

void loop() {
  

  if(id){ // If slave

    rs.read();
    if(!rs.rx_buffer[0])
      PORTD = PORTD & 0xDF;
    else
      PORTD = PORTD | 0x20;

  }else{  // If master

    if( ((PINC & (1<<PINC1) )>>PINC1) != b1 ){
      rs.write(&b1, 1, 1);
      b1 = ((PINC & (1<<PINC1) )>>PINC1);
    }
    if( ((PINC & (1<<PINC2) )>>PINC2) != b2 )
      rs.write(&b2, 1, 2);
      b2 = ((PINC & (1<<PINC2) )>>PINC2);
  } 

}