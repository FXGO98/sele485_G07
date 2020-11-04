/*
    Desenvolvido por: João Conceição e Francisco Oliveira
*/

#define DEFAULT_BAUD_RATE 9600
#define RX_BUFFER_SIZE 32

class RS_485{

        unsigned char id;
    
        
        size_t rx_buffer_index;
        
        void setAsMaster();

        void setAsSlave();

        int send(unsigned char data);

    public:

        unsigned char rx_buffer[RX_BUFFER_SIZE];

        void setId(unsigned char id);

        void setup();

        void setBaudRate(unsigned int rate);

        int write(unsigned char *data, size_t data_size,unsigned char slave_id);

        int read();
     
};
