CC=clang++ #Compiler
CFLAGS= -c -std=c++11  -Wall -g#Compiler Flags
LDFLAGS=-lportaudio -lSDL2 #Linker options
SOURCES=NewPlayer.cpp Recorder.cpp main.cpp Goertzel.cpp NewPhysicalLayer.cpp DTMFTones.cpp    #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE)
