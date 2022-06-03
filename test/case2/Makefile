.PHONY: all, clean

all:
	$(CXX) test_case.cc -g -std=c++17 -DDEBUG -DMACRO='"TEST TEST"' -o test_case.o -c
	$(CXX) test_case.o -o test_case

clean:
	@rm -f *.o test_case
