#ifndef _pin_config_h_
#define _pin_config_h_

/* 모터 핀
 * 5, 6   : 왼쪽 모터
 * 10, 11 : 오른쪽 모터
*/
const int motor_pin[] = {
    5, 6, 10, 11
};

#define LEFT_M motor_pin[0]
#define RIGHT_M motor_pin[2]

const int gun_pin = 2;

const int ir_pin = A0;

const int leds[] = { 9, 12 };

// 상태변수
enum {
  STANDBY=0, GOGOGO=1, ERROR=-1
};

const int servo_pin = 3;

const int rst_pin = A1;

#endif