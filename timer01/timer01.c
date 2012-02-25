
//-------------------------------------------------------------------
//-------------------------------------------------------------------

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void ASMDELAY ( unsigned int );

extern void uart_init ( void );
extern void uart_putc ( unsigned char );
extern void hexstring ( unsigned int );
extern void hexstrings ( unsigned int );

unsigned int GETPC ( void );
void IRQEN ( void );
void FIQEN ( void );

#define TIMER0_COUNT 0xD1000000
#define TIMER1_COUNT 0xD1000004
#define TIMER1_MATCH 0xD1000014
#define TIMER_CONTROL 0xD1000020
#define TIMER_STATUS 0xD1000024

//-------------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra,rb,rc;

    uart_init();
    hexstring(0x12345678);

    IRQEN();
    FIQEN();
    hexstring(GETPC());

    ra=GET32(TIMER0_COUNT);
    for(rb=0;rb<10;)
    {
        rc=GET32(TIMER0_COUNT);
        if((rc-ra)>=0x100)
        {
            uart_putc(0x30+rb);
            ra=rc;
            rb++;
        }
    }

    ra=GET32(TIMER1_COUNT);
    for(rb=0;rb<10;)
    {
        rc=GET32(TIMER1_COUNT);
        if((rc-ra)>=0x40)
        {
            uart_putc(0x30+rb);
            ra=rc;
            rb++;
        }
    }

    PUT32(TIMER1_MATCH,0xFFF);
    PUT32(TIMER_CONTROL,0x0C02);
    for(rb=0;rb<10000;rb++) GETPC();


    hexstring(0x12345678);
    return(0);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
