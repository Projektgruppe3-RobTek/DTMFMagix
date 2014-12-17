DTMFMagix
=========

This is DTMFMagix.

DTMFMagix is an application which can establish a communication between two computers using a speaker and a microphone at each end.

It communicates using DTMF-Tones, which is detected at the reciever side.

DTMFMagix implements a DataLinkLayer to enable errordetection and resending of lost messages.

The application implements a file-transfer frontend to demonstrate the communication.

Compilation and Requirements
----------------------------

The application depends on the following external libraries

* Portaudio  -- To play audio
* SDL2       -- To record audio
* Boost      -- For various filesystem operations
* Crypto++   -- For md5sum calculation
* bzip2      -- For compression
* readline   -- For interactive commandline usage
* QT5		 -- For GUI

Furthermore it relies on the standard c++11 library and either clang++ or g++.

### Ubuntu/Debian etc.

install the requeried libraries

`sudo apt-get install portaudio19-dev libsdl2-dev libboost-all-dev libcrypto++-dev libbz2-dev libreadline-dev clang build-essential libjack-dev libjack0 qt5-default`

Compile

`make`

Run the application

`./DTMFMagix`

### Windows

Download the required libraries and link them corretly (this should be possible. It is a challenge however)


