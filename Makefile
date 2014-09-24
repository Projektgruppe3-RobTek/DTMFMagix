CC=clang++ #Compiler
CFLAGS= -c -std=c++11  -Ofast#Compiler Flags
LDFLAGS=-lportaudio -lSDL2 -lallegro -lallegro_primitives -lfftw3 #Linker options
SOURCES=Player.cpp Recorder.cpp main.cpp   #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE)
