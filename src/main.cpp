//Defining clock frequency as 16MHz
#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD_RATE 9600
#define BRC ((F_CPU/(BAUD_RATE<<4))-1) //Baud Rate Calculator

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

//Pin que permite a Escrita ou Leitura de dados
#define W_R_ENABLE PC0

//Slave IDs
#define SLAVE1 0x01
#define SLAVE2 0x02



volatile uint8_t id = 0x00;

static uint8_t b1_prev = 1, b2_prev = 1;


ISR(PCINT0_vect)
{
  // Define o id do device lendo o input dos DIP Switches
  id = PINB & ((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3));
}



ISR(USART_RX_vect){	// Interrupt responsável por tratar dos dados recebidos

  static uint8_t error, frame_addr, frame_data;
  
  error = UCSR0A;
  frame_addr = UCSR0B;
  frame_data = UDR0;
	
  if ( error & ((1<<FE0)|(1<<DOR0)|(1<<UPE0)) )
    return;
  
  if(frame_addr & (1<<RXB80)){        // Caso seja uma frame de endereço
	
    if(frame_data == id){	// Caso esta frame seja destinada para este slave permite a recepção de pacotes de dados
    	UCSR0A &= ~(1<<MPCM0);	
    }else{
    	UCSR0A |= (1<<MPCM0);
    }
 	return;
  }else{   // Só é possível entrar neste modo se for o slave que se encontra selecionad
 
    switch(frame_data){
       	case 0x00:
    		PORTB &= ~(1 << PB5);
        break;
        case 0x01:
        	PORTB |= (1 << PB5);
        break;        
      }
    
     UCSR0A |= (1<<MPCM0);
  }
	
}


void PCINT0_setup()
{
  //Choosing Pins to monitor change
  PCMSK0 |= ((1 << PCINT0)|(1 << PCINT1)|(1 << PCINT2)|(1 << PCINT3));

  //Enabling Pin Change Interrupt 0
  PCICR |= (1 << PCIE0);

}


void rs485_send(uint8_t slave_id, uint8_t data){
  
  while(!( UCSR0A & (1<<UDRE0)));	// Espera até o buffer do TX esteja vazio
  UCSR0B |= (1<<TXB80);  // Para enviar frames de endereço
  UDR0 = slave_id; // Envia o adreço do slave a falar com
	
  while(!( UCSR0A & (1<<UDRE0))); // Espera até o buffer do TX esteja vazio
  UCSR0B &= ~(1<<TXB80); // Para enviar frames de dados
  UDR0 = data;    // Envia o byte de dados a enviar

}


void usart_init()
{

  UBRR0H = (uint8_t)(BRC>>8);
  UBRR0L = (uint8_t)BRC;


  // Define que usa 9-bits, paridade impar e 1 stop-bit
  UCSR0C = (1<<UCSZ00)|(1<<UCSZ01)|(1<<UPM00)|(1<<UPM01)|(0<<USBS0);
  UCSR0B = (1<<UCSZ02);


  if(id == 0x00)
  {
    UCSR0B = (1<<TXEN0);	// Define que suporta transmissão
  }
  else
  {
    UCSR0B = (1<<RXEN0);	// Define que suporta recepção
    UCSR0A |= (1<<MPCM0);	// Define que o slave ignora todas as mensagens que não sejam de endereço
    UCSR0B |= (1 << RXCIE0);  // Faz enable dos interrupts ao receber um pacote
  }
  
}

int main(void){
  
  DDRB &= ~((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3));	// Define os DIP switches como entradas
  DDRC &= ~((1<<B1)|(1<<B2));	// Define os botões	como entradas	
  
  DDRC |= (1<<W_R_ENABLE);	// Define o pin de w/r como saída
  DDRB |= (1<<LED);   // Define a saída do LED

  //Ativar Resistências de Pull-Up
  PORTB = ((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3));
  PORTC = ((1 << B1)|(1 << B2));
  

  // Define o id do device lendo o input dos DIP Switches
  id = PINB & ((1 << D0)|(1 << D1)|(1 << D2)|(1 << D3)); 
  
  PCINT0_setup();  

  usart_init();
   
  sei();

  while(1){


    if(id==0x00)
    {
      PORTC |= (1<<W_R_ENABLE);	// Permite a transmissão (enable no max485)

      if(!(PINC & (1<<B1))){	// Verifica se o botão 1 foi premido ou não, para evitar enviar repetidamente dados, verifica se houve um transiçao
        
        if(b1_prev){
          rs485_send(SLAVE1,0x01);
          b1_prev = 0x00;
        }
      }
      
      else{
        
        if(!b1_prev){
          rs485_send(SLAVE1,0x00);
          b1_prev = 0x01;
        }
      
      }
    

    
      if(!(PINC & (1<<B2))){ // Verifica se o botão 2 foi premido ou não, para evitar enviar repetidamente dados, verifica se houve um transiçao
        
        if(b2_prev){
          rs485_send(SLAVE2,0x01);
          b2_prev = 0x00;
        }
      
      }
      
      else{
        
        if(!b2_prev){
          rs485_send(SLAVE2,0x00);
          b2_prev = 0x01;
        }
      
      }

      
    }

    else{
    
      PORTC &= ~(1<<W_R_ENABLE); // Desativa a transmissão (enable no max485)

    }
    
  }
  
}
