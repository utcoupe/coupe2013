# Makefile for visio

OBJS = main.o camManager.o logger.o
FLAGS = -std=c++0x -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy
CC = g++

# construction de l'exe

visio : $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

# dependences
main.o : camManager.h logger.h
camManage.o : camManager.h
logger.o :

clean : 
	rm visio *.o
