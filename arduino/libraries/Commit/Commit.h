#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

#ifndef Commit_h
#define Commit_h

class Commit{
	private:
		unsigned long _start;
		uint32_t _color;
		int _ringIndex;
		int _ringLength;
		int _stripLength;
		int _nSequences;
	public:
		Commit(unsigned long start, uint32_t color, int ringIndex, int ringLength, int stripLength, int nSequences);
		void update();
		void update(Adafruit_NeoPixel strip, Adafruit_NeoPixel ring);
};

Commit::Commit(unsigned long start, uint32_t color, int ringIndex, int ringLength, int stripLength, int nSequences)
{
	_start = start;
	_color = color;
	_ringIndex = ringIndex;
	_ringLength = ringLength;
	_stripLength = stripLength;
	_nSequences = nSequences;
	delay(100);
}

void Commit::update(Adafruit_NeoPixel strip, Adafruit_NeoPixel ring)
{
	
}

void Commit::update()
{
	Serial.println("myNSequences: " + _nSequences);
}



#endif
