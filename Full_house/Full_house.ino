//Khai bao thu vien
#include "DHT.h"
#include <Keypad.h>
#include <Key.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Khai bao cac chan
const int DHTPIN = 22;     //Cam bien nhiet do & do am
const int DHTTYPE = DHT11;
const int SVPIN = 12;      //Dong co cua chinh
#define CB_MUA = 2      //Cam bien mua
#define CB_TROM = 3     //Cam bien bao trom
//Thiet bi phong khach
#define den_pk 48
#define quat_pk 46
#define button_den_pk 53
#define button_quat_pk 52
#define button_cua_pk 51
#define button_bao_trom A10
//Thiet bi phong ngu
#define den_pn 44
#define quat_pn 42
#define button_den_pn 50
#define button_quat_pn A15
//Thiet bi phong bep
#define den_pb 40
#define quat_pb 38
#define button_den_pb A14
#define button_quat_pb A13
//Thiet bi phong ve sinh
#define den_vs 36
#define quat_vs 34
#define button_den_vs A12
#define button_quat_vs A11

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
bool state_den_pn = false;
bool state_quat_pn = false;
bool state_den_pb = false;
bool state_quat_pb = false;
bool state_den_vs = false;
bool state_quat_vs = false;
bool state_door = false;
bool wait = false;
long wait_time, wait_time1;
int temp, humid;

long last;
char buffer[50];
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
//Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial1.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Xin chao!");

  pinMode(den_pk, OUTPUT);
  pinMode(quat_pk, OUTPUT);
  pinMode(button_den_pk, INPUT);
  pinMode(button_quat_pk, INPUT);
  digitalWrite(button_den_pk, HIGH);
  digitalWrite(button_quat_pk, HIGH);

  pinMode(den_pn, OUTPUT);
  pinMode(quat_pn, OUTPUT);
  pinMode(button_den_pn, INPUT);
  pinMode(button_quat_pn, INPUT);
  digitalWrite(button_den_pn, HIGH);
  digitalWrite(button_quat_pn, HIGH);

  pinMode(den_pb, OUTPUT);
  pinMode(quat_pb, OUTPUT);
  pinMode(button_den_pb, INPUT);
  pinMode(button_quat_pb, INPUT);
  digitalWrite(button_den_pb, HIGH);
  digitalWrite(button_quat_pb, HIGH);

  pinMode(den_vs, OUTPUT);
  pinMode(quat_vs, OUTPUT);
  pinMode(button_den_vs, INPUT);
  pinMode(button_quat_vs, INPUT);
  digitalWrite(button_den_vs, HIGH);
  digitalWrite(button_quat_vs, HIGH);

  pinMode(SVPIN, OUTPUT);

  digitalWrite(den_pk, LOW);
  digitalWrite(quat_pk, LOW);
  digitalWrite(den_pn, LOW);
  digitalWrite(quat_pn, LOW);
  digitalWrite(den_pb, LOW);
  digitalWrite(quat_pb, LOW);
  digitalWrite(den_vs, LOW);
  digitalWrite(quat_vs, LOW);

  temp = dht.readTemperature();
  humid = dht.readHumidity();

  servo_close(SVPIN);

  cli();                                  //Disable global interrupt
  /* Reset Timer/Counter1 */
  TCCR1A = 0; 
  TCCR1B = 0;
  TIMSK1 = 0;
  /* Setup Timer/Counter1 */ 
  TCCR1B |= (1 << CS11) | (1 << CS10);    // prescale = 64
  TCNT1 = 40536;
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
  if (error == 3) {
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

  if (digitalRead(button_den_pk) == 0) {
    DEN_PHONG_KHACH();
  }
  if (digitalRead(button_quat_pk) == 0) {
    QUAT_PHONG_KHACH();
  }
  if (digitalRead(button_den_pn) == 0) {
    DEN_PHONG_NGU();
  }
  if (digitalRead(button_quat_pn) == 0) {
    QUAT_PHONG_NGU();
  }
  if (digitalRead(button_den_pb) == 0) {
    DEN_PHONG_BEP();
  }
  if (digitalRead(button_quat_pb) == 0) {
    QUAT_PHONG_BEP();
  }
  if (digitalRead(button_den_vs) == 0) {
    DEN_PHONG_VS();
  }
  if (digitalRead(button_quat_vs) == 0) {
    QUAT_PHONG_VS();
  }

  if (Serial1.available()) {
    cmd = Serial1.read();
    if (cmd == '0') {
      digitalWrite(den_pk, 0);
      state_den_pk = false;
    } else if (cmd == '1') {
      digitalWrite(den_pk, 1);
      state_den_pk = true;
    } else if ((cmd == '2') || (temp < 27)) {
      digitalWrite(quat_pk, 0);
      state_quat_pk = false;
    } else if ((cmd == '3') || (temp >= 27)) {
      digitalWrite(quat_pk, 1);
      state_quat_pk = true;
    } else if ((cmd == '4')) {
      digitalWrite(den_pn, 0);
      state_den_pn = false;
    } else if ((cmd == '5')) {
      digitalWrite(den_pn, 1);
      state_den_pn = true;
    } else if ((cmd == '6') || (temp < 27)) {
      digitalWrite(quat_pn, 0);
      state_quat_pn = false;
    } else if ((cmd == '7') || (temp >= 27)) {
      digitalWrite(quat_pn, 1);
      state_quat_pn = true;
    } else if ((cmd == '8')) {
      digitalWrite(den_pb, 0);
      state_den_pb = false;
    } else if ((cmd == '9')) {
      digitalWrite(den_pb, 1);
      state_den_pb = true;
    } else if ((cmd == 'a') || (temp < 27)) {
      digitalWrite(quat_pb, 0);
      state_quat_pb = false;
    } else if ((cmd == 'b') || (temp >= 27)) {
      digitalWrite(quat_pb, 1);
      state_quat_pb = true;
    } else if ((cmd == 'c')) {
      digitalWrite(den_vs, 0);
      state_den_vs = false;
    } else if ((cmd == 'd')) {
      digitalWrite(den_vs, 1);
      state_den_vs = true;
    } else if ((cmd == 'e') || (temp < 27)) {
      digitalWrite(quat_vs, 0);
      state_quat_vs = false;
    } else if ((cmd == 'f') || (temp >= 27)) {
      digitalWrite(quat_vs, 1);
      state_quat_vs = true;
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
    if (state_quat_pk == 0) {
      digitalWrite(quat_pk, 1);
      state_quat_pk = 1;
    } else {
      digitalWrite(quat_pk, 0);
      state_quat_pk = 0;
    }
  }
}
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
  if ((digitalRead(button_quat_pn) == 0)) {
    if (state_quat_pn == 0) {
      digitalWrite(quat_pn, 1);
      state_quat_pn = 1;
    } else {
      digitalWrite(quat_pn, 0);
      state_quat_pn = 0;
    }
  }
}
void DEN_PHONG_BEP() {
  if (state_den_pb == 0) {  //Neu den dang tat thi bat len
    digitalWrite(den_pb, 1);
    state_den_pb = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(den_pb, 0);
    state_den_pb = 0;
  }
}

