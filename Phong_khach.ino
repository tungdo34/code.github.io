//Khai bao thu vien
#include "DHT.h"

//Khai bao cac chan
const int DHTPIN = 4;
const int DHTTYPE = DHT11;
#define den_pk 13
#define quat_pk 12
#define button_den_pk 2
#define button_quat_pk 3

//Bien trang thai cua thiet bi
bool state_den_pk = false;
bool state_quat_pk = false;
float temp, humid;
char cmd;  //Nhan lenh tu app
/*
  + Den: 0, 1
  + Quat: 2, 3

*/
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  // Khoi tao Timer 1

  TCCR1A = 0;
  TCCR1B = 0;     // thanh ghi de cau hinh ti le chia cua Timer
  TIMSK1 = 0 ;    // thanh ghi quy dinh hinh thuc ngat

  // duoi day la cau hinh cho Timer
  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10); // ti le chia la 1/64
  TCNT1 = 34286;      // thoi gian nhay vao ngat la sau moi 500ms
  TIMSK1 = (1 << TOIE1); // hinh thuc ngat la ngat khi tran
  sei() ;           // cho phep ngat toan cuc

  Serial.begin(9600);
  dht.begin();

  pinMode(den_pk, OUTPUT);
  pinMode(quat_pk, OUTPUT);
  pinMode(button_den_pk, INPUT_PULLUP);
  pinMode(button_quat_pk, INPUT_PULLUP);

  digitalWrite(den_pk, LOW);
  digitalWrite(quat_pk, LOW);

  attachInterrupt(digitalPinToInterrupt(button_den_pk), DEN_PHONG_KHACH, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_quat_pk), QUAT_PHONG_KHACH, FALLING);

  temp = dht.readTemperature();
  humid = dht.readHumidity();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();
    if (cmd == '0') {
      digitalWrite(den_pk, 0);
      state_den_pk = false;
//      Serial.println(state_den_pk);
    } else if (cmd == '1') {
      digitalWrite(den_pk, 1);
      state_den_pk = true;
//      Serial.println(state_den_pk);
    } else if ((cmd == '2') || (temp < 27)) {
      digitalWrite(quat_pk, 0);
      state_quat_pk = false;
//      Serial.println(state_quat_pk);
    } else if ((cmd == '3') || (temp >= 27) {
      digitalWrite(quat_pk, 1);
      state_quat_pk = true;
//      Serial.println(state_quat_pk);
    }
//    Serial.println(cmd);
  }
}

/*Chuong trinh con */
void DEN_PHONG_KHACH() {
  if (state_den_pk == 0) {  //Neu den dang tat thi bat len
    digitalWrite(den_pk, 1);
    state_den_pk = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(den_pk, 0);
    state_den_pk = 0;
  }
}

void QUAT_PHONG_KHACH() {
  if ((digitalRead(button_quat_pk) == 0)) {
    if (state_quat_pk == 0) {  //Neu den dang tat thi bat len
      digitalWrite(quat_pk, 1);
      state_quat_pk = 1;
    } else {  //Nguoc lai, neu den dang bat thi tat di
      digitalWrite(quat_pk, 0);
      state_quat_pk = 0;
    }
  }
}

//Chuong trinh con ngat Timer1
ISR(TIMER1_OVF_vect) { // Ngắt của timer 1
  temp = dht.readTemperature();                                                                                                                           
  humid = dht.readHumidity();
  Serial.write(temp);
  Serial.write(humid);
  TCNT1 = 34286;
}
