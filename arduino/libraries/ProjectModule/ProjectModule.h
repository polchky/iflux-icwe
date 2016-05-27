#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "LinkedList.h"
#include "Commit.h"

#ifndef ProjectModule_h
#define ProjectModule_h

class ProjectModule
	{
		public:
			ProjectModule(int stripPin, int nStripLeds, int ringPin, int nRingLeds, int ringOffset);
			void init();
			void addCommit(Commit *commit);
			void update();
			void stop();
			void setRingDisplay(uint32_t colors[]);
			void playCommits(unsigned long start, uint32_t color, int nSequences, int ringPixels[], int nRingPixels);
			void test(int n);
		private:
			int offset(int pos);

			Adafruit_NeoPixel _ring;
			Adafruit_NeoPixel _strip;
			int _nRingLeds;
			int _nStripLeds;
			int _ringOffset;
			unsigned long _millis;
			LinkedList<Commit*> _commits;
	};

ProjectModule::ProjectModule(int stripPin, int nStripLeds, int ringPin, int nRingLeds, int ringOffset)
{
        _strip = Adafruit_NeoPixel(nStripLeds, stripPin, NEO_GRB + NEO_KHZ800);
        _nStripLeds = nStripLeds;
        _ring  = Adafruit_NeoPixel(nRingLeds,  ringPin,  NEO_GRB + NEO_KHZ800);
        _nRingLeds = nRingLeds;
        _ringOffset = ringOffset;
	_commits = LinkedList<Commit*>();
}

void ProjectModule::init()
{
        _strip.begin();
        _strip.show();
        _ring.begin();
        _ring.show();
}

void ProjectModule::addCommit(Commit *commit)
{
	_commits.add(commit);
}

void ProjectModule::update()
{
        _millis = millis();
	int index = 0;
	int size = _commits.size();
	while (index < size){
		_commits.get(index)->update();
		index++;
	}
}

void ProjectModule::stop()
{

}

int ProjectModule::offset(int pos)
{
        return (pos + _ringOffset) % _nRingLeds;
}

void ProjectModule::setRingDisplay(uint32_t colors[])
{
	        for (int i=0; i<_nRingLeds; i++) {
			                _strip.setPixelColor(offset(i), colors[i]);
					        }
		        _ring.show();
}

void ProjectModule::playCommits(unsigned long start, uint32_t color, int nSequences, int ringPixels[], int nRingPixels)
{

}

void ProjectModule::test(int n)
{
	        for(int i=0; i<16; i++){
			                _ring.setPixelColor(i, _ring.Color(0, 0, 0));
					        }
		        _ring.setPixelColor(n, _ring.Color(100, 0, 0));
			        Serial.println(_ring.Color(100, 0, 0));
				        Serial.println(_ring.Color(100, 0, 1));
					        _ring.show();
}


#endif
