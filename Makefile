OS:=$(shell lsb_release -si) #OS
CC=clang++ #Compiler
CFLAGS= -c -std=c++11 -w -Ofast   #Compiler Flags

ifeq ($(OS),Arch)
    LDFLAGS=-lportaudio -lSDL2 -lboost_filesystem -lboost_system -lcryptopp -lbz2 -lreadline#Linker options
else
    LDFLAGS=-lportaudio -lSDL2 -lboost_filesystem -lboost_system -lcrypto++ -lbz2 -lreadline#Linker options
endif
SOURCES= AppLayer.cpp DTMFTones.cpp Player.cpp Recorder.cpp main.cpp Goertzel.cpp PhysicalLayerEmu.cpp DataLinkLayer.cpp  autoCompletion.cpp #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE) newmedia.txt media.txt
