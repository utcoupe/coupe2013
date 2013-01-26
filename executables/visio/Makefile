# Makefile for visio

OBJS = main.o camManager.o logger.o calibManager.o helper.o
FLAGS = -g -std=c++0x -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy
CC = g++

# construction de l'exe

visio : $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

# dependences
main.o : camManager.h logger.h calibManager.h colorsets.h
camManager.o : camManager.h helper.h
calibManager.o : camManager.h colorsets.h
logger.o :
helper.o : helper.h


clean_o: 
	rm *.o
clean:
	rm visio *.o
