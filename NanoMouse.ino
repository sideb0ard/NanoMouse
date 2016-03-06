// #define DEBUG

#include <Servo.h>
#include "NanoMouseMotors.h"
#include "NanoMouseSensors.h"
#include "NanoMouseMaze.h"

NanoMouseMotors motors;

//<leftEmittor, leftDetector, frontEmittor, frontDetector, rightE, RightD
NanoMouseSensors<4, A7, 3, A6, 2, A5> sensors;

const byte ledPin = 13;

const byte buttonPin = 9;

int targetFront;
int thresholdFront;

int targetSide;
int thresholdSide;

NanoMouseMaze<4, 6> maze;

void setup()
{

  motors.attach(6, 5);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  sensors.configure();

  maze.mouseRow = 3;
  maze.mouseColumn = 0;
  maze.mouseHeading = NORTH;

  maze.targetRow = 2;
  maze.targetColumn = 4;

  Serial.begin(9600);
  while (digitalRead(buttonPin)) {}

  delay(200);

  calibrate();

  do
  {
    scanWalls();
    maze.solve();

    #ifdef DEBUG
    maze.print();
    while (digitalRead(buttonPin)) {}
    delay(200);
    #endif

    turnTowardBestNeighbor();
    forwardWhiskers();
  }
  while (maze.values[maze.mouseRow][maze.mouseColumn] != 0);

  #ifdef DEBUG
  scanWalls();
  maze.solve();
  maze.print();
  #endif
  

}

void loop()
{
  //navigateLabryrinth(state());
}

void scanWalls()
{
  if (sensors.front > thresholdFront)
    maze.addWalls(maze.mouseHeading);
  if (sensors.right > thresholdSide)
    maze.addWalls((maze.mouseHeading + 1) % 4);
  if (sensors.left > thresholdSide)
    maze.addWalls((maze.mouseHeading + 3) % 4);
}

void turnTowardBestNeighbor()
{
  byte desiredHeading = maze.findBestNeighbor();
  int difference = maze.mouseHeading - desiredHeading;

  if (difference == 1 || difference == -3)
    motors.turn(LEFT, 90);
  else if (difference == 3 || difference == -1)
    motors.turn(RIGHT, 90);
  else if (abs(difference) == 2)
    motors.turn(RIGHT, 180);
  // else do nothing

  sensors.initialize();
  maze.mouseHeading = desiredHeading;

}

byte state()
{

  byte event = 0;

  sensors.sense();
  if (sensors.front > thresholdFront) {
    event += 1;
  }
  if (sensors.left > thresholdSide) {
    event += 2;
  }
  if (sensors.right > thresholdSide) {
    event += 4;
  }

  return event;
}

void calibrate()
{
  sensors.initialize();
  targetSide = (sensors.left + sensors.right) / 2;

  motors.turn(RIGHT, 90);
  sensors.initialize();
  targetFront = sensors.front;
  thresholdSide = (targetSide + 2 * sensors.left) / 3;

  motors.turn(LEFT, 90);
  sensors.initialize();
  thresholdFront = (targetFront + sensors.front) / 2;
}

void forwardWhiskers()
{
  unsigned long startingTime = millis();

  while (sensors.front < targetFront && millis() - startingTime < 1505)
  {
    sensors.sense();

    if (sensors.left > thresholdSide && sensors.right > thresholdSide)
      motors.forwardProportional( sensors.left - sensors.right);
    else if (sensors.right > thresholdSide)
      motors.forwardProportional( targetSide - sensors.right );
    else if (sensors.left > thresholdSide)
      motors.forwardProportional( sensors.left - targetSide );
    else
      motors.forward();
  }

  motors.stop();

  maze.mouseRow += neighboringCells[maze.mouseHeading][0];
  maze.mouseColumn += neighboringCells[maze.mouseHeading][1];

}
void avoid(byte event)
{
  switch (event)
  {
    case 1: // front sensor triggered
      if (random(2))
        motors.turn(LEFT, random(90, 181));
      else
        motors.turn(RIGHT, random(90, 181));
      sensors.initialize();
      break;
    case 2: // left sensor triggered
      motors.turn(RIGHT, random(30, 61));
      sensors.initialize();
      break;
    case 4: // right sensor triggered
      motors.turn(LEFT, random(30, 61));
      sensors.initialize();
      break;
    default:
      motors.forward();
      break;
  }
}

void navigateLabryrinth(byte event)
{
  switch (event)
  {
    case 0:
    case 1:
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
      break;
    case 2:
      forwardWhiskers();
      break;
    case 3:
      motors.turn(RIGHT, 90);
      sensors.initialize();
      break;
    case 4:
      forwardWhiskers();
      break;
    case 5:
      motors.turn(LEFT, 90);
      sensors.initialize();
    case 6:
      forwardWhiskers();
      break;
    case 7:
      digitalWrite(ledPin, HIGH);
      break;
  }
}

