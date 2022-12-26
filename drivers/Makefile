all: ../bin/sanderos/drivers/ps2key.sys ../bin/sanderos/drivers/test.sys ../bin/sanderos/drivers/rtl.sys

../bin/sanderos/drivers/ps2key.sys: ps_2_keyboard.c 
	$(CC) -c ps_2_keyboard.c -o ../bin/sanderos/drivers/ps2key.sys -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mgeneral-regs-only

../bin/sanderos/drivers/test.sys: test.c
	$(CC) -c test.c -o ../bin/sanderos/drivers/test.sys -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mgeneral-regs-only 

../bin/sanderos/drivers/rtl.sys: rtl.c
	$(CC) -c rtl.c -o ../bin/sanderos/drivers/rtl.sys -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mgeneral-regs-only 