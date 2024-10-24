#include "Arduino.h"
#include "MagicRC.h"
#include "Pin_Config.h"

MagicRC::MagicRC(uint8_t receivePin, uint8_t transmitPin, uint8_t ia1,uint8_t ib1,uint8_t ia2,uint8_t ib2) {
  btSerial = new SoftwareSerial(receivePin, transmitPin);
  btSerial->setTimeout(125);
  leftSpeed = ia1;
  leftWheel = ib1;
  rightSpeed = ia2;
  rightWheel = ib2;
  fire_chk_rc = STANDBY;
  pinMode(leftSpeed, OUTPUT);
  pinMode(leftWheel, OUTPUT);
  pinMode(rightSpeed, OUTPUT);
  pinMode(rightWheel, OUTPUT);
  pinMode(gun_pin, OUTPUT); digitalWrite(gun_pin, HIGH);
  pinMode(ir_pin, INPUT);
  digitalWrite(rst_pin, HIGH); pinMode(rst_pin, OUTPUT);
  pinMode(leds[0], OUTPUT); pinMode(leds[1], OUTPUT);
  digitalWrite(leds[0], LOW); digitalWrite(leds[1], LOW);
  setSpeed(255);
}

void MagicRC::begin(long baudrate) {
  btSerial->begin(baudrate);
}

void MagicRC::attach_feed(Feed * obj) {
  m = obj;
}

void MagicRC::setSpeed(uint8_t speed_) {
  speed = speed_;
}

void MagicRC::run() {
  static unsigned long now = 0;
  static unsigned long ir_tick=0, fire_tick=0;
  
  now = millis();
  
  if (btSerial->available()) {
    char c = btSerial->read();

    switch (c) {
      case 'S':
        stop();
        break;
      case 'F':
        forward();
        break;
      case 'B':
        backward();
        break;
      case 'L':
        turnLeft();
        break;
      case 'R':
        turnRight();
        break;
      case '0':
        setSpeed(0);
        break;
      case '1':
        setSpeed(25);
        break;
      case '2':
        setSpeed(50);
        break;
      case '3':
        setSpeed(75);
        break;
      case '4':
        setSpeed(100);
        break;
      case '5':
        setSpeed(125);
        break;
      case '6':
        setSpeed(150);
        break;
      case '7':
        setSpeed(175);
        break;
      case '8':
        setSpeed(200);
        break;
      case '9':
        setSpeed(225);
        break;
      case 'q':
        setSpeed(255);
        break;
      case 'K':
        fire_tick = now;
        fire();
        break;
      case 'k':
        break;
      case 'P':
        m->act();
        break;
      case 'p':
        m->act();
        break;
      case 'N':
        digitalWrite(rst_pin, LOW);
        break;
      case 'n':
        
        break;
      
    }
  }

  if(fire_chk_rc == GOGOGO && fire_tick < now && now - fire_tick >= 50) {
    if(digitalRead(ir_pin) == HIGH) {  // 공이 발사되었을 시
      digitalWrite(leds[0], LOW); 
      digitalWrite(leds[1], HIGH);
      digitalWrite(gun_pin, HIGH);
      fire_chk_rc = STANDBY;
    }
    
    fire_tick = now;
  }
}

void MagicRC::forward() {
  digitalWrite(leftWheel, LOW);
  analogWrite(leftSpeed, speed);
  digitalWrite(rightWheel, LOW);
  analogWrite(rightSpeed, speed-10);
}

void MagicRC::backward() {
  digitalWrite(leftWheel, HIGH);
  analogWrite(leftSpeed, 255-speed);
  digitalWrite(rightWheel, HIGH);
  analogWrite(rightSpeed, 255-speed+10);
}

void MagicRC::turnLeft() {
  digitalWrite(leftWheel, HIGH);
  analogWrite(leftSpeed, 255-speed);
  digitalWrite(rightWheel, LOW);
  analogWrite(rightSpeed, speed);
}

void MagicRC::turnRight() {
  digitalWrite(leftWheel, LOW);
  analogWrite(leftSpeed, speed);
  digitalWrite(rightWheel, HIGH);
  analogWrite(rightSpeed, 255-speed);
}

void MagicRC::stop() {
  digitalWrite(leftWheel, LOW);
  digitalWrite(rightWheel, LOW);
  analogWrite(leftSpeed,0);
  analogWrite(rightSpeed,0);
}

void MagicRC::fire() {
  if(digitalRead(ir_pin) == LOW) {
    digitalWrite(leds[0], HIGH);
    digitalWrite(leds[1], LOW);
    digitalWrite(gun_pin, LOW);
    fire_chk_rc = GOGOGO;  // 현재 상태를 발사상태로 변경
  } else {
    fire_chk_rc = ERROR;  // 발사할 공이 없으므로 오류.
  }
}

void MagicRC::fire_force() {
  digitalWrite(leds[0], HIGH);
  digitalWrite(leds[1], LOW);
  digitalWrite(gun_pin, LOW);
  fire_chk_rc = GOGOGO;  // 현재 상태를 발사상태로 변경
}