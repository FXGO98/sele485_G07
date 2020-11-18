#define F_CPU		16000000

#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD_RATE 9600

#define SLAVE1 0x01
#define SLAVE2 0x02

volatile uint8_t id = 0x00;

static uint8_t pc1_prev = 1, pc2_prev = 1;

ISR(USART_RX_vect){	// Interrupt responsável por tratar dos dados recebidos

  static uint8_t error, frame_addr, frame_data;
  
  error = UCSR0A;
  frame_addr = UCSR0B;
  frame_data = UDR0;
	
  if ( error & ((1<<FE0)|(1<<DOR0)|(1<<UPE0)) )
        return;
  
  if(frame_addr & (1<<RXB80)){        // Caso seja uma frame de adereço
	
    if(frame_data == id){	// Caso esta frame seja destinada para este slave permite a recepção de pacotes de dados
    	UCSR0A &= ~(1<<MPCM0);	
    }else{
    	UCSR0A |= (1<<MPCM0);
    }
 	return;
  }else{   // Só é possível entrar neste modo se for o slave que se encontra selecionad
 
    switch(frame_data){
       	case 0:
    		PORTB &= ~(1 << PB5);
        break;
        case 1:
        	PORTB |= (1 << PB5);
        break;        
      }
    
     UCSR0A |= (1<<MPCM0);
  }
	
}

void rs485_send(uint8_t slave_id, uint8_t data){
  
  PORTC |= (1<<PC0);	// Permite a transmissão (enable no max485)
  /*
  while(!( UCSR0A & (1<<UDRE0)));
  UCSR0B |= (1<<TXB80);  // Para enviar frames de dados
  UDR0 = slave_id; 
	*/
  while(!( UCSR0A & (1<<UDRE0)));
  UCSR0B &= ~(1<<TXB80); 
  UDR0 = data;    // Envia o byte de dados a enviar
 
  PORTC &= ~(1<<PC0); // Desativa a transmissão (enable no max485)

}

int main(void){
  
  DDRB &= ~((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3));	// Define as entradas dos DIP switches
  //PORTB |= ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3));	// Define os pull-ups para os DIP switches
  
  DDRC |= (1<<PC0);	// Define o pin de w/r como saída
  
  id = PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3));	// Define o id do device
  
  UBRR0H = (uint8_t)(BAUD_RATE>>8);
  UBRR0L = (uint8_t)BAUD_RATE;
  
  UCSR0B = (1<<UCSZ02)|(1<<RXEN0)|(1<<TXEN0);	// Define que suporta transmissão e recepção
  UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);	// Define que usa 9-bits
  
  
  if(id){
    DDRB |= (1<<PB5);   // Define a saída do LED
    UCSR0A |= (1<<MPCM0);	// Define que o slave ignora todas as mensagens que não sejam de adereço
    UCSR0B |= (1 << RXCIE0);  // Faz enable dos interrupts ao receber um pacote
  }else{
  	DDRC &= ~((1<<PC1)|(1<<PC2));	// Define a entrada dos botões		
  }
   
  sei();

  while(1){ 
    
    if(!(PINC & (1<<PC1))){	// Verifica se o botão 1 foi premido ou não, para evitar enviar repetidamente dados, verifica se houve um transiçao
      if(pc1_prev){
        rs485_send(SLAVE1,0x01);
        pc1_prev = 0x00;
      }
    }else{
      if(!pc1_prev){
        rs485_send(SLAVE1,0x00);
        pc1_prev = 0x01;
      }
    }
    
    if(!(PINC & (1<<PC2))){ // Verifica se o botão 2 foi premido ou não, para evitar enviar repetidamente dados, verifica se houve um transiçao
      if(pc2_prev){
        rs485_send(SLAVE2,0x01);
        pc2_prev = 0x00;
      }
    }else{
      if(!pc2_prev){
        rs485_send(SLAVE2,0x00);
        pc2_prev = 0x01;
      }
    }
  
    
   }

  
}
