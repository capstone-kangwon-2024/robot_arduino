#include "Arduino.h"
#include "Feed_Motor.h"

void Feed::init(int open_angle, int close_angle, int m_delay) {
  cur_ang_mode = false;
  pin = servo_pin;
  angle[OPEN_ANG] = open_angle;
  angle[CLOSE_ANG] = close_angle;
  s_delay = m_delay;
}

void Feed::toggle_mode() {
  cur_ang_mode = !cur_ang_mode;
}

void Feed::change_angle(int a, int b) {
  angle[OPEN_ANG] = a;
  angle[CLOSE_ANG] = b;
}

void Feed::change_delay(int a) {
  s_delay = a;
}

void Feed::act() {
  m.attach(pin);
  m.write(angle[OPEN_ANG]);
  delay(300);

  int i=angle[OPEN_ANG];
  if(angle[OPEN_ANG] <= angle[CLOSE_ANG]) {
    for(; i <= angle[CLOSE_ANG]; i++) {
      m.write(i);
      delay(s_delay);
    }
  } else {
    for(; i >= angle[CLOSE_ANG]; i--) {
      m.write(i);
      delay(s_delay);
    }
  }
  delay(50);
  m.detach();
}

bool Feed::getMode() {
  return cur_ang_mode;
}