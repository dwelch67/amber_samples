
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include "testdata.h"
unsigned int edata[TESTDATALEN];
unsigned int fdata[TESTDATALEN];

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void ASMDELAY ( unsigned int );

extern void uart_init ( void );
extern void uart_putc ( unsigned char );
extern void hexstring ( unsigned int );
extern void hexstrings ( unsigned int );

//-------------------------------------------------------------------------
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))
//-------------------------------------------------------------------------
void btea(unsigned int *v, int n, unsigned rounds, unsigned int const key[4])
{
    unsigned int y, z, sum;
    unsigned p, /*rounds,*/ e;
    if (n > 1) {          /* Coding Part */
      //rounds = 6 + 52/n;
      sum = 0;
      z = v[n-1];
      do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p=0; p<n-1; p++) {
          y = v[p+1];
          z = v[p] += MX;
        }
        y = v[0];
        z = v[n-1] += MX;
      } while (--rounds);
    } else if (n < -1) {  /* Decoding Part */
      n = -n;
      //rounds = 6 + 52/n;
      sum = rounds*DELTA;
      y = v[0];
      do {
        e = (sum >> 2) & 3;
        for (p=n-1; p>0; p--) {
          z = v[p-1];
          y = v[p] -= MX;
        }
        z = v[n-1];
        y = v[0] -= MX;
      } while ((sum -= DELTA) != 0);
    }
  }
//-------------------------------------------------------------------------
int run_tea_test ( void )
{
    unsigned int ra;
    unsigned int errors;
    unsigned int data[2];
    unsigned int key[4];

    errors=0;

    key[0]=0x11111111;
    key[1]=0x22222222;
    key[2]=0x33333333;
    key[3]=0x44444444;

    hexstring(0xAABBCCDD);

    if((TESTDATALEN&1))
    {
        hexstring(0xBADBAD00);
        return(1);
    }

    for(ra=0;ra<TESTDATALEN;ra++) edata[ra]=testdata[ra];

    btea(edata,TESTDATALEN,6 + 52/TESTDATALEN,key);

    for(ra=0;ra<TESTDATALEN;ra++) fdata[ra]=edata[ra];

    btea(fdata,-TESTDATALEN,6 + 52/TESTDATALEN,key);

    for(ra=0;ra<TESTDATALEN;ra++)
    {
        if(fdata[ra]!=testdata[ra])
        {
            errors++;
            hexstrings(ra); hexstrings(fdata[0]); hexstring(testdata[ra]);
        }
        if(errors>20) break;
    }
    hexstring(errors);
    if(errors)
    {
        hexstring(0xBADBAD99);
        return(1);
    }

    hexstring(0x00112233);
    return(0);
}
//-------------------------------------------------------------------------
int notmain ( void )
{
    unsigned int beg,end;

    uart_init();
    hexstring(0x12340000);

    beg=GET32(0xD1000000);
    run_tea_test();
    end=GET32(0xD1000000);
    hexstring(end-beg);

    hexstring(0x12345678);
    return(0);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
