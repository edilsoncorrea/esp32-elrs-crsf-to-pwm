#include <Arduino.h>

const int receiverPin = 2;  

volatile unsigned long pulseStart = 0;
volatile unsigned long pulseWidth = 0;

bool pronto = false;

void handleReceiverSignal() {
  if (digitalRead(receiverPin) == HIGH) {
    pulseStart = micros();
  } else {
    pulseWidth = micros() - pulseStart;
  }
}

#if defined(ARDUINO_AVR_NANO)
int IN1 = 5;
int IN2 = 6;
int ENA = 10;
#else
int IN1 = 3;
int IN2 = 4;
int ENA = 6;
#endif


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

  int valorFrente = valorEixo > 0 ? valorEixoAbsoluto : 0;
  int valorRe = valorEixo < 0 ? valorEixoAbsoluto : 0;

  analogWrite(IN1, valorFrente);
  analogWrite(IN2, valorRe);
  digitalWrite(ENA, HIGH);
}


void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);

  // #if defined(ARDUINO_AVR_NANO)
  // TCCR0B = (TCCR0B & 0b11111000) | 0x03; // Prescaler 64 1KHz
  // #endif

  pinMode(receiverPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(receiverPin), handleReceiverSignal, CHANGE);

  InicializarConfPinos();

  delay(500); 
}

void loop() {
      int resolucao = 255;

      unsigned long width;
  
      noInterrupts();
      width = pulseWidth;
      interrupts();

      int dutyCycle = 0;

      if ((!pronto) && ((width > 1470) && (width < 1530))) {
        pronto = true;
      } 

      if (pronto && (width > 0)) {
        dutyCycle = map(width, 991, 2014, -resolucao, resolucao);
        dutyCycle = min(dutyCycle, resolucao);
        dutyCycle = max(dutyCycle, -resolucao);
      }

      Serial.print("width: ");
      Serial.print(width);

      Serial.print("\tdutyCycle: ");
      Serial.print(dutyCycle);

      Serial.print("\tmillis(): ");
      Serial.print(millis());

      Serial.print("\tPronto: ");
      Serial.println(pronto);

      ControlarMotor(dutyCycle);

      delay(50);
}

