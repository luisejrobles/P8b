/*
	Universidad Autonoma de Baja California
	Microprocesadores y Microcontroladores
		Jimenez Robles Luis Eduardo
				01208396
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#define BUFFER_SIZE 64
#define BUFFER_EMPY(buffer) (buffer.in_idx == buffer.out_idx)
#define BUFFER_FULL(buffer) (buffer.in_idx == (buffer.out_idx-1))
#define MOD(n) (n&(BUFFER_SIZE-1))
#define RX_IN_EN (UCSR0B = (1<< RXCIE0))	//rx interrupt enable
#define TX_IN_EN (UCSR0B = (1<<UDRIE0))		//tx interrupt enable

typedef struct {
	char buffer[BUFFER_SIZE];       //espacio reservado 
	volatile unsigned char in_idx;  //indice entrada (Head) 
	volatile unsigned char out_idx; //indice salida (tail)
} ring_buffer_t;

char UART0_getchar(void);
uint8_t UART0_available(void);
void BUFFER_INIT(ring_buffer_t buffer);
void UART0_puts(char *cad);
void UART0_putchar(char data);
void UART0_Init(uint16_t mode);

ring_buffer_t transmision;
ring_buffer_t recepcion;

int main(void)
{	
	BUFFER_INIT(transmision);
	BUFFER_INIT(recepcion);
	
    while (1) 
    {
    }
}
void UART0_Init(uint16_t mode)
{
	/*Función para inicializar el puerto serie del ATmega1280/2560 
	  Si mode es 0    9600,8,N,1
	  Si mode es 1    19200,8,N,1 */
	UCSR0A = (1<<U2X0);		//Usart double speed
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|~(1<<UCSZ02); // Reception enable | Transmission enable | 9bit disable
	UCSR0C = (3<<UCSZ00);	//8bit enable
	if(!mode)
	{
		UBRR0 = 207;	//9600 baud rate UBRR
	}else if(mode)
	{
		UBRR0 = 103;	//19.2k baud rate UBRR
	}
}	  
void BUFFER_INIT(ring_buffer_t buffer)
{
	buffer.in_idx = 0;
	buffer.out_idx = 0;
}
void UART0_puts(char *cad)
{	
	while(*cad)
	{
		*cad++;
	}
}
void UART0_putchar(char data)
{	
	if(!(BUFFER_FULL(transmision)))
	{
		transmision.buffer[transmision.in_idx] = data;	//guardado char en el arreglo
		transmision.in_idx = MOD(transmision.in_idx++);	//incrementando pocision en el arreglo
		TX_IN_EN;	//tx interrupt enable
	}
}
char UART0_getchar(void)
{
	char c;
	while( !(BUFFER_EMPY(recepcion)))
	{
		RX_IN_EN;	//rx interrupt enable
		c = recepcion.buffer[recepcion.out_idx];	//guardando dato de pos del arreglo
		recepcion.out_idx = MOD(recepcion.out_idx++);	//incrementando out
		return c;
	}
}
uint8_t UART0_available(void)
{
	if( !(BUFFER_FULL(transmision)))
	{
		return 1;
	}
	return 0;
}
ISR(USART0_RX_vect)	//Rx interrupt
{
	recepcion.buffer[recepcion.in_idx] = UDR0;
	recepcion.out_idx = MOD(recepcion.in_idx++);
	//UCSR0B =	
}
ISR(USART0_UDRE_vect)	//tx interrupt
{
	UDR0 = transmision.buffer[transmision.out_idx];
	transmision.out_idx = MOD(transmision.out_idx++);
	while(!(UCSR0A &(1<<UDRE0)))
	{
		UCSR0B &= 216<<TXB80;
	}
}
