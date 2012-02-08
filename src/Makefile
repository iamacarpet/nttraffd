all: nttraffd

nttraffd: main.o ttraff.o functions.o config.o pidfile.o Chameleon.o
	g++ -lcurl main.o ttraff.o functions.o config.o pidfile.o Chameleon.o -o nttraffd

main.o: main.cpp main.h
	g++ -c main.cpp

ttraff.o: ttraff.cpp ttraff.h
	g++ -c ttraff.cpp

functions.o: functions.cpp functions.h
	g++ -c functions.cpp

config.o: config.cpp config.h
	g++ -c config.cpp

pidfile.o: libs/pidfile.cpp libs/pidfile.h
	g++ -c libs/pidfile.cpp

Chameleon.o: libs/Chameleon.cpp libs/Chameleon.h
	g++ -c libs/Chameleon.cpp

clean:
	rm -fr *.o nttraffd

install: nttraffd
	install -m 755 nttraffd /usr/local/bin/nttraffd
