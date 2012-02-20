
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Va25_core.h"

Va25_core *top;
#if VM_TRACE
VerilatedVcdC *trace;
#endif



char gstring[80];
char newline[1024];
unsigned char hexline[1024];


//16Kbytes
#define ROMMASK ((64<<10)-1)
unsigned int rom[(ROMMASK+1)>>2];
#define RAMBASE 0x40000000
#define RAMMASK ((64<<10)-1)
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
        }
    }

    //printf("%u lines processed\n",line);
    //printf("%08X\n",maxadd);

    //for(ra=0;ra<maxadd;ra+=4)
    //{
        //printf("0x%08X: 0x%04X\n",ra,rom[ra>>2]);
    //}

    return(0);


}


void write_ram ( unsigned int addr, unsigned int data, unsigned int mask )
{
    //printf("write_ram(0x%08X,0x%08X,0x%08X)\n",addr,data,mask);
    if(mask==0xFFFFFFFF)
    {
        ram[addr>>2]=data;
    }
    else
    {
        ram[addr>>2]&=~mask;
        ram[addr>>2]|=mask&data;
    }
}



int main(int argc, char *argv[])
{
    unsigned int lasttick;
    unsigned int tick;
    unsigned int addr;
    unsigned int mask;
    unsigned int simhalt;
    unsigned int did_reset;

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


#if VM_TRACE
    Verilated::traceEverOn(true);
#endif

    top = new Va25_core;

#if VM_TRACE
    trace = new VerilatedVcdC;
    top->trace(trace, 99);
    trace->open("test.vcd");
#endif

    top->i_system_rdy = 0;
    simhalt=0;
    did_reset=0;
    tick=0;
    lasttick=tick;
    while (!Verilated::gotFinish())
    {
        top->i_wb_dat[0]=0;
        top->i_wb_dat[1]=0;
        top->i_wb_dat[2]=0;
        top->i_wb_dat[3]=0;
        top->i_wb_ack=0;

        tick++;
        if(tick<lasttick) printf("tick rollover\n");
        lasttick=tick;

        if(did_reset)
        {
            if(top->o_wb_cyc)
            {
                if(top->o_wb_sel)
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
                            top->i_wb_dat[0]=rom[((addr>>4)<<2)+0];
                            top->i_wb_dat[1]=rom[((addr>>4)<<2)+1];
                            top->i_wb_dat[2]=rom[((addr>>4)<<2)+2];
                            top->i_wb_dat[3]=rom[((addr>>4)<<2)+3];
                        }
                    }
                    else if((addr>=RAMBASE)&&(addr<=(RAMBASE+RAMMASK)))
                    {
                        addr&=RAMMASK;
                        if(top->o_wb_we)
                        {
                            //write ram
                            if((tick&1)==0)
                            {
                                if(top->o_wb_sel==0xFFFF)
                                {
                                    //all lanes on, just write
                                    write_ram(addr+0x0,top->o_wb_dat[0],0xFFFFFFFF);
                                    write_ram(addr+0x4,top->o_wb_dat[1],0xFFFFFFFF);
                                    write_ram(addr+0x8,top->o_wb_dat[2],0xFFFFFFFF);
                                    write_ram(addr+0xC,top->o_wb_dat[3],0xFFFFFFFF);
                                }
                                else
                                {
                                    if(top->o_wb_sel&0x000F)
                                    {
if(top->o_wb_sel&0xFFF0) printf("unexpected mask\n");
                                        if((top->o_wb_sel&0x000F)==0x000F)
                                        {
                                            write_ram(addr,top->o_wb_dat[0],0xFFFFFFFF);
                                        }
                                        else
                                        {
                                            mask=0;
                                            if(top->o_wb_sel&(1<<0)) mask|=0x000000FF;
                                            if(top->o_wb_sel&(2<<0)) mask|=0x0000FF00;
                                            if(top->o_wb_sel&(4<<0)) mask|=0x00FF0000;
                                            if(top->o_wb_sel&(8<<0)) mask|=0xFF000000;
                                            write_ram(addr,top->o_wb_dat[0],mask);
                                        }
                                    }
                                    if(top->o_wb_sel&0x00F0)
                                    {
if(top->o_wb_sel&0xFF0F) printf("unexpected mask\n");
                                        if((top->o_wb_sel&0x00F0)==0x00F0)
                                        {
                                            write_ram(addr,top->o_wb_dat[1],0xFFFFFFFF);
                                        }
                                        else
                                        {
                                            mask=0;
                                            if(top->o_wb_sel&(1<<4)) mask|=0x000000FF;
                                            if(top->o_wb_sel&(2<<4)) mask|=0x0000FF00;
                                            if(top->o_wb_sel&(4<<4)) mask|=0x00FF0000;
                                            if(top->o_wb_sel&(8<<4)) mask|=0xFF000000;
                                            write_ram(addr,top->o_wb_dat[1],mask);
                                        }
                                    }
                                    if(top->o_wb_sel&0x0F00)
                                    {
if(top->o_wb_sel&0xF0FF) printf("unexpected mask\n");
                                        if((top->o_wb_sel&0x0F00)==0x0F00)
                                        {
                                            write_ram(addr,top->o_wb_dat[2],0xFFFFFFFF);
                                        }
                                        else
                                        {
                                            mask=0;
                                            if(top->o_wb_sel&(1<<8)) mask|=0x000000FF;
                                            if(top->o_wb_sel&(2<<8)) mask|=0x0000FF00;
                                            if(top->o_wb_sel&(4<<8)) mask|=0x00FF0000;
                                            if(top->o_wb_sel&(8<<8)) mask|=0xFF000000;
                                            write_ram(addr,top->o_wb_dat[2],mask);
                                        }
                                    }
                                    if(top->o_wb_sel&0xF000)
                                    {
if(top->o_wb_sel&0x0FFF) printf("unexpected mask\n");
                                        if((top->o_wb_sel&0xF000)==0xF000)
                                        {
                                            write_ram(addr,top->o_wb_dat[3],0xFFFFFFFF);
                                        }
                                        else
                                        {
                                            mask=0;
                                            if(top->o_wb_sel&(1<<12)) mask|=0x000000FF;
                                            if(top->o_wb_sel&(2<<12)) mask|=0x0000FF00;
                                            if(top->o_wb_sel&(4<<12)) mask|=0x00FF0000;
                                            if(top->o_wb_sel&(8<<12)) mask|=0xFF000000;
                                            write_ram(addr,top->o_wb_dat[3],mask);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            //read ram
                            top->i_wb_dat[0]=ram[((addr>>4)<<2)+0];
                            top->i_wb_dat[1]=ram[((addr>>4)<<2)+1];
                            top->i_wb_dat[2]=ram[((addr>>4)<<2)+2];
                            top->i_wb_dat[3]=ram[((addr>>4)<<2)+3];
                            if((tick&1)==0)
                            {
                                //printf("read  ram[0x%X]=0x%08X\n",addr,ram[addr>>2]);
                            }
                        }
                    }
                    else
                    {


                        //peripherals
                        if(addr==0xD0000000)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("%c",top->o_wb_dat[0]&0xFF);
                                }
                            }
                        }
                        if(addr==0xD1000000)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("cant write or change timer tick\n");
                                }
                            }
                            else
                            {
                                top->i_wb_dat[0]=tick;
                                top->i_wb_dat[1]=tick;
                                top->i_wb_dat[2]=tick;
                                top->i_wb_dat[3]=tick;
                            }
                        }
                        if(addr==0xE0000000)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("show 0x%08X\n",top->o_wb_dat[0]);
                                }
                            }
                        }
                        if(addr==0xF0000000)
                        {
                            simhalt=1;
                        }
                    }
                }
                top->i_wb_ack=1;
            }

        }
        else
        {
            if (tick > 11)
            {
                top->i_system_rdy = 1;
                did_reset = 1;
            }
        }
        top->i_clk = (tick & 1);
        top->eval();
#if VM_TRACE
        trace->dump(tick);
        if(tick>200000) break;
#endif
        if(simhalt) break;
    }
#if VM_TRACE
    trace->close();
#endif
    top->final();
    return 0;
}

