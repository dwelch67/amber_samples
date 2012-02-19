

obj_dir/Va23_core : a23_core.v vmain.cpp 
	verilator --cc a23_core.v --exe vmain.cpp --trace --trace-underscore 
	make -C obj_dir/ -f Va23_core.mk

clean :
	rm -rf obj_dir
	rm -f test.vcd
	rm -f *.s.csv
	rm -f *.s.o


