# Makefile for visio

OBJS = service.o camManager.o calibManager.o logger.o helper.o 
FLAGS = 
CC = g++

# construction de l'exe

visio : $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

# dependences
service.o : service.cpp
	g++ service.cpp -c `pkg-config jsoncpp --cflags --libs` -lzmq
camManager.o : camManager.cpp
	g++ camManager.cpp -c `pkg-config --cflags --libs opencv`
calibManager.o : calibManager.cpp
	g++ calibManager.cpp -c `pkg-config --cflags --libs opencv`
logger.o : logger.cpp
	g++ logger.cpp -c
helper.o : helper.cpp
	g++ helper.cpp -c

clean_o: 
	rm *.o
clean:
	rm visio *.o
