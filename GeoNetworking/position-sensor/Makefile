all: position_sensor

position_sensor: position_sensor.o
	gcc -o position_sensor position_sensor.o -lgps -lm

position_sensor.o: position_sensor.c
	gcc -o position_sensor.o -c position_sensor.c -lgps -W -Wall -std=gnu89 -pedantic -lm

clean:
	rm -rf *.o

mrproper: clean
	rm -rf position_sensor

