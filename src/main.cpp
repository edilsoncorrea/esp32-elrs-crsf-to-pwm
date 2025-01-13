#include <Arduino.h>
#include <ESP32Servo.h>

const int receiverPin = 2;  
volatile unsigned long pulseStart = 0;
volatile unsigned long pulseWidth = 0;

void handleReceiverSignal() {
  if (digitalRead(receiverPin) == HIGH) {
    pulseStart = micros();
  } else {
    pulseWidth = micros() - pulseStart;
  }
}


int aileronsPin = 10;

int IN1 = 3;
int IN2 = 4;
int ENA = 6;

void InicializarConfPinos() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  digitalWrite(ENA, LOW);
}  

void ControlarMotor(int valorEixo) {
  int valorEixoAbsoluto = abs(valorEixo);

  if (valorEixoAbsoluto < 1) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    digitalWrite(ENA, LOW);
    return;
  }


  // analogWrite(IN1, 512);
  // analogWrite(IN2, 900);

  int valorFrente = valorEixo > 0 ? valorEixoAbsoluto : 0;
  int valorRe = valorEixo < 0 ? valorEixoAbsoluto : 0;


  analogWrite(IN1, valorFrente);
  analogWrite(IN2, valorRe);
  digitalWrite(ENA, HIGH);

  Serial.print("Frente: ");
  Serial.print(valorFrente);

  Serial.print("\nRe: ");
  Serial.println(valorRe);
}


void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);

  pinMode(receiverPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(receiverPin), handleReceiverSignal, CHANGE);

  InicializarConfPinos();

  delay(500); 
}

void loop() {
      
      unsigned long width;
  
      noInterrupts();
      width = pulseWidth;
      interrupts();

      int dutyCycle = map(width, 1000, 2000, -255, 255);
      dutyCycle = constrain(dutyCycle, -255, 255);


      // Serial.print("Thritle_0: ");
      // Serial.println(dutyCycle);

      ControlarMotor(dutyCycle);

      delay(50);
}

