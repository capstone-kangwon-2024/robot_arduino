#include <ArduinoSTL.h>
#include <string>
using namespace std;

#include <Wire.h>
#include "Pin_Config.h"  // 모든 하드웨어 핀 정보
#include "Feed_Motor.h"  // 간식 배급모터 제어 기능

#include <SoftwareSerial.h>
#include "MagicRC.h"  // 원격조종 프로그램 참조
MagicRC myRC(7, 8, motor_pin[1], motor_pin[0], motor_pin[3], motor_pin[2]); // Bluetooth TX, RX

#define FOR(i, b, e) for(int i=b; i<=e; i++)
#define dW digitalWrite
#define dR digitalRead
#define aW analogWrite
#define aR analogRead

// 수신명령어 리스트
#define COMMAND '1'
#define FORWARD "F"
#define BACKWARD "B"
#define LEFT     "L"
#define RIGHT    "R"
#define STOP     "S"
#define RC       "RC"
#define FIRE     "FIRE"
#define FORCE_FIRE "FFIRE"
#define CHANGE_MODE "M"
#define FEED        "Feed"

String data = "";       // i2c통신에서 수신한 명령어 저장
bool isCmdRecv = false; // 명령 수신 여부
void receiveEvent(int howMany) {
  data = "";

  while (Wire.available()) {
    char recv = Wire.read();

    data += recv;
  } 
  
  if(data[0] == COMMAND) { 
    data = data.substring(1);  // 젯슨나노로부터 명령 수신 시 첫번째 문자는 의미없는 문자임. 그러기에 가공함
    isCmdRecv = true;
  }

  data += '\0';
  
  printf("Recved = %s(%d)\n", data.c_str(), data.length());
}

int angle_info[2][2] = {{120, 160}, {0, 60}};  // 이상하게 어느순간 서보모터 기준위치가 뒤틀려짐. 그걸 위한 계수
int angle[2] = {0, 0};
Feed m;

void motor_set() {
  FOR(i, 0, 3) {
    m_speed[LEFT]  = 180;
    m_speed[RIGHT] = 165;
    m_speed[SERVO] = 10;

    angle[OPEN_ANG]  = angle_info[0][OPEN_ANG];
    angle[CLOSE_ANG] = angle_info[0][CLOSE_ANG];

    m.init(angle[OPEN_ANG], angle[CLOSE_ANG], m_speed[SERVO]);
    myRC.attach_feed(&m);

    pinMode(motor_pin[i], OUTPUT);
  }
}


void setup() {
  Serial.begin(9600); 
  myRC.begin(9600);
  Wire.begin(0x8);

  Wire.onReceive(receiveEvent);

  motor_set();
  pinMode(gun_pin, OUTPUT); dW(gun_pin, HIGH);
  pinMode(ir_pin, INPUT);
  pinMode(leds[0], OUTPUT);   pinMode(leds[1], OUTPUT);
  digitalWrite(leds[0], LOW); digitalWrite(leds[1], LOW);
}

