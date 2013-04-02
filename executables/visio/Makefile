# Makefile for visio

OBJS = service.o camManager.o calibManager.o logger.o helper.o 
FLAGS = -std=gnu++11 `pkg-config --cflags --libs opencv` `pkg-config jsoncpp --cflags --libs` -lzmq
CC = g++

# construction de l'exe

visio : $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $@ 

# dependences
service.o : service.cpp
	g++ service.cpp -c 
camManager.o : camManager.cpp
	g++ camManager.cpp -c 
calibManager.o : calibManager.cpp
	g++ calibManager.cpp -c
logger.o : logger.cpp
	g++ logger.cpp -c
helper.o : helper.cpp
	g++ helper.cpp -c

clean_o: 
	rm *.o
clean:
	rm visio *.o
