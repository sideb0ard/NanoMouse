#include <Arduino.h>
#include <Servo.h>

#define LEFT 1
#define RIGHT -1

class NanoMouseMotors {

  private:

    Servo leftServo;
    Servo rightServo;
    static const byte power = 250;

  public:

    void attach(byte leftMotor, byte rightMotor)
    {
      leftServo.attach(leftMotor);
      rightServo.attach(rightMotor);
    }

    void stop(int time = 200)
    {
      leftServo.writeMicroseconds(1500);
      rightServo.writeMicroseconds(1500);
      delay(time);
    }

    void forward()
    {
      leftServo.writeMicroseconds(1000 + power);
      rightServo.writeMicroseconds(2000 - power);
    }

    void forwardTime(unsigned int time)
    {
      forward();
      delay(time);
      stop();
    }

    void turn(int direction, int degrees)
    {
      leftServo.writeMicroseconds(1500 + power * direction);
      rightServo.writeMicroseconds(1500 + power * direction);
      delay(degrees * 6.0);
      stop();
    }

    void square(int len, int direction)
    {
      for (int i = 0; i < 4; i++) {
        forward();
        delay(1000 * len);
        turn(direction, 90);
      }
    }

    void forwardProportional(int error)
    {
      const byte Kp = 1.5;
      leftServo.writeMicroseconds(1000 + power - Kp*error);
      rightServo.writeMicroseconds(2000 - power - Kp*error);
    }
    
};
