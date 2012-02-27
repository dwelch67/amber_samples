
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
input                       i_wb_err    ,

input       [31:0]          timer0_count,
input       [31:0]          timer1_count,
input       [31:0]          timer1_match,
input       [31:0]          timer_control,
input       [31:0]          timer_status,
input       [7:0]           uart_char,
input                       uart_char_strobe,
input       [31:0]          test_out,
input                       test_out_strobe
);

wire        amber_i_clk       ;
wire        amber_i_irq       ;
wire        amber_i_firq      ;
wire        amber_i_system_rdy;
wire [31:0] amber_o_wb_adr    ;
wire [3:0]  amber_o_wb_sel    ;
wire        amber_o_wb_we     ;
wire [31:0] amber_i_wb_dat    ;
wire [31:0] amber_o_wb_dat    ;
wire        amber_o_wb_cyc    ;
wire        amber_o_wb_stb    ;
wire        amber_i_wb_ack    ;
wire        amber_i_wb_err    ;

a23_core u_core (
    .i_clk         (amber_i_clk       ),
    .i_irq         (amber_i_irq       ),
    .i_firq        (amber_i_firq      ),
    .i_system_rdy  (amber_i_system_rdy),
    .o_wb_adr      (amber_o_wb_adr    ),
    .o_wb_sel      (amber_o_wb_sel    ),
    .o_wb_we       (amber_o_wb_we     ),
    .i_wb_dat      (amber_i_wb_dat    ),
    .o_wb_dat      (amber_o_wb_dat    ),
    .o_wb_cyc      (amber_o_wb_cyc    ),
    .o_wb_stb      (amber_o_wb_stb    ),
    .i_wb_ack      (amber_i_wb_ack    ),
    .i_wb_err      (amber_i_wb_err    )
);

assign amber_i_clk        = i_clk        ;
assign amber_i_irq        = i_irq        ;
assign amber_i_firq       = i_firq       ;
assign amber_i_system_rdy = i_system_rdy ;
assign amber_i_wb_dat     = i_wb_dat     ;
assign amber_i_wb_ack     = i_wb_ack     ;
assign amber_i_wb_err     = i_wb_err     ;

assign o_wb_adr     = amber_o_wb_adr    ;
assign o_wb_sel     = amber_o_wb_sel    ;
assign o_wb_we      = amber_o_wb_we     ;
assign o_wb_dat     = amber_o_wb_dat    ;
assign o_wb_cyc     = amber_o_wb_cyc    ;
assign o_wb_stb     = amber_o_wb_stb    ;

endmodule


