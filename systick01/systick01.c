
//-------------------------------------------------------------------
//-------------------------------------------------------------------

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void ASMDELAY ( unsigned int );

extern void uart_init ( void );
extern void uart_putc ( unsigned char );
extern void hexstring ( unsigned int );


//-------------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    uart_init();
    hexstring(0x12345678);

    for(ra=0;ra<10;ra++)
    {
        PUT32(0xE0000000,ra);
    }
    rb=GET32(0xD1000000);
    PUT32(0xE0000000,rb);
    rb=GET32(0xD1000000);
    PUT32(0xE0000000,rb);
    rb=GET32(0xD1000000);
    PUT32(0xE0000000,rb);
    rb=GET32(0xD1000000);
    for(ra=0;ra<10;ra++)
    {
        while(1)
        {
            rc=GET32(0xD1000000);
            if((rc-rb)>300) break;
        }
        PUT32(0xE0000000,ra);
    }

    hexstring(0x12345678);
    return(0);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
