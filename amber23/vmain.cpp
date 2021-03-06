
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Vamber_wrapper.h"

Vamber_wrapper *top;
#if VM_TRACE
VerilatedVcdC *trace;
#endif



char gstring[80];
char newline[1024];
unsigned char hexline[1024];


//16Kbytes
#define ROMMASK 0x00FFFFFF
unsigned int rom[(ROMMASK+1)>>2];
#define RAMBASE 0x40000000
#define RAMMASK 0x00FFFFFF
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
        t=hexline[3];
        if(t!=0x02)
        {
            if(len&3)
            {
                printf("bad length\n");
                return(1);
            }
        }

        //:0011223344
        //;llaaaattdddddd
        //01234567890
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
            case 0x02:
                addhigh=hexline[5];
                addhigh<<=8;
                addhigh|=hexline[4];
                addhigh<<=16;
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

int main(int argc, char *argv[])
{
    unsigned int lasttick;
    unsigned int tick;
    unsigned int addr;
    unsigned int mask;
    unsigned int simhalt;
    unsigned int did_reset;
    unsigned int timer_control;
    unsigned int timer_status;
    unsigned int timer0_tick;
    unsigned int timer1_tick;
    unsigned int timer1_match;

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

    top = new Vamber_wrapper;

#if VM_TRACE
    trace = new VerilatedVcdC;
    top->trace(trace, 99);
    trace->open("test.vcd");
#endif

    top->i_system_rdy = 0;
    simhalt=0;
    did_reset=0;
    tick=0;
    timer0_tick=0;
    timer1_tick=0;
    timer1_match=0xFFFFFFFF;
    timer_control=0;
    timer_status=0;
    lasttick=tick;
    while (!Verilated::gotFinish())
    {

        top->i_wb_dat=0;
        top->i_wb_ack=0;

        top->uart_char = 0x00;
        top->uart_char_strobe = 0;
        top->test_out = 0x00000000;
        top->test_out_strobe = 0;

        tick++;
        if(tick<lasttick) printf("tick rollover\n");
        lasttick=tick;

        if((tick&1)==0)
        {
            timer0_tick++;
            if(timer_control&0x0002)
            {
                if(timer1_tick>=timer1_match)
                {
                    timer_status|=0x00000C00;
                    timer1_tick=0x00000000;
                }
                else
                {
                    timer1_tick++;
                }
            }
            else
            {
                timer1_tick++;
            }
        }

        top->i_irq=0;
        if((timer_control&timer_status)&0x400) top->i_irq=1;
        top->i_firq=0;
        if((timer_control&timer_status)&0x800) top->i_firq=1;

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
                            top->i_wb_dat=rom[addr>>2];
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
                                if(top->o_wb_sel==0x0F)
                                {
                                    //all lanes on, just write
                                    ram[addr>>2]=top->o_wb_dat;
                                    //printf("write ram[0x%X]=0x%08X\n",addr,ram[addr>>2]);
                                }
                                else
                                {
                                    //read-modify-write
                                    mask=0;
                                    if(top->o_wb_sel&1) mask|=0x000000FF;
                                    if(top->o_wb_sel&2) mask|=0x0000FF00;
                                    if(top->o_wb_sel&4) mask|=0x00FF0000;
                                    if(top->o_wb_sel&8) mask|=0xFF000000;
                                    ram[addr>>2]&=~mask;
                                    ram[addr>>2]|=top->o_wb_dat&mask;
                                    //printf("write ram[0x%X]=0x%08X\n",addr,ram[addr>>2]);
                                }
                            }
                        }
                        else
                        {
                            //read ram
                            top->i_wb_dat=ram[addr>>2];
                            if((tick&1)==0)
                            {
                                //printf("read  ram[0x%X]=0x%08X\n",addr,ram[addr>>2]);
                            }
                        }
                    }
                    else
                    {
                        //peripherals
                        //dummy uart tx register
                        if(addr==0xD0000000)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("%c",top->o_wb_dat&0xFF);

                                    top->uart_char=top->o_wb_dat&0xFF;
                                    top->uart_char_strobe=1;
                                }
                            }
                        }
                        //timer starts here
                        if(addr==0xD1000000)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("cant write timer0 tick\n");
                                }
                            }
                            else
                            {
                                top->i_wb_dat=timer0_tick;
                            }
                        }
                        if(addr==0xD1000004)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("cant write timer1 tick\n");
                                }
                            }
                            else
                            {
                                top->i_wb_dat=timer1_tick;
                            }
                        }
                        if(addr==0xD1000014)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    timer1_match=top->o_wb_dat;
                                    //printf("write timer1 match 0x%08X\n",timer1_match);
                                }
                            }
                            else
                            {
                                top->i_wb_dat=timer1_match;
                            }
                        }
                        if(addr==0xD1000020)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    timer_control=top->o_wb_dat;
                                    //printf("write timer control 0x%08X\n",timer_control);
                                    if(timer_control&0x0002)
                                    {
                                        timer1_tick=0x00000000;
                                        timer_status&=~0xC00;
                                    }
                                }
                            }
                            else
                            {
                                top->i_wb_dat=timer_control;
                            }
                        }
                        if(addr==0xD1000024)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    timer_status&=~top->o_wb_dat;
                                }
                            }
                            else
                            {
                                top->i_wb_dat=timer_status;
                            }
                        }
                        //debug register
                        if(addr==0xE0000000)
                        {
                            if(top->o_wb_we)
                            {
                                if((tick&1)==0)
                                {
                                    printf("show 0x%08X\n",top->o_wb_dat);
                                    top->test_out = top->o_wb_dat;
                                    top->test_out_strobe = 1;
                                }
                            }
                        }
                        //stop simulation
                        if(addr==0xF0000000)
                        {
                            simhalt=1;
                        }
                    }
                    top->i_wb_ack=1;
                }
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

        top->timer0_count = timer0_tick;
        top->timer1_count = timer1_tick;
        top->timer1_match = timer1_match;
        top->timer_status = timer_status;
        top->timer_control = timer_control;



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

