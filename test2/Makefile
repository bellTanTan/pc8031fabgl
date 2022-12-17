D88FILE = [OS]_IPL_TEST_disk_(BASIC_formatted).d88

all : $(D88FILE)

$(D88FILE) : updateIPL ipltest.bin
	./updateIPL

updateIPL : updateIPL.c
	gcc -Wall -o updateIPL updateIPL.c

ipltest.bin : ipltest.asm
	z80asm --input=ipltest.asm --output=ipltest.bin --list=ipltest.lst
