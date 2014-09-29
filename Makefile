CC=clang++ #Compiler
CFLAGS= -c -std=c++11  -g#Compiler Flags
LDFLAGS=-lportaudio -lSDL2 #Linker options
SOURCES=Player.cpp Recorder.cpp main.cpp Goertzel.cpp   #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE)
