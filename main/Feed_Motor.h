#ifndef _Feed_Motor_h_
#define _Feed_Motor_h_

#include <Servo.h>
#include "Pin_Config.h"

#define SERVO "servo"
#define OPEN_ANG 0
#define CLOSE_ANG 1

class Feed {
  private:
    Servo m;
    int pin;
    int angle[2];
    int s_delay;
    bool cur_ang_mode;

  public:
    void init(int open_angle, int close_angle, int m_delay);
    void change_angle(int a, int b);
    void act();
    void toggle_mode();
    void change_delay(int a);
    bool getMode();
};

#endif