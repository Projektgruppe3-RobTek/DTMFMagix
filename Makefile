CC=clang++ #Compiler
CFLAGS= -c -std=c++11 -Ofast   -fPIE   #Compiler Flags
DEFINES=
INCPATH=-I/usr/include/qt5/QtWidgets/ -I/usr/include/qt5/ -I/usr/include/qt5/QtCore/ -I/usr/include/qt/QtWidgets/ -I/usr/include/qt/ -I/usr/include/qt/QtCore/

LDFLAGS=-lportaudio -lSDL2 -lboost_filesystem -lboost_system -lcryptopp -lbz2 -lreadline -lQt5Core -lQt5Gui -lQt5Widgets#Linker options

SOURCES= AppLayer.cpp DTMFTones.cpp Player.cpp Recorder.cpp main.cpp Goertzel.cpp PhysicalLayer.cpp DataLinkLayer.cpp autoCompletion.cpp $(QTCPP) $(MOCFILES) #cpp files

OBJECTS=$(SOURCES:.cpp=.o)  #Object files
EXECUTEABLE=DTMFMagix #Output name
HEADERS=ui_dtmfmagix.h
MOCFILES=$(addprefix moc_,$(QTCPP))
QTCPP= mythread.cpp dtmfmagix.cpp
all: $(HEADERS) $(SOURCES) $(EXECUTEABLE)
	
$(EXECUTEABLE): $(OBJECTS) 
	$(CC)    $(OBJECTS) -o $(EXECUTEABLE) $(LDFLAGS)


.cpp.o:
	$(CC)  $(CFLAGS) $(INCPATH) $(DEFINES)   $< -o $@


clean:  ; rm *.o $(EXECUTEABLE) newmedia.txt media.txt $(MOCFILES) $(HEADERS) 


moc_%.cpp: %.h
		moc $(DEFINES) $(INCPATH) $< -o $@

ui_%.h: %.ui
		uic $< -o $@

