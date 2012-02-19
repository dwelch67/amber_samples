
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Va23_core.h"

Va23_core *top;
VerilatedVcdC *trace;



char gstring[80];
char newline[1024];
unsigned char hexline[1024];


//16Kbytes
#define ROMMASK ((16<<10)-1)
unsigned int rom[(ROMMASK+1)>>2];
#define RAMBASE 0x40000000
#define RAMMASK ((16<<10)-1)
unsigned int ram[(RAMMASK+1)>>2];

//-----------------------------------------------------------------------------
int readhex ( FILE *fp )
{
//unsigned int errors;

unsigned int addhigh;
unsigned int add;

unsigned int ra;
//unsigned int rb;

//unsigned int pages;
//unsigned int page;


unsigned int line;

unsigned char checksum;

unsigned int len;
unsigned int hexlen;
unsigned int maxadd;

unsigned char t;

    maxadd=0;

    addhigh=0;
    memset(rom,0xFF,sizeof(rom));

    line=0;
    while(fgets(newline,sizeof(newline)-1,fp))
    {
        line++;
        //printf("%s",newline);
        if(newline[0]!=':')
        {
            printf("Syntax error <%u> no colon\n",line);
            continue;
        }
        gstring[0]=newline[1];
        gstring[1]=newline[2];
        gstring[2]=0;
        len=strtoul(gstring,NULL,16);
        for(ra=0;ra<(len+5);ra++)
        {
            gstring[0]=newline[(ra<<1)+1];
            gstring[1]=newline[(ra<<1)+2];
            gstring[2]=0;
            hexline[ra]=(unsigned char)strtoul(gstring,NULL,16);
        }
        checksum=0;
        for(ra=0;ra<(len+5);ra++) checksum+=hexline[ra];
        //checksum&=0xFF;
        if(checksum)
        {
            printf("checksum error <%u>\n",line);
        }
        add=hexline[1]; add<<=8;
        add|=hexline[2];
        add|=addhigh;
        if(add>ROMMASK)
        {
            printf("address too big 0x%08X\n",add);
            //return(1);
            continue;
        }
        if(add&3)
        {
            printf("bad address 0x%08X\n",add);
            return(1);
        }
        if(len&3)
        {
            printf("bad length\n");
            return(1);
        }
        //:0011223344
        //;llaaaattdddddd
        //01234567890
        t=hexline[3];
        switch(t)
        {
            default:
                printf("UNKOWN type %02X <%u>\n",t,line);
                break;
            case 0x00:
                for(ra=0;ra<len;ra+=4)
                {
                    if(add>ROMMASK)
                    {
                        printf("address too big 0x%08X\n",add);
                        break;
                    }
                    rom[add>>2]                  =hexline[ra+4+3];
                    rom[add>>2]<<=8; rom[add>>2]|=hexline[ra+4+2];
                    rom[add>>2]<<=8; rom[add>>2]|=hexline[ra+4+1];
                    rom[add>>2]<<=8; rom[add>>2]|=hexline[ra+4+0];
                    //printf("%08X: %02X\n",add,t);
                    add+=4;
                    if(add>maxadd) maxadd=add;
                }
                break;
            case 0x01:
                printf("End of data\n");
                break;
            case 0x04:
                    addhigh=hexline[ra+4];
                    addhigh<<=8; addhigh|=hexline[ra+5];
            //:02 0000 04 0300 F7
                addhigh<<=16;
                printf("addhigh %08X\n",addhigh);
                break;

        }
    }

    //printf("%u lines processed\n",line);
    //printf("%08X\n",maxadd);

    //if(maxadd&0x7F)
    //{
        //maxadd+=0x80;
        //maxadd&=0xFFFFFF80;
        //printf("%08X\n",maxadd);
    //}

    for(ra=0;ra<maxadd;ra+=4)
    {
        printf("0x%08X: 0x%04X\n",ra,rom[ra>>2]);
    }

    return(0);


}

int main(int argc, char *argv[])
{
    unsigned int tick;
    int ret;
    unsigned int addr;

    FILE *fp;

//    Verilated::commandArgs(argc, argv);

    if(argc<2)
    {
        fprintf(stderr,".hex file not specified\n");
        return(1);
    }
    fp=fopen(argv[1],"rt");
    if(fp==NULL)
    {
        fprintf(stderr,"Error opening file [%s]\n",argv[1]);
        return(1);
    }
    if(readhex(fp)) return(1);
    fclose(fp);



    Verilated::traceEverOn(true);

    top = new Va23_core;

    trace = new VerilatedVcdC;

    top->trace(trace, 99);
    trace->open("test.vcd");

    top->i_system_rdy = 0;
    while (!Verilated::gotFinish())
    {

        top->i_wb_dat=0;
        top->i_wb_ack=0;


        tick++;
        if (tick > 11)
        {
            top->i_system_rdy = 1;


            if(top->o_wb_cyc)
            {
                if(top->o_wb_cyc)
                {
                    if(top->o_wb_sel==0xF)
                    {
                        addr=top->o_wb_adr;
                        if(addr<=ROMMASK)
                        {
                            if(top->o_wb_we)
                            {
                                printf("Error trying to write to rom 0x%08X\n",addr);
                            }
                            else
                            {
                                top->i_wb_dat=rom[addr>>2];
                            }
                        }
                        else
                        {
                            if(addr>=RAMBASE)
                            {
                                if(addr<=(RAMBASE+RAMMASK))
                                {
                                    addr&=RAMMASK;
                                    if(top->o_wb_we)
                                    {
                                        ram[addr>>2]=top->o_wb_dat;
                                    }
                                    else
                                    {
                                        top->i_wb_dat=ram[addr>>2];
                                    }
                                }
                            }
                        }
                        top->i_wb_ack=1;
                    }
                }
            }
        }
        top->i_clk = (tick & 1);


        top->eval();
        trace->dump(tick);

        if(tick>2000) break;
    }
    trace->close();
    top->final();
    return 0;
}

