#include <avr/io.h>
#include <avr/interrupt.h>

#define BUFFER_SIZE 64
#define BUFFER_EMPTY(buffer) buffer.in_idx == buffer.out_idx
#define BUFFER_FULL(buffer) buffer.in_idx == MOD(buffer.out_idx - 1)
#define MOD(n) n&(BUFFER_SIZE-1)

char UART0_getchar( void );
unsigned int atoi(char *str);
uint8_t UART0_available( void );
void BUFFER_INIT(void);
void itoa(char *str, uint16_t number, uint8_t base);
void UART0_gets(char *str);
void UART0_Ini(uint16_t mode);
void UART0_putchar(char data);
void UART0_puts(char *str);

typedef struct
{
	char buffer[BUFFER_SIZE];				/*	espacio reservado			*/
	volatile unsigned char in_idx :6;		/*	indice entrada (Head)		*/
	volatile unsigned char out_idx :6;		/*	indice entrada (tail)		*/
} ring_buffer_t;

ring_buffer_t transmision;
ring_buffer_t recepcion;

int main( void )
{
	
	char cad[20];
	uint16_t num;
	UART0_Ini(0);
	while(1)
	{
		UART0_getchar();
		UART0_puts("\n\rIntroduce un nÃºmero:\n\r");
		UART0_gets(cad);
		num = atoi(cad);
		itoa(cad,num,2);
		UART0_puts("\n\rBin:");
		UART0_puts(cad);
		itoa(cad,num,16);
		UART0_puts("\n\rHEx:");
		UART0_puts(cad);
	}
	return 0;
}
char UART0_getchar( void )
{
	while(BUFFER_EMPTY(recepcion));					//loop mientras el buffer se encuentre vacío
	return recepcion.buffer[recepcion.out_idx++];	//regresa el caracter de la posicion del buffer
}
ISR( USART0_RX_vect )	//Rx interrupt
{
	while(BUFFER_FULL(recepcion));	//Loop mientras buffer está lleno
	recepcion.buffer[recepcion.in_idx++] = UDR0;	//Metiendo el dato de UDR0 al  buffer
}
ISR( USART0_UDRE_vect )	//Tx interrupt
{
	if(BUFFER_EMPTY(transmision))
	{
		UCSR0B &= (~(1<<UDRIE0));	//Disable Tx interruption
	}else
	{
		UDR0 = transmision.buffer[transmision.out_idx++];	/* Manda el dato de la cola al UDR0*/
	}
}
uint8_t UART0_available( void )
{
	if(!BUFFER_FULL(recepcion))
	{
		return 1;	//si no está lleno
	}
	return 0;	//lleno
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
	while(count != 0 )
	{
		val = *str--;	//tomando el valor
		val = val - '0';	//obteniendo valor crudo
		if(val >=0 && val <=9)
		{
			num = num + (val * exp);	//almacenando valor crudo*exp en num
			exp = exp*10;
			count--;
		}
	}
	return num;
}
void BUFFER_INIT(void)	//Inicializando los buffers
{
	recepcion.in_idx = 0;
	recepcion.out_idx = 0;
	transmision.in_idx = 0;
	transmision.out_idx = 0;
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
void UART0_Ini(uint16_t mode)
{
		/*Función para inicializar el puerto serie del ATmega1280/2560 
	  Si mode es 0    9600,8,N,1
	  Si mode es 1    19200,8,N,1 */
	UCSR0A = (1<<U2X0);		//Usart double speed
	UCSR0B = ((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0))&(~(1<<UCSZ02)); // Reception enable | Transmission enable | 9bit disable| RX interruption enable
	UCSR0C = (3<<UCSZ00);	//8bit enable
	if(!mode)
	{
		UBRR0 = 207;	//9600 baud rate UBRR
	}else if(mode)
	{
		UBRR0 = 103;	//19.2k baud rate UBRR
	}
	sei();	//Interupciones globales
	BUFFER_INIT();
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
void UART0_putchar(char data)
{
	while(BUFFER_FULL(transmision));				//loop mientras el buffer esté lleno
	
	if(BUFFER_EMPTY(transmision))					//si buffer está vacío
	{
		transmision.buffer[transmision.in_idx++] = data;  //ingreso dato al buffer para que lo capte la interrupcion
		UCSR0B |= (1<<UDRIE0);				//Tx interrupt enable
	}
	else
	{
		transmision.buffer[transmision.in_idx++] = data;  /*ingresa el dato en la cabeza y aumenta*/
	}
}
void UART0_puts(char *str)
{
	while(*str)
		UART0_putchar(*str++);
}