


void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void PUT16 ( unsigned int, unsigned short );

extern void uart_init ( void );
extern void uart_putc ( unsigned char );
extern void uart_fifo_flush ( void );
extern void hexstrings ( unsigned int d );
extern void hexstring ( unsigned int d );
extern void cache_enable ( void );

void IRQEN ( void );
void FIQEN ( void );
#define TIMER0_COUNT 0xD1000000
#define TIMER1_COUNT 0xD1000004
#define TIMER1_MATCH 0xD1000014
#define TIMER_CONTROL 0xD1000020
#define TIMER_STATUS 0xD1000024

#include "zlib.h"
#include "ztest.h"

#define CHUNK 3000

unsigned char bin[CHUNK];
unsigned char bout[CHUNK];

unsigned int mallocoff;



unsigned int malloc ( unsigned int x )
{
    unsigned int y;
    hexstrings(x);
    x+=7;
    x&=~7;
    hexstrings(x);

    y=mallocoff;
    hexstring(y);
    mallocoff+=x;
    return(y);
}

void free ( unsigned int x )
{
    hexstrings(x); hexstring(x);
}

void memcpy ( unsigned char *d, const unsigned char *s, int n )
{
    while(n--)
    {
        *d=*s;
        d++;
        s++;
    }
}
void memset ( unsigned char *d, const unsigned c, int n )
{
    while(n--)
    {
        *d=c;
        d++;
    }
}

int onetime ( void )
{
    unsigned int ra;
    unsigned int rc;
    int ret;
    unsigned long reslen;
    unsigned long complen;

    mallocoff=0x40100000;

    complen=CHUNK;
    ret = compress(bin,&complen,testdata,TESTDATALEN);
    hexstring(ret);
    hexstring(complen);
    for(rc=0,ra=0;ra<complen;ra++) rc+=bin[ra];
    hexstring(rc);


    reslen=CHUNK;
    ret = uncompress(bout,&reslen,bin,complen);
    hexstring(ret);
    hexstring(reslen);


    ret=0;
    for(ra=0;ra<TESTDATALEN;ra++)
    {
        if(testdata[ra]!=bout[ra]) ret++; // printf("%04X %02X %02X\n",ra,testdata[ra],bout[ra]);
    }
    hexstring(ret);
    hexstring(mallocoff);

    return(ret);
}


int notmain ( void )
{
    unsigned int beg,end;
    unsigned int one,two;

    uart_init();
    hexstring(0x12345678);

    IRQEN();
    FIQEN();
    PUT32(TIMER1_MATCH,0xFFF);
    PUT32(TIMER_CONTROL,0x0C02);

    beg=GET32(0xD1000000);
    if(onetime()) return(1);
    end=GET32(0xD1000000);
    hexstring(end-beg);
    one=end-beg;

    hexstring(0x12345678);

    cache_enable();

    beg=GET32(0xD1000000);
    if(onetime()) return(1);
    end=GET32(0xD1000000);
    hexstring(end-beg);
    two=end-beg;
    hexstring(0x12345678);

    hexstring(one-two);
    hexstring(0x12345678);

    return(0);
}

