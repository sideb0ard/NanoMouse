#include <Arduino.h>

template <byte leftEmitter, byte leftDetector, byte frontEmitter, byte frontDetector, byte rightEmitter, byte rightDetector>

class NanoMouseSensors
{
  private:

    byte index;
    static const byte numReadings = 20;

    int frontAmbient;
    int frontCombined;
    int frontReflected;
    int frontTotal;
    int frontReadings[numReadings];
    int frontSmoothed;

    int leftAmbient;
    int leftCombined;
    int leftReflected;
    int leftTotal;
    int leftReadings[numReadings];
    int leftSmoothed;

    int rightAmbient;
    int rightCombined;
    int rightReflected;
    int rightTotal;
    int rightReadings[numReadings];
    int rightSmoothed;



  public:

    int front;
    int left;
    int right;

    void configure()
    {
      pinMode(leftEmitter, OUTPUT);
      pinMode(frontEmitter, OUTPUT);
      pinMode(rightEmitter, OUTPUT);
    }

    void sense()
    {
      digitalWrite(frontEmitter, HIGH);
      delay(1);
      frontCombined = analogRead(frontDetector);
      digitalWrite(frontEmitter, LOW);
      delay(1);
      frontAmbient = analogRead(frontDetector);
      frontReflected = frontCombined - frontAmbient;
      frontTotal -= frontReadings[index];
      frontReadings[index] = frontReflected;
      frontTotal += frontReadings[index];
      frontSmoothed = frontTotal / numReadings;

      digitalWrite(leftEmitter, HIGH);
      delay(1);
      leftCombined = analogRead(leftDetector);
      digitalWrite(leftEmitter, LOW);
      delay(1);
      leftAmbient = analogRead(leftDetector);
      leftReflected = leftCombined - leftAmbient;
      leftTotal -= leftReadings[index];
      leftReadings[index] = leftReflected;
      leftTotal += leftReadings[index];
      leftSmoothed = leftTotal / numReadings;

      digitalWrite(rightEmitter, HIGH);
      delay(1);
      rightCombined = analogRead(rightDetector);
      digitalWrite(rightEmitter, LOW);
      delay(1);
      rightAmbient = analogRead(rightDetector);
      rightReflected = rightCombined - rightAmbient;
      rightTotal -= rightReadings[index];
      rightReadings[index] = rightReflected;
      rightTotal += rightReadings[index];
      rightSmoothed = rightTotal / numReadings;


      index++;
      if (index >= numReadings) {
        index = 0;
      }

      front = frontSmoothed;
      left = leftSmoothed;
      right = rightSmoothed;

    }

    void view()
    {
      Serial.print(leftCombined);
      Serial.print("\t");
      Serial.print(leftSmoothed);
      Serial.print("\t");
      Serial.print(frontSmoothed);
      Serial.print("\t");
      Serial.println(rightSmoothed);
    }

    void initialize()
    {
      for (byte i = 0; i < numReadings; i++)
        sense();
    }
};

