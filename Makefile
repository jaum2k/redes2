all: projeto garbler teste

projeto: projeto.c 
	gcc -c projeto.c -o projeto.o
	ar  rcs libmean.a      projeto.o
	gcc -c -fPIC projeto.c -o projeto.o 
	gcc -shared -Wl,-soname,libmean.so.1 -o libmean.so.1.0.1 projeto.o
	
garbler: garbler.c 
	gcc -c garbler.c -o garbler.o
	ar  rcs libmean.a      garbler.o
	gcc -c -fPIC garbler.c -o garbler.o 
	gcc -shared -Wl,-soname,libmean.so.1 -o libmean.so.1.0.1 garbler.o

teste: teste.c
	gcc -static teste.c -L. -lmean -o teste -lpthread
	gcc teste.c -o dynamically_linked -L. -lmean -lpthread
