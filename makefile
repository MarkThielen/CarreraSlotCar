CC = g++
CFLAGS = -std=c++11

PB=protoc

PB_DIRS=./pb
PB_FLAGS=--cpp_out=$(PB_DIRS)

LDFLAGS = 
LDLIBS = -lncurses -largtable2 -lprotobuf -lzmq
DEBUGFLAGS=-ggdb

all: clean ProtoBuf SlotHub

SlotHub: ProtoBuf
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) SlotHub.c ttyTools.c ControlUnit.cpp TextDisplay.cpp CarStatus.cpp TrackStatus.cpp CarreraResponse.cpp pb/TrackStatusMessage.pb.cc pb/CarStatusMessage.pb.cc -o SlotHub

ProtoBuf:
	$(PB) -I$(PB_DIRS) $(PB_FLAGS) $(PB_DIRS)/CarStatusMessage.proto
	$(PB) -I$(PB_DIRS) $(PB_FLAGS) $(PB_DIRS)/TrackStatusMessage.proto	


CarStatus:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) CarStatus.cpp

CarreraResponse:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) CarreraResponse.cpp


TrackStatus:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) TrackStatus.cpp


ControlUnit:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) ControlUnit.cpp

clean:
	rm -rf *o SlotHub
	rm -f $(PB_DIRS)/*.pb.cc $(PB_DIRS)/*.pb.h
