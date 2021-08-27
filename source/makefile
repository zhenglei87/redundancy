GCC=gcc
GCPP=g++
RM=rm
LD=ld


all:
	$(LD) -r -b binary -o usage.o usage.txt
	$(GCC) -g -Wall main.c common.c recover.c split.c usage.o -I .\
	
clean:
	$(RM) -f *.a
	$(RM) -f *.o
	$(RM) -f *.dll
	$(RM) -f *.exe
	$(RM) -f *.s
	$(RM) -f *.r