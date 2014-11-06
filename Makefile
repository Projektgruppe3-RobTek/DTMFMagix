CC=clang++ #Compiler
CFLAGS= -c -std=c++11  -Wall  #Compiler Flags
LDFLAGS=-lportaudio -lSDL2 -lboost_filesystem -lboost_system #Linker options
SOURCES= DTMFTones.cpp NewPlayer.cpp Player.cpp Recorder.cpp Receiver.cpp Goertzel.cpp physicalLayerEmu.cpp NewPhysicalLayer.cpp DataLinkLayer.cpp   #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=Receiver #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE) newmedia.txt media.txt
