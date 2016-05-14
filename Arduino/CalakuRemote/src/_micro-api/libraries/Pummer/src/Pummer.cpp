// 
// 
// 
#include "Arduino.h"
#include "Pummer.h"



Pummer::Pummer(int pinR, int pinG, int pinB, boolean anode)
	{
		pinMode(pR = pinR, OUTPUT);
		pinMode(pG = pinG, OUTPUT);
		pinMode(pB = pinB, OUTPUT);
		nR = nG = nB = 0;
		reverse = anode;
		show();
		goal(255, 255, 255);
	}

void Pummer::show()
	{
		analogWrite(pR, reverse ? (255 - nR) : nR);
		analogWrite(pG, reverse ? (255 - nG) : nG);
		analogWrite(pB, reverse ? (255 - nB) : nB);
	}

boolean Pummer::done() 
	{ 
		return last == when; 
	}

void Pummer::goal(byte r, byte g, byte b, unsigned long speed)
	{
		lR = nR; lG = nG; lB = nB;
		wR = r; wG = g; wB = b;
		last = millis();
		when = last + speed;
	}

void Pummer::loop()
	{
		unsigned long now = millis();
		if (now > when)
		{
			if (last == when)
				return;
			nR = wR; nG = wG; nB = wB;
			last = when;
		}
		else
		{
			nR = map(now, last, when, lR, wR);
			nG = map(now, last, when, lG, wG);
			nB = map(now, last, when, lB, wB);
		}
		show();
	}