bool gun_stat = HIGH;
bool ir_monitor = false;
bool rc_stat = false;
bool auto_deter_flag = false;
bool auto_deter_flag_root = true;
bool func1_stat = false;
int fire_chk = -1;
void loop() {
  static unsigned long now = 0;
  static unsigned long ir_tick=0, fire_tick=0, wdt_tick=0;
  
  now = millis();
  
  // 명령어 수신 시 해석 및 처리를 진행합니다
  if(isCmdRecv) {
    if(data == FORWARD) {          // 전진
      dW(LEFT_M  , 0);
      aW(LEFT_M+1, m_speed[LEFT]);

      dW(RIGHT_M  , 0);
      aW(RIGHT_M+1, m_speed[RIGHT]);
    } else if(data == BACKWARD) {  // 후진
      dW(LEFT_M  , HIGH);
      aW(LEFT_M+1, 255-m_speed[LEFT]);

      dW(RIGHT_M  , HIGH);
      aW(RIGHT_M+1, 255-m_speed[RIGHT]);
    } else if(data == LEFT) {      // 좌회전
      aW(LEFT_M,   m_speed[LEFT]);
      dW(LEFT_M+1, 0);

      aW(RIGHT_M,   0); 
      aW(RIGHT_M+1, m_speed[RIGHT]);
    } else if(data == RIGHT) {     // 우회전
      aW(LEFT_M,   255-m_speed[LEFT]);
      dW(LEFT_M+1, HIGH);
      
      aW(RIGHT_M,   m_speed[RIGHT]);
      dW(RIGHT_M+1, 0);
    } else if(data == STOP) {      // 정지
      dW(LEFT_M  , 0);
      dW(LEFT_M+1, 0);

      dW(RIGHT_M  , 0);
      dW(RIGHT_M+1, 0);
    } else if(data == FIRE) {      // 발사 
      if(auto_deter_flag_root==false && auto_deter_flag) goto END_CMD;
      if(dR(ir_pin) == LOW) {
        dW(leds[0], HIGH);
        dW(gun_pin, LOW);
        fire_chk = GOGOGO;  // 현재 상태를 발사상태로 변경
      } else {
        fire_chk = ERROR;  // 발사할 공이 없으므로 오류.
      }
      
    } else if(data == FORCE_FIRE) {  // 강제발사
      cout << "강제발사 수신" << endl;
      dW(leds[0], HIGH);
      dW(gun_pin, LOW);
      fire_tick = now+1500;
      fire_chk = GOGOGO;  // 현재 상태를 발사상태로 변경
    } else if(data == RC) {    // 원격조종모드
      isCmdRecv = false;
      rc_stat = !rc_stat;
      dW(leds[1], rc_stat);
    } else if(data == FEED) {  // 간식 배급
      m.act();
    } else if(data.indexOf("SPD") != -1) {   // 속도조절 명령
      int L_spd, R_spd;
      int servo_delay = -1;

      if(6 < data.length()) {
        L_spd = data.substring(3, 6).toInt();
        R_spd = data.substring(6, 9).toInt();
      } else {
        servo_delay = data.substring(3).toInt();
      }
      
      
      if(servo_delay == -1) {  
        m_speed[LEFT] = L_spd; m_speed[RIGHT] = R_spd;
        cout << "변경 SPD = " << L_spd << ", " << R_spd << endl;
      }
      else {
        m.change_delay(servo_delay);
      }
    } else if(data.indexOf("ANG1") != -1) {  // 간식 배급모터 설정명령1
      int a1 = data.substring(4, 7).toInt();
      int a2 = data.substring(7).toInt();

      angle_info[0][OPEN_ANG] = a1; angle_info[0][CLOSE_ANG] = a2;
      m.change_angle(a1, a2);

      if(m.getMode()) m.toggle_mode();

      cout << "변경 ANG1 = " << a1 << ", " << a2 << endl;
    } else if(data.indexOf("ANG2") != -1) {  // 간식 배급모터 설정명령2
      int a1 = data.substring(4, 7).toInt();
      int a2 = data.substring(7).toInt();

      angle_info[1][OPEN_ANG] = a1; angle_info[1][CLOSE_ANG] = a2;
      m.change_angle(a1, a2);

      if(!m.getMode()) m.toggle_mode();

      cout << "변경 ANG2 = " << a1 << ", " << a2 << endl;
    } else if(data.indexOf("TOG_ANG") != -1) {  // 간식 배급모터 설정명령3
      int changed_ang[2];

      if(m.getMode()) { changed_ang[OPEN_ANG] = angle_info[0][OPEN_ANG]; changed_ang[CLOSE_ANG] = angle_info[0][CLOSE_ANG]; }
      else            { changed_ang[OPEN_ANG] = angle_info[1][OPEN_ANG]; changed_ang[CLOSE_ANG] = angle_info[1][CLOSE_ANG]; }

      m.toggle_mode();
      m.change_angle(changed_ang[OPEN_ANG], changed_ang[CLOSE_ANG]);

      cout << "각도 토글 = " << changed_ang[OPEN_ANG] << ", " << changed_ang[CLOSE_ANG] << endl;
    } END_CMD:
    isCmdRecv = false;  // 수신 명령 처리하였으니 비활성화
  }
  
    if(dR(ir_pin) == HIGH) {  // 공이 발사되었을 시
      digitalWrite(leds[0], LOW); 
      digitalWrite(leds[1], rc_stat);
      func1_stat = false;
      dW(gun_pin, HIGH);
      fire_chk = STANDBY;
    }
    
    fire_tick = now;
  } 

  // 원격조종 프로그램 참조실행
  myRC.run();
}