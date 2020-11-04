/*
    Desenvolvido por: João Conceição e Francisco Oliveira
*/

#include <Arduino.h>
#include "RS_485.h"

/*

    TODO:
        1. Definir e iniciar timer para no caso de colisão entre masters
        2. Terminar a leitura 
        3. Terminar o envio

*/

void RS_485::setAsMaster(){

}

void RS_485::setAsSlave(){

    UCSR0A = (1<<MPCM0);

}
int RS_485::send(unsigned char data){

    while(1){

        UDR0 = data;    // Envia o byte de dados a enviar

        while ( !( UCSR0A & (1<<UDRE0)) );  // Espera até ter enviado o byte todo

        if(data != UDR0)    // Caso o byte presente no registo rx corresponder ao byte enviado que dizer que não houve colisões
            break;

        // TODO : Implementar systema de back off usando timers ( timer2 provavelmente porque tem 16 bits)    
    }

    return 0;
}

void RS_485::setId(unsigned char id){

    if(id)
        setAsSlave();
    else
        setAsMaster();
    
    this->id = id;

}

void RS_485::setup(){

    setBaudRate(9600);

    UCSR0B = (1<<UCSZ02)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);

    setAsSlave();

    this->id = 0;
    this->rx_buffer_index = 0;

};       

void RS_485::setBaudRate(unsigned int rate){

    UBRR0H = (unsigned char)(rate>>8);
    UBRR0L = (unsigned char)rate;

}

int RS_485::write(unsigned char *data, size_t data_size, unsigned char slave_id){
    
    size_t i;

    while ( !( UCSR0A & (1<<UDRE0)) );

    UCSR0B = UCSR0B | (1<<TXB80);   // Para enviar uma frame de adereço

    send(slave_id);
    
    UCSR0B = UCSR0B & (0xff ^ (1<<TXB80));  // Para enviar frames de dados

    for(i = 0; i < data_size; i++) // Envia as frames de dados
        send(data[i]);
    
    return 0;
}

int RS_485::read(){

    unsigned char error, frame_addr, frame_data;

    while ( !(UCSR0A & (1<<RXC0)) );

    error = UCSR0A;
    frame_addr = UCSR0B;
    frame_data = UDR0;

    if ( error & ((1<<FE0)|(1<<DOR0)|(1<<UPE0)) )
        return -1;
    
    if( frame_addr & (1<<RXB80) ){        // Caso seja uma frame de adereço

        UCSR0A = (frame_data == this->id) ? UCSR0A & (0xff ^ (1<<MPCM0)) :  UCSR0A | (1<<MPCM0);    // Retira o slave do modo idle ou mantem caso ele já esteja quando o endereço enviado pelo master é o do slave
        rx_buffer_index = (frame_data == this->id) ? 0 : rx_buffer_index;

    }else{                                  // Só é possível entrar neste modo se for o slave que se encontra selecionado

        rx_buffer[rx_buffer_index++] = frame_data;    // Adiciona a frame de dados recebida ao buffer de dados
        
        if(rx_buffer_index == RX_BUFFER_SIZE)     // Caso haja um overflow do buffer de dados
            return -1;
        
        if(read())
            return -1;

    }

    return 0;
} 


