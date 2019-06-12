prog: LEDS.o
	gcc -o prog LEDS.o -lpthread -lwiringPi
	
LEDS.o: LEDS.c
	gcc -c LEDS.c
