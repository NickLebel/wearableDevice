
#
#	Makefile for wearable device
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoarmv7le
#TARGET = -Vgcc_ntoaarch64le



CFLAGS += $(DEBUG) $(TARGET) -Wall
LDFLAGS+= $(DEBUG) $(TARGET)
BINS = dataGen server
all: $(BINS) 

clean:
	rm -f *.o $(BINS) dataGen.d server.d;

server.o: server.c defs.h 
dataGen.o: dataGen.c defs.h
