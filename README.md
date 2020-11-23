sele485_G07:

EasyEDA Project link: https://easyeda.com/account/project/detail?project=81f3bfc2622d43b184a9f72df1e261ae



### Descrição da Solução


Na inicialização para além da definição dos Inputs e Outputs, também é definido o ID do dispositivo a partir da leitura dos Inputs dos DIP Switches, que vai definir se o mesmo irá trabalhar como **Master  (ID = 0x00)** ou se irá trabalhar como **Slave (ID entre 0x01 e 0x0F)**. Dar enable ao modo Multi-processor nos slaves, e colocar o bit PC0 no estado LOW para permitir a receção de dados e no Master colocar o mesmo bit no estado HIGH para permitir a transmissão de dados.

É feita a inicialização e configuração da ISR (Interrupt Service Routine) de PCINT0 através da função **PCINT0_setup()** para monitorar os Inputs dos DIP Switches e caso o Endereço seja mudado fisicamente, o dispositivo possa ser reinicializado com a configuração para o novo endereço. 

Segue-se a inicialização e configuração do USART que adquire um perfil diferente para o Master e para o Slave. Em ambos é definido o Baud Rate e a semântica das mensagens (9 bits, paridade impar e 1 stop -bit) , mas enquanto no Master é ativada a transmissão de dados, no Slave é ativada a recessão de dados, a **Interrupt USART_RX** e o modo Multi-processor.



Na função **main()** no caso de o dispositivo ser um Slave apenas é colocado o bit PC0 no estado LOW para permitir a receção de dados, enquanto que no caso de o dispositivo ser um Master é colocado o bit PC0 no estado HIGH para permitir a transmissão de dados e é configurado o envio de mensagens em resposta à transição de estado dos botões através da função **rs485_send()**. Esta última função envia 2 mensagens, a primeira com o endereço do slave a que se destinam os dados e a segunda com os dados.

Na Interrupt USART_RX é feita a deteção de erros nas mensagens recebidas, o tratamento da informação e a atuação do LED.
