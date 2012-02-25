
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <sys/types.h>


#include "zlib.h"
#include "ztest.h"

void PUT32 ( unsigned int, unsigned int );
void PUT16 ( unsigned int, unsigned short );

extern void uart_putc ( unsigned char );
extern void uart_fifo_flush ( void );


#define CHUNK 3000

unsigned char bin[CHUNK];
unsigned char bout[CHUNK];

unsigned int mallocoff;

void hexstring ( unsigned int d )
{
    printf("0x%08X\n",d);
    ////unsigned int ra;
    //unsigned int rb;
    //unsigned int rc;

    //rb=32;
    //while(1)
    //{
        //rb-=4;
        //rc=(d>>rb)&0xF;
        //if(rc>9) rc+=0x37; else rc+=0x30;
        //uart_putc(rc);
        //if(rb==0) break;
    //}
    //uart_putc(0x0D);
    //uart_putc(0x0A);
}


//unsigned int malloc ( unsigned int x )
//{
    //unsigned int y;

    //hexstring(hexoff,x); hexoff+=0x40;

    //y=mallocoff;
    //mallocoff+=x;
    //return(y);
//}

//void free ( unsigned char *x )
//{
//}

int main(int argc, char **argv)
{
    unsigned int ra;
    unsigned int rc;
    int ret;
    unsigned long reslen;
    unsigned long complen;

    mallocoff=0x00030000;

    hexstring(0x12345678);

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


















    return(0);
}

