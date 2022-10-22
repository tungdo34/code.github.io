//Khai bao thu vien
#include "DHT.h"
#include <Servo.h>
#include <Keypad.h>
#include <Key.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Khai bao cac chan
const int DHTPIN = 4;
const int DHTTYPE = DHT11;
#define den_pk 13
#define quat_pk 12
#define button_den_pk 2
#define button_quat_pk 3

// Khai bao cua Keypad - PassWord cho cua chinh
const byte ROWS = 4;
const byte COLS = 4;
int k = 0; // đếm số kí tự trong Pass đúng
int i = 0; //giới hạn kí tự của pass
int error = 0;
char keys[ROWS][COLS] =
{ {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
}; // Keypad Form

char pass[] = {'1', '2', '3', '4', '5', '6'}; // pass nguoi dung dat
char newpass[6];
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {2, 3, 4, 5};
int f = 0; // to Enter Clear Display one time

//Bien trang thai cua thiet bi
bool state_den_pk = false;
bool state_quat_pk = false;
int temp, humid;
long last;
char buffer[50];
char cmd;  //Nhan lenh tu app
/*
  + Den: 0, 1
  + Quat: 2, 3

*/
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
//  // Khoi tao Timer 1
//
//  TCCR1A = 0;
//  TCCR1B = 0;     // thanh ghi de cau hinh ti le chia cua Timer
//  TIMSK1 = 0 ;    // thanh ghi quy dinh hinh thuc ngat
//
//  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10); // ti le chia la 1/64
//  TCNT1 = 34286;      // thoi gian nhay vao ngat la sau moi 500ms
//  TIMSK1 = (1 << TOIE1); // hinh thuc ngat la ngat khi tran
//  sei() ;           // cho phep ngat toan cuc

  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  

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
//  temp = 27;
//  humid =65;

  last = 0;
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
    } else if ((cmd == '3') || (temp >= 27)) {
      digitalWrite(quat_pk, 1);
      state_quat_pk = true;
//      Serial.println(state_quat_pk);
    }
  }
  if ((millis() - last) >= 1500) {
    SEND_DATA();
    last = millis();
    temp = dht.readTemperature();
    humid = dht.readHumidity();
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

void SEND_DATA() {
  Serial.write(temp);                                                                                                                                   
  Serial.write(humid);
}

//void passWord() {
//  char key = keypad.getKey();
//  if (key != NO_KEY && i < 6)
//  {
//    if (f == 0)
//    {
//      lcd.clear();
//      f = 1;
//    }
//    lcd.setCursor(0, 0);
//    lcd.print("Mat khau:");
//    lcd.setCursor(i, 1);
//    lcd.print("*");
//    newpass[i] = key;
//    if (newpass[i] == pass[i]) k++;
//    i++;
//  }
//  if (k == 6) {
////    state_door = 1; //
////    x = 1;
////    if (state_denpk == 0) { // neu den dang tat thi bat den len
////      digitalWrite(relay_denpk, 0); // bat den
////      state_denpk = 1; // doi trang thai den la ON
////    }
////    if (state_dieuhoapk == 0) { // neu dieu hoa dang tat thi bat len
////      digitalWrite(relay_dieuhoapk, 0); // bat den
////      state_dieuhoapk = 1; // doi trang thai den la ON
//    }
//    k = 7;
//    return;
//  } else if (k < 6 && i == 6) {
//    lcd.clear();
//    lcd.print("Mat khau sai !");
//    delay(1000);
//    i = 0;
//    k = 0;
//    error++;
//    if (error < 5) {
//      lcd.clear();
//      lcd.print("  Xin moi nhap");
//      lcd.setCursor(0, 1);
//      lcd.print("Mat khau:");
//    }
//    f = 0;
//    return;
//  }
//}
