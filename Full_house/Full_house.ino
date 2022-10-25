//Khai bao thu vien
#include <SoftwareSerial.h>
#include "DHT.h"
#include <Keypad.h>
#include <Key.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <avr/interrupt.h>

//Khai bao cac chan
const int DHTPIN = 22;     //Cam bien nhiet do & do am
const int DHTTYPE = DHT11;
volatile const int SVPIN = 12;      //Dong co cua chinh
#define CB_MUA = 50     //Cam bien mua
#define CB_TROM = 52     //Cam bien bao trom
//Thiet bi phong khach
#define den 48
#define quat 46
#define quat_enb 46
#define button_den 19
#define button_quat 18
#define button_cua 2
#define button_bao_trom 3
#define rxPin 51
#define txPin 53

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
byte rowPins[ROWS] = {37, 35, 33, 31};
byte colPins[COLS] = {29, 27, 25, 23};
int f = 0; // to Enter Clear Display one time

//Bien trang thai cua thiet bi
bool state_den_pk = false;
bool state_quat_pk = false;
bool state_den_pn, state_quat_pn, state_den_pb, state_quat_pb, state_den_vs, state_quat_vs;
volatile bool state_baotrom = false;
volatile bool state_door = false;
volatile bool state_button_door = false;
volatile bool wait = false;
volatile long wait_time, wait_time1;
int nhiet_do_pk, do_am_pk, nhiet_do_pn, do_am_pn;

long last;
char cmd = '0';  //Nhan lenh tu app
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
//Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
SoftwareSerial Serial4 (rxPin, txPin); //RX pin: 51, TX pin: 53

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial4.begin(9600);  //Giao tiep phong ve sinh
  Serial.begin(9600);   //Giao tiep ESP
  Serial2.begin(9600);  //Giao tiep phong ngu
  Serial3.begin(9600);  //Giao tiep phong bep
  Serial.flush(); 
  Serial2.flush(); 
  Serial3.flush(); 
  
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Xin chao!");
  
  pinMode(den, OUTPUT);
  pinMode(quat, OUTPUT);
  pinMode(quat_enb, OUTPUT);
  pinMode(button_den, INPUT_PULLUP);
  pinMode(button_quat, INPUT_PULLUP);
  pinMode(button_cua, INPUT_PULLUP);
  pinMode(button_bao_trom, INPUT_PULLUP);

  pinMode(SVPIN, OUTPUT);

  digitalWrite(den, LOW);
  digitalWrite(quat_enb, LOW);
  analogWrite(quat, 0);

  nhiet_do_pk = dht.readTemperature();
  do_am_pk = dht.readHumidity();

  servo_close(SVPIN);

  attachInterrupt(digitalPinToInterrupt(button_den), DEN_PHONG_KHACH, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_quat), QUAT_PHONG_KHACH, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_cua), CUA_PHONG_KHACH, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_bao_trom), BAO_TROM, FALLING);

  cli();                                  //Disable global interrupt
  /* Reset Timer/Counter1 */
  TCCR1A = 0; 
  TCCR1B = 0;
  TIMSK1 = 0;
  /* Setup Timer/Counter1 */ 
  TCCR1B |= (1 << CS11) | (1 << CS10);    // prescale = 64
  TCNT1 = 51472;
  TIMSK1 = (1 << TOIE1);                  // Overflow interrupt enable 
  sei();

  last = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (state_door == true) {
    if ((millis() - wait_time) > 1500) {
      lcd.clear();
      lcd.print("Xin chao!");
      state_door = false;
      servo_close(SVPIN);
    }
  }
  if (error == 5) {
    if ((millis() - wait_time1) > 5000) {
      lcd.clear();
      lcd.print("Xin chao!");
      error = 0;
    }
  }
  if (wait) {
    if ((millis() - wait_time1) > 1500) {
      lcd.clear();
      lcd.print("Xin chao!");
      wait = false;
    }
  }

  if (Serial.available()) {
    cmd = Serial.read();
    if (cmd == '0') {
      digitalWrite(den, 0);
      state_den_pk = false;
    } else if (cmd == '1') {
      digitalWrite(den, 1);
      state_den_pk = true;
    } else if (cmd == '2') {
      digitalWrite(quat_enb, 0);
      state_quat_pk = false;
    } else if (cmd == '3') {
      digitalWrite(quat_enb, 1);
      state_quat_pk = true;
    } else if (cmd == '4') {
      state_den_pn = false;
    } else if (cmd == '5') {
      state_quat_pn = true;
    } else if (cmd == '6') {
      state_den_pb = false;
    } else if (cmd == '7') {
      state_quat_pb = true;
    } else if (cmd == '8') {
      state_den_vs = false;
    } else if (cmd == '9') {
      state_quat_vs = true;
    }
  }
  if (digitalRead(button_cua) == 0) {
    if (state_door == 0) {
      state_door = true;
      servo_open(SVPIN);
    }
    if (state_door == 1) {
      state_door = false;
      servo_close(SVPIN);
    }
  }
