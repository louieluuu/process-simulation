all:
	gcc -g -Wall -Werror -c main.c pcb.c running.c queue.c \
	ipc.c semaphore.c info.c init.c
	gcc -g -Wall -Werror -o main main.o pcb.o running.o queue.o \
	ipc.o semaphore.o info.o list.o init.o

clean:
	rm main.o pcb.o running.o queue.o ipc.o semaphore.o info.o init.o main

zip: 
	zip a3.zip makefile README \
	main.c pcb.c running.c queue.c ipc.c semaphore.c info.c init.c \
	pcb.h running.h queue.h ipc.h semaphore.h info.h init.h \
	list.h list.o structs.h 