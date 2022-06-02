.PHONY: all, clean

all:
	$(CXX) test_case.cc -g -std=c++17 -DDEBUG -DMACRO='"TEST TEST"' -o test_case.o -c

clean:
	@rm -f *.o test_case
