
VERILOG_FILES = \
	a23_alu.v \
	a23_barrel_shift.v \
	a23_coprocessor.v \
	a23_core.v \
	a23_decode.v \
	a23_execute.v \
	a23_fetch.v \
	a23_functions.v \
	a23_localparams.v \
	a23_multiply.v \
	a23_register_bank.v \
	a23_wishbone.v \
	generic_sram_byte_en.v \
	generic_sram_line_en.v

obj_dir/Va23_core : vmain.cpp $(VERILOG_FILES)
	verilator --cc a23_core.v --exe vmain.cpp --trace --trace-underscore 
	make -C obj_dir/ -f Va23_core.mk

clean :
	rm -rf obj_dir
	rm -f test.vcd


