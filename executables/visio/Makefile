# Makefile for visio

OBJS = camManager.o logger.o calibManager.o helper.o service.o
FLAGS = `pkg-config jsoncpp --cflags --libs` -lzmq -std=c++0x -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy
CC = gcc

# construction de l'exe

visio : $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

# dependences
# main.o : camManager.h logger.h calibManager.h colorsets.h
camManager.o : camManager.h helper.h
calibManager.o : camManager.h colorsets.h
logger.o :
helper.o : helper.h
service.o : zhelpers.hpp service.hpp camManager.h logger.h calibManager.h

clean_o: 
	rm *.o
clean:
	rm visio *.o
