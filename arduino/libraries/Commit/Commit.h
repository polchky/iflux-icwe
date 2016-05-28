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
		unsigned long _millis;
		unsigned long _step;
		int offset(int pos);
		
	public:
		Commit(unsigned long start, uint32_t color, int ringIndex, int ringLength, int stripLength, int nSequences);
		void update();
		void update(Adafruit_NeoPixel strip, Adafruit_NeoPixel ring);
		boolean isFinished();
};

Commit::Commit(unsigned long start, uint32_t color, int ringIndex, int ringLength, int stripLength, int nSequences)
{
	_start = start;
	_color = color;
	_ringIndex = ringIndex;
	_ringLength = ringLength;
	_stripLength = stripLength;
	_nSequences = nSequences;
	_step = 200;
}

void Commit::update(Adafruit_NeoPixel strip, Adafruit_NeoPixel ring)
{
	_millis = millis();
	if(_millis < _start){
		Serial.println("too soon!");
		return;
	}
	int currentStep = (_millis - _start) / _step;
	Serial.println("oki");
	Serial.println(currentStep);
	Serial.println(_millis);
	Serial.println(_start);
}

void Commit::update()
{
	
}

boolean Commit::isFinished()
{
	return false;
}

int Commit::offset(int pos)
{
	return _stripLength - 1 - pos;
}


#endif
