nash :
	mkdir -p bin
	gcc main.c modules/input.c modules/common.c -o bin/nash
debug :
	mkdir -p bin 
	gcc main.c modules/input.c modules/common.c -o bin/nash -g -static -Wall 
clean :
	rm -r bin
