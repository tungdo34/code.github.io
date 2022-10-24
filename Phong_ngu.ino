//Khai bao thu vien
#include <avr/interrupt.h>
#include <Servo.h>
#include <Wire.h>

//Khai bao cac chan
#define cb_rem A0
#define cb_nd A0
#define den 13
#define quat_enb 12
#define quat 11
#define rem 10
#define button_den 2
#define button_quat 3

//Servo servo;
//Bien trang thai cua thiet bi
volatile int nhiet_do;
volatile bool state_den = false;
volatile bool state_quat = false;
volatile bool state_rem = false;
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
void setup() {
  Serial.begin(9600);

  pinMode(den, OUTPUT);
  pinMode(quat, OUTPUT);
  pinMode(quat_enb, OUTPUT);
  pinMode(rem, OUTPUT);
  pinMode(button_den, INPUT_PULLUP);
  pinMode(button_quat, INPUT_PULLUP);
  pinMode(cb_rem, INPUT);

  digitalWrite(den, LOW);
  digitalWrite(quat, LOW);
  analogWrite(quat, 0);
  
//  servo.attach(rem);
  attachInterrupt(digitalPinToInterrupt(button_den), DEN_PHONG_NGU, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_quat), QUAT_PHONG_NGU, FALLING);

  /* Reset Timer/Counter1 */
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
  /* Setup Timer/Counter1 */
  TCCR1B |= (1 << CS12);    // prescale = 128
  TCNT1 = 42097;            // Interrupt after 1.5s
  TIMSK1 = (1 << TOIE1);    // Overflow interrupt enable
  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();
    if (cmd == '0') {
      digitalWrite(den, 0);
      state_den = false;
    } else if (cmd == '1') {
      digitalWrite(den, 1);
      state_den = true;
    } else if (cmd == '2') {
      digitalWrite(quat_enb, 0);
      state_quat = false;
    } else if (cmd == '3') {
      digitalWrite(quat_enb, 1);
      state_quat = true;
    }
  }
  if (state_quat == 1) {
    if (nhiet_do < 30) {
      analogWrite(quat, 128);
    }
  
    if ( (nhiet_do > 30) && (nhiet_do <= 40) ) {
      analogWrite(quat, 128);
    }
  
    if (nhiet_do > 40) {
      analogWrite(quat, 128);
    }
  }
  REM();
}

/*Chuong trinh con */
void DEN_PHONG_NGU() {
  if (state_den == 0) {  //Neu den dang tat thi bat len
    digitalWrite(den, 1);
    state_den = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(den, 0);
    state_den = 0;
  }
}

void QUAT_PHONG_NGU() {
  if ((digitalRead(button_quat) == 0)) {
    if (state_quat == 0) {  //Neu den dang tat thi bat len
      digitalWrite(quat, 1);
      state_quat = 1;
    } else {  //Nguoc lai, neu den dang bat thi tat di
      digitalWrite(quat, 0);
      state_quat = 0;
    }
  }
}

void REM() {
  if (analogRead(cb_rem) <= 150) { //troi sang, mo rem
    servo(2300);
  }
  if ((analogRead(cb_rem) > 150) && (analogRead(cb_rem) <= 500)) {
    servo(1450);
  }
  if (analogRead(cb_rem) > 500) { //troi toi, dong het rem
    servo(600);
  }  
}

void servo(int pul_time) {
//Pulses duration: 600 - 0deg; 1450 - 90deg; 2300 - 180deg
digitalWrite(rem,HIGH);
  long t = micros();
  while ((micros() - t) < pul_time);
  digitalWrite(rem,LOW);
  t = micros();
  while ((micros() - t) < (20000 - pul_time));
}

void SEND_DATA() {
  Serial.write(nhiet_do); 
  Serial.write(state_den);
  Serial.write(state_quat);
  Serial.write(state_rem);
}

ISR(TIMER1_OVF_vect) {                                                                                                  
  nhiet_do = 5 * (analogRead(cb_nd)) * 100 / 1024;
  SEND_DATA();                                                                                                           
  TCNT1 = 42097;                                                                                                        
}
