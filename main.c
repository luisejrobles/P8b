/*
	Universidad Autonoma de Baja California
	Microprocesadores y Microcontroladores
		Jimenez Robles Luis Eduardo
				01208396
 */ 
#include <avr/io.h>
#define BUFFER_SIZE 64

typedef struct {
	char buffer[BUFFER_SIZE];       //espacio reservado 
	volatile unsigned char in_idx;  //indice entrada (Head) 
	volatile unsigned char out_idx; //indice salida (tail)
	volatile unsigned int in_pos;	//posicion de in
	volatile unsigned int out_pos;	//posicion de out
} ring_buffer_t;

void BUFFER_INIT(struct ring_buffer_t *buffer);


int main(void)
{
    ring_buffer_t transmision;
	ring_buffer_t recepcion;
	
	BUFFER_INIT(&transmision);
	BUFFER_INIT(&recepcion);
	
    while (1) 
    {
    }
}

void BUFFER_INIT(struct ring_buffer_t *buffer)
{
	buffer->in_idx = 0;
	buffer->out_idx = 0;
	buffer->in_pos = 0;
	buffer->out_pos = 0;
}

void putchar(char data, struct ring_buffer_t)
{
	
}
void puts(char *cad)
{
	while( !(IS_BUFFER_FULL()) && *cad)
	{
				
	}
}

char UART0_getchar(char c)
{
	while( IS_BUFFER_FULL() )
	{
		
	}
	
}
int IS_BUFFER_FULL(int in_pos, int out_pos)
{
	if(in == (out-1))
	{
		return 1;
	}
	return 0;
}
int IS_BUFFER_EMTPY(int in_pos,int out_pos)
{
	if(in_pos == out_pos)
	{
		return 1;
	}
	return 0;
}
uint8_t UART0_available(int in_pos,int out_pos)
{
	if( (in_pos != (out_pos -1)) && (in_pos == out_pos) )
	{
		return 1;
	}
	return 0;
}
ISR(SIG_USART0_RECV)
{
	
}
ISR(SIG_USART0_DATA)
{
	
}
