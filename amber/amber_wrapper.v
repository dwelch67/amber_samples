
module amber_wrapper
(
input                       i_clk       ,
input                       i_irq       ,
input                       i_firq      ,
input                       i_system_rdy,
output      [31:0]          o_wb_adr    ,
output      [3:0]           o_wb_sel    ,
output                      o_wb_we     ,
input       [31:0]          i_wb_dat    ,
output      [31:0]          o_wb_dat    ,
output                      o_wb_cyc    ,
output                      o_wb_stb    ,
input                       i_wb_ack    ,
input                       i_wb_err
);

wire        wire_i_clk       ;
wire        wire_i_irq       ;
wire        wire_i_firq      ;
wire        wire_i_system_rdy;
wire [31:0] wire_o_wb_adr    ;
wire [3:0]  wire_o_wb_sel    ;
wire        wire_o_wb_we     ;
wire [31:0] wire_i_wb_dat    ;
wire [31:0] wire_o_wb_dat    ;
wire        wire_o_wb_cyc    ;
wire        wire_o_wb_stb    ;
wire        wire_i_wb_ack    ;
wire        wire_i_wb_err    ;

wire        timer_read;
wire [31:0] timer_read_data;
wire        timer_write;

reg [31:0]  timer0_count;
reg [31:0]  timer1_count;
reg [31:0]  timer1_match;
reg [31:0]  timer_control;
reg [31:0]  timer_status;
wire        timer_irq;
wire        timer_fiq;

a23_core u_core (
    .i_clk         (wire_i_clk       ),
    .i_irq         (wire_i_irq       ),
    .i_firq        (wire_i_firq      ),
    .i_system_rdy  (wire_i_system_rdy),
    .o_wb_adr      (wire_o_wb_adr    ),
    .o_wb_sel      (wire_o_wb_sel    ),
    .o_wb_we       (wire_o_wb_we     ),
    .i_wb_dat      (wire_i_wb_dat    ),
    .o_wb_dat      (wire_o_wb_dat    ),
    .o_wb_cyc      (wire_o_wb_cyc    ),
    .o_wb_stb      (wire_o_wb_stb    ),
    .i_wb_ack      (wire_i_wb_ack    ),
    .i_wb_err      (wire_i_wb_err    )
);

assign wire_i_clk        = i_clk       ;
assign wire_i_irq        = i_irq | timer_irq ;
assign wire_i_firq       = i_firq ;//| timer_fiq ;
assign wire_i_system_rdy = i_system_rdy;
assign wire_i_wb_dat     =  timer_read ? timer_read_data : i_wb_dat ;
assign wire_i_wb_ack     =  timer_read  ? 1'b1          :
                            timer_write ? 1'b1          : i_wb_ack ;
assign wire_i_wb_err     = i_wb_err    ;

assign o_wb_adr     = wire_o_wb_adr    ;
assign o_wb_sel     = wire_o_wb_sel    ;
assign o_wb_we      = wire_o_wb_we     ;
assign o_wb_dat     = wire_o_wb_dat    ;
assign o_wb_cyc     = wire_o_wb_cyc    ;
assign o_wb_stb     = wire_o_wb_stb    ;


always @ ( posedge i_clk )
begin
    timer_read = 1'b0;
    timer_write = 1'b0;
    timer_read_data = 32'b0;
    timer_fiq = timer_control[11]&timer_status[11];
    timer_irq = timer_control[10]&timer_status[10];

    if (!i_system_rdy)
    begin
        timer0_count <= 32'b0;
        timer1_count <= 32'b0;
        timer1_match <= 32'b0;
        timer_control <= 32'b0;
        timer_status <= 32'b0;
    end
    else
    begin
        timer0_count <= timer0_count + 32'b1;
        timer1_count <= timer1_count + 32'b1;
        if(wire_o_wb_cyc && wire_o_wb_we && (wire_o_wb_sel==4'b1111) && (wire_o_wb_adr == 32'hD1000020))
        begin
            timer_control <= wire_o_wb_dat;
            if(wire_o_wb_dat[1] == 1'b1)
            begin
                timer1_count <= 32'b0;
                timer_status[11:10] <= 2'b00;
                timer_write = 1'b1;
            end
            else
            begin
                if(timer_control[1] == 1'b1)
                begin
                    if(timer1_count == timer1_match)
                    begin
                        timer1_count <= 32'b0;
                        timer_status[11] <= 1'b1;
                        timer_status[10] <= 1'b1;
                    end
                end
            end
        end
        else
        begin
            if(timer_control[1] == 1'b1)
            begin
                if(timer1_count == timer1_match)
                begin
                    timer1_count <= 32'b0;
                    timer_status[11] <= 1'b1;
                    timer_status[10] <= 1'b1;
                end
            end
        end
        if(wire_o_wb_cyc && wire_o_wb_we && (wire_o_wb_sel==4'b1111) && (wire_o_wb_adr == 32'hD1000014))
        begin
            timer1_match <= wire_o_wb_dat;
            timer_write = 1'b1;
        end
        if(wire_o_wb_cyc && wire_o_wb_we && (wire_o_wb_sel==4'b1111) && (wire_o_wb_adr == 32'hD1000024))
        begin
            if(wire_o_wb_dat[11] == 1'b1)
            begin
                timer_status[11] <= 1'b0;
            end
            if(wire_o_wb_dat[10] == 1'b1)
            begin
                timer_status[10] <= 1'b0;
            end
            timer_write = 1'b1;
        end
        if(wire_o_wb_cyc && (!wire_o_wb_we) && (wire_o_wb_sel==4'b1111) && (wire_o_wb_adr[31:24] == 8'hD1))
        begin
            timer_read = 1'b1;
            case ( wire_o_wb_adr[7:0] )
                8'h00: timer_read_data = timer0_count;
                8'h04: timer_read_data = timer1_count;
                8'h14: timer_read_data = timer1_match;
                8'h20: timer_read_data = timer_control;
                8'h24: timer_read_data = timer_status;
                default: timer_read_data = 32'hFFFFFFFF;
            endcase
        end
    end
end

endmodule


