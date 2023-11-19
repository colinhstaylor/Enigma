enigma: main.o enigma.o
	g++ -g main.o enigma.o -o enigma
main.o: main.cpp enigma.h errors.h
	g++ -Wall -g -c main.cpp
enigma.o: enigma.cpp enigma.h errors.h
	g++ -Wall -g -c enigma.cpp

.PHONY : clean
clean:
	rm -f *.o enigma