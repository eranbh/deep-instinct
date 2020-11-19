obj-m += lkm_sort.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	g++ -Wall -std=c++17 main.cpp -o main
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
