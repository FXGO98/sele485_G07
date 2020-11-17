#define F_CPU		16000000

#include <avr/io.h>

#define BAUD_RATE 9600

unsigned char id = 0;

void rs485_send(unsigned char slave_id, unsigned char data){
  
  while ( !( UCSR0A & (1<<UDRE0)) );
  
  UCSR0B |= (1<<TXB80);  // Para enviar frames de dados
  
  UDR0 = slave_id;
  
  while ( !( UCSR0A & (1<<UDRE0)) );

  UCSR0B &= ~(1<<TXB80); 

  UDR0 = data;    // Envia o byte de dados a enviar

}

unsigned char rs485_read(){
  
  unsigned char error, frame_addr, frame_data;
  
  while ( !(UCSR0A & (1<<RXC0)) );
  
  error = UCSR0A;
  frame_addr = UCSR0B;
  frame_data = UDR0;
	
  if ( error & ((1<<FE0)|(1<<DOR0)|(1<<UPE0)) )
        return 0xFF;
  
  if( (frame_addr & (1<<RXB80)) && (frame_data == id)){        // Caso seja uma frame de adereço
	
    UCSR0A &= ~(1<<MPCM0);
    return 0xFF;
 
  }else{                                  // Só é possível entrar neste modo se for o slave que se encontra selecionad
    UCSR0A |= (1<<MPCM0);
    return frame_data;    

  }
  
}

unsigned char last_b1 = 0, last_b2 = 0;

int main(void){
  
  DDRB &= ~((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3));	// Define as entradas dos DIP switches
  PORTB |= ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3));
  
  id = 0x0F ^ (PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)));
  
  UBRR0H = (unsigned char)(BAUD_RATE>>8);
  UBRR0L = (unsigned char)BAUD_RATE;
  
  UCSR0B = (1<<UCSZ02)|(1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
  
  if(id){
  	DDRB |= (1<<PB5); // Define a saída do LED
    UCSR0A |= (1<<MPCM0);	// Define que o slave ignora todas as mensagens que não sejam de adereço
  }else{
  	DDRC &= ~((1<<PC1)|(1<<PC2));	// Define a entrada dos botões
  	PORTC |= (1 << PC1) | (1 << PC2);	// Define os pull-ups dos botões			
  }
   
  while(1){
   
    if(!id){
      	
      //if(!(PINC & (1 << PC1)) != last_b1){
   		//if( !(PINC & (1 << PC1)) ){
          //	rs485_send(1,1);
        //}else{
         // 	rs485_send(1,0);
        //}
        //last_b1 = !(PINC & (1 << PC1));
      //}
      	/*if( !(PINC & (1 << PC2)) ){
          	rs485_send(2,0);
      	}*/
      rs485_send(1,0xFF);
    }else{
      	
      switch(rs485_read()){
       	case 0:
    		PORTB &= ~(1 << PB5);
        break;
        case 1:
        	PORTB |= (1 << PB5);
        break;        
      }

    }
    
   /* 
   if(!id){
    	while (!( UCSR0A & (1<<UDRE0))); 
          UDR0 = 0x01; 
    }else{
      	  while ( !(UCSR0A & (1<<RXC0)) );
      	  if(UDR0)
      	  	PORTB |= (1 << PB5);
    }
 */
  }
  
}




