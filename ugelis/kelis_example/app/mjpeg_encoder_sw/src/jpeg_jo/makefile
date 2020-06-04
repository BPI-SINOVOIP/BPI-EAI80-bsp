# makefile for ffjpeg project
# written by rockcarry

CC      = gcc
CCFLAGS = -Wall -O0 -g

jpeg.o:
	gcc    -c -g -o jpeg.o jpeg.c
	
mjpeg.o:
	gcc    -c -g -o mjpeg.o mjpeg.c

ff_wrapper.o:
	gcc    -c -g -o ff_wrapper.o ff_wrapper.c	

OBJS = \
    jpeg.o mjpeg.o ff_wrapper.o
    

EXES = \
    mjpeg



all : $(EXES)

$(EXES) : $(OBJS)
	$(CC)   $(CCFLAGS) -o $@ $(OBJS) -lm 
clean :
	-rm -f *.o


# rockcarry
# 2016.1.5
# THE END



