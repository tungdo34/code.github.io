//Khai bao th vien
#include <avr/interrupt.h>
#include <Servo.h>
#include <Wire.h>

//Khai bao cac chan
#define cb_rem A0
#define cb_nd A1
#define cb_mua A2
#define den_pn 13
#define quat_pn_enb 12
#define quat_pn 11
const int rem = 10;
#define day_phoi 9
#define den_vs 8
#define button_den_pn 2
#define button_quat_pn 3
#define button_den_vs 4

//Servo servo;
//Bien trang thai cua thiet bi
volatile int nhiet_do=25;
volatile bool state_den_pn = false;
volatile bool state_quat_pn = false;
volatile bool state_den_vs = false;
volatile bool state_rem = false;
long last;
char cmd;  //Nhan lenh tu app
/*                                                                                                                      
    + Den phong khach: 0, 1                                                                                               
    + Quat phong khach: 2, 3                                                                                              
    + Den phong ngu: 4, 5                                                                                                 
    + Quat phong ngu: 6, 7                                                                                                
    + Den phong bep: 8, 9                                                                                                 
    + Quat phong bep: a, b                                                                                                
    + Den phong ve sinh: c, d                                                                                             
    + Quat phong ve sinh: e, f                                                                                            
*/
Servo svr_rem, svr_day;
void setup() {
  Serial.begin(9600);

  pinMode(den_pn, OUTPUT);
  pinMode(quat_pn, OUTPUT);
  pinMode(quat_pn_enb, OUTPUT);
  pinMode(button_den_pn, INPUT_PULLUP);
  pinMode(button_quat_pn, INPUT_PULLUP);
  pinMode(den_vs, OUTPUT);
  pinMode(button_den_vs, INPUT_PULLUP);
  pinMode(cb_rem, INPUT);
  pinMode(cb_mua, INPUT);
  svr_rem.attach(rem);
  svr_rem.write(1);
  svr_day.attach(day_phoi);
  svr_day.write(0);

  digitalWrite(den_pn, LOW);
  digitalWrite(quat_pn_enb, LOW);
  analogWrite(quat_pn, 0);
  
  attachInterrupt(digitalPinToInterrupt(button_den_pn), DEN_PHONG_NGU, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_quat_pn), QUAT_PHONG_NGU, FALLING);
  
  cli();
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT20);
  sei();

  nhiet_do = (500*analogRead(cb_nd))/1024;
  last = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();
    if (cmd == '4') {
      digitalWrite(den_pn, 0);
      state_den_pn = false;
    } else if (cmd == '5') {
      digitalWrite(den_pn, 1);
      state_den_pn = true;
    } else if (cmd == '6') {
      digitalWrite(quat_pn_enb, 0);
      state_quat_pn = false;
    } else if (cmd == '7') {
      digitalWrite(quat_pn_enb, 1);
      state_quat_pn = true;
    } else if (cmd == 'c') {
      digitalWrite(den_vs, 0);
      state_den_vs = false;
    } else if (cmd == 'd') {
      digitalWrite(den_vs, 1);
      state_den_vs = true;
    }
  }
  if (state_quat_pn == 1) {
    if (nhiet_do < 30) {
      analogWrite(quat_pn, 128);
    }
  
    if ( (nhiet_do > 30) && (nhiet_do <= 40) ) {
      analogWrite(quat_pn, 192);
    }
  
    if (nhiet_do > 40) {
      analogWrite(quat_pn, 250);
    }
  }
  REM();
  DAY_PHOI();
  if (millis() - last > 1500) {
    nhiet_do = (500*analogRead(cb_nd))/1024;
//    SEND_DATA();
    last = millis();
  }
}

/*Chuong trinh con */
void DEN_PHONG_NGU() {
  if (state_den_pn == 0) {  //Neu den dang tat thi bat len
    digitalWrite(den_pn, 1);
    state_den_pn = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(den_pn, 0);
    state_den_pn = 0;
  }
}

void QUAT_PHONG_NGU() {
  if (state_quat_pn == 0) {  //Neu den dang tat thi bat len
    digitalWrite(quat_pn_enb, 1);
    state_quat_pn = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(quat_pn_enb, 0);
    state_quat_pn = 0;
  }
}

void REM() {
  if (analogRead(cb_rem) <= 150) { //troi sang, mo rem
    svr_rem.write(179);
  }
  if ((analogRead(cb_rem) > 150) && (analogRead(cb_rem) <= 500)) {
    svr_rem.write(90);
  }
  if (analogRead(cb_rem) > 500) { //troi toi, dong het rem
    svr_rem.write(1);
  }  
}
void DAY_PHOI() {
  if (digitalRead(cb_mua) == 1) { //troi mua, keo day phoi vao
    svr_day.write(0);
  } else svr_day.write(180);
}
//void SEND_DATA() {
////  Serial.write(nhiet_do); 
//  Serial.write(state_den_pn);
//  Serial.write(state_quat_pn);
//}
ISR (PCINT2_vect) {
//  long t = 0;
//  while (millis() - t < 10);
  if (digitalRead(button_den_vs) == 0) {
    state_den_vs = !state_den_vs;
    digitalWrite(den_vs, state_den_vs);
  }
}