void QUAT_PHONG_BEP() {
  if ((digitalRead(button_quat_pb) == 0)) {
    if (state_quat_pb == 0) {
      digitalWrite(quat_pb, 1);
      state_quat_pb = 1;
    } else {
      digitalWrite(quat_pb, 0);
      state_quat_pb = 0;
    }
  }
}
void DEN_PHONG_VS() {
  if (state_den_vs == 0) {  //Neu den dang tat thi bat len
    digitalWrite(den_vs, 1);
    state_den_vs = 1;
  } else {  //Nguoc lai, neu den dang bat thi tat di
    digitalWrite(den_vs, 0);
    state_den_vs = 0;
  }
}

void QUAT_PHONG_VS() {
  if ((digitalRead(button_quat_vs) == 0)) {
    if (state_quat_vs == 0) {
      digitalWrite(quat_vs, 1);
      state_quat_vs = 1;
    } else {
      digitalWrite(quat_vs, 0);
      state_quat_vs = 0;
    }
  }
}

void SEND_DATA() {
  Serial1.write(temp);                                                                                              
  Serial1.write(humid);
  Serial1.write(state_den_pk);
  Serial1.write(state_quat_pk);
  Serial1.write(state_den_pn);
  Serial1.write(state_quat_pb);
  Serial1.write(state_den_pb);
  Serial1.write(state_quat_pb);
  Serial1.write(state_den_vs);
  Serial1.write(state_quat_vs);
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
    if (error == 3) {
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
//  delayMicroseconds(1450);
  long t = micros();
  while ((micros() - t) < 1450);
  digitalWrite(pin,LOW);
//  delayMicroseconds(18550);
  t = micros();
  while ((micros() - t) < 18550);
}
void servo_close(int pin) {
  digitalWrite(pin,HIGH);
//  delayMicroseconds(600);
  long t = micros();
  while ((micros() - t) < 600);
  digitalWrite(pin,LOW);
//  delayMicroseconds(19400);
  t = micros();
  while ((micros() - t) < 19400);
}
ISR(TIMER1_OVF_vect) {
  passWord();
  TCNT1 = 40536;
}
