# All Targets
all: calc

# Tool invocations
# Executable "calc" depends on the file calc.o
calc: calc.o calcasm.o
	gcc -g -Wall -o calc calc.o calcasm.o

calc.o: calc.c 
	gcc -g -Wall -c -o calc.o calc.c 

calcasm.o: calc.asm
	nasm -g -f elf64 -w+all -o calcasm.o calc.asm

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o calc