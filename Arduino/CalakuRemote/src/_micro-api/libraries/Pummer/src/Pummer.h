// Pummer.h

#ifndef Pummer_h
#define Pummer_h

#include "arduino.h"

class Pummer {
public:
	byte lR, lG, lB;
	byte nR, nG, nB; //actual
	byte wR, wG, wB; //objtivo
	int pR, pG, pB;
	unsigned long last, when;
	boolean reverse;


	Pummer(int pinR, int pinG, int pinB, boolean anode = false);

	void show();

	boolean done();

	void goal(byte r, byte g, byte b, unsigned long speed = 500);

	void loop();
};

#endif

