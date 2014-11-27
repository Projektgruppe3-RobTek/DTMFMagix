OS:=$(shell lsb_release -si) #OS
CC=x86_64-w64-mingw32-g++.exe #Compiler
CFLAGS= -c -std=c++11 -g -DWIN    #Compiler Flags

ifeq ($(OS),Arch)
    LDFLAGS=-lportaudio -lSDL2 -lboost_filesystem -lboost_system -lcryptopp -lbz2 -lreadline#Linker options
else
    LDFLAGS=  -Bstatic -lboost_filesystem -lboost_system -lboost_system-mt -lboost_thread-mt  -lbz2 -lreadline -lpthread -lSDL2main -lSDL2 -lmingw32 -lportaudio   #Linker options
endif
SOURCES= AppLayer.cpp DTMFTones.cpp Player.cpp Recorder.cpp main.cpp Goertzel.cpp PhysicalLayer.cpp DataLinkLayer.cpp  autoCompletion.cpp #cpp files
OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name

all: $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)

.cpp.o:
	$(CC)  $(CFLAGS)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE) newmedia.txt media.txt
