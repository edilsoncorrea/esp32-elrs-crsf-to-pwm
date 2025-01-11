#include <Arduino.h>
#include "crsf.h"
#include <ESP32Servo.h>

#define RXD2 6
#define TXD2 7

#define SBUS_BUFFER_SIZE 25
uint8_t _rcs_buf[25] {};
uint16_t _raw_rc_values[RC_INPUT_MAX_CHANNELS] {};
uint16_t _raw_rc_count{};

int aileronsPin = 10;

int IN1 = 3; 
int IN2 = 4;

Servo myServo;

void InicializarConfPinos() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
}  

void ControlarMotor(int valorEixo) {
  int valorEixoAbsoluto = abs(valorEixo);

  if (valorEixoAbsoluto < 1) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    return;
  }

  analogWrite(IN1, valorEixo > 0 ? valorEixoAbsoluto : 0);
  analogWrite(IN2, valorEixo < 0 ? valorEixoAbsoluto : 0);
}

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  Serial1.begin(420000, SERIAL_8N1, RXD2, TXD2);

  InicializarConfPinos();

  myServo.attach(aileronsPin);

  myServo.write(90);
  delay(500); 
}

void loop() {
  while (Serial1.available()) {
    size_t numBytesRead = Serial1.readBytes(_rcs_buf, SBUS_BUFFER_SIZE);
    
    if(numBytesRead > 0)
    {
      crsf_parse(&_rcs_buf[0], SBUS_BUFFER_SIZE, &_raw_rc_values[0], &_raw_rc_count, RC_INPUT_MAX_CHANNELS );


      int throttle = _raw_rc_values[2];
      int ailerons = _raw_rc_values[0];

      ailerons = constrain(ailerons, 1000, 2000);
      throttle = constrain(throttle, 1000, 2000);
      _raw_rc_values[4] = constrain(_raw_rc_values[4], 1000, 2000);

      int aileronsAngle = map(ailerons, 989, 2012, 20, 160);
      int throttleMapped = map(throttle, 991, 2012, -255, 255);
      int switchMapped = map(_raw_rc_values[4], 1000, 2000, 0, 100);

      Serial.print("Thritle_0: ");
      Serial.print(throttleMapped);
      Serial.print("\tThrottle: ");
      Serial.println(_raw_rc_values[2]);

      myServo.write(aileronsAngle);

      ControlarMotor(throttleMapped);
    }
  }
}