.PHONY: all, clean

all:
	$(CC) test_case.c -MM -MF test_case.o.d -MT test_case.o -o test_case.o -c
	$(CC) test_case.c -MM -MF test_case.oo.d -MQ test_case.oo -o test_case.o -c

clean:
	@rm -f *.o test_case *.d
