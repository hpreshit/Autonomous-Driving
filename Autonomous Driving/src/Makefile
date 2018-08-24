include sources.mk

CC=g++
CFLAGS = #-g3 -O0 #-std=gnu11
LIBFLAGS = `pkg-config --libs opencv` -pthread -lrt -lwiringPi  -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video
INCLUDE:= -I./../include
OUTPUT_FILE = rtes_project.out

SRCS:= *.c *.cpp

.PHONY:all
all:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUTPUT_FILE) $(SRCS) $(LIBFLAGS)

.PHONY:clean
clean:
	rm -f $(OUTPUT_FILE)
