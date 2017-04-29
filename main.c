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
#define RX_IN_EN (UCSR0B |= (1<<RXCIE0))	//rx interrupt enable
#define TX_IN_EN (UCSR0B |= (1<<UDRIE0))		//tx interrupt enable

typedef struct {
	char buffer[BUFFER_SIZE];       //espacio reservado 
	volatile unsigned char in_idx: 6;  //indice entrada (Head) 
	volatile unsigned char out_idx: 6; //indice salida (tail)
} ring_buffer_t;

char UART0_getchar(void);
uint8_t UART0_available(void);
unsigned int atoi(char *str);
void itoa(char *str, uint16_t number, uint8_t base);
void BUFFER_INIT(ring_buffer_t buffer);
void UART0_gets(char *str);
void UART0_puts(char *cad);
void UART0_putchar(char data);
void UART0_Init(uint16_t mode);

ring_buffer_t transmision;
ring_buffer_t recepcion;


int main(void)
{	
	char cad[20];
	uint16_t num;
	
	UART0_Init(0);
	BUFFER_INIT(transmision);
	BUFFER_INIT(recepcion);
	
    while (1) 
    {
		UART0_puts("\n\rUniversidad Autonoma de Baja California\n\r Practica 8a");
		UART0_puts("\n\n\rIntroduce un número:\n\r");
		UART0_gets(cad);
		num = atoi(cad);
		itoa(cad,num,10);
		UART0_puts("\n\rDec:");
		UART0_puts(cad);
		itoa(cad,num,2);
		UART0_puts("\n\rBin:");
		UART0_puts(cad);
    }
	
}
void itoa(char *str, uint16_t number, uint8_t base)
{
	unsigned int cociente, residuo,count = 0, i=0, j;
	char c;
	cociente = number;
	do{
		residuo = cociente%base;
		cociente = cociente/base;
		if(residuo > 9)	//si es mayor a 9, agregar el respectivo para imprimir letra.
		{
			c = residuo + 55;
			}else{
			c = residuo + '0'; //agregar el respectivo para crear el caracter de numero
		}
		*str++ = c;
		count++;
	}while( cociente != 0 );
	*str= '\0';
	str -=count;
	j = count -1;
	//==============invertir cadena==================
	while(i < j)
	{
		if( *(str+i) != *(str+j))
		{
			c = *(str+i);
			*(str+i) = *(str+j);
			*(str+j) = c;
		}
		i++;
		j--;
	}
}
void BUFFER_INIT(void)
{
	buffer.in_idx = 0;
	buffer.out_idx = 0;
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
void UART0_gets(char *str)
{
	unsigned char c;
	unsigned int i=0;
	do{
		c = UART0_getchar();
		if( (i<=18)&&(c!=8)&&(c!=13) )	//validacion menor al fin del arreglo, backspace y enter
		{
			UART0_putchar(c);
			*str++ = c;
			i++;
		}
		if( (c==8) && (i>0) )		//validacion backspace
		{
			UART0_putchar('\b');
			UART0_putchar(' ');
			UART0_putchar(8);
			*str--='\0';
			i--;
		}
	}while(c != 13);
	*str = '\0';
}
void UART0_puts(char *cad)
{	
	while(*cad)
	{
		UART0_putchar(*cad++);
	}
}
void UART0_putchar(char data)
{	
	while(BUFFER_FULL(transmision));
	
	if(!(BUFFER_FULL(transmision)))
	{
		transmision.buffer[transmision.in_idx] = data;	//guardado char en el arreglo
		transmision.in_idx = MOD(transmision.in_idx++);	//incrementando pocision en el arreglo
		TX_IN_EN;	//tx interrupt enable
	}else if(BUFFER_EMPY(transmision))
	{
		transmision.buffer[transmision.in_idx] = data;	//guardado char en el arreglo
		transmision.in_idx = MOD(transmision.in_idx++);	//incrementando pocision en el arreglo
		TX_IN_EN;	//tx interrupt enable
	}
}
char UART0_getchar(void)
{
	char c;
	while( !(UCSR0A&(1<<RXC0)));
	
	RX_IN_EN;	//rx interrupt enable
	c = recepcion.buffer[recepcion.out_idx];	//guardando dato de la posicion del arreglo
	recepcion.out_idx = MOD(recepcion.out_idx++);	//incrementando out
	
	return c;
}
uint8_t UART0_available(void)
{
	if( !(BUFFER_FULL(recepcion)))
	{
		return 1;
	}
	return 0;
}
unsigned int atoi(char *str)
{
	unsigned int num = 0, exp = 1, val, count = 0;
	//contando digitos en la cadena============
	while(*str)
	{
		str++;
		count++;
	}
	str--;	//no tomando en cuenta '\0'
	while(count != 0)
	{
		val = *str--;	//tomando el valor
		val = val - '0';	//obteniendo valor crudo
		num = num + (val * exp);	//almacenando valor crudo*exp en num
		exp = exp*10;
		count--;
	}
	return num;
}
ISR(USART0_RX_vect)	//Rx interrupt
{
	recepcion.buffer[recepcion.in_idx] = UDR0;
	recepcion.in_idx = MOD(recepcion.in_idx++);
	UCSR0B &= (127<<TXB80);
}
ISR(USART0_UDRE_vect)	//tx interrupt
{
	if(BUFFER_EMPY(transmision){
		UCSR0B &= ~(1<<UDRIE0); 
	}else{
		UDR0 = transmision.buffer[transmision.out_idx];
		transmision.out_idx = MOD(transmision.out_idx++);
		while(!(UCSR0A&(1<<UDRE0)));
		UCSR0B &= (223<<TXB80);
	}
}
