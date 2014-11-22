CC=clang++-3.5 #Compiler
CFLAGS= -c -std=c++11  -Wall   #Compiler Flags
LDFLAGS=-lportaudio -lSDL2 -lboost_filesystem -lboost_system -lcrypto++#Linker options
SOURCES= AppLayer.cpp DTMFTones.cpp Player.cpp Recorder.cpp main.cpp Goertzel.cpp PhysicalLayer.cpp DataLinkLayer.cpp   #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE) newmedia.txt media.txt
