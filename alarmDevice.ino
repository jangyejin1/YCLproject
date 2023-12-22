#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <core_build_options.h>
#include <swRTC.h>
#include <SoftwareSerial.h> //추가

SoftwareSerial BTSerial(2, 3); //추가 tx, rx
swRTC rtc;

//Arduino1
#define LIGHT_BT1 A2 //RED
#define LIGHT_BT2 A1 //GREEN
#define RED 4
#define GREEN 5
#define BLUE 6
#define YELLOW 7

#define BT 9 //BLUE 누르면 1
#define BT_2 8 //red 누르면 1

//Arduino2
//#define SPK 8
#define MOTOR 10

LiquidCrystal_I2C lcd(0x27,16,2);
String lcdString = "";  //객체 선언 : 출력 할 글자 저장
int reset = 0;
int Bright = 45; //밝기 임계값
int wake = 0; //알림이 울리면
int count = 0; //패턴 번호
int temp = 730; // 7시 30분
//Button reset
int BT1;
int BT2;
int BT3;
int BT4; //추가

//Button Read Function
void BT_READ(){
  BT1 = analogRead(LIGHT_BT1); //yellow
  BT2 = analogRead(LIGHT_BT2); //GREEN
  BT3 = !digitalRead(BT); //BLUE
  BT4 = !digitalRead(BT_2); //red
  //Serial.println(BT1);
}

//LED 초기화 함수
void SET_LED(){
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(YELLOW, LOW);
}

void resetSystem() {
  count = 1;
  //Serial.println(count); 
  digitalWrite(MOTOR, HIGH);
  delay(2000); 
  digitalWrite(MOTOR, LOW);

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(YELLOW, LOW);

  //Serial.println(reset);
}

//알람 끄는 부분
void ON_LED(){
  if (count == 1){  //count 1 | red | BT4 | count 2 (BLUE)
    BT_READ();
    //Serial.println(BT1);
    digitalWrite(RED,HIGH);  

    if (BT4 == 1){
      count = 2;
      //Serial.println("c:"+ count);
      digitalWrite(RED, LOW);
      digitalWrite(BLUE,HIGH);
    }
    //reset
    if (BT1 < Bright || BT2 < Bright || BT3 == 1){ //bt1 | bt2 | bt3 | moter 2sec
      resetSystem();
      reset++;
      Serial.println(reset);
    }
  }
  delay(500);

  if (count == 2){ //count 2 | BLUE | button | count 3 (GREEN)
    BT_READ();
    if (BT3 == 1){
      count = 3;
      //Serial.println("c:"+ count);
      digitalWrite(BLUE, LOW);
      digitalWrite(GREEN,HIGH);
    }
    if (BT1 < Bright || BT2 < Bright || BT4 == 1){ //bt1 | bt2 | bt4 | moter 2sec
      resetSystem();
      reset++;
      Serial.println(reset);
    }
  }
  delay(500);

  if (count == 3){ //count 3 | GREEN | button | count 4 (YELLOW)
    BT_READ();
    //Serial.println("green:"+ BT2);
    if (BT2 < Bright){
      count = 4;
      digitalWrite(GREEN, LOW);
      digitalWrite(YELLOW, HIGH);
    }
    if (BT1 < Bright || BT3 == 1 || BT4 == 1){ //bt1 | bt3 | bt4 | moter 2sec
      resetSystem();
      reset++;
      Serial.println(reset);
    }
  }
  delay(500);

  if (count == 4){
    BT_READ();
    //Serial.println("yellow"+ BT);
    if (BT1 < Bright){
      count = 5;
      digitalWrite(YELLOW, LOW);
    }
    if (BT2 < Bright || BT3 == 1 || BT4 == 1){ //bt2 | bt3 | bt4 | moter 2sec
      resetSystem();
      reset++;
      Serial.println(reset);
    }
  }
  delay(1000);
}

//Alarm Function
//Change BlueTooth send
void ALARM(int send){
  if (send == 0){
    //tone(SPK, 10000);
    //digitalWrite(MOTOR,HIGH);
    BTSerial.write('0'); 
    count = 1;
  }
  else if (send == 1){
    //noTone(SPK);
    //digitalWrite(MOTOR,LOW);
    BTSerial.write('1');
    SET_LED();
    count = 0;
    wake = 0;
  }
}

//10보다 작은수를 출력할때 앞에 0을 출력
void Set_lowThanTen(int time) {
  if(time < 10) {
    lcd.print("0");
    lcd.print(time%10);
  }
  else
    lcd.print(time);
}

//유효한 알람시간인지 체크
int checkTheAlarmClock(int time) {
  if(time/100 < 24 && time %100 < 60) {
    Serial.println("Success");
    return time;
  }
  else {
    Serial.println("Failed");
    return 0;
  }  
}

//알람이 울릴시간인지 체크
void checkTheAlarmTime(int alarmHour, int alarmMinute, int alarmSec) {
  if(alarmHour == rtc.getHours() && alarmMinute == rtc.getMinutes() && alarmSec == rtc.getSeconds()) {
    delay(500);
    wake = 1;
  }
}

void timeAlert(){
  SET_LED();  
  int day;
  lcd.setCursor(0,0);//커서를 0,0에 지정 
  
  //1초 단위로 갱신하며 현재시간을 LCD에 출력
  Set_lowThanTen(rtc.getHours()); 
  lcd.print(":");
  Set_lowThanTen(rtc.getMinutes());
  lcd.print(":");
  Set_lowThanTen(rtc.getSeconds());

  //날짜를 LCD에 출력
  lcd.print("[");
  Set_lowThanTen(rtc.getMonth());
  lcd.print("/");
  Set_lowThanTen(rtc.getDay());
  lcd.print("]");

  //세팅된 알람시간을 LCD에 출력
  lcd.setCursor(0,1);
  lcd.print("Alarm ");
  Set_lowThanTen(temp/100);
  lcd.print(":");
  Set_lowThanTen(temp%100); 
  
  //1초마다 LCD갱신
  lcdString = "";                      //문자열 초기화
  lcd.print("               ");        //전 글씨 삭제
  delay(1000);
  
  //알람이 울릴 시간인지 체크
  checkTheAlarmTime(temp/100, temp%100, 0);
}

void setup()
{
  //LCD RESET
  lcd.backlight();
  lcd.begin(16,2);         //LCD 크기 지정, 2줄 16칸
  lcd.clear();             //화면 초기화
  
  //SET TIME
  rtc.stopRTC();           //stop
  rtc.setTime(07,29,50);    //hour, min, sec
  rtc.setDate(3, 12, 2023);  //day, month, year
  rtc.startRTC();          //start

  //ARDUINO 1
  pinMode(BT, INPUT_PULLUP);
  pinMode(BT_2, INPUT_PULLUP);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  //ARDUINO 2
  //pinMode(SPK, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  
  //PORT
  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop()
{ 
  Serial.println(reset);
  timeAlert(); //TIME DISPLAY code
  //Serial.println(wake);
  if (wake == 1){
    ALARM(0);//BLT ON code(지금은 출력만 조절)
    // ALERT in LCD code
    //Serial.println(wake);
    while (count < 5){
      ON_LED();//LED code
    }
    ALARM(1);//BLT OFF code(지금은 출력만 조절)
  }
}