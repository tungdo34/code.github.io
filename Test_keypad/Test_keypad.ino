//Khai bao thu vien
#include "DHT.h"
//#include <Servo.h>
#include <Keypad.h>
#include <Key.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Khai bao cac chan
const int DHTPIN = A2;
const int DHTTYPE = DHT11;
const int SVPIN = 12;
#define den_pk 13
#define quat_pk 11
#define button_den_pk A0
#define button_quat_pk A1

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
byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};
int f = 0; // to Enter Clear Display one time

//Bien trang thai cua thiet bi
bool state_den_pk = false;
bool state_quat_pk = false;
bool door_state = false;
bool wait = false;
long wait_time, wait_time1;
int temp, humid;

long last;
char buffer[50];
char cmd;  //Nhan lenh tu app
/*
  + Den: 0, 1
  + Quat: 2, 3

*/
//Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  cli();                                  //Disable global interrupt
  /* Reset Timer/Counter1 */
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  /* Setup Timer/Counter1 */
  TCCR1B |= (1 << CS11) | (1 << CS10);    // prescale = 64
  TCNT1 = 40536;
  TIMSK1 = (1 << TOIE1);                  // Overflow interrupt enable
  sei();                                  //Enable global interrupt

  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Xin chao!");

  pinMode(den_pk, OUTPUT);
  pinMode(quat_pk, OUTPUT);
  pinMode(button_den_pk, INPUT_PULLUP);
  pinMode(button_quat_pk, INPUT_PULLUP);
  pinMode(SVPIN, OUTPUT);

  digitalWrite(den_pk, LOW);
  digitalWrite(quat_pk, LOW);

  attachInterrupt(digitalPinToInterrupt(button_den_pk), DEN_PHONG_KHACH, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_quat_pk), QUAT_PHONG_KHACH, FALLING);

//  myservo.attach(SVPIN);
//  myservo.write(0);

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
  if (door_state == true) {
    if ((millis() - wait_time) > 1500) {
      lcd.clear();
      lcd.print("Xin chao!");
      door_state = false;
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
    door_state = true;
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
      door_state = false;
      i = 0;
      k = 0;
      f = 0;
      return;
    } else {
      lcd.clear();
      lcd.print("Mat khau sai !");
      wait = true;
      wait_time1 = millis();
      door_state = false;
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
  TCNT1 = 53035;
}
