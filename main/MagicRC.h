#ifndef _MagicRC_h_
#define _MagicRC_h_

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Feed_Motor.h"

using namespace std;

class MagicRC{
public:
	MagicRC(uint8_t receivePin, uint8_t transmitPin, uint8_t ia1,uint8_t ib1,uint8_t ia2,uint8_t ib2);
	void begin(long speed); // set bluetooth baud rate
  void attach_feed(Feed * obj);
	void setSpeed(uint8_t speed); // set speed
	void run(); // must call this when autonomous mode or control mode
	// control commands
	void forward();
	void backward();
	void turnLeft();
	void turnRight();
	void stop();
  void fire();
  void fire_force();

private:
	SoftwareSerial* btSerial;	// bluetooth class
  Feed* m;

	uint8_t speed;
	uint8_t leftWheel;
	uint8_t leftSpeed;
	uint8_t rightWheel;
	uint8_t rightSpeed;
  uint8_t fire_chk_rc;
};

#endif