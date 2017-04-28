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

char UART0_getchar(char c,ring_buffer_t *rec);
int IS_BUFFER_EMTPY(int in_pos,int out_pos);
int IS_BUFFER_FULL(int in_pos, int out_pos);
uint8_t UART0_available(int in_pos,int out_pos);
void BUFFER_INIT(ring_buffer_t *buffer);
void puts(char *cad, ring_buffer_t * trans);
void putchar(char data,ring_buffer_t **trans);



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

void BUFFER_INIT(ring_buffer_t *buffer)
{
	buffer->in_idx = 0;
	buffer->out_idx = 0;
	buffer->in_pos = 0;
	buffer->out_pos = 0;
}

void puts(char *cad, ring_buffer_t * trans)
{	
	int cont = 0;
	while( !(IS_BUFFER_FULL(trans->in_pos,trans->out_pos)) && *cad)
	{
				putchar(*cad++,&trans);
				cont++;
	}
}
void putchar(char data, ring_buffer_t **trans)
{	
	(*trans)->buffer[(*trans)->]
}
char UART0_getchar(char c, ring_buffer_t *rec)
{
	while( !IS_BUFFER_EMTPY(rec->in_idx,rec->out_pos))
	{
		return rec->buffer[rec->out_pos];
		rec->out_pos++;
	}
}
int IS_BUFFER_FULL(int in_pos, int out_pos)
{
	if(in_pos == (out_pos-1))
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
