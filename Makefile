GCC=g++
all: test

gc.o: gc.cpp gc.h
	$(GCC) -fPIC -g -c -Wall gc.cpp
    
test: libgc.so
	$(GCC) -o test -L. -lgc main.cpp
    
libgc.so: gc.o
	$(GCC) -shared -Wall,-soname,libgc.so \
        	-o libgc.so gc.o
     