//  if (state_button_door == 1) {
//    if (state_door == 0) {
//      servo_open(SVPIN);
//      state_door = true;
//    } 
//    if (state_door == 1) {
//      servo_close(SVPIN);
//      state_door = false;
//    }
//    state_button_door = false;
//  }

  if (Serial2.available()) {
    nhiet_do_pn = Serial2.read();
    do_am_pn = Serial2.read();
  }
  if ((millis() - last) >= 1500) {
    nhiet_do_pk = dht.readTemperature();
    do_am_pk = dht.readHumidity();
    SEND_DATA();
    last = millis();
  }
}

/* Chuong trinh con phuc vu ngat */
void DEN_PHONG_KHACH() {
  if (state_den_pk == 0) {  //Neu den dang tat thi bat len
    digitalWrite(den, 1);
    state_den_pk = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(den, 0);
    state_den_pk = 0;
  }
}

void QUAT_PHONG_KHACH() {
  if (state_quat_pk == 0) {
    digitalWrite(quat, 1);
    state_quat_pk = 1;
  } else {
    digitalWrite(quat, 0);
    state_quat_pk = 0;
  }
}
void CUA_PHONG_KHACH() {
//  if (state_door == 0) {
//    digitalWrite(SVPIN,HIGH); 
//    long t = micros();
//    while ((micros() - t) < 1450);
//    digitalWrite(SVPIN,LOW);
//    t = micros();
//    while ((micros() - t) < 18550);  
//  }
//  if (state_door == 1) {
//    digitalWrite(SVPIN,HIGH); 
//    long t = micros();
//    while ((micros() - t) < 600);
//    digitalWrite(SVPIN,LOW);
//    t = micros();
//    while ((micros() - t) < 19400);  
//  }
  if (state_door == 0) {
    state_door = true;
    servo_open(SVPIN);
  } else {
    state_door = false;
    servo_close(SVPIN);
  }
//  state_button_door = !state_button_door;
}
void BAO_TROM(){
  state_baotrom = true;
}
/* Chuong trinh con */
void SEND_DATA() {
  Serial.write(nhiet_do_pk);                                                                                            
  Serial.write(do_am_pk);
//  Serial.write(nhiet_do_pn);                                                                                            
//  Serial.write(do_am_pn);
  Serial.write(state_den_pk);
  Serial.write(state_quat_pk);
//  Serial.write(state_den_pn);
//  Serial.write(state_quat_pb);
//  Serial.write(state_den_pb);
//  Serial.write(state_quat_pb);
//  Serial.write(state_den_vs);
//  Serial.write(state_quat_vs);
}

void passWord() {
  sei();
  char key = keypad.getKey();
  if (key != NO_KEY && i < 6)
  {
    if (f == 0)
    {
      lcd.clear();
      f = 1;
    }
    lcd.setCursor(0, 0);
    lcd.print("Mat khau:");
    lcd.setCursor(i, 1);
    lcd.print("*");
    newpass[i] = key;
    if (newpass[i] == pass[i]) k++;
    i++;
  }
  if (k == 6) {
    lcd.clear();
    lcd.print("Moi vao!");
    state_door = true;
    servo_open(SVPIN);
    wait_time = millis();
    i = 0;
    k = 0;
    f = 0;
    return;
  }
  else if (k < 6 && i == 6) {
    error++;
    if (error == 5) {
      lcd.clear();
      lcd.print("Sai 5 lan !");
      lcd.setCursor(0, 1);
      lcd.print("Nhap lai sau 5s.");
      wait_time1 = millis();
      state_door = false;
      i = 0;
      k = 0;
      f = 0;
      return;
    } else {
      lcd.clear();
      lcd.print("Mat khau sai !");
      wait = true;
      wait_time1 = millis();
      state_door = false;
      i = 0;
      k = 0;
      f = 0;
      return;
    }
  }
}
void servo_open(int pin) {
  digitalWrite(pin,HIGH);
  long t = micros();
  while ((micros() - t) < 1450);
  digitalWrite(pin,LOW);
  t = micros();
  while ((micros() - t) < 18550);
}
void servo_close(int pin) {
  digitalWrite(pin,HIGH);
  long t = micros();
  while ((micros() - t) < 600);
  digitalWrite(pin,LOW);
  t = micros();
  while ((micros() - t) < 19400);
}
ISR(TIMER1_OVF_vect) {
  passWord();
  TCNT1 = 51472;
}
