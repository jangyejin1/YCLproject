//Arduino2
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3);
#define SPK 8
#define MOTOR 9
int data;

void setup() {
  Serial.begin(9600);

  pinMode(SPK, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  BTSerial.begin(9600);
}

void loop() {
data = BTSerial.read(); // 블루투스로 전송된 데이터 읽기
    if (data == '0') { // 알람이 켜지면
      tone(SPK, 10000); 
      digitalWrite(MOTOR,HIGH);
      Serial.println("ALARM-ON!");
    }
    else if (data == '1') { // 알람 꺼진 상태
      noTone(SPK);
      digitalWrite(MOTOR,LOW);
      Serial.println("ALARM-OFF!");
    }
}

