
//-------------------------------------------------------------------
//-------------------------------------------------------------------

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void ASMDELAY ( unsigned int );

extern void uart_init ( void );
extern void uart_putc ( unsigned char );
extern void hexstring ( unsigned int );
extern void hexstrings ( unsigned int );
extern void showstring ( char * );

const char hello_world[]="Hello World!\r\n";

//-------------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra;

    uart_init();
    hexstring(0x12345678);

    showstring((char *)hello_world);
    for(ra=0;ra<10;ra++) hexstring(ra);

    hexstring(0x12345678);
    return(0);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
